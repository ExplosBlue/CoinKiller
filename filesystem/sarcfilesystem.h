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

#ifndef SARCFILESYSTEM_H
#define SARCFILESYSTEM_H

#include "filebase.h"
#include <QDebug>

class SarcFilesystem : public FilesystemBase
{
public:
    SarcFilesystem(FileBase* file);
    ~SarcFilesystem();

    bool directoryExists(QString path);
    void directoryContents(QString path, QDir::Filter filter, QList<QString>& out);

    bool fileExists(QString path);
    FileBase* openFile(QString path);
    bool save(FileBase* file);
    bool deleteFile(QString path);
    bool renameFile(QString path, QString newName);
    bool renameDir(QString path, QString newName);
    bool changeFileDir(QString path, QString newName);

    bool copyFile(QString path, QString newPath) { Q_UNUSED(path); Q_UNUSED(newPath); return false; } // TODO

private:
    FileBase* sarc;

    struct InternalSarcFile
    {
        QString name;
        quint32 offset, size;

        quint32 nameOffset;
        quint32 nameHash;

        quint32 entryOffset;
    };

    quint32 numFiles;
    quint32 hashMult;

    quint32 sfatOffset;
    quint32 sfntOffset;
    quint32 dataOffset;

    QHash<QString,InternalSarcFile*> files;
    void repack();


    quint32 filenameHash(QString &name);

    const quint8 dataAlign = 0x10;

    quint32 align(quint32 v, quint32 a)
    {
        return (v + a - 1) / a * a;
    }

    void writeAlign(quint32 a)
    {
        quint8 padSize = align(sarc->pos(), a) - sarc->pos();
        for (quint8 i=0; i < padSize; i++) sarc->write8(0x00);
    }

    static bool hashSort(InternalSarcFile* f1, InternalSarcFile* f2)
    {
        return f1->nameHash < f2->nameHash;
    }
};

#endif // SARCFILESYSTEM_H
