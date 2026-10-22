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

#include "filesystem/filesystem.h"
#include "ctpk.h"

#include <QPainter>
#include <QList>

typedef QHash<quint32,quint8> TileGrid;

class Game;

enum SlopeFlag : quint8
{
    SlopeNone           = 0x00,

    SlopeUpSide         = 0x80,
    SlopeDownSide       = 0x81,
    SlopeUpsideDownSide = 0x82,
    SlopeUpsideUpSide   = 0x83,
    SlopeSection1       = 0x84,
    SlopeSection2       = 0x85,

    SlopeUpBase         = 0x90,
    SlopeDownBase       = 0x91,
    SlopeUpsideDownBase = 0x92,
    SlopeUpsideUpBase   = 0x93,
};

enum SlopeDirection : quint8
{
    SlopeDirectionUp         = 0,
    SlopeDirectionDown       = 1,
    SlopeDirectionUpsideDown = 2,
    SlopeDirectionUpsideUp   = 3,
};

static constexpr quint8 SlopeDirectionMask = 0x0F;

struct ObjectRow
{
    quint8 slopeFlags = 0;
    quint8 xRepeatStart = 0xFF, xRepeatEnd = 0;

    QList<quint8> data;
};

struct ObjectDef
{
    quint8 width, height;
    quint8 randomisation;
    quint8 unkFlag; // whatever this is

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
    void drawObject(QPainter& painter, TileGrid& grid, int num, int x, int y, int w, int h, float zoom, const QRect& clip = QRect());
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
    void setInternalName(QString name);

    quint16 getOverlayTile(int selTile);
    quint16 getOverlayTileSlot(int selTile);
    void setOverlayTile(int selTile, int ovTile, int slot);
    bool getRandomizeH(int tile);
    void setRandomizeH(int tile, bool value);
    bool getRandomizeV(int tile);
    void setRandomizeV(int tile, bool value);
    quint8 getRandomizeTiles(int tile);
    void setRandomizeTiles(int tile, quint8 value);
    void setObjectBehavior(int selObj, int type, int hStart, int hEnd, int vStart, int vEnd);

    void Render2DTiles(bool toggle) { draw2D = toggle; }
    void Render3DOverlay(bool toggle) { draw3D = toggle; }
    void clearScaledTiles();

    Game* game;

    QImage& getImage();
    void setImage(QImage& img, uint quality = 1, bool dither = false);
    void setImageRegion(QImage& sheet, const QList<int>& tiles, uint quality = 1, bool dither = false);

    void save();

    static QImage padTilesetImage(const QImage& img, quint32 outWidth = 512, quint32 outHeight = 512);

private:
    bool getObjectCell(int objNbr, int x, int y, int& row, int& col);

    QString name;
    bool drawOverrides = false;
    int slot = 0;
    SarcFilesystem* archive;
    Ctpk* ctpk;

    QImage texImage;
    QPixmap texPixmap;

    QHash<quint32, QPixmap> scaledTiles;
    qreal scaledTilesScale = 1.0;
    const QPixmap& getScaledTile(int num, qreal scale);


    QList<ObjectDef*> objectDefs;


    void drawTile(QPainter& painter, TileGrid& grid, int num, int x, int y, float zoom, int item, quint8 gridLayer);
    void drawRow(QPainter& painter, TileGrid& grid, const ObjectRow& row, int x, int y, int w, float zoom, quint8 gridLayer, const QRect& clip);


    quint8 behaviors[441][8];
    quint16 overlays3D[441];

    bool draw3D;
    bool draw2D;
};

struct TilesetEntry
{
    QString fileName;
    QString name;
};

struct TilesetSubgroup
{
    QString name;
    QList<TilesetEntry> tilesets;
};

struct TilesetGroup
{
    QString name;
    QList<TilesetEntry> tilesets;
    QList<TilesetSubgroup> subgroups;

    QList<TilesetEntry> allTilesets() const;
    TilesetEntry* findTileset(const QString& fileName);
};

class TilesetList
{
public:
    TilesetList(FilesystemBase* fs);

    QList<TilesetGroup> groups() const;

    static bool matchesSlot(const QString& fileName, int slot);
    static int slotForFileName(const QString& fileName);
    static QString stripExtension(const QString& fileName);

    QString displayName(const QString& fileName) const;
    QString groupName(const QString& fileName) const;

private:
    FilesystemBase* fs;
};

#endif // TILESET_H
