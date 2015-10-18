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

// Obj Baseclass
class Object
{
public:
    Object();
    void setPosition(int x, int y);
    void resize(int width, int height);
    int getx() const;
    int gety() const;
    int getwidth() const;
    int getheight() const;

protected:
    int x, y;
    int width, height;
};


// Bgdat Object
class BgdatObject: public Object
{
public:
    BgdatObject();
    BgdatObject(int x, int y, int width, int height, int id);
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
    void setSpriteOffset(int offsetx, int offsety);
    int getid() const;

protected:
    int offsetx, offsety;
    int id;
};


// Entrance Object
class Entrance: public Object
{
public:
    Entrance();
    Entrance(int x, int y, int id);
    int getid() const;
protected:
    int id;
};


// Zone Object
class Zone: public Object
{
public:
    Zone();
    Zone(int x, int y, int width, int height, int id);
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
protected:
    int id;
    int nodeOffset;
    int numberOfNodes;
    QList<ProgressPathNode> nodes;
};

#endif // OBJECTS_H
