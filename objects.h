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

#ifndef OBJECTS_H
#define OBJECTS_H

#include <QtCore>
#include <QPainter>
#include <QList>

// Obj Baseclass
class Object
{
public:
    Object();
    virtual ~Object() {}
    void setPosition(qint32 x, qint32 y);
    void increasePosition(qint32 deltax, qint32 deltay, qint32 snap = 0);
    void resize(qint32 width, qint32 height);
    void increaseSize(qint32 deltax, qint32 deltay, qint32 snap = 0);
    virtual qint32 getType() const { return -1; }
    virtual bool isResizable() const { return false; }
    qint32 getx() const { return x; }
    qint32 gety() const { return y; }
    qint32 getwidth() const { return width; }
    qint32 getheight() const { return height; }
    qint32 getOffsetX() const { return offsetx; }
    qint32 getOffsetY() const { return offsety; }
    bool clickDetection(qint32 xcheck, qint32 ycheck);
    bool clickDetection(QRect rect);

    virtual QString toString(qint32 xOffset, qint32 yOffset) const;
    // First Number:
    // -1: Invalid
    // 0: BgdatObject
    // 1: Sprite
    // 2: Entrance
    // 3: Zone
    // 4: Location
    // 5: Path
    // 6: Progress Path

protected:
    qint32 x, y;
    qint32 width, height;
    qint32 offsetx, offsety;
    qint32 dragX, dragY;
    qint32 resizeX, resizeY;
};

// Bgdat Object
class BgdatObject: public Object
{
public:
    BgdatObject() {}
    BgdatObject(BgdatObject* obj);
    BgdatObject(qint32 x, qint32 y, qint32 width, qint32 height, qint32 id, qint32 layer);
    qint32 getType() const;
    bool isResizable() const { return true; }
    qint32 getid() const;
    void setTsID(qint32 tsID);
    void setObjID(qint32 objID);
    qint32 getLayer() const;
    QString toString(qint32 xOffset, qint32 yOffset) const;
    void setLayer(qint32 layer) { this->layer = layer; }
protected:
    qint32 id;
    qint32 layer;
};


// Sprite Object
class Sprite: public Object
{
public:
    Sprite() {}
    Sprite(Sprite* spr);
    Sprite(qint32 x, qint32 y, qint32 id);
    qint32 getType() const;
    bool isResizable() const { return false; }
    qint32 getid() const;
    void setid(qint32 id) { this->id = id; this->setRect(); }
    void setByte(qint32 id, quint8 nbr);
    void setNybble(qint32 id, quint8 nbr);
    quint8 getByte(qint32 id) const;
    quint8 getNybble(qint32 id) const;
    void setRect();
    QString toString(qint32 xOffset, qint32 yOffset) const;
    qint32 getNybbleData(qint32 startNybble, qint32 endNybble);
    void setNybbleData(qint32 data, qint32 startNybble, qint32 endNybble);
protected:
    qint32 id;
    quint8 spriteData[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
};


// Entrance Object
class Entrance: public Object
{
public:
    Entrance() {}
    Entrance(Entrance* entr);
    Entrance(qint32 x, qint32 y, qint16 cameraX, qint16 cameraY, quint8 id, quint8 destArea, quint8 destEntr, quint8 entrType, quint8 settings, quint8 unk1, quint8 unk2);
    qint32 getType() const { return 2; }
    bool isResizable() const { return false; }
    QString toString(qint32 xOffset, qint32 yOffset) const;
    quint8 getid() const { return id; }
    quint8 getDestArea() const { return destArea; }
    quint8 getDestEntr() const { return destEntr; }
    quint8 getEntrType() const { return entrType; }
    quint8 getSettings() const { return settings; }
    quint16 getCameraX() const { return cameraX; }
    quint16 getCameraY() const { return cameraY; }
    quint8 getUnk1() const { return unk1; }
    quint8 getUnk2() const { return unk2; }
    void setId(quint8 id) { this->id = id; }
    void setEntrType(quint8 entrType) { this->entrType = entrType; }
    void setDestEntr(quint8 destEntr) { this->destEntr = destEntr; }
    void setDestArea(quint8 destArea) { this->destArea = destArea; }
    void setCameraX(qint16 cameraX) { this->cameraX = cameraX; }
    void setCameraY(qint16 cameraY) { this->cameraY = cameraY; }
    void setSettings(quint8 settings) { this->settings = settings; }
    void setSettingsBit(bool value, qint32 bit) { settings ^= (-(qint32)value ^ settings) & (1 << bit); }
protected:
    qint16 cameraX;
    qint16 cameraY;
    quint8 id;
    quint8 destArea;
    quint8 destEntr;
    quint8 entrType;
    quint8 settings;
    quint8 unk1;
    quint8 unk2;
};


// Zone Object

// Structs used while parsing
struct tempZoneBounding
{
    quint16 id;
    qint32 upperBound;
    qint32 lowerBound;
    qint32 unkUpperBound;
    qint32 unkLowerBound;
    quint16 upScrolling;
};

struct tempZoneBackground
{
    quint16 id;
    quint8 xScrollRate;
    quint8 yScrollRate;
    quint8 xPos;
    quint8 yPos;
    QString name;
    quint8 unk1;
};

class Zone: public Object
{
public:
    Zone() {}
    Zone(Zone* zone);
    Zone(qint32 x, qint32 y, qint32 width, qint32 height, quint8 id, quint8 progPathId, quint8 musicId, quint8 multiplayerTracking, quint16 unk1);
    bool clickDetection(qint32 xcheck, qint32 ycheck);
    bool clickDetection(QRect rect);
    void setBounding(tempZoneBounding bounding);
    void setBackground(tempZoneBackground background);
    qint32 getType() const { return 3; }
    bool isResizable() const { return true; }
    quint8 getid() const { return id; }
    quint8 getProgPathId() const { return progPathId; }
    quint8 getMusicId() const { return musicId; }
    quint8 getMultiplayerTracking() const { return multiplayerTracking; }
    quint16 getUnk1() const { return unk1; }
    qint32 getUpperBound() const { return upperBound; }
    qint32 getLowerBound() const { return lowerBound; }
    qint32 getUnkUpperBound() const { return unkUpperBound; }
    qint32 getUnkLowerBound() const { return unkLowerBound; }
    quint32 getUpScrolling() const { return upScrolling; }
    quint8 getXScrollRate() const { return xScrollRate; }
    quint8 getYScrollRate() const { return yScrollRate; }
    quint8 getBgXPos() const { return bgXPos; }
    quint8 getBgYPos() const { return bgYPos; }
    QString getBgName() const { return bgName; }
    quint8 getBgUnk1() const { return bgUnk1; }
    void setID(quint8 id) { this->id = id; }
    void setProgPathId(quint8 progPathId) { this->progPathId = progPathId; }
    void setMusicID(quint8 musicId) { this->musicId = musicId; }
    void setMultiplayerTracking(quint8 multiplayerTracking) { this->multiplayerTracking = multiplayerTracking; }
    void setBackgroundName(QString bgName) { this->bgName = bgName; }
    void setUpScrolling(quint16 upScrolling) { this->upScrolling = upScrolling; }
    void setUpperBound(qint32 upperBound) { this->upperBound = upperBound; }
    void setLowerBound(qint32 lowerBound) { this->lowerBound = lowerBound; }
    void setUnkUpperBound(qint32 unkUpperBound) { this->unkUpperBound = unkUpperBound; }
    void setUnkLowerBound(qint32 unkLowerBound) { this->unkLowerBound = unkLowerBound; }
protected:
    quint8 id;
    quint8 progPathId;
    quint8 musicId;
    quint8 multiplayerTracking;
    quint16 unk1;
    // Boundings
    qint32 upperBound = 0;
    qint32 lowerBound = 0;
    qint32 unkUpperBound = 0;
    qint32 unkLowerBound = 0;
    quint16 upScrolling = 0;
    // Background
    quint8 xScrollRate = 0;
    quint8 yScrollRate = 0;
    quint8 bgXPos = 0;
    quint8 bgYPos = 0;
    QString bgName = "Nohara";
    quint8 bgUnk1 = 0;
};


// Location Object
class Location: public Object
{
public:
    Location() {}
    Location(Location* loc);
    Location(qint32 x, qint32 y, qint32 width, qint32 height, qint32 id);
    qint32 getType() const;
    bool isResizable() const { return true; }
    qint32 getid() const;
    void setId(quint8 id) { this->id = id; }
    QString toString(qint32 xOffset, qint32 yOffset) const;
protected:
    qint32 id;
};


// Path Node
class Path;
class PathNode: public Object
{
public:
    PathNode() {}
    PathNode(PathNode* node, Path* parentPath);
    PathNode(qint32 x, qint32 y, float speed, float accel, quint32 unk1, Path *parentPath);
    qint32 getType() const { return 5; }
    bool isResizable() const { return false; }
    float getSpeed() const { return speed; }
    float getAccel() const { return accel; }
    float getUnk1() const { return unk1; }
    Path* getParentPath() const { return parentPath; }
    void setSpeed(float speed) { this->speed = speed; }
    void setAccel(float accel) { this->accel = accel; }
protected:
    Path* parentPath;
    float speed;
    float accel;
    quint32 unk1;
};


// Path
class Path
{
public:
    Path() {}
    Path(Path* path);
    ~Path() { foreach (PathNode* node, nodes) delete node; }
    Path(quint16 id, quint16 unk1);
    void insertNode(PathNode* node, qint32 index = -1);
    void removeNode(PathNode* node);
    quint16 getid() const { return id; }
    quint16 getUnk1() const { return unk1; }
    qint32 getNumberOfNodes() const { return nodes.size(); }
    QList<PathNode*> getNodes() const;
    PathNode* getNode(qint32 id) const { return nodes[id]; }
    qint32 getIndexOfNode(PathNode* node) { return nodes.indexOf(node); }
    void setId(qint32 id) { this->id = id; }
protected:
    quint16 id;
    quint16 unk1;
    QList<PathNode*> nodes;
};


// Progress Path Node
class ProgressPath;
class ProgressPathNode: public Object
{
public:
    ProgressPathNode() {}
    ProgressPathNode(ProgressPathNode* node, ProgressPath* parentPath);
    ProgressPathNode(qint32 x, qint32 y, ProgressPath* parentPath);
    qint32 getType() const { return 6; }
    bool isResizable() const { return false; }
    ProgressPath* getParentPath() const { return parentPath; }
private:
    ProgressPath* parentPath;
};


// Progress Path
class ProgressPath
{
public:
    ProgressPath() {}
    ProgressPath(ProgressPath* path);
    ~ProgressPath() { foreach (ProgressPathNode* node, nodes) delete node; }
    ProgressPath(quint16 id, quint8 alternatePathFlag);
    void insertNode(ProgressPathNode* node, qint32 index = -1);
    void removeNode(ProgressPathNode* node);
    quint16 getid() const { return id; }
    quint8 getAlternatePathFlag() const { return alternatePathFlag; }
    qint32 getNumberOfNodes() const { return nodes.size(); }
    QList<ProgressPathNode*> getNodes() const;
    ProgressPathNode* getNode(qint32 id) const { return nodes[id]; }
    qint32 getIndexOfNode(ProgressPathNode* node) { return nodes.indexOf(node); }
    void setId(qint32 id) { this->id = id; }
    void setAlternatePathFlag(bool flag) { this->alternatePathFlag = flag; }
protected:
    quint16 id;
    quint8 alternatePathFlag;
    QList<ProgressPathNode*> nodes;
};

#endif // OBJECTS_H
