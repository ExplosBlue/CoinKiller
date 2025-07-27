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

enum ObjectType
{
    INVALID,
    BGDATOBJECT,
    SPRITE,
    ENTRANCE,
    ZONE,
    LOCATION,
    PATHNODE,
    PROGRESSPATHNODE
};

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
    virtual ObjectType getType() const { return ObjectType::INVALID; }
    virtual bool isResizable() const { return false; }
    virtual bool doRender(QRect r) { return r.intersects(QRect(x + offsetx, y + offsety, width, height)); }
    qint32 getx() const { return x; }
    qint32 gety() const { return y; }
    qint32 getwidth() const { return width; }
    qint32 getheight() const { return height; }
    qint32 getOffsetX() const { return offsetx; }
    qint32 getOffsetY() const { return offsety; }
    bool clickDetection(qint32 xcheck, qint32 ycheck);
    bool clickDetection(QRect rect);

    virtual QString toString(qint32 xOffset, qint32 yOffset) const;

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
    ObjectType getType() const { return ObjectType::BGDATOBJECT; }
    bool isResizable() const { return true; }
    qint32 getid() const;
    void setTsID(qint32 tsID);
    qint32 getTsID() const;
    void setObjID(qint32 objID);
    qint32 getObjID() const;
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
    ~Sprite() { delete selectionRects; }
    Sprite(Sprite* spr);
    Sprite(qint32 x, qint32 y, qint16 id);
    ObjectType getType() const { return ObjectType::SPRITE; }
    bool isResizable() const { return false; }
    virtual bool doRender(QRect r) { return r.intersects(QRect(x + offsetx + renderOffsetX, y + offsety + renderOffsetY, width + renderOffsetW, height + renderOffsetH)); }
    qint16 getid() const;
    void setid(qint16 id) { this->id = id; this->setRect(); }
    void setByte(qint32 id, quint8 nbr);
    void setNybble(qint32 id, quint8 nbr);
    quint8 getByte(qint32 id) const;
    quint8 getNybble(qint32 id) const;
    void setRect();
    QString toString(qint32 xOffset, qint32 yOffset) const;
    qint32 getNybbleData(qint32 startNybble, qint32 endNybble) const;
    void setNybbleData(qint32 data, qint32 startNybble, qint32 endNybble);
    quint8 getLayer() { return this->layer; }
    void setLayer(quint8 layer) { this->layer = layer; }

    qint32 getBits(qint32 startBit, qint32 endBit) const;
    void setBits(qint32 data, qint32 startBit, qint32 endBit);

    bool clickDetection(qint32 xcheck, qint32 ycheck);
    bool clickDetection(QRect rect);

    QList<QRect>* getSelectionRects() const { return selectionRects; }

protected:
    qint16 id;
    quint8 spriteData[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    qint32 renderOffsetX;
    qint32 renderOffsetY;
    qint32 renderOffsetW;
    qint32 renderOffsetH;
    quint8 layer = 0;

    QList<QRect>* selectionRects;
};


// Entrance Object
class Entrance: public Object
{
public:
    Entrance() {}
    Entrance(Entrance* entr);
    Entrance(qint32 x, qint32 y, qint16 cameraX, qint16 cameraY, quint8 id, quint8 destArea, quint8 destEntr, quint8 entrType, quint16 settings, quint8 unk1, quint8 unk2);
    ObjectType getType() const { return ObjectType::ENTRANCE; }
    bool isResizable() const { return false; }
    QString toString(qint32 xOffset, qint32 yOffset) const;
    quint8 getid() const { return id; }
    quint8 getDestArea() const { return destArea; }
    quint8 getDestEntr() const { return destEntr; }
    quint8 getEntrType() const { return entrType; }
    quint16 getSettings() const { return settings; }
    qint16 getCameraX() const { return cameraX; }
    qint16 getCameraY() const { return cameraY; }
    quint8 getUnk1() const { return unk1; }
    quint8 getUnk2() const { return unk2; }
    void setId(quint8 id) { this->id = id; }
    void setEntrType(quint8 entrType) { this->entrType = entrType; this->setRect(); }
    void setDestEntr(quint8 destEntr) { this->destEntr = destEntr; }
    void setDestArea(quint8 destArea) { this->destArea = destArea; }
    void setCameraX(qint16 cameraX) { this->cameraX = cameraX; }
    void setCameraY(qint16 cameraY) { this->cameraY = cameraY; }
    void setSettings(quint16 settings) { this->settings = settings; }
    void setSettingsBit(bool value, qint32 bit) { settings ^= (-(qint32)value ^ settings) & (1 << bit); }
    void setUnk1(quint8 unk1) { this->unk1 = unk1; }
    void setUnk2(quint8 unk2) { this->unk2 = unk2; }
    void setRect();
protected:
    qint16 cameraX;
    qint16 cameraY;
    quint8 id;
    quint8 destArea;
    quint8 destEntr;
    quint8 entrType;
    quint16 settings;
    quint8 unk1;
    quint8 unk2;
};


// Zone Object

class ZoneBounding
{
public:
    ZoneBounding() {}
    ZoneBounding(quint16 id, quint32 primaryUpperBound, qint32 primaryLowerBound, qint32 secondaryUpperBound, qint32 secondaryLowerBound, quint16 upScrolling);
    quint16 getId() const { return id; }
    qint32 getPrimaryUpperBound() const { return primaryUpperBound; }
    qint32 getPrimaryLowerBound() const { return primaryLowerBound; }
    qint32 getSecondaryUpperBound() const { return secondaryUpperBound; }
    qint32 getSecondaryLowerBound() const { return secondaryLowerBound; }
    quint32 getUpScrolling() const { return upScrolling; }
    void setId(quint16 id) { this->id = id; }
    void setPrimaryUpperBound(qint32 primaryUpperBound) { this->primaryUpperBound = primaryUpperBound; }
    void setPrimaryLowerBound(qint32 primaryLowerBound) { this->primaryLowerBound = primaryLowerBound; }
    void setSecondaryUpperBound(qint32 secondaryUpperBound) { this->secondaryUpperBound = secondaryUpperBound; }
    void setSecondaryLowerBound(qint32 secondaryLowerBound) { this->secondaryLowerBound = secondaryLowerBound; }
    void setUpScrolling(quint16 upScrolling) { this->upScrolling = upScrolling; }

protected:
    quint16 id = 0;
    qint32 primaryUpperBound = 0;
    qint32 primaryLowerBound = 0;
    qint32 secondaryUpperBound = 0;
    qint32 secondaryLowerBound = 0;
    quint16 upScrolling = 0;
};

class ZoneBackground
{
public:
    ZoneBackground() {}
    ZoneBackground(quint16 id, qint16 yPos, qint16 xPos, QString name, quint8 parallaxMode);
    quint16 getId() const { return id; }
    qint16 getYPos() const { return yPos; }
    qint16 getXPos() const { return xPos; }
    QString getName() const { return name; }
    quint8 getParallaxMode() const { return parallaxMode; }
    void setId(quint16 id) { this->id = id; }
    void setYPos(qint16 yPos) { this->yPos = yPos; }
    void setXPos(qint16 xPos) { this->xPos = xPos; }
    void setName(QString name) { this->name = name; }
    void setParallaxMode(quint8 parallaxMode) { this->parallaxMode = parallaxMode; }

protected:
    quint16 id = 0;
    qint16 yPos = 0;
    qint16 xPos = 0;
    QString name = "Nohara";
    quint8 parallaxMode = 0;
};

class Zone: public Object
{
public:
    Zone() {}
    Zone(Zone* zone);
    Zone(qint32 x, qint32 y, qint32 width, qint32 height, quint8 id, quint8 progPathId, quint8 musicId, quint8 multiplayerTracking, quint16 unk1, quint8 boundingId, quint8 backgroundId, quint8 cameraFlags);
    bool clickDetection(qint32 xcheck, qint32 ycheck);
    bool clickDetection(QRect rect);
    ObjectType getType() const { return ObjectType::ZONE; }
    bool isResizable() const { return true; }
    QString toString(qint32 xOffset, qint32 yOffset) const;
    quint16 getUnk1() const { return unk1; }
    quint8 getid() const { return id; }
    quint8 getBoundingId() const { return boundingId; }
    quint8 getMultiplayerTracking() const { return multiplayerTracking; }
    quint8 getProgPathId() const { return progPathId; }
    quint8 getMusicId() const { return musicId; }
    quint8 getBackgroundId() const { return backgroundId; }
    quint8 getCameraFlags() const { return cameraFlags; }
    void setUnk1(quint32 unk1) { this->unk1 = unk1; }
    void setID(quint8 id) { this->id = id; }
    void setBoundingId(quint8 boundingId) { this->boundingId = boundingId; }
    void setMultiplayerTracking(quint8 multiplayerTracking) { this->multiplayerTracking = multiplayerTracking; }
    void setProgPathId(quint8 progPathId) { this->progPathId = progPathId; }
    void setMusicID(quint8 musicId) { this->musicId = musicId; }
    void setBackgroundId(quint8 backgroundId) { this->backgroundId = backgroundId; }
    void setCameraFlags(quint8 cameraFlags) { this->cameraFlags = cameraFlags; }

protected:
    quint16 unk1;
    quint8 id;
    quint8 boundingId;
    quint8 multiplayerTracking;
    quint8 progPathId;
    quint8 musicId;
    quint8 backgroundId;
    quint8 cameraFlags;
};


// Location Object
class Location: public Object
{
public:
    Location() {}
    Location(Location* loc);
    Location(qint32 x, qint32 y, qint32 width, qint32 height, qint32 id);
    ObjectType getType() const { return ObjectType::LOCATION; }
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
    PathNode(qint32 x, qint32 y, Path *parentPath);
    PathNode(qint32 x, qint32 y, float speed, float accel, quint16 delay, qint16 rotation, quint8 variableField, quint8 nextPathID, Path *parentPath);
    ObjectType getType() const { return ObjectType::PATHNODE; }
    bool isResizable() const { return false; }
    float getSpeed() const { return speed; }
    float getAccel() const { return accel; }
    quint16 getDelay() const { return delay; }
    qint16 getRotation() const { return rotation; }
    quint8 getVariableField() const { return variableField; }
    quint8 getNextPathID() const { return nextPathID; }
    Path* getParentPath() const { return parentPath; }
    void setSpeed(float speed) { this->speed = speed; }
    void setAccel(float accel) { this->accel = accel; }
    void setDelay(quint16 delay) { this->delay = delay; }
    void setRotation(qint16 rotation) { this->rotation = rotation; }
    void setVariableField(quint8 variableField) { this->variableField = variableField; }
    void setNextPathID(quint8 nextPathID) { this->nextPathID = nextPathID; }

    QString toString(qint32 xOffset, qint32 yOffset) const;
protected:
    Path* parentPath;
    float speed;
    float accel;
    quint16 delay;
    qint16 rotation;
    quint8 variableField;
    quint8 nextPathID;
};


// Path
class Path
{
public:
    Path() {}
    Path(Path* path);
    ~Path() { qDeleteAll(nodes); }
    Path(quint16 id, quint16 loop);
    void insertNode(PathNode* node, qint32 index = -1);
    void removeNode(PathNode* node);
    quint16 getid() const { return id; }
    quint16 getLoop() const { return loop; }
    qint32 getNumberOfNodes() const { return nodes.size(); }
    QList<PathNode*> getNodes() const;
    PathNode* getNode(qint32 id) const { return nodes[id]; }
    qint32 getIndexOfNode(PathNode* node) { return nodes.indexOf(node); }
    void setId(qint32 id) { this->id = id; }
    void setLoop(qint32 loop) {this->loop = loop; }
    QString toString(qint32 xOffset, qint32 yOffset);
    void swapNodes(qsizetype node1, qsizetype node2) { nodes.swapItemsAt(node1, node2); }

protected:
    quint16 id;
    quint16 loop;
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
    ObjectType getType() const { return ObjectType::PROGRESSPATHNODE; }
    bool isResizable() const { return false; }
    ProgressPath* getParentPath() const { return parentPath; }
    QString toString(qint32 xOffset, qint32 yOffset) const;
private:
    ProgressPath* parentPath;
};


// Progress Path
class ProgressPath
{
public:
    ProgressPath() {}
    ProgressPath(ProgressPath* path);
    ~ProgressPath() { qDeleteAll(nodes); }
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
    void setAlternatePathFlag(quint8 flag) { this->alternatePathFlag = flag; }
    QString toString(qint32 xOffset, qint32 yOffset);
protected:
    quint16 id;
    quint8 alternatePathFlag;
    QList<ProgressPathNode*> nodes;
};

#endif // OBJECTS_H
