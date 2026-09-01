#include "ctpk.h"
#include "settingsmanager.h"

#include <QDataStream>
#include <QDebug>
#include <QtEndian>
#include <Etc1.h>
#include "crc32.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <ctime>
#include <thread>
#include <vector>

namespace
{

constexpr quint32 ctpkHeaderSize = 0x20;
constexpr quint32 ctpkEntrySize = 0x20;

constexpr quint32 bitmapEntrySize = sizeof(quint32);
constexpr quint32 hashEntrySize = 2 * sizeof(quint32);
constexpr quint32 conversionInfoEntrySize = sizeof(quint32);

constexpr quint32 hashSectionAlignment = 0x8;
constexpr quint32 textureSectionAlignment = 0x80;

constexpr char ctpkMagic[4] = {'C', 'T', 'P', 'K'};

constexpr quint32 alignUp(quint32 value, quint32 alignment)
{
    return (value + alignment - 1) / alignment * alignment;
}

void writeZeroBytes(QDataStream& out, qint64 count)
{
    while (count > 0)
    {
        const int chunk = static_cast<int>(std::min<qint64>(count, 0x4000));
        const QByteArray zeros(chunk, '\0');
        out.writeRawData(zeros.constData(), chunk);
        count -= chunk;
    }
}

void writePadding(QDataStream& out, quint64 position)
{
    const qint64 padding = static_cast<qint64>(position) - out.device()->pos();
    if (padding > 0)
        writeZeroBytes(out, padding);
}

}

Etc1::Etc1PackParams Ctpk::makePackParams(uint quality, bool dither)
{
    Etc1::Etc1PackParams pack_params;
    if (quality == 0)
        pack_params.mQuality = Etc1::Etc1Quality::Low;
    else if (quality == 1)
        pack_params.mQuality = Etc1::Etc1Quality::Medium;
    else
        pack_params.mQuality = Etc1::Etc1Quality::High;
    pack_params.mDithering = dither;
    return pack_params;
}

void Ctpk::packEtc1Block16(const QImage& img, int blockX, int blockY,
                           Etc1::Etc1PackParams& params, quint8 out[16])
{
    quint64 alphaBlock = 0;
    unsigned int packData[4 * 4];

    for (int sy = 0; sy < 4; sy++)
    {
        for (int sx = 0; sx < 4; sx++)
        {
            const int x_ = blockX + sx;
            const int y_ = blockY + sy;

            QColor c = img.pixelColor(x_, y_);
            packData[sy * 4 + sx] = (c.red() << 0) | (c.green() << 8) | (c.blue() << 16) | (0xFF << 24);
            alphaBlock |= (static_cast<quint64>(c.alpha() >> 4) & 0xF) << (4ULL * (sx * 4 + sy));
        }
    }

    quint64 etc1block;
    Etc1::packEtc1Block(&etc1block, packData, params);

    std::memcpy(out, &alphaBlock, sizeof(quint64));
    const quint64 cBlock = qbswap(etc1block);
    std::memcpy(out + 8, &cBlock, sizeof(quint64));
}

Ctpk::Ctpk(FileBase* file)
{
    this->file = file;

    file->open();
    file->seek(0);

    QString magic;
    file->readStringASCII(magic, 4);

    if (magic != "CTPK")
        throw std::runtime_error("CTPK: invalid file");

    // Parse CTPK Header

    version = file->read16();
    numEntries = file->read16();

    texSectionOffset = file->read32();
    texSectionSize = file->read32();
    hashSectionOffset = file->read32();
    infoSectionOffset = file->read32();


    // Parse Entries

    for (uint i = 0; i < numEntries; i++)
    {
        file->seek((i + 1) * 0x20);

        CtpkEntry* entry = new CtpkEntry();
        entry->filenameOffset = file->read32();
        entry->dataSize = file->read32();
        entry->dataOffset = file->read32();
        entry->format = static_cast<TextrueFormat>(file->read32());
        updataEntryHasAlpha(entry);
        entry->width = file->read16();
        entry->height = file->read16();
        entry->mipLevel = file->read8();
        entry->type = file->read8();
        entry->unk = file->read16();
        entry->bmpSizeOffset = file->read32();
        entry->unixTimestamp = file->read32();
        entries.append(entry);
    }


    // Parse Info 1 (Whatever this is)
    for (uint i = 0; i< numEntries; i++)
        entries[i]->info1 = file->read32();

    // Parse Hashes
    file->seek(hashSectionOffset);

    for (uint i = 0; i< numEntries; i++)
    {
        entries[i]->filenameHash = file->read32();
        file->skip(4);      // Hash Index?
    }


    // Parse Info 2 (Whatever this is, something about the texture)

    file->seek(infoSectionOffset);

    for (uint i = 0; i< numEntries; i++)
        entries[i]->info2 = file->read32();


    // Parse Filenames

    foreach (CtpkEntry* entry, entries)
    {
        file->seek(entry->filenameOffset);
        file->readStringASCII(entry->filename);
    }

    file->close();
    //printInfo();
}

Ctpk::~Ctpk()
{
    delete file;
    qDeleteAll(entries);
}

Ctpk::CtpkEntry* Ctpk::getEntryByFilename(QString filename)
{
    foreach (CtpkEntry* entry, entries)
    {
        if (entry->filename == filename)
            return entry;
    }

    return NULL;
}

void Ctpk::updataEntryHasAlpha(CtpkEntry* entry)
{
    switch (entry->format)
    {
    case RGBA4444:
    case RGBA5551:
    case RGBA8888:
    case ETC1_A4:
        entry->hasAlpha = true;
        break;
    default:
        entry->hasAlpha = false;
        break;
    }
}

QImage Ctpk::getTexture(quint32 entryIndex)
{
    if (entryIndex > numEntries-1)
    {
        throw std::runtime_error("CTPK: Texture Index out ouf Bounds");
    }

    return getTexture(entries[entryIndex]);
}

QImage Ctpk::getTexture(QString filename)
{
    CtpkEntry* entry = getEntryByFilename(filename);

    if (entry == NULL)
    {
        throw std::runtime_error("CTPK: Texture not found");
    }

    return getTexture(entry);
}

QImage Ctpk::getTexture(CtpkEntry* entry)
{
    QImage::Format imgFormat;

    if (entry->hasAlpha)
    {
        if (SettingsManager::getInstance()->get("premultiplyAlpha", true).toBool())
            imgFormat = QImage::Format_RGBA8888_Premultiplied;
        else
            imgFormat = QImage::Format_RGBA8888;
    }
    else
        imgFormat = QImage::Format_RGB888;


    QImage tex = QImage(entry->width, entry->height, imgFormat);

    switch (entry->format)
    {
        case ETC1:
        case ETC1_A4:
            getTextureETC1(entry, &tex);
            break;
        default:
            getTextureRaster(entry, &tex);
            break;
    }

    return tex;
}

void Ctpk::getTextureRaster(CtpkEntry* entry, QImage* tex)
{    
    quint8* data = tex->scanLine(0);
    bool premultiply = (tex->format() == QImage::Format_RGBA8888_Premultiplied);

    file->open();
    file->seek(texSectionOffset + entry->dataOffset);

    for (quint32 y = 0; y < entry->height; y += 8)
    {
        for (quint32 x = 0; x < entry->width; x += 8)
        {
            for (quint32 ty = 0; ty < 8; ty += 4)
            {
                for (quint32 tx = 0; tx < 8; tx += 4)
                {
                    for (quint32 sy = 0; sy < 4; sy += 2)
                    {
                        for (quint32 sx = 0; sx < 4; sx += 2)
                        {
                            for (quint32 yy = 0; yy < 2; yy++)
                            {
                                for (quint32 xx = 0; xx < 2; xx++)
                                {

                                    quint32 r = 0, g = 0, b = 0, a = 0;

                                    switch (entry->format)
                                    {
                                    case RGBA4444:
                                    {
                                        quint16 i = file->read16();
                                        r = ((i >> 12) & 0xF) << 4;
                                        g = ((i >> 8) & 0xF) << 4;
                                        b = ((i >> 4) & 0xF) << 4;
                                        a = (i & 0xF) << 4;
                                        break;
                                    }
                                    case RGBA5551:
                                    {
                                        quint16 i = file->read16();
                                        r = ((i >> 11) & 0x1F) << 3;
                                        g = ((i >> 6) & 0x1F) << 3;
                                        b = ((i >> 1) & 0x1F) << 3;
                                        if (i & 1)
                                            a = 255;
                                        else
                                            a = 0;
                                        break;
                                    }
                                    case RGBA8888:
                                    {
                                        a = file->read8();
                                        b = file->read8();
                                        g = file->read8();
                                        r = file->read8();
                                        break;
                                    }
                                    case RGB565:
                                    {
                                        quint16 i = file->read16();
                                        r = ((i >> 11) & 0x1F) << 3;
                                        g = ((i >> 5) & 0x3F) << 2;
                                        b = (i & 0x1F) << 3;
                                        break;
                                    }
                                    case RGB888:
                                    {
                                        b = file->read8();
                                        g = file->read8();
                                        r = file->read8();
                                        break;
                                    }
                                    default:
                                    {
                                        throw std::runtime_error("CTPK: Unsupported Texture Format");
                                        break;
                                    }
                                    }

                                    if (entry->hasAlpha)
                                    {
                                        quint32 dstpos = ((yy + sy + ty + y) * entry->width + xx + sx + tx + x) * 4;

                                        if (premultiply)
                                        {
                                            r = (r * a) / 255;
                                            g = (g * a) / 255;
                                            b = (b * a) / 255;
                                        }

                                        data[dstpos + 0] = r;
                                        data[dstpos + 1] = g;
                                        data[dstpos + 2] = b;
                                        data[dstpos + 3] = a;
                                    }
                                    else
                                    {
                                        quint32 dstpos = ((yy + sy + ty + y) * entry->width + xx + sx + tx + x) * 3;
                                        data[dstpos + 0] = r;
                                        data[dstpos + 1] = g;
                                        data[dstpos + 2] = b;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    file->close();
}

void Ctpk::getTextureETC1(CtpkEntry* entry, QImage* tex)
{
    const qint32 etc1_mod[8][2] =
    {
        {2, 8}, {5, 17}, {9, 29}, {13, 42},
        {18, 60}, {24, 80}, {33, 106}, {47, 183}
    };

    quint8* data = tex->scanLine(0);
    bool premultiply = (tex->format() == QImage::Format_RGBA8888_Premultiplied);

    file->open();
    file->seek(texSectionOffset + entry->dataOffset);

    for (quint32 y = 0; y < entry->height; y += 8)
    {
        for (quint32 x = 0; x < entry->width; x += 8)
        {
            for (quint32 ty = 0; ty < 8; ty += 4)
            {
                for (quint32 tx = 0; tx < 8; tx += 4)
                {
                    quint64 alpha = 0;
                    if (entry->format == ETC1_A4)
                    {
                        alpha = static_cast<quint64>(file->read32());
                        alpha |= (static_cast<quint64>(file->read32()) << 32);
                    }

                    quint16 subindexes = file->read16();
                    quint16 negative = file->read16();
                    quint32 flags_col = file->read32();

                    for (quint32 sx = 0; sx < 4; sx++)
                    {
                        for (quint32 sy = 0; sy < 4; sy++)
                        {
                            qint32 r, g, b;
                            quint32 tsx;

                            // dirty
                            if (flags_col & 0x1)
                                tsx = sy;
                            else
                                tsx = sx;

                            if (flags_col & 0x2)
                            {
                                r = (flags_col & 0xF8000000) >> 24;
                                g = (flags_col & 0x00F80000) >> 16;
                                b = (flags_col & 0x0000F800) >> 8;
                                if (tsx >= 2)
                                {
                                    qint32 dr = (flags_col & 0x07000000) >> 21;
                                    qint32 dg = (flags_col & 0x00070000) >> 13;
                                    qint32 db = (flags_col & 0x00000700) >> 5;
                                    if (dr & 0x20) dr -= 0x40;
                                    if (dg & 0x20) dg -= 0x40;
                                    if (db & 0x20) db -= 0x40;
                                    r += dr;
                                    g += dg;
                                    b += db;
                                }

                                r |= (r >> 5);
                                g |= (g >> 5);
                                b |= (b >> 5);
                            }
                            else
                            {
                                if (tsx >= 2)
                                {
                                    r = (flags_col & 0x0F000000) >> 20;
                                    g = (flags_col & 0x000F0000) >> 12;
                                    b = (flags_col & 0x00000F00) >> 4;
                                }
                                else
                                {
                                    r = (flags_col & 0xF0000000) >> 24;
                                    g = (flags_col & 0x00F00000) >> 16;
                                    b = (flags_col & 0x0000F000) >> 8;
                                }

                                r |= (r >> 4);
                                g |= (g >> 4);
                                b |= (b >> 4);
                            }

                            quint32 mod_index = (flags_col >> (tsx>=2 ? 2:5)) & 0x7;
                            qint32 mod = etc1_mod[mod_index][subindexes & 0x1];
                            if (negative & 0x1) mod = -mod;

                            r = clampColor(r + mod);
                            g = clampColor(g + mod);
                            b = clampColor(b + mod);



                            if (entry->hasAlpha)
                            {
                                quint8 a = alpha & 0xF;
                                a |= (a << 4);

                                quint32 dstpos = ((sy + ty + y) * entry->width + sx + tx + x) * 4;

                                if (premultiply)
                                {
                                    r = (r * a) / 255;
                                    g = (g * a) / 255;
                                    b = (b * a) / 255;
                                }

                                data[dstpos + 0] = r;
                                data[dstpos + 1] = g;
                                data[dstpos + 2] = b;
                                data[dstpos + 3] = a;
                            }
                            else
                            {
                                quint32 dstpos = ((sy + ty + y) * entry->width + sx + tx + x) * 3;
                                data[dstpos + 0] = r;
                                data[dstpos + 1] = g;
                                data[dstpos + 2] = b;
                            }

                            subindexes >>= 1;
                            negative >>= 1;
                            alpha >>= 4ULL;
                        }
                    }
                }
            }
        }
    }

    file->close();
}

void Ctpk::setTextureEtc1(quint32 entryIndex, QImage& img, bool alpha, uint quality, bool dither)
{
    assert(entryIndex < numEntries);

    TextrueFormat format = alpha ? ETC1_A4 : ETC1;

    CtpkEntry* entry = entries[entryIndex];
    assert(entry->format == format);
    assert(entry->width == img.width());
    assert(entry->height == img.height());

    Etc1::Etc1PackParams pack_params = makePackParams(quality, dither);

    const quint32 tilesX = img.width() / 8;
    const quint32 tilesY = img.height() / 8;
    const quint32 totalBlocks = tilesX * tilesY * 4;
    const quint32 bytesPerBlock = alpha ? 16 : 8;

    std::vector<quint8> output(static_cast<size_t>(totalBlocks) * bytesPerBlock, 0);

    auto packBlock = [&](quint32 idx) {
        const quint32 t = idx & 3;
        const quint32 rem = idx >> 2;
        const int x = static_cast<int>((rem % tilesX) * 8);
        const int y = static_cast<int>((rem / tilesX) * 8);
        const int tx = static_cast<int>(t & 1) * 4;
        const int ty = static_cast<int>(t >> 1) * 4;

        quint8 blockData[16];
        packEtc1Block16(img, x + tx, y + ty, pack_params, blockData);

        quint8* dst = &output[static_cast<size_t>(idx) * bytesPerBlock];

        if (alpha)
        {
            std::memcpy(dst, blockData, sizeof(quint64));
            dst += sizeof(quint64);
        }

        std::memcpy(dst, blockData + 8, sizeof(quint64));
    };

    const quint32 hwThreads = static_cast<quint32>(std::thread::hardware_concurrency());
    const quint32 threadCount = std::clamp<quint32>(hwThreads, 1, totalBlocks);

    if (totalBlocks <= 1 || threadCount == 1)
    {
        for (quint32 idx = 0; idx < totalBlocks; ++idx)
            packBlock(idx);
    }
    else
    {
        std::atomic<quint32> nextBlock{0};

        std::vector<std::thread> workers;
        workers.reserve(threadCount);
        for (quint32 w = 0; w < threadCount; ++w)
        {
            workers.emplace_back([&]() {
                for (;;)
                {
                    const quint32 idx = nextBlock.fetch_add(1, std::memory_order_relaxed);
                    if (idx >= totalBlocks) return;
                    packBlock(idx);
                }
            });
        }

        for (auto& thread : workers)
            thread.join();
    }

    file->open();
    file->seek(texSectionOffset + entry->dataOffset);
    file->writeData(output.data(), output.size());
    file->save();
    file->close();
}

void Ctpk::setTextureEtc1Region(quint32 entryIndex, QImage& img, QRect region, uint quality, bool dither)
{
    assert(entryIndex < numEntries);

    CtpkEntry* entry = entries[entryIndex];
    assert(entry->format == ETC1_A4);
    assert(img.width() == static_cast<int>(entry->width));
    assert(img.height() == static_cast<int>(entry->height));

    Etc1::Etc1PackParams pack_params = makePackParams(quality, dither);

    const quint32 tilesX = entry->width / 8;

    // Clamp the region to the texture and expand it to whole 4x4 ETC1 blocks
    const int rx0 = std::max(0, region.x());
    const int ry0 = std::max(0, region.y());
    const int rx1 = std::min(static_cast<int>(entry->width),  region.x() + region.width());
    const int ry1 = std::min(static_cast<int>(entry->height), region.y() + region.height());
    if (rx1 <= rx0 || ry1 <= ry0)
        return;

    const int bx0 = rx0 / 4;
    const int by0 = ry0 / 4;
    const int bx1 = (rx1 - 1) / 4;
    const int by1 = (ry1 - 1) / 4;

    file->open();

    for (int by = by0; by <= by1; by++)
    {
        for (int bx = bx0; bx <= bx1; bx++)
        {
            const quint32 tx = bx / 2;
            const quint32 ty = by / 2;
            const quint32 t = (bx & 1) | ((by & 1) << 1);
            const quint32 idx = (ty * tilesX + tx) * 4 + t;

            quint8 blockData[16];
            packEtc1Block16(img, bx * 4, by * 4, pack_params, blockData);

            file->seek(texSectionOffset + entry->dataOffset + static_cast<qint64>(idx) * 16);
            file->writeData(blockData, sizeof(blockData));
        }
    }

    file->close();
}

void Ctpk::save()
{
    file->save();
}

quint32 Ctpk::hashFilename(const QString& filename) const
{
    quint32 table[256];
    crc32::generate_table(table);

    const QByteArray ascii = filename.toUtf8();
    return crc32::update(table, 0, ascii.constData(), ascii.size());
}

void Ctpk::rebuild()
{
    QList<quint32> oldDataOffsets;
    oldDataOffsets.reserve(entries.size());
    for (const CtpkEntry* entry : std::as_const(entries))
        oldDataOffsets.append(entry->dataOffset);

    const QByteArray oldTexData = readTextureSection();
    const Layout layout = computeLayout();

    commit(serialize(layout, oldTexData, oldDataOffsets));
}

Ctpk::Layout Ctpk::computeLayout()
{
    quint32 cursor = ctpkHeaderSize + numEntries * (ctpkEntrySize + bitmapEntrySize);

    for (CtpkEntry* entry : std::as_const(entries))
    {
        entry->filenameOffset = cursor;
        cursor += static_cast<quint32>(entry->filename.toLatin1().size()) + 1;
    }

    cursor = alignUp(cursor, hashSectionAlignment);
    hashSectionOffset = cursor;
    cursor += numEntries * hashEntrySize;

    infoSectionOffset = cursor;
    cursor += numEntries * conversionInfoEntrySize;

    cursor = alignUp(cursor, textureSectionAlignment);
    texSectionOffset = cursor;

    quint32 texPos = cursor;
    for (CtpkEntry* entry : std::as_const(entries))
    {
        texPos = alignUp(texPos, textureSectionAlignment);
        entry->dataOffset = texPos - texSectionOffset;
        texPos += entry->dataSize;
    }

    texSectionSize = texPos - texSectionOffset;

    Layout layout;
    layout.hashSectionOffset = hashSectionOffset;
    layout.infoSectionOffset = infoSectionOffset;
    layout.texSectionOffset = texSectionOffset;
    layout.texSectionSize = texSectionSize;
    layout.totalSize = texPos;
    return layout;
}

QByteArray Ctpk::readTextureSection() const
{
    const quint64 fileSize = file->size();
    if (texSectionOffset >= fileSize)
        return QByteArray();

    const int size = static_cast<int>(std::min<quint64>(texSectionSize, fileSize - texSectionOffset));
    QByteArray data(size, '\0');

    file->open();
    file->seek(texSectionOffset);
    file->readData(reinterpret_cast<quint8*>(data.data()), static_cast<quint64>(size));
    file->close();

    return data;
}

QByteArray Ctpk::serialize(const Layout& layout, const QByteArray& oldTexData, const QList<quint32>& oldDataOffsets) const
{
    QByteArray blob;
    QDataStream out(&blob, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);

    out.writeRawData(ctpkMagic, sizeof(ctpkMagic));
    out << static_cast<quint16>(version);
    out << static_cast<quint16>(numEntries);
    out << layout.texSectionOffset;
    out << layout.texSectionSize;
    out << layout.hashSectionOffset;
    out << layout.infoSectionOffset;
    out << quint64(0);

    for (const CtpkEntry* entry : entries)
    {
        out << entry->filenameOffset;
        out << entry->dataSize;
        out << entry->dataOffset;
        out << static_cast<quint32>(entry->format);
        out << entry->width;
        out << entry->height;
        out << entry->mipLevel;
        out << entry->type;
        out << entry->unk;
        out << entry->bmpSizeOffset;
        out << entry->unixTimestamp;
    }

    for (const CtpkEntry* entry : entries)
        out << entry->dataSize;

    for (const CtpkEntry* entry : entries)
    {
        const QByteArray name = entry->filename.toLatin1();
        out.writeRawData(name.constData(), name.size());
        out << quint8(0);
    }

    writePadding(out, layout.hashSectionOffset);

    for (quint32 i = 0; i < numEntries; i++)
    {
        out << entries[i]->filenameHash;
        out << i;
    }

    for (const CtpkEntry* entry : entries)
        out << entry->info2;

    writePadding(out, layout.texSectionOffset);

    for (quint32 i = 0; i < numEntries; i++)
    {
        const CtpkEntry* entry = entries[i];

        const qint64 available = static_cast<qint64>(oldTexData.size()) - oldDataOffsets[i];
        const qint64 copied = std::max<qint64>(0, std::min<qint64>(available, entry->dataSize));

        if (copied > 0)
            out.writeRawData(oldTexData.constData() + oldDataOffsets[i], static_cast<int>(copied));

        if (copied < static_cast<qint64>(entry->dataSize))
            writeZeroBytes(out, static_cast<qint64>(entry->dataSize) - copied);

        if (i + 1 < numEntries)
            writePadding(out, layout.texSectionOffset + entries[i + 1]->dataOffset);
    }

    return blob;
}

void Ctpk::commit(QByteArray data)
{
    file->open();
    file->resize(static_cast<quint64>(data.size()));
    file->seek(0);
    file->writeData(reinterpret_cast<quint8*>(data.data()), static_cast<quint64>(data.size()));
    file->close();
}

void Ctpk::setFilename(QString newName)
{
    foreach (CtpkEntry* entry, entries)
    {
        entry->filename = newName;
        entry->filenameHash = hashFilename(newName);
    }

    rebuild();
}

void Ctpk::setTextureSize(quint32 entryIndex, quint32 w, quint32 h)
{
    assert(entryIndex < numEntries);

    CtpkEntry* entry = entries[entryIndex];
    entry->width = w;
    entry->height = h;

    static const quint32 bpp[14] = {32, 24, 16, 16, 16, 16, 16, 8, 8, 8, 4, 4, 4, 8};
    if (static_cast<quint32>(entry->format) < 14)
        entry->dataSize = w * h * bpp[entry->format] / 8;
    entry->unixTimestamp = static_cast<quint32>(time(nullptr));

    rebuild();
}

void Ctpk::printInfo()
{
    qDebug() << "CTPK Info:";
    qDebug() << "- Version:" << version;
    qDebug() << "- Entry Count:" << numEntries;
    qDebug() << "- Texture Section Offset:" << texSectionOffset;
    qDebug() << "- Texture Section Size:" << texSectionSize;
    qDebug() << "- Hash Section Offset:" << hashSectionOffset;
    qDebug() << "- Info Section Offset:" << infoSectionOffset;

    qDebug() << "- Entries:";
    for (uint i = 0; i < numEntries; i++)
    {
        CtpkEntry* entry = entries[i];

        qDebug() << "  - Entry" << i;
        qDebug() << "    - Filename:" << entry->filename;
        qDebug() << "    - Filename Offset:" << entry->filenameOffset;
        qDebug() << "    - Filename Hash:" << entry->filenameHash;
        qDebug() << "    - Format:" << entry->format;
        qDebug() << "    - Width:" << entry->width;
        qDebug() << "    - Height:" << entry->height;
        qDebug() << "    - Mip Level:" << entry->mipLevel;
        qDebug() << "    - Type:" << entry->type;
        qDebug() << "    - Bitmap Size Offset:" << entry->bmpSizeOffset;
        qDebug() << "    - Unix Timestamp:" << entry->unixTimestamp;
        qDebug() << "    - Info 1:" << entry->info1;
        qDebug() << "    - Info 2:" << entry->info2;
        qDebug() << "    - Unknown:" << entry->unk;
    }
}
