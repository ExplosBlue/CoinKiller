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

ExternalFile::ExternalFile(FilesystemBase *fs, QString path)
{
    parent = fs;
    file = new QFile(path);

    openCount = 0;
}

ExternalFile::ExternalFile(FilesystemBase* fs)
{
    parent = fs;
    file = new QTemporaryFile();

    openCount = 0;
}

ExternalFile::~ExternalFile()
{
    delete file;
}


void ExternalFile::open()
{
    if (openCount == 0)
        file->open(QIODevice::ReadWrite);

    openCount++;
}

void ExternalFile::close()
{
    openCount--;
    if (openCount == 0)
    {
        file->close();
        parent->save(this); // we never know
    }

    if (openCount < 0)
        throw std::logic_error("MemoryFile: openCount<0");
}

quint64 ExternalFile::readData(quint8* data, quint64 len)
{
    return file->read((char*)data, len);
}

quint64 ExternalFile::writeData(quint8* data, quint64 len)
{
    return file->write((const char*)data, len);
}

quint64 ExternalFile::pos()
{
    return file->pos();
}

bool ExternalFile::seek(quint64 pos)
{
    return file->seek(pos);
}

quint64 ExternalFile::size()
{
    return file->size();
}

bool ExternalFile::resize(quint64 size)
{
    return file->resize(size);
}

