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
#include <QList>

class Game;


class Tileset
{
public:
    Tileset(Game* game, QString name);
    ~Tileset();


    void drawTile(QPainter& painter, int num, int x, int y, float zoom);
    void drawObject(QPainter& painter, int num, int x, int y, int w, int h, float zoom);


    Game* game;


private:
    SarcFilesystem* archive;
    Ctpk* texture;

    QImage* texImage;


    struct ObjectRow
    {
        quint8 slopeFlags;
        quint8 xRepeatStart, xRepeatEnd;

        QList<quint8> data;
    };

    struct ObjectDef
    {
        quint8 width, height;
        quint8 flags1, flags2; // whatever those are

        quint8 yRepeatStart, yRepeatEnd;

        QList<ObjectRow> rows;
    };

    int numObjects;
    ObjectDef* objectDef;


    void drawRow(QPainter& painter, ObjectDef& def, ObjectRow& row, int x, int y, int w, float zoom);
};

#endif // TILESET_H
