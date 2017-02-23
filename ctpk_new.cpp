#include "ctpk_new.h"

#include <QDebug>

Ctpk_new::Ctpk_new(FileBase* file)
{
    this->file = file;

    file->open();
    file->seek(0);

    QString magic;
    file->readStringASCII(magic, 4);

    if (magic != "CTPK")
    {
        throw std::runtime_error("CTPK: invalid file");
    }


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
        entry->format = file->read32();
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
    {
        entries[i]->info1 = file->read32();
    }


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
    {
        entries[i]->info2 = file->read32();
    }


    // Parse Filenames

    foreach (CtpkEntry* entry, entries)
    {
        file->seek(entry->filenameOffset);
        file->readStringASCII(entry->filename);
    }


    file->close();


    printInfo();
}

Ctpk_new::~Ctpk_new()
{
    delete file;
}


void Ctpk_new::printInfo()
{
    qDebug() << "CTPK Info:";
    qDebug() << "- Version:" << version;
    qDebug() << "- Entry Count:" << numEntries;
    qDebug() << "- Texture Section Offset:" << texSectionOffset;
    qDebug() << "- Texture Section Size:" << texSectionSize;
    qDebug() << "- Hash Section Offset:" << hashSectionOffset;
    qDebug() << "- Info Section Offset:" << infoSectionOffset;

    qDebug() << "- Entries:";
    for (int i = 0; i < numEntries; i++)
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
