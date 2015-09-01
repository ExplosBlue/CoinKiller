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


FileBase* FileBase::getSubfile(FilesystemBase* container, quint64 offset, quint64 size)
{
    open();
    seek(offset);
    FileBase* ret;

    if (size >= 32*1024*1024)
    {
        // backed by temporary file
        ret = new ExternalFile(container);
        ret->open();
        ret->resize(size);
        ret->seek(0);

        quint8* tempbuf = new quint8[4096];
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
        delete[] tempbuf;
    }
    else
    {
        // backed by memory
        quint8* data = new quint8[size];
        readData(data, size);
        ret = new MemoryFile(container, data, size);
    }

    close();
    return ret;
}
