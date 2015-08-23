#include "sarcfilesystem.h"

SarcFilesystem::SarcFilesystem(FileBase* file)
{
    quint32 header[8];

    // TODO: someshit if the file was already opened??
    file->open();

    /*file->readData((quint8*)header, 8*4);
    qDebug("SARC: %08X %08X %08X %08X | %08X %08X %08X %08X",
           header[0], header[1], header[2], header[3],
           header[4], header[5], header[6], header[7]);*/
    file->seek(0);
    quint32 tag = file->read32();
    if (tag != 0x43524153)
    {
        qDebug("SARC: bad tag %08X", tag);
        return; // TODO proper error handling!!!
    }

    /*file->seek(0x98);
    QString perefouras;
    file->readStringASCII(perefouras, 0);
    file->seek(0xB4);
    QString maitredestenebres;
    file->readStringASCII(maitredestenebres, 0);

    qDebug("père fouras");
    qDebug(perefouras.toStdString().c_str());
    qDebug("maître des ténèbres");
    qDebug(maitredestenebres.toStdString().c_str());*/

    // SARC header
    file->skip(0x8);
    dataOffset = file->read32();
    file->skip(0x4);

    // SFAT header
    sfatOffset = (quint32)file->pos();
    file->skip(0x6);
    numFiles = file->read16();
    hashMult = file->read32();

    sfntOffset = sfatOffset + 0xC + (numFiles * 0x10);

    qDebug("dataoffset %08X | numfiles %d | hashmult %08X", dataOffset, numFiles, hashMult);
    qDebug("sfatOffset %08X | sfntOffset %08X", sfatOffset, sfntOffset);
    QString darp = "course/course1_bgdatL1.bin"; qDebug("hash: %08X", filenameHash(darp));

    for (quint32 i = 0; i < numFiles; i++)
    {
        file->seek(sfatOffset + 0xC + (i * 0x10));
        InternalSarcFile* entry = new InternalSarcFile();

        entry->nameHash = file->read32();
        entry->nameOffset = (file->read32() & 0x00FFFFFF) << 2;
        entry->offset = file->read32();
        entry->size = file->read32() - entry->offset;

        file->seek(sfntOffset + 0x8 + entry->nameOffset);
        file->readStringASCII(entry->name, 0);

        qDebug("---------------");
        qDebug(entry->name.toStdString().c_str());
        qDebug(" -- OFFSET %08X SIZE %08X HASH %08X/%08X", entry->offset, entry->size, entry->nameHash, filenameHash(entry->name));

        files.insert(entry->name, *entry);
    }

    file->close();
}



quint32 SarcFilesystem::filenameHash(QString& name)
{
    quint32 ret = 0;
    for (int i = 0; i < name.size(); i++)
    {
        ret *= hashMult;
        ret += name[i].toLatin1();
    }
    return ret;
}
