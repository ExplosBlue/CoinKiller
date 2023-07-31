#include "ctpk.h"
#include "settingsmanager.h"

#include <QDebug>
#include <QtEndian>
#include "rg_etc1.h"
#include "crc32.h"

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
        entry->format = (TextrueFormat)file->read32();
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

                                    quint32 r, g, b, a;

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
                    quint64 alpha;
                    if (entry->format == ETC1_A4)
                    {
                        alpha = (quint64)file->read32();
                        alpha |= ((quint64)file->read32() << 32);
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

    rg_etc1::etc1_pack_params pack_params;
    if (quality == 0)
        pack_params.m_quality = rg_etc1::cLowQuality;
    else if (quality == 1)
        pack_params.m_quality = rg_etc1::cMediumQuality;
    else
        pack_params.m_quality = rg_etc1::cHighQuality;
    pack_params.m_dithering = dither;

    printf("ETC1 compression progress: 0%%");
    fflush(stdout);

    file->open();
    file->seek(texSectionOffset + entry->dataOffset);

    for (int y = 0; y < img.height(); y += 8)
    {
        for (int x = 0; x < img.width(); x += 8)
        {
            for (int ty = 0; ty < 8; ty += 4)
            {
                for (int tx = 0; tx < 8; tx += 4)
                {
                    quint64 alphaData = 0;
                    unsigned int packData[4*4];

                    for (int sx = 0; sx < 4; sx++)
                    {
                        for (int sy = 0; sy < 4; sy++)
                        {
                            int x_ = x+tx+sx;
                            int y_ = y+ty+sy;

                            QColor c = img.pixelColor(x_, y_);
                            packData[sy*4 + sx] = (c.red()<<0) | (c.green()<<8) | (c.blue()<<16) | (0xFF<<24);

                            alphaData = (alphaData >> 4ULL) | (static_cast<quint64>(c.alpha() >> 4) << 60ULL);
                        }
                    }

                    if (alpha)
                        file->write64(alphaData);

                    quint64 etc1block;
                    rg_etc1::pack_etc1_block(&etc1block, packData, pack_params);

                    file->write64(qbswap(etc1block));
                }
            }
        }

        printf("\rETC1 compression progress: %d%%", (y+8)*100 / img.height());
        fflush(stdout);
    }

    printf("\rETC1 compression progress: Done.\n");
    fflush(stdout);

    file->save();
    file->close();
}

void Ctpk::setFilename(QString newName)
{
    // Hacky fix to ensure ctpk filename is correct when creating new tilesets
    // TODO: implement proper ctpk writing so files
    for (uint i = 0; i< numEntries; i++)
    {
        file->open();
        file->seek(entries[i]->filenameOffset);
        file->writeStringASCII(newName, 36);

        // Calculate filename hash
        file->seek(hashSectionOffset + i*64);

        quint32 table[256];
        crc32::generate_table(table);

        std::string utf8_text = newName.toUtf8().constData();
        const void * a = utf8_text.c_str();

        quint32 crc = crc32::update(table, 0, a, utf8_text.size());
        file->write32(crc);

        file->save();
        file->close();
    }
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
