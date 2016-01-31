/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

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

    for (quint32 i = 0; i < numFiles; i++)
    {
        file->seek(sfatOffset + 0xC + (i * 0x10));
        InternalSarcFile* entry = new InternalSarcFile();
        entry->entryOffset = (quint32)file->pos();

        entry->nameHash = file->read32();
        entry->nameOffset = (file->read32() & 0x00FFFFFF) << 2;
        entry->offset = file->read32();
        entry->size = file->read32() - entry->offset;

        file->seek(sfntOffset + 0x8 + entry->nameOffset);
        file->readStringASCII(entry->name, 0);

        files.insert(entry->name, entry);
    }

    file->close();
}

SarcFilesystem::~SarcFilesystem()
{
    delete sarc;
}


bool SarcFilesystem::directoryExists(QString path)
{
    if (path[0] == '/')
        path.remove(0,1);

    for (int i = 0; i < files.size(); i++)
    {
        QString thispath = files.keys()[i];
        if (thispath.size() <= path.size())
            continue;

        QString thispathbase = thispath;
        thispathbase.truncate(path.size());
        if (thispathbase == path)
            return true;
    }

    return false;
}

void SarcFilesystem::directoryContents(QString path, QDir::Filter filter, QList<QString>& out)
{
    if (path[0] == '/')
        path.remove(0,1);

    out.clear();

    for (int i = 0; i < files.size(); i++)
    {
        QString thispath = files.keys()[i];
        if (thispath.size() <= path.size())
            continue;

        QString thispathbase = thispath;
        thispathbase.truncate(path.size());
        if (thispathbase == path)
        {
            QString pathindir = thispath;
            if (path.size())
                pathindir.remove(0, path.size()+1);

            int slashidx = pathindir.indexOf('/');
            bool isdir = slashidx != -1;
            if ((isdir && (filter & QDir::Dirs)) || ((!isdir) && (filter & QDir::Files)))
            {
                if (isdir) pathindir.truncate(slashidx);

                if (!out.contains(pathindir))
                    out.append(pathindir);
            }
        }
    }
}


bool SarcFilesystem::fileExists(QString path)
{
    if (path[0] == '/')
        path.remove(0,1);

    return files.contains(path);
}

FileBase* SarcFilesystem::openFile(QString path)
{
    if (path[0] == '/')
        path.remove(0,1);

    if (!files.contains(path))
    {
        throw std::runtime_error("SARC: file "+path.toStdString()+" doesn't exist");
    }

    InternalSarcFile* entry = files[path];

    FileBase* ret = sarc->getSubfile(this, dataOffset+entry->offset, entry->size);
    ret->setIdPath(path);
    return ret;
}

bool SarcFilesystem::save(FileBase *file)
{
    QString path = file->getIdPath();
    quint32 writeoffset = 0;

    file->open();
    quint32 writesize = file->size();

    sarc->open();

    if (files.contains(path))
    {
        // reinsert existing file

        quint32 hash = filenameHash(path);
        InternalSarcFile* thisfile = files[path];

        qDebug("new file size %08X", writesize);

        if (thisfile == NULL)
            throw std::logic_error("thisfile is NULL, shouldn't happen");

        writeoffset = dataOffset + thisfile->offset;
        quint32 sizediff = writesize - thisfile->size;
        qDebug("moving data %08X", sizediff);
        qDebug("writeoffset = %08X + %08X", dataOffset, thisfile->offset);

        // move shit
        quint32 sizetomove = sarc->size() - (writeoffset + thisfile->size);
        qDebug("allocating %08X (%08X - (%08X + %08X))", sizetomove, (quint32)sarc->size(), writeoffset, thisfile->size);
        quint8* tempbuf = new quint8[sizetomove]; // TODO might fail if it's too big
        sarc->seek(writeoffset + thisfile->size);
        sarc->readData(tempbuf, sizetomove);
        sarc->seek(writeoffset + writesize);
        sarc->writeData(tempbuf, sizetomove);
        delete[] tempbuf;

        qDebug("done moving crap");

        // fix size
        sarc->seek(thisfile->entryOffset + 0xC);
        sarc->write32(thisfile->offset + writesize);

        qDebug("file size fixed");

        // fix offsets of files that come later
        for (int i = 0; i < files.size(); i++)
        {
            InternalSarcFile* tofix = files.values()[i];
            if (tofix == thisfile)
                continue;
            if (tofix->offset <= thisfile->offset)
                continue;

            qDebug("file %s (%08X %08X %08X) gets fixed -> %08X", "lalala", tofix->entryOffset, tofix->offset, tofix->size,
                   tofix->offset+sizediff);

            tofix->offset += sizediff;

            sarc->seek(tofix->entryOffset + 0x8);
            sarc->write32(tofix->offset);
            sarc->write32(tofix->offset + tofix->size);
        }

        // fix the internal shit
        thisfile->size = writesize;
    }
    else
    {
        // insert new file

        // TODO
        return true;
    }

    /*quint8* tempbuf = new quint8[4096];
        quint64 pos = 0;
        while (pos < size)
        {
            quint64 toread = 4096;
            if ((pos+toread) > size) toread = size-pos;

            readData(tempbuf, toread);
            ret->writeData(tempbuf, toread);
            pos += toread;
        }

        ret->close();
        delete[] tempbuf;*/
    // same lazy shit
    file->seek(0);
    quint8* tempbuf = new quint8[writesize];
    file->readData(tempbuf, writesize);
    sarc->seek(writeoffset);
    sarc->writeData(tempbuf, writesize);

    file->close();
    sarc->save();
    sarc->close();

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
