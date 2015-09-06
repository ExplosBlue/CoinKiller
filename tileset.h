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

#ifndef TILESET_H
#define TILESET_H

#include "filesystem.h"
#include "ctpk.h"

#include <QPainter>

class Game;


class Tileset
{
public:
    Tileset(Game* game, QString name);
    ~Tileset();


    void drawTile(QPainter& painter, int num, int x, int y, float zoom);


    Game* game;


private:
    SarcFilesystem* archive;
    Ctpk* texture;

    QImage* texImage;
};

#endif // TILESET_H
