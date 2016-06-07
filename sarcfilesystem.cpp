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

#include <QDebug>

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

    file->open();
    sarc->open();

    if (files.contains(path))
    {
        // reinsert existing file

        InternalSarcFile* thisfile = files[path];

        if (thisfile == NULL)
            throw std::logic_error("thisfile is NULL, shouldn't happen");


        // temp data after the file
        quint32 oldmoveoffset = dataOffset + thisfile->offset + thisfile->size;

        // get first file offset after thisfile
        quint32 bufOffset = 0xFFFFFFFF;
        bool lastFile = true;
        for (int i = 0; i < files.size(); i++)
        {
            InternalSarcFile* ifile = files.values()[i];
            if (ifile == thisfile)
                continue;
            if (ifile->offset <= thisfile->offset)
                continue;
            if (ifile->offset + dataOffset < bufOffset)
            {
                bufOffset = ifile->offset + dataOffset;
                lastFile = false;
            }
        }
        quint32 oldPad = bufOffset - oldmoveoffset;

        quint32 tempsize = sarc->size() - bufOffset;
        qDebug() << "temping data: from" << bufOffset << "-" << tempsize << "bytes";
        qDebug() << "old pad size:" << oldPad;

        quint8* tempbuf = new quint8[tempsize];     // TODO: might fail if it's too big
        sarc->seek(bufOffset);
        sarc->readData(tempbuf, tempsize);


        // resize properly
        quint32 writeOffset = dataOffset + thisfile->offset;
        quint32 writeSize = file->size();
        quint32 newSarcSize = sarc->size();
        if (!lastFile)
            newSarcSize +=  -oldPad + align(writeOffset+writeSize, dataAlign) - (writeOffset+writeSize);

        qDebug() << "new SARC size:" << newSarcSize << "old:" << sarc->size();
        sarc->resize(newSarcSize);

        sarc->seek(0x8);
        sarc->write32(sarc->size());


        // write file and fix SFAT
        quint8* filetempbuf = new quint8[writeSize];
        file->seek(0);
        file->readData(filetempbuf, writeSize);
        sarc->seek(dataOffset + thisfile->offset);
        sarc->writeData(filetempbuf, writeSize);
        delete[] filetempbuf;


        // write align and following data if existing and fix their SFAT entries
        if (!lastFile)
        {
            quint8 padSize = align(sarc->pos(), dataAlign) - sarc->pos();
            for (quint8 i=0; i < padSize; i++) sarc->write8(0x00);
            quint32 fix_diff = sarc->pos() - bufOffset;
            sarc->writeData(tempbuf, tempsize);

            for (int i = 0; i < files.size(); i++)
            {
                InternalSarcFile* tofix = files.values()[i];
                if (tofix == thisfile)
                    continue;
                if (tofix->offset <= thisfile->offset)
                    continue;

                tofix->offset += fix_diff;

                sarc->seek(tofix->entryOffset + 0x8);
                sarc->write32(tofix->offset);
                sarc->write32(tofix->offset + tofix->size);
            }
        }

        delete[] tempbuf;

        // fix thisfile SFAT entry
        sarc->seek(thisfile->entryOffset + 0xC);
        sarc->write32(thisfile->offset + writeSize);
    }
    else
    {
        // insert new file

        // TODO
        return true;
    }

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
