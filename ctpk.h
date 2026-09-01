#ifndef CTPK_H
#define CTPK_H

#include "filesystem/filesystem.h"

#include <Etc1.h>
#include <QImage>

class Ctpk
{
public:
    Ctpk(FileBase* file);
    ~Ctpk();

    QImage getTexture(quint32 entryIndex);
    QImage getTexture(QString filename);

    void setTextureEtc1(quint32 entryIndex, QImage& img, bool alpha, uint quality = 1, bool dither = false);

    void setTextureEtc1Region(quint32 entryIndex, QImage& img, QRect region, uint quality = 1, bool dither = false);

    void rebuild();

    void save();

    void setFilename(QString newName);

    void setTextureSize(quint32 entryIndex, quint32 w, quint32 h);

private:
    FileBase* file;

    enum TextrueFormat
    {
        RGBA8888 = 0,
        RGB888 = 1,
        RGBA5551 = 2,
        RGB565 = 3,
        RGBA4444 = 4,
        LA88 = 5,
        HL8 = 6,
        L8 = 7,
        A8 = 8,
        LA44 = 9,
        L4 = 10,
        A4 = 11,
        ETC1 = 12,
        ETC1_A4 = 13
    };

    struct Layout
    {
        quint32 hashSectionOffset = 0;
        quint32 infoSectionOffset = 0;
        quint32 texSectionOffset = 0;
        quint32 texSectionSize = 0;
        quint32 totalSize = 0;
    };

    struct CtpkEntry
    {
        quint32 filenameOffset;
        quint32 dataSize;
        quint32 dataOffset;
        TextrueFormat format;
        quint16 width;
        quint16 height;
        quint8 mipLevel;
        quint8 type;
        quint16 unk;
        quint32 bmpSizeOffset;
        quint32 unixTimestamp;

        quint32 info1;

        quint32 filenameHash;

        quint32 info2;


        QString filename;
        bool hasAlpha;
    };

    quint32 version;
    quint32 numEntries;
    quint32 texSectionOffset;
    quint32 texSectionSize;
    quint32 hashSectionOffset;
    quint32 infoSectionOffset;

    QList<CtpkEntry*> entries;


    CtpkEntry* getEntryByFilename(QString filename);

    QImage getTexture(CtpkEntry* entry);
    void getTextureRaster(CtpkEntry* entry, QImage* tex);
    void getTextureETC1(CtpkEntry* entry, QImage* tex);

    qint32 clampColor(qint32 val)
    {
        if (val > 255) return 255;
        if (val < 0) return 0;
        return val;
    }

    static Etc1::Etc1PackParams makePackParams(uint quality, bool dither);
    static void packEtc1Block16(const QImage& img, int blockX, int blockY,
                                Etc1::Etc1PackParams& params, quint8 out[16]);

    void updataEntryHasAlpha(CtpkEntry* entry);

    quint32 hashFilename(const QString& filename) const;

    Layout computeLayout();
    QByteArray readTextureSection() const;
    QByteArray serialize(const Layout& layout, const QByteArray& oldTexData, const QList<quint32>& oldDataOffsets) const;
    void commit(QByteArray data);

    void printInfo();

};

#endif // CTPK_H
