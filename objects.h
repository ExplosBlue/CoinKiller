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

// Obj Baseclass
class Object
{
public:
    Object();
    virtual ~Object();
    void setPosition(int x, int y);
    void resize(int width, int height);
    virtual int getType() const;
    int getx() const;
    int gety() const;
    int getwidth() const;
    int getheight() const;
    int getOffsetX() const;
    int getOffsetY() const;
    bool clickDetection(int xcheck, int ycheck, int wcheck, int hcheck);

    void setDrag(int dragX, int dragY);
    int getDragX() const;
    int getDragY() const;

protected:
    int x, y;
    int width, height;
    int offsetx, offsety;
    int dragX, dragY;
};


// Bgdat Object
class BgdatObject: public Object
{
public:
    BgdatObject();
    BgdatObject(int x, int y, int width, int height, int id);
    int getType() const;
    int getid() const;
protected:
    int id;
};


// Sprite Object
class Sprite: public Object
{
public:
    Sprite();
    Sprite(int x, int y, int id);
    int getType() const;
    int getid() const;
    void setByte(int id, qint8 nbr);
    void setNybble(int id, qint8 nbr);
    qint8 getByte(int id) const;
    qint8 getNybble(int id) const;
    void setRect();

protected:
    int id;
    qint8 spriteData[8];
};


// Entrance Object
class Entrance: public Object
{
public:
    Entrance();
    Entrance(int x, int y, int cameraX, int cameraY, int id, int destArea, int destEntr, int type);
    int getType() const;
    int getid() const;
protected:
    int cameraX;
    int cameraY;
    int id;
    int destArea;
    int destEntr;
    int type;
};


// Zone Object
class Zone: public Object
{
public:
    Zone();
    Zone(int x, int y, int width, int height, int id);
    int getType() const;
    int getid() const;
protected:
    int id;
};


// Location Object
class Location: public Object
{
public:
    Location();
    Location(int x, int y, int width, int height, int id);
    int getType() const;
    int getid() const;
protected:
    int id;
};


// Path Node
class PathNode: public Object
{
public:
    PathNode();
    PathNode(int x, int y, float speed, float accel);
    int getType() const;
protected:
    float speed;
    float accel;
};


// Path
class Path
{
public:
    Path();
    Path(int id, int nodesOffset, int numberOfNodes);
    void insertNode(PathNode &node);
    //void removeNodeAt(int id);
    int getid() const;
    int getNodesOffset() const;
    int getNumberOfNodes() const;
    QList<PathNode> getNodes() const;
    PathNode& getNodeReference(int id);
protected:
    int id;
    int nodeOffset;
    int numberOfNodes;
    QList<PathNode> nodes;
};


// Progress Path Node
class ProgressPathNode: public Object
{
public:
    ProgressPathNode();
    ProgressPathNode(int x, int y);
    int getType() const;
};


// Progress Path
class ProgressPath
{
public:
    ProgressPath();
    ProgressPath(int id, int nodesOffset, int numberOfNodes);
    void insertNode(ProgressPathNode &node);
    //void removeNodeAt(int id);
    int getid() const;
    int getNodesOffset() const;
    int getNumberOfNodes() const;
    QList<ProgressPathNode> getNodes() const;
    ProgressPathNode& getNodeReference(int id);
protected:
    int id;
    int nodeOffset;
    int numberOfNodes;
    QList<ProgressPathNode> nodes;
};


// Zone Boundings
class ZoneBounding
{
public:
    ZoneBounding();
    ZoneBounding(quint32 upperBound, quint32 lowerBound, quint32 unkUpperBound, quint32 unkLowerBound, quint16 id, quint16 upScrolling);
    // TODO: Getters and Setters
protected:
    quint32 upperBound;
    quint32 lowerBound;
    quint32 unkUpperBound;
    quint32 unkLowerBound;
    quint16 id;
    quint16 upScrolling;
};


// Zone Background
class ZoneBackground
{
public:
    ZoneBackground();
    ZoneBackground(quint16 id, quint8 xScrollRate, quint8 yScrollRate, quint8 xPos, quint8 yPos, QString name);
    // TODO: Getters and Setters
protected:
    quint16 id;
    quint8 xScrollRate;
    quint8 yScrollRate;
    quint8 xPos;
    quint8 yPos;
    QString name;
    // quint8 unk1;
};

#endif // OBJECTS_H
