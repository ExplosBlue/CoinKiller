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

ExternalFilesystem::ExternalFilesystem(QString base)
{
    basepath = base;
}

ExternalFilesystem::~ExternalFilesystem()
{
}


bool ExternalFilesystem::directoryExists(QString path)
{
    return QDir(basepath + path).exists();
}

void ExternalFilesystem::directoryContents(QString path, QDir::Filter filter, QList<QString>& out)
{
    out.clear();

    QStringList res = QDir(basepath + path).entryList(filter|QDir::NoDotAndDotDot, QDir::Name|QDir::IgnoreCase|QDir::DirsFirst);
    for (int i = 0; i < res.size(); i++)
    {
        out.append(res[i]);
    }
}


bool ExternalFilesystem::fileExists(QString path)
{
    return QFile(basepath + path).exists();
}

// path be like: /game/course/level.bin
// (completely fictive filename invented for the sake of this example)
FileBase* ExternalFilesystem::openFile(QString path)
{
    //QFile file(path);
    //if (!file.exists()) throw new std::runtime_exception("File doesn't exist");
    return new ExternalFile(this, basepath + path);
}

bool ExternalFilesystem::save(FileBase *file)
{
    // no real need to do shit here!
    return true;
}

