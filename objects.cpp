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

Sprite::Sprite()
{

}

Sprite::Sprite(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

void Sprite::setSpriteOffset(int offsetx, int offsety)
{
    this->offsetx = offsetx;
    this->offsety = offsety;
}

int Sprite::getid() const { return id; }
