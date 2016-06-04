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

typedef QHash<quint32,quint8> TileGrid;

class Game;

struct ObjectRow
{
    quint8 slopeFlags;
    quint8 xRepeatStart, xRepeatEnd;

    QList<quint8> data;
};

struct ObjectDef
{
    quint8 width, height;
    quint8 randomisation;
    quint8 unkFlag; // whatever this is

    quint8 slopeY; // where the second slope block starts
    quint8 yRepeatStart, yRepeatEnd;

    QList<ObjectRow> rows;
};


class Tileset
{
public:
    Tileset(Game* game, QString name);
    ~Tileset();

    QString getName() const { return name; }
    int getSlot() { return slot; }

    void drawTile(QPainter& painter, TileGrid& grid, int num, int x, int y, float zoom, int item);
    void drawObject(QPainter& painter, TileGrid& grid, int num, int x, int y, int w, int h, float zoom);
    quint8 getBehaviorByte(int tile, int byte);
    void setBehaviorByte(int tile, int byte, quint8 value);

    int getNumObjects() { return objectDefs.size(); }
    ObjectDef* getObjectDef(int i) { return objectDefs[i]; }
    quint8 getData(int objNbr, int x, int y, int byte);
    void setData(int objNbr, int x, int y, int byte, int value);

    void addObject(int objNbr);
    void removeObject(int objNbr);
    void moveObjectUp(int objNbr);
    void moveObjectDown(int objNbr);
    void resizeObject(int objNubr, int width, int height);
    void setSlot(int slot);

    quint16 getOverlayTile(int selTile);
    void setOverlayTile(int selTile, int ovTile, int slot);
    bool getRandomizeH(int tile);
    void setRandomizeH(int tile, bool value);
    bool getRandomizeV(int tile);
    void setRandomizeV(int tile, bool value);
    quint8 getRandomizeTiles(int tile);
    void setRandomizeTiles(int tile, quint8 value);
    void setObjectBehavior(int selObj, int type, int hStart, int hEnd, int vStart, int vEnd);

    Game* game;

    QImage* getImage()
    {
        return texImage;
    }

    void save();

private:
    QString name;
    bool drawOverrides = false;
    int slot;
    SarcFilesystem* archive;
    Ctpk* texture;

    QImage* texImage;


    QList<ObjectDef*> objectDefs;


    void drawRow(QPainter& painter, TileGrid& grid, ObjectDef& def, ObjectRow& row, int x, int y, int w, float zoom);


    quint8 behaviors[441][8];
    quint16 overlays3D[441];
};

#endif // TILESET_H
