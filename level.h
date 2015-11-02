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

#ifndef LEVEL_H
#define LEVEL_H

#include "filesystem.h"
#include "tileset.h"
#include "objects.h"

class Game;

class Level
{
public:
    Level(Game* game, int world, int level, int area);
    ~Level();
    void save();

    Game* game;

    Tileset* tilesets[4];

    quint16 timeLimit;

    QList<BgdatObject> objects[2];
    QList<Sprite> sprites;
    QList<Entrance> entrances;
    QList<Zone> zones;
    QList<Location> locations;
    QList<Path> paths;
    QList<ProgressPath> progressPaths;

    void getName(QString& name)
    {
        name = QString("%1-%2 area %3").arg(world).arg(level).arg(area);
    }

private:
    SarcFilesystem* archive;
    int world, level, area;
};

#endif // LEVEL_H
