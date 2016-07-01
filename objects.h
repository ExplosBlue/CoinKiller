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
    void setPosition(int x, int y);
    void increasePosition(int deltax, int deltay, int snap = 0);
    void resize(int width, int height);
    void increaseSize(int deltax, int deltay, int snap = 0);
    virtual int getType() const { return -1; }
    virtual bool isResizable() const { return false; }
    int getx() const { return x; }
    int gety() const { return y; }
    int getwidth() const { return width; }
    int getheight() const { return height; }
    int getOffsetX() const { return offsetx; }
    int getOffsetY() const { return offsety; }
    bool clickDetection(int xcheck, int ycheck);
    bool clickDetection(QRect rect);

    virtual QString toString(int xOffset, int yOffset) const;
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
    int x, y;
    int width, height;
    int offsetx, offsety;
    int dragX, dragY;
    int resizeX, resizeY;
};

// Bgdat Object
class BgdatObject: public Object
{
public:
    BgdatObject() {}
    BgdatObject(int x, int y, int width, int height, int id, int layer);
    int getType() const;
    bool isResizable() const { return true; }
    int getid() const;
    int getLayer() const;
    QString toString(int xOffset, int yOffset) const;
    void setLayer(int layer) { this->layer = layer; }
protected:
    int id;
    int layer;
};


// Sprite Object
class Sprite: public Object
{
public:
    Sprite() {}
    Sprite(int x, int y, int id);
    int getType() const;
    bool isResizable() const { return false; }
    int getid() const;
    void setByte(int id, quint8 nbr);
    void setNybble(int id, quint8 nbr);
    quint8 getByte(int id) const;
    quint8 getNybble(int id) const;
    void setRect();
    QString toString(int xOffset, int yOffset) const;
    int getNybbleData(int startNybble, int endNybble);
    void setNybbleData(int data, int startNybble, int endNybble);
protected:
    int id;
    quint8 spriteData[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
};


// Entrance Object
class Entrance: public Object
{
public:
    Entrance() {}
    Entrance(int x, int y, qint16 cameraX, qint16 cameraY, quint8 id, quint8 destArea, quint8 destEntr, quint8 entrType, quint8 settings, quint8 unk1, quint8 unk2);
    int getType() const { return 2; }
    bool isResizable() const { return false; }
    QString toString(int xOffset, int yOffset) const;
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
    void setSettingsBit(bool value, int bit) { settings ^= (-value ^ settings) & (1 << bit); }
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
    Zone(int x, int y, int width, int height, quint8 id, quint8 progPathId, quint8 musicId, quint8 multiplayerTracking, quint16 unk1);
    bool clickDetection(int xcheck, int ycheck);
    bool clickDetection(QRect rect);
    void setBounding(tempZoneBounding bounding);
    void setBackground(tempZoneBackground background);
    int getType() const { return 3; }
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
    Location(int x, int y, int width, int height, int id);
    int getType() const;
    bool isResizable() const { return true; }
    int getid() const;
    void setId(quint8 id) { this->id = id; }
    QString toString(int xOffset, int yOffset) const;
protected:
    int id;
};


// Path Node
class Path;
class PathNode: public Object
{
public:
    PathNode() {}
    PathNode(int x, int y, float speed, float accel, quint32 unk1, Path *parentPath);
    int getType() const { return 5; }
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
    ~Path() { foreach (PathNode* node, nodes) delete node; }
    Path(quint16 id, quint16 unk1);
    void insertNode(PathNode *node);
    void removeNode(PathNode* node);
    quint16 getid() const { return id; }
    quint16 getUnk1() const { return unk1; }
    int getNumberOfNodes() const { return nodes.size(); }
    QList<PathNode*> getNodes() const;
    PathNode* getNode(int id) const { return nodes[id]; }
    void setId(int id) { this->id = id; }
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
    ProgressPathNode(int x, int y, ProgressPath* parentPath);
    int getType() const { return 6; }
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
    ~ProgressPath() { foreach (ProgressPathNode* node, nodes) delete node; }
    ProgressPath(quint16 id, quint8 alternatePathFlag);
    void insertNode(ProgressPathNode* node);
    void removeNode(ProgressPathNode* node);
    quint16 getid() const { return id; }
    quint8 getAlternatePathFlag() const { return alternatePathFlag; }
    int getNumberOfNodes() const { return nodes.size(); }
    QList<ProgressPathNode*> getNodes() const;
    ProgressPathNode* getNode(int id) const { return nodes[id]; }
    void setId(int id) { this->id = id; }
    void setAlternatePathFlag(bool flag) { this->alternatePathFlag = flag; }
protected:
    quint16 id;
    quint8 alternatePathFlag;
    QList<ProgressPathNode*> nodes;
};

#endif // OBJECTS_H
