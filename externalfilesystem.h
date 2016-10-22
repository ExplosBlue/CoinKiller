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

#ifndef EXTERNALFILESYSTEM_H
#define EXTERNALFILESYSTEM_H


class ExternalFilesystem : public FilesystemBase
{
public:
    ExternalFilesystem(QString base);
    ~ExternalFilesystem();

    bool directoryExists(QString path);
    void directoryContents(QString path, QDir::Filter filter, QList<QString>& out);

    bool fileExists(QString path);
    FileBase* openFile(QString path);
    bool save(FileBase* file);
    bool deleteFile(QString path);
    bool renameFile(QString path, QString newName);
    bool renameDir(QString path, QString newName);

    bool copyFile(QString path, QString newPath);

private:
    QString basepath;
};

#endif // EXTERNALFILESYSTEM_H
