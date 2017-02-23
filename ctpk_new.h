#ifndef CTPK_NEW_H
#define CTPK_NEW_H

#include "filesystem.h"


class Ctpk_new
{
public:
    Ctpk_new(FileBase* file);
    ~Ctpk_new();

private:
    FileBase* file;

    struct CtpkEntry
    {
        quint32 filenameOffset;
        quint32 dataSize;
        quint32 dataOffset;
        quint32 format;
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
    };

    quint32 version;
    quint32 numEntries;
    quint32 texSectionOffset;
    quint32 texSectionSize;
    quint32 hashSectionOffset;
    quint32 infoSectionOffset;

    QList<CtpkEntry*> entries;


    void printInfo();

};

#endif // CTPK_NEW_H
