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
#include "unitsconvert.h"

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

void Object::increasePosition(int deltax, int deltay, int snap)
{
    this->x += deltax;
    this->y += deltay;

    if (snap == 10)
    {
        x = toNext10(x);
        y = toNext10(y);
    }
}

void Object::resize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Object::increaseSize(int deltax, int deltay, int snap)
{
    this->width += deltax;
    this->height += deltay;

    if (snap == 10)
    {
        width = toNext10(width + x) - x;
        height = toNext10(height + y) - y;
    }
}

bool Object::clickDetection(int xcheck, int ycheck)
{
    return QRect(x+offsetx,y+offsety,width,height).contains(xcheck, ycheck);
}

bool Object::clickDetection(QRect rect)
{
    return rect.intersects(QRect(x+offsetx,y+offsety,width,height));
}

QString Object::toString(int, int) const { return QString(""); }


// BgdatObject
BgdatObject::BgdatObject(int x, int y, int width, int height, int id, int layer)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
    this->layer = layer;
}

BgdatObject::BgdatObject(BgdatObject *obj)
{
    x = obj->getx();
    y = obj->gety();
    width = obj->getwidth();
    height = obj->getheight();
    id = obj->getid();
    layer = obj->getLayer();

}

int BgdatObject::getType() const { return 0; }
int BgdatObject::getid() const { return id; }
int BgdatObject::getLayer() const { return layer; }

// Format: 0:ID:Layer:X:Y:Width:Height
QString BgdatObject::toString(int xOffset, int yOffset) const { return QString("0:%1:%2:%3:%4:%5:%6").arg(id).arg(layer).arg(x+xOffset).arg(y+yOffset).arg(width).arg(height); }


// Sprite
Sprite::Sprite(int x, int y, int id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

Sprite::Sprite(Sprite *spr)
{
    x = spr->getx();
    y = spr->gety();
    id = spr->getid();
    for (int i = 0; i < 12; i++) spriteData[i] = spr->getByte(i);
    setRect();
}

void Sprite::setRect()
{
    width = 20;
    height = 20;
    offsetx = 0;
    offsety = 0;
    switch (id) {
    case 3: // Cheep Chomp
        width = 79;
        height = 67;
        offsetx = -31;
        offsety = -26;
        break;
    case 8: // Swoop
        width = 13;
        height = 22;
        offsetx = 3;
        break;
    case 9: // Whomp
        width = 75;
        height = 57;
        offsetx = -18;
        offsety = 3;
        break;
    case 19: // Desert Crater
        width = 65;
        height = 18;
        offsetx = -22;
        offsety = 19;
        break;
    case 22: // Special Exit Controller
        width = qMax(getNybble(7) * 20, 20);
        height = qMax(getNybble(4) * 20, 20);
        break;
    case 29: // Bob-omb
        width = 29;
        height = 29;
        offsetx = -5;
        offsety = -8;
        break;
    case 31: case 32: case 33: // Boomerang/Fire/Hammer Bro.
        width = 27;
        height = 47;
        offsetx = -5;
        offsety = -13;
        break;
    case 35: // Lava Bubble
        width = 20;
        height = 21;
        offsety = 10;
        break;
    case 52: // Checkpoint Flag
        width = 37;
        height = 65;
        offsetx = 1;
        offsety = -25;
        break;
    case 66: case 67: // Pipe Cannons
        width = 40;
        height = 80;
        break;
    case 69: // Door
        width = 43;
        height = 60;
        break;
    case 70: // Castle Boss Door
        width = 66;
        height = 75;
        offsetx = -3;
        offsety = 5;
        break;
    case 71: // Closed Door
        width = 40;
        height = 60;
        break;
    case 72: // Final Boss Door
        width = 144;
        height = 144;
        offsety = -45;
        break;
    case 73: // Ghost House Door
        width = 41;
        height = 60;
        offsety = -1;
        break;
    case 75: // Tower Boss Door
        width = 65;
        height = 75;
        offsetx = -2;
        offsety = 5;
        break;
    case 76: // Big Thwomp
        width = 102;
        height = 114;
        offsetx = -11;
        offsety = -3;
        break;
    case 77: // Thwomp
        width = 54;
        height = 59;
        offsetx = -7;
        offsety = -2;
        break;
    case 81: // Fireball Pipe - ! Junction
        width = 40;
        height = 40;
        break;
    case 83: // Fish Bone
        width = 33;
        height = 21;
        offsetx = -4;
        break;
    case 92: // Grinder
        width = 49;
        height = 49;
        offsetx = -6;
        offsety = -6;
        break;
    case 93: // Scuttlebug
        width = 52;
        height = 49;
        offsetx = -18;
        offsety = -21;
        break;
    case 95: // Blooper
        width = 28;
        height = 36;
        offsetx = -4;
        offsety = -9;
        break;
    case 97: // End of Level Flag
        width = 62;
        height = 200;
        offsetx = -22;
        break;
    case 99: // Wiggler
        width = 89;
        height = 41;
        offsety = -20;
        break;
    case 108: // Spider Web
        width = 80;
        height = 80;
        break;
    case 109: // Signboard
        width = 40;
        height = 40;
        break;
    case 110: // Dry Bones
        width = 28;
        height = 36;
        offsetx = -6;
        offsety = -15;
        break;
    case 111: // Giant Dry Bones
        width = 41;
        height = 55;
        offsetx = -8;
        offsety = 7;
        break;
    case 114: // Floating Box
        if (getNybble(5) == 1) // Big
        {
            width = 122;
            height = 121;
            offsetx = -31;
            offsety = 2;
        }
        else
        {
            width = 62;
            height = 61;
            offsetx = -1;
            offsety = 3;
        }
        break;
    case 115: // Bullet Bill Launcher
        width = 20;
        height = (2 + getNybble(4)) * 20;
        if (getNybble(10) == 0)
            offsety = -height+20;
        break;
    case 117: // Banzai Bill Launcher
        width = 80;
        height = 103;
        offsety = -3;
        break;
     case 119: // Banzai Bill
    {
        width = 110;
        height = 82;
        offsety = -24;
        offsetx = -48;
        break;
    }
    case 124: // Mushroom Platform
        width = getNybble(6)*20 + 68;
        height = 20;
        offsetx = -(width / 2);
        break;
    case 125: // Bowser
        width = 138;
        height = 108;
        offsetx = -58;
        offsety = -87;
        break;
    case 135: // Goomba
        width = 21;
        height = 24;
        offsetx = -1;
        offsety = -4;
        break;
    case 136: // Bone Goomba
        width = 22;
        height = 24;
        offsetx = -2;
        offsety = -4;
        break;
    case 137: // Micro Goomba
        width = 11;
        height = 13;
        offsetx = 4;
        offsety = 8;
        break;
    case 138: // Paragoomba
        width = 25;
        height = 33;
        offsetx = -1;
        offsety = -12;
        break;
    case 139: // Goomba Tower
        width = 21;
        height = getNybble(5) * 21 + 4;
        offsety = - height + 21 + 1;
        break;
    case 140: // Crowber
        width = 31;
        height = 21;
        offsetx = -5;
        offsety = -6;
        break;
    case 143: // 	Conveyor Belt Switch
        width = 28;
        height = 42;
        offsetx = -4;
        offsety = -21;
        break;
    case 150: // Seesaw Lift
        width = 280;
        height = 20;
        offsetx= 20;
        break;
    case 165: // Koopa Troopa
        width = 27;
        height = 35;
        offsetx = -5;
        offsety = -15;
        break;
    case 167: // Pipe Piranha Plant - Down
        width = 30;
        height = 37;
        offsetx = 5;
        offsety = 40;
        break;
    case 175: // Grounded Piranha Plant
        width = 54;
        height = 33;
        offsetx = -8;
        break;
    case 181: // Pipe Piranha Plant - Left
        width = 37;
        height = 30;
        offsetx = -37;
        offsety = 5;
        break;
    case 182: // Pipe Piranha Plant - Right
        width = 37;
        height = 30;
        offsetx = 40;
        offsety = 5;
        break;
    case 183: // Pipe Piranha Plant - Up
        width = 30;
        height = 37;
        offsetx = 5;
        offsety = -37;
        break;
    case 184: // Parabomb
        width = 29;
        height = 38;
        offsetx = -5;
        offsety = -18;
        break;
    case 185: // Koopa Paratroopa
        width = 32;
        height = 36;
        offsetx = -7;
        offsety = -15;
        break;
    case 189: case 190: case 191: case 192: case 193: // Rect Blocks
        width = getNybble(15)*20+20;
        if (width == 20) width = 40;
        height = getNybble(13)*20+20;
        if (height == 20) height = 40;
        if (getNybble(9) == 1 || getNybble(9) == 3) { height += 20; offsety-=20; }
        if (getNybble(9) == 2 || getNybble(9) == 3) height += 20;
        if (getNybble(9) == 4 || getNybble(9) == 6) { width += 20; offsetx-=20; }
        if (getNybble(9) == 5 || getNybble(9) == 6) width += 20;
        if (id == 192)
        {
            width += 6;
            offsetx -= 3;
        }
        break;
    case 194: // Cheep Cheep
        width = 22;
        height = 21;
        offsety = -3;
        break;
    case 195: // Big Cheep Cheep
        width = 60;
        height = 56;
        offsety = -5;
        break;
    case 205: // Red Ring
        width = 39;
        height = 56;
        offsety = 2;
        break;
    case 206: // Gold Ring
        width = 40;
        height = 56;
        offsety = 2;
        break;
    case 211: // Roy Koopa
        width = 78;
        height = 47;
        offsetx = -46;
        offsety = -27;
        break;
    case 215: // Bob-omb Cannon
        width = 31;
        height = 32;
        if (getNybble(5) != 1) offsetx = -1;
        else offsetx = -10;
        offsety = -12;
        break;
    case 219: // Star Coin
        width = 40;
        height = 40;
        break;
    case 221: // ! Switch
        width = 22;
        height = 24;
        offsetx = -1;
        if (getNybble(5) != 1) offsety = -4;
        break;
    case 223: // ? Switch
        width = 22;
        height = 24;
        offsetx = -1;
        if (getNybble(5) != 1) offsety = -4;
        break;
    case 225: // P Switch
        width = 22;
        height = 24;
        offsetx = -1;
        if (getNybble(5) != 1) offsety = -4;
        break;
    case 234: // Spiked Ball
        width = 38;
        height = 38;
        offsetx = 1;
        offsety = 1;
        break;
    case 235: // Big Spiked Ball
        width = 76;
        height = 76;
        offsetx = 12;
        offsety = 2;
        break;
    case 236: // Mega Spiked Ball
        width = 154;
        height = 154;
        offsetx = -23;
        offsety = -7;
        break;
    case 240: // Urchin
        if (getNybble(5) != 1)
        {
            width = 26;
            height = 26;
            offsetx = -3;
            offsety = -3;
        }
        else
        {
            width = 51;
            height = 48;
            offsetx = -5;
            offsety = -4;
        }
        break;
    case 255: // Bowser Head Statue
        width = 40;
        height = 42;
        offsetx = -18;
        offsety = -1;
        break;
    case 267: case 275: case 276: // Long Question Blocks
        width = 60;
        height = 20;
        break;
    case 269: // Gold Ship
        width = 285;
        height = 96;
        break;
    case 270: // Icy Spiked Ball
        width = 38;
        height = 38;
        offsetx = 1;
        offsety = 1;
        break;
    case 273: // Coin Roulette Block
        width = 31;
        height = 30;
        offsetx = -5;
        offsety = -5;
        break;
    case 274: // Flying Gold Block Spawn Point
        width = 28;
        height = 23;
        offsetx = -4;
        offsety = -1;
        break;
    case 277: // Switchable Conveyor Belt
        width = 160;
        break;
    case 278: // Assist Block
        width = 26;
        height = 26;
        offsetx = -3;
        offsety = -5;
        break;
    case 279: // Lemmy Ball
        width = 30;
        height = 30;
        offsetx = -3;
        offsety = -15;
        break;
    case 280: // + Clock
        width = 22;
        height = 26;
        offsetx = -3;
        offsety = -1;
        break;
    case 287: // Toad House Door
        width = 40;
        height = 60;
        break;
    case 293: // Punching Glove
        width = 32;
        height = 35;
        break;
    case 294: // Warp Cannon
        width = 100;
        height = 67;
        offsetx = -40;
        offsety = -27;
        break;
    case 296: // Toad
        width = 25;
        height = 39;
        offsetx = -1;
        offsety = -16;
        break;
    case 302: // Moon Coin
        width = 40;
        height = 40;
        break;
    case 322: // Big Grinder
        width = 86;
        height = 86;
        offsetx = -23;
        offsety = -23;
        break;
    default:
        width = 20;
        height = 20;
        offsetx = 0;
        offsety = 0;
    }
}

int Sprite::getType() const { return 1; }
int Sprite::getid() const { return id; }

quint8 Sprite::getByte(int id) const { return spriteData[id]; }
void Sprite::setByte(int id, quint8 nbr) { spriteData[id] = nbr; }

quint8 Sprite::getNybble(int id) const
{
    if (id%2 == 0) return spriteData[id/2] >> 4;
    else return spriteData[id/2] & 0xF;
}

void Sprite::setNybble(int id, quint8 nbr)
{
    if (id%2 == 0) spriteData[id/2] = ((spriteData[id/2] & 0xF) | nbr << 4);
    else spriteData[id/2] = ((spriteData[id/2] & 0xF0) | nbr);
}

int Sprite::getNybbleData(int startNybble, int endNybble)
{
    int data = 0;

    for (int i = startNybble; i <= endNybble; i++)
        data = data << 4 | getNybble(i);

    return data;
}

void Sprite::setNybbleData(int data, int startNybble, int endNybble)
{
    for (int i = endNybble; i >= startNybble; i--)
    {
        setNybble(i, (quint8)(data & 0xF));
        data = data >> 4;
    }
}

// Format: 1:ID:X:Y:SD0:SD1:...:SD11
QString Sprite::toString(int xOffset, int yOffset) const
{
    QString str("1:%1:%2:%3");
    for (int i=0; i<12; i++) str.append(QString(":%1").arg(spriteData[i]));
    return str.arg(id).arg(x+xOffset).arg(y+yOffset);
}


// Entrance
Entrance::Entrance(int x, int y, qint16 cameraX, qint16 cameraY, quint8 id, quint8 destArea, quint8 destEntr, quint8 entrType, quint8 settings, quint8 unk1, quint8 unk2)
{
    this->x = x;
    this->y = y;
    this->cameraX = cameraX;
    this->cameraY = cameraY;
    this->id = id;
    this->destArea = destArea;
    this->destEntr = destEntr;
    this->entrType = entrType;
    this->settings = settings;
    this->unk1 = unk1;
    this->unk2 = unk2;
}

Entrance::Entrance(Entrance *entr)
{
    x = entr->getx();
    y = entr->gety();
    cameraX = entr->getCameraX();
    cameraY = entr->getCameraY();
    id = entr->getid();
    destArea = entr->getDestArea();
    destEntr = entr->getDestEntr();
    entrType = entr->getEntrType();
    settings = entr->getSettings();
    unk1 = entr->getUnk1();
    unk2 = entr->getUnk2();
}

// Format: 2:ID:Type:X:Y:DestArea:DestEntr:CamX:CamY:Settings
QString Entrance::toString(int xOffset, int yOffset) const { return QString("2:%1:%2:%3:%4:%5:%6:%7:%8:%9").arg(id).arg(entrType).arg(x+xOffset).arg(y+yOffset).arg(destArea).arg(destEntr).arg(cameraX).arg(cameraY).arg(settings); }


// Zone
Zone::Zone(int x, int y, int width, int height, quint8 id, quint8 progPathId, quint8 musicId, quint8 multiplayerTracking, quint16 unk1)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
    this->progPathId = progPathId;
    this->musicId = musicId;
    this->multiplayerTracking = multiplayerTracking;
    this->unk1 = unk1;
}

Zone::Zone(Zone *zone)
{
    x = zone->getx();
    y = zone->gety();
    width = zone->getwidth();
    height = zone->getheight();
    id = zone->getid();
    progPathId = zone->getProgPathId();
    musicId = zone->getMusicId();
    multiplayerTracking = zone->getMultiplayerTracking();
    unk1 = zone->getUnk1();
    upperBound = zone->getUpperBound();
    lowerBound = zone->getLowerBound();
    unkUpperBound = zone->getUnkUpperBound();
    unkLowerBound = zone->getUnkLowerBound();
    upScrolling = zone->getUpScrolling();
    xScrollRate = zone->getXScrollRate();
    yScrollRate = zone->getYScrollRate();
    bgXPos = zone->getBgXPos();
    bgYPos = zone->getBgYPos();
    bgName = zone->getBgName();
    bgUnk1 = zone->getBgUnk1();
}

bool Zone::clickDetection(int xcheck, int ycheck)
{
    return QRect(x,y,38,18).contains(xcheck, ycheck);
}

bool Zone::clickDetection(QRect rect)
{
    return rect.intersects(QRect(x,y,38,18));
}

void Zone::setBounding(tempZoneBounding bounding)
{
    this->upperBound = bounding.upperBound;
    this->lowerBound = bounding.lowerBound;
    this->unkUpperBound = bounding.unkUpperBound;
    this->unkLowerBound = bounding.unkLowerBound;
    this->upScrolling = bounding.upScrolling;
}

void Zone::setBackground(tempZoneBackground background)
{
    this->xScrollRate = background.xScrollRate;
    this->yScrollRate = background.yScrollRate;
    this->bgXPos = background.xPos;
    this->bgYPos = background.yPos;
    this->bgName = background.name;
    this->bgUnk1 = background.unk1;
}


// Location
Location::Location(int x, int y, int width, int height, int id)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->id = id;
}

Location::Location(Location *loc)
{
    x = loc->getx();
    y = loc->gety();
    width = loc->getwidth();
    height = loc->getheight();
    id = loc->getid();
}

int Location::getType() const { return 4; }
int Location::getid() const { return id; }

// Format: 4:ID:X:Y:Width:Height
QString Location::toString(int xOffset, int yOffset) const { return QString("4:%1:%2:%3:%4:%5").arg(id).arg(x+xOffset).arg(y+yOffset).arg(width).arg(height); }


// Path
Path::Path(quint16 id, quint16 unk1)
{
    this->id = id;
    this->unk1 = unk1;
}

Path::Path(Path *path)
{
    id = path->getid();
    unk1 = path->getUnk1();
    foreach (PathNode* node, path->getNodes())
        nodes.append(new PathNode(node, this));
}

void Path::insertNode(PathNode* node, int index)
{
    if (index == -1)
        nodes.append(node);
    else
        nodes.insert(index, node);
}

void Path::removeNode(PathNode *node)
{
    nodes.removeOne(node);
}

QList<PathNode*> Path::getNodes() const { return nodes; }

// Path Node
PathNode::PathNode(int x, int y, float speed, float accel, quint32 unk1, Path* parentPath)
{
    this->x = x;
    this->y = y;
    this->speed = speed;
    this->accel = accel;
    this->unk1 = unk1;
    this->parentPath = parentPath;
}

PathNode::PathNode(PathNode *node, Path* parentPath)
{
    x = node->getx();
    y = node->gety();
    speed = node->getSpeed();
    accel = node->getAccel();
    unk1 = node->getUnk1();
    this->parentPath = parentPath;
}

// Progress Path
ProgressPath::ProgressPath(quint16 id, quint8 alternatePathFlag)
{
    this->id = id;
    this->alternatePathFlag = alternatePathFlag;
}

ProgressPath::ProgressPath(ProgressPath *path)
{
    id = path->getid();
    alternatePathFlag = path->getAlternatePathFlag();
    foreach (ProgressPathNode* node, path->getNodes())
        nodes.append(new ProgressPathNode(node, this));
}

void ProgressPath::insertNode(ProgressPathNode* node, int index)
{
    if (index == -1)
        nodes.append(node);
    else
        nodes.insert(index, node);
}

void ProgressPath::removeNode(ProgressPathNode *node)
{
    nodes.removeOne(node);
}

QList<ProgressPathNode*> ProgressPath::getNodes() const { return nodes; }

// Progress Path Node
ProgressPathNode::ProgressPathNode(int x, int y, ProgressPath *parentPath)
{
    this->x = x;
    this->y = y;
    this->parentPath = parentPath;
}

ProgressPathNode::ProgressPathNode(ProgressPathNode *node, ProgressPath *parentPath)
{
    x = node->getx();
    y = node->gety();
    this->parentPath = parentPath;
}
