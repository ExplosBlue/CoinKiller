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

    if (!path.endsWith("/"))
        path.append("/");

    for (int i = 0; i < files.size(); i++)
    {
        QString thispath = files.keys()[i];

        if (thispath.startsWith(path))
            return true;
    }

    return false;
}

void SarcFilesystem::directoryContents(QString path, QDir::Filter filter, QList<QString>& out)
{
    if (path[0] == '/')
        path.remove(0,1);

    if (path != "" && !path.endsWith("/"))
        path.append("/");

    out.clear();

    for (int i = 0; i < files.size(); i++)
    {
        QString thispath = files.keys()[i];

        if (thispath.startsWith(path))
        {
            thispath.remove(0, path.length());

            int slashidx = thispath.indexOf('/');
            bool isdir = (slashidx != -1);

            if ((isdir && (filter & QDir::Dirs)) || ((!isdir) && (filter & QDir::Files)))
            {
                if (isdir) thispath = thispath.left(slashidx);
                if (!out.contains(thispath))
                    out.append(thispath);
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
    sarc->open();
    file->open();

    numFiles = files.size();    // just to be sure

    QString path = file->getIdPath();

    if (path.isNull() || path.isEmpty())
        throw std::runtime_error("tried saving file without specified filename");

    if (!files.contains(path))
    {
        InternalSarcFile* entry = new InternalSarcFile();
        entry->name = path;
        entry->size = file->size();
        files.insert(entry->name, entry);

        numFiles++;
    }
    else
    {
        InternalSarcFile* thisfile = files[path];

        if (thisfile == NULL)
            throw std::logic_error("thisfile is NULL, shouldn't happen");

        thisfile->size = file->size();
    }


    // Generate sorted List, since SARCs want to be ordered after their File Name Hash
    QList<InternalSarcFile*> sortedFiles;
    for (int i = 0; i < files.size(); i++)
    {
        InternalSarcFile* ifile = files.values()[i];
        ifile->nameHash = filenameHash(ifile->name);
        sortedFiles.append(ifile);
    }
    qSort(sortedFiles.begin(), sortedFiles.end(), hashSort);


    // recreate SARC

    // get correct size
    quint32 newSarcSize = 0x28;             // SARC, SFAT and SFNT headers
    newSarcSize += numFiles*0x10;           // SFAT nodes

    quint32 sfntNodeOffset = newSarcSize;

    for (int i = 0; i < sortedFiles.size(); i++)  // SFNT nodes
    {
        InternalSarcFile* ifile = sortedFiles[i];
        ifile->nameOffset = (newSarcSize-sfntNodeOffset) / 4;
        newSarcSize += ifile->name.length() + 1;
        newSarcSize = align(newSarcSize, 4);
    }

    newSarcSize = align(newSarcSize, dataAlign);
    quint32 newDataOffset = newSarcSize;

    quint32 oldOffsets[numFiles];

    for (int i = 0; i < sortedFiles.size(); i++)  // data
    {
        InternalSarcFile* ifile = sortedFiles[i];
        oldOffsets[i] = ifile->offset;
        ifile->offset = newSarcSize - newDataOffset;
        newSarcSize += ifile->size;
        if (i < sortedFiles.size() - 1) newSarcSize = align(newSarcSize, dataAlign);
    }


    // buffer old SARC file data
    quint8* oldSarc = new quint8[sarc->size() - dataOffset];
    sarc->seek(dataOffset);
    sarc->readData(oldSarc, sarc->size() - dataOffset);

    sarc->resize(newSarcSize);
    sarc->seek(0);


    dataOffset = newDataOffset;


    // write SARC

    // Header
    sarc->write32(0x43524153);      // Magic
    sarc->write16(0x14);            // Header Length
    sarc->write16(0xFEFF);          // Byte order mark (little endian)
    sarc->write32(newSarcSize);     // SARC size
    sarc->write32(newDataOffset);   // Beginning of data
    sarc->write32(0x00000100);

    // SFAT Header
    sarc->write32(0x54414653);      // Magic
    sarc->write16(0xC);             // Header Length
    sarc->write16(numFiles);        // Node count
    sarc->write32(hashMult);        // Hash Multiplier

    // SFAT Nodes
    for (int i = 0; i < sortedFiles.size(); i++)
    {
        InternalSarcFile* ifile = sortedFiles[i];
        sarc->write32(filenameHash(ifile->name));   // File Name Hash
        sarc->write32(ifile->nameOffset);           // File name table entry
        sarc->seek(sarc->pos()-1);
        sarc->write8(0x1);                          // whatever this is
        sarc->write32(ifile->offset);               // Beginning of node file data
        sarc->write32(ifile->offset + ifile->size); // End of node file data
    }

    // SFNT Header
    sarc->write32(0x544E4653);      // Magic
    sarc->write16(0x8);             // Header Length
    sarc->write16(0x0);             // whatever this is

    // Filename Strings
    for (int i = 0; i < sortedFiles.size(); i++)
    {
        InternalSarcFile* ifile = sortedFiles[i];
        sarc->writeStringASCII(ifile->name);
        writeAlign(4);
    }

    // File data
    for (int i = 0; i < sortedFiles.size(); i++)
    {
        writeAlign(dataAlign);

        InternalSarcFile* ifile = sortedFiles[i];

        if (ifile->name == path)
        {
            quint8* filetempbuf = new quint8[file->size()];
            file->seek(0);
            file->readData(filetempbuf, file->size());
            sarc->writeData(filetempbuf, file->size());
            delete[] filetempbuf;

            continue;
        }

        sarc->writeData(oldSarc+oldOffsets[i], ifile->size);
    }

    delete[] oldSarc;

    file->close();
    sarc->save();
    sarc->close();

    return true;
}

void SarcFilesystem::repack()
{
    // oddly copy-pasted the code from the save method, but who cares :P

    sarc->open();

    numFiles = files.size();    // just to be sure

    // Generate sorted List, since SARCs want to be ordered after their File Name Hash
    QList<InternalSarcFile*> sortedFiles;
    for (int i = 0; i < files.size(); i++)
    {
        InternalSarcFile* ifile = files.values()[i];
        ifile->nameHash = filenameHash(ifile->name);
        sortedFiles.append(ifile);
    }
    qSort(sortedFiles.begin(), sortedFiles.end(), hashSort);


    // recreate SARC

    // get correct size
    quint32 newSarcSize = 0x28;             // SARC, SFAT and SFNT headers
    newSarcSize += numFiles*0x10;           // SFAT nodes

    quint32 sfntNodeOffset = newSarcSize;

    for (int i = 0; i < sortedFiles.size(); i++)  // SFNT nodes
    {
        InternalSarcFile* ifile = sortedFiles[i];
        ifile->nameOffset = (newSarcSize-sfntNodeOffset) / 4;
        newSarcSize += ifile->name.length() + 1;
        newSarcSize = align(newSarcSize, 4);
    }

    newSarcSize = align(newSarcSize, dataAlign);
    quint32 newDataOffset = newSarcSize;

    quint32 oldOffsets[numFiles];

    for (int i = 0; i < sortedFiles.size(); i++)  // data
    {
        InternalSarcFile* ifile = sortedFiles[i];
        oldOffsets[i] = ifile->offset;
        ifile->offset = newSarcSize - newDataOffset;
        newSarcSize += ifile->size;
        if (i < sortedFiles.size() - 1) newSarcSize = align(newSarcSize, dataAlign);
    }


    // buffer old SARC file data
    quint8* oldSarc = new quint8[sarc->size() - dataOffset];
    sarc->seek(dataOffset);
    sarc->readData(oldSarc, sarc->size() - dataOffset);

    sarc->resize(newSarcSize);
    sarc->seek(0);


    dataOffset = newDataOffset;


    // write SARC

    // Header
    sarc->write32(0x43524153);      // Magic
    sarc->write16(0x14);            // Header Length
    sarc->write16(0xFEFF);          // Byte order mark (little endian)
    sarc->write32(newSarcSize);     // SARC size
    sarc->write32(newDataOffset);   // Beginning of data
    sarc->write32(0x00000100);

    // SFAT Header
    sarc->write32(0x54414653);      // Magic
    sarc->write16(0xC);             // Header Length
    sarc->write16(numFiles);        // Node count
    sarc->write32(hashMult);        // Hash Multiplier

    // SFAT Nodes
    for (int i = 0; i < sortedFiles.size(); i++)
    {
        InternalSarcFile* ifile = sortedFiles[i];
        sarc->write32(filenameHash(ifile->name));   // File Name Hash
        sarc->write32(ifile->nameOffset);           // File name table entry
        sarc->seek(sarc->pos()-1);
        sarc->write8(0x1);                          // whatever this is
        sarc->write32(ifile->offset);               // Beginning of node file data
        sarc->write32(ifile->offset + ifile->size); // End of node file data
    }

    // SFNT Header
    sarc->write32(0x544E4653);      // Magic
    sarc->write16(0x8);             // Header Length
    sarc->write16(0x0);             // whatever this is

    // Filename Strings
    for (int i = 0; i < sortedFiles.size(); i++)
    {
        InternalSarcFile* ifile = sortedFiles[i];
        sarc->writeStringASCII(ifile->name);
        writeAlign(4);
    }

    // File data
    for (int i = 0; i < sortedFiles.size(); i++)
    {
        writeAlign(dataAlign);
        InternalSarcFile* ifile = sortedFiles[i];
        sarc->writeData(oldSarc+oldOffsets[i], ifile->size);
    }

    delete[] oldSarc;

    sarc->save();
    sarc->close();
}


bool SarcFilesystem::deleteFile(QString path)
{
    if (path[0] == '/')
        path.remove(0,1);

    if (!files.contains(path))
        return false;

    files.remove(path);

    repack();

    return true;
}

bool SarcFilesystem::renameFile(QString path, QString newName)
{
    if (path[0] == '/')
        path.remove(0,1);

    if (!files.contains(path))
        return false;

    InternalSarcFile* thisfile = files.value(path);

    if (thisfile == NULL)
        throw std::logic_error("thisfile is NULL, shouldn't happen");

    QStringList splitted = thisfile->name.split("/");
    splitted[splitted.length()-1] = newName;

    thisfile->name = "";

    for(int i = 0; i < splitted.length(); i++)
    {
        thisfile->name.append(splitted.at(i));
        if (i < splitted.length()-1) thisfile->name.append("/");
    }

    files.remove(path);
    files.insert(thisfile->name, thisfile);

    repack();

    return true;
}

bool SarcFilesystem::renameDir(QString path, QString newPath)
{
    if (path[0] == '/')
        path.remove(0,1);

    if (path.endsWith("/"))
        path.chop(1);

    if (newPath[0] == '/')
        newPath.remove(0,1);

    if (newPath.endsWith("/"))
        newPath.chop(1);

    foreach (InternalSarcFile* thisfile, files.values())
    {
        if (thisfile == NULL)
            throw std::logic_error("thisfile is NULL, shouldn't happen");

        if (thisfile->name.startsWith(path + "/"))
        {
            QString oldName = thisfile->name;
            files.remove(oldName);

            QString newName = oldName.replace(path, newPath);
            thisfile->name = newName;

            files.insert(thisfile->name, thisfile);
        }
    }

    repack();

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
