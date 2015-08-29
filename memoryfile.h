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

#ifndef MEMORYFILE_H
#define MEMORYFILE_H


class MemoryFile : public FileBase
{
public:
    MemoryFile(FilesystemBase *fs, quint8* blob, quint32 size);
    ~MemoryFile();

    void open();
    void close();
    quint64 readData(quint8* data, quint64 len);
    quint64 writeData(quint8* data, quint64 len);
    quint64 pos();
    bool seek(quint64 pos);
    quint64 size();
    bool resize(quint64 size);


private:
    quint8* data;
    quint32 _size;

    quint32 _pos;
};

#endif // MEMORYFILE_H
