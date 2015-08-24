#include "filesystem.h"

SarcFilesystem::SarcFilesystem(FileBase* file)
{
    sarc = file;

    // TODO: someshit if the file was already opened??
    file->open();

    file->seek(0);
    quint32 tag = file->read32();
    if (tag != 0x43524153)
    {
        qDebug("SARC: bad tag %08X", tag);
        throw std::runtime_error("SARC: invalid file");
    }

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


FileBase* SarcFilesystem::openFile(QString path)
{
    if (path[0] == '/')
        path.remove(0,1);

    if (!files.contains(path))
    {
        throw std::runtime_error("SARC: file "+path.toStdString()+" doesn't exist");
    }

    InternalSarcFile& entry = files[path];

    FileBase* ret = sarc->getSubfile(this, dataOffset+entry.offset, entry.size);
    ret->setIdPath(path);
    return ret;
}

bool SarcFilesystem::save(FileBase *file)
{
    // todo: do shit here

    return true;
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
