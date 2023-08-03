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

MemoryFile::MemoryFile(FilesystemBase* fs, quint8* blob, quint32 size)
{
    this->parent = fs;
    this->data = blob;
    this->_size = size;
    this->_pos = 0;

    openCount = 0;
}

MemoryFile::MemoryFile(FilesystemBase *fs, quint32 size)
{
    this->parent = fs;
    this->data = new quint8[size];
    memset(data, 0, size);
    this->_size = size;
    this->_pos = 0;

    openCount = 0;
}

MemoryFile::~MemoryFile()
{
    delete[] data;
}


void MemoryFile::open()
{
    openCount++;
}

void MemoryFile::save()
{
    if (parent) parent->save(this);
}

void MemoryFile::close()
{
    openCount--;
    if (openCount == 0)
    {
        //if (parent) parent->save(this);
    }

    if (openCount < 0)
        throw std::logic_error("MemoryFile: openCount<0");
}

quint64 MemoryFile::readData(quint8* data, quint64 len)
{
    // prevent out-of-range read
    if ((_pos+len) > _size)
        len = _size-_pos;

    if (len < 1 || len > 0xFFFFFFFF)
        return 0;

    switch (len)
    {
    case 1: *data = this->data[_pos]; break;
    case 2: *(quint16*)data = *(quint16*)&this->data[_pos]; break;
    case 4: *(quint32*)data = *(quint32*)&this->data[_pos]; break;
    case 8: *(quint64*)data = *(quint64*)&this->data[_pos]; break;
    default: memcpy(data, &this->data[_pos], len); break;
    }

    _pos += len;
    return len;
}

quint64 MemoryFile::writeData(quint8* data, quint64 len)
{
    // resize the file if needed
    // (it is still more efficient to resize the file prior to writing)
    if ((_pos+len) > _size)
        resize(_pos+len);
        //len = _size-_pos;

    if (len < 1 || len > 0xFFFFFFFF)
        return 0;

    switch (len)
    {
    case 1: this->data[_pos] = *data; break;
    case 2: *(quint16*)&this->data[_pos] = *(quint16*)data; break;
    case 4: *(quint32*)&this->data[_pos] = *(quint32*)data; break;
    case 8: *(quint64*)&this->data[_pos] = *(quint64*)data; break;
    default: memcpy(&this->data[_pos], data, len); break;
    }

    _pos += len;
    return len;
}

quint64 MemoryFile::pos()
{
    return _pos;
}

bool MemoryFile::seek(quint64 pos)
{
    this->_pos = pos;
    return true;
}

quint64 MemoryFile::size()
{
    return _size;
}

bool MemoryFile::resize(quint64 size)
{
    if (size == 0)
    {
        delete[] this->data;
        this->data = NULL;
        this->_size = 0;
    }

    if (size < 1 || size > 0xFFFFFFFF)
        return false;

    quint8* newdata = new quint8[size];
    if (size >= this->_size) memcpy(newdata, this->data, this->_size);
    else memcpy(newdata, this->data, size);
    delete[] this->data;
    this->data = newdata;
    this->_size = size;

    return true;
}
