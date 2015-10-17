#include "objects.h"

Object::Object()
{
    x = 0;
    y = 0;
    width = 1;
    height = 1;
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

int Object::getx() const { return x; }
int Object::gety() const { return y; }
int Object::getwidth() const { return width; }
int Object::getheight() const { return height; }


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
    width = 20;
    height = 20;
}

Sprite::Sprite(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
    width = 20;
    height = 20;
}

void Sprite::setSpriteOffset(int offsetx, int offsety)
{
    this->offsetx = offsetx;
    this->offsety = offsety;
}

int Sprite::getid() const { return id; }


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

// Progress Path Node
ProgressPathNode::ProgressPathNode(int x, int y)
{
    this->x = x;
    this->y = y;
}
