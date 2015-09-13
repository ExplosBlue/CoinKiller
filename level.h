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

class Game;


struct BgdatObject
{
    quint16 id;
    quint16 x, y; // [RoadrunnerWMC] signed position??? needs investigation
    quint16 width, height;

    // extra crap, but it's all zeroes
};


class Level
{
public:
    Level(Game* game, int world, int level, int area);
    ~Level();


    Game* game;

    Tileset* tilesets[4];

    QList<BgdatObject> objects[3];


private:
    SarcFilesystem* archive;
    int area;
};

#endif // LEVEL_H
