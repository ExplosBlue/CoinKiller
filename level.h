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
    Level(Game *game, SarcFilesystem* archive, int area, QString lvlName);
    ~Level();
    void save();

    Game* game;

    Tileset* tilesets[4];

    // Area Settings
    quint16 timeLimit;
    quint8 toadHouseFlag;
    quint16 specialLevelFlag;
    quint16 specialLevelFlag2;
    quint16 unk1;
    quint8 unk2;
    quint16 coinRushTimeLimit;

    QList<BgdatObject*> objects[2];
    QList<Sprite*> sprites;
    QList<Entrance*> entrances;
    QList<Location*> locations;
    QList<Zone*> zones;
    QList<Path*> paths;
    QList<ProgressPath*> progressPaths;

    void getName(QString& name)
    {
        name = QString("%1, Area %2").arg(lvlName).arg(area);
    }

    void add(QList<Object*> objs);
    void add(Object* obj);
    void remove(QList<Object*> objs);
    void remove(Object* obj);
    void move(QList<Object*> objs, int deltax, int deltay);
    void raise(Object *obj);
    void lower(Object* obj);
    void raiseLayer(BgdatObject *obj);
    void lowerLayer(BgdatObject* obj);

    Entrance* newEntrance(int x, int y);
    Zone* newZone(int x, int y);
    Location* newLocation(int x, int y);
    Path* newPath();
    ProgressPath* newProgressPath();

    int getAreaID() { return area; }
    void setAreaID(int id) { this->area = id; }

private:
    SarcFilesystem* archive;
    QString lvlName;
    int area;

    quint8 getNextZoneID(Object* obj);
};

#endif // LEVEL_H
