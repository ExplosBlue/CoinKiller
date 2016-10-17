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

#ifndef FILESYSTEMBASE_H
#define FILESYSTEMBASE_H


class FilesystemBase
{
public:
    virtual ~FilesystemBase() {}

    virtual bool directoryExists(QString path)=0;
    virtual void directoryContents(QString path, QDir::Filter filter, QList<QString>& out)=0;

    virtual bool fileExists(QString path)=0;
    virtual FileBase* openFile(QString path)=0;
    virtual bool save(FileBase* file)=0;
    virtual bool deleteFile(QString path)=0;
    virtual bool renameFile(QString path, QString newName)=0;
    virtual bool renameDir(QString patn, QString newName)=0;
};


#endif // FILESYSTEMBASE_H

