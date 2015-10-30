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

#include "objects.h"

#include <QPainter>

Object::Object()
{
    x = 0;
    y = 0;
    width = 20;
    height = 20;
    offsetx = 0;
    offsety = 0;
}

void Object::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Object::resize(int width, int height)
{
    this->width = width;
    this->height = height;
}

bool Object::clickDetection(int xClick, int yClick)
{
    if (xClick >= x && xClick < x+width && yClick >= y && yClick < y+height) return true;
    else return false;
}

int Object::getx() const { return x; }
int Object::gety() const { return y; }
int Object::getwidth() const { return width; }
int Object::getheight() const { return height; }
int Object::getOffsetX() const { return offsetx; }
int Object::getOffsetY() const { return offsety; }

// BgdatObject
BgdatObject::BgdatObject()
{

}

BgdatObject::BgdatObject(int x, int y, int width, int height, int id)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
}

int BgdatObject::getid() const { return id; }


// Sprite
Sprite::Sprite()
{

}

Sprite::Sprite(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

bool Sprite::clickDetection(int xClick, int yClick)
{
    if (xClick >= x+offsetx && xClick < x+offsetx+width && yClick >= y+offsety && yClick < y+offsety+height) return true;
    else return false;
}

void Sprite::setRect()
{
    switch (id) {
    case 52: // Checkpoint Flag
        width = 38;
        height = 65;
        offsety = -40;
    case 66: // Pipe Cannon
        width = 40;
        height = 80;
        break;
    case 97: //End of Level Flag
        width = 62;
        height = 200;
        offsetx = -22;
        break;
    case 110: // Dry Bones
        width = 26;
        height = 38;
        offsetx = -6;
        offsety = -18;
        break;
    case 135: // Goomba
        width = 24;
        height = 24;
        offsetx = -2;
        offsety = -4;
        break;
    case 139: // Goomba Tower
        width = 21;
        height = getNybble(5) * 21 + 4;
        offsety = - height + 21 + 1;
        break;
    case 165: // Koopa Troopa
        width = 28;
        height = 37;
        offsetx = -8;
        offsety = -16;
        break;
    case 219: // Star Coin
        width = 40;
        height = 40;
        break;
    case 267: // Long Question Block
        width = 60;
        height = 20;
        break;
    case 273: // Coin Roulette Block
        width = 31;
        height = 30;
        offsetx = -5;
        offsety = -5;
        break;
    default:
        width = 20;
        height = 20;
        offsetx = 0;
        offsety = 0;
    }
}

int Sprite::getid() const { return id; }

qint8 Sprite::getByte(int id) const { return spriteData[id]; }
void Sprite::setByte(int id, qint8 nbr) { spriteData[id] = nbr; }

qint8 Sprite::getNybble(int id) const
{
    if (id%2 == 0) return spriteData[id/2] & 0xF0;
    else return spriteData[id/2] & 0x0F;
}


// Entrance
Entrance::Entrance()
{

}

Entrance::Entrance(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

int Entrance::getid() const { return id; }


// Zone
Zone::Zone()
{

}

Zone::Zone(int x, int y, int width, int height, int id)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
}

int Zone::getid() const { return id; }


// Location
Location::Location()
{

}

Location::Location(int x, int y, int width, int height, int id)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
}

int Location::getid() const { return id; }


// Path
Path::Path(int id, int nodesOffset, int numberOfNodes)
{
    this->id = id;
    this->nodeOffset = nodesOffset;
    this->numberOfNodes = numberOfNodes;
}

void Path::insertNode(PathNode &node)
{
    nodes.append(node);
}

int Path::getid() const { return id; }
int Path::getNodesOffset() const { return nodeOffset; }
int Path::getNumberOfNodes() const { return numberOfNodes; }
QList<PathNode> Path::getNodes() const { return nodes; }
PathNode& Path::getNodeReference(int id) { return nodes[id]; }


// Path Node
PathNode::PathNode(int x, int y, float speed, float accel)
{
    this->x = x;
    this->y = y;
    this->speed = speed;
    this->accel = accel;
}


// Progress Path
ProgressPath::ProgressPath(int id, int nodesOffset, int numberOfNodes)
{
    this->id = id;
    this->nodeOffset = nodesOffset;
    this->numberOfNodes = numberOfNodes;
}

void ProgressPath::insertNode(ProgressPathNode &node)
{
    nodes.append(node);
}

int ProgressPath::getid() const { return id; }
int ProgressPath::getNodesOffset() const { return nodeOffset; }
int ProgressPath::getNumberOfNodes() const { return numberOfNodes; }
QList<ProgressPathNode> ProgressPath::getNodes() const { return nodes; }
ProgressPathNode& ProgressPath::getNodeReference(int id) { return nodes[id]; }

// Progress Path Node
ProgressPathNode::ProgressPathNode(int x, int y)
{
    this->x = x;
    this->y = y;
}
