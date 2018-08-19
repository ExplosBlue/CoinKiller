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

void Object::setPosition(qint32 x, qint32 y)
{
    this->x = x;
    this->y = y;
}

void Object::increasePosition(qint32 deltax, qint32 deltay, qint32 snap)
{
    this->x += deltax;
    this->y += deltay;

    if (snap == 10)
    {
        x = toNext10(x);
        y = toNext10(y);
    }
}

void Object::resize(qint32 width, qint32 height)
{
    this->width = width;
    this->height = height;
}

void Object::increaseSize(qint32 deltax, qint32 deltay, qint32 snap)
{
    this->width += deltax;
    this->height += deltay;

    if (snap == 10)
    {
        width = toNext10(width + x) - x;
        height = toNext10(height + y) - y;
    }
}

bool Object::clickDetection(qint32 xcheck, qint32 ycheck)
{
    return QRect(x+offsetx,y+offsety,width,height).contains(xcheck, ycheck);
}

bool Object::clickDetection(QRect rect)
{
    return rect.intersects(QRect(x+offsetx,y+offsety,width,height));
}

QString Object::toString(qint32, qint32) const { return QString(""); }


// BgdatObject
BgdatObject::BgdatObject(qint32 x, qint32 y, qint32 width, qint32 height, qint32 id, qint32 layer)
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

qint32 BgdatObject::getType() const { return 0; }
qint32 BgdatObject::getid() const { return id; }
qint32 BgdatObject::getLayer() const { return layer; }

void BgdatObject::setTsID(qint32 tsID)
{
    id = (id & 0xFFF) | (tsID << 12);
}

void BgdatObject::setObjID(qint32 objID)
{
    id = (id & 0xF000) | objID;
}

// Format: 0:ID:Layer:X:Y:Width:Height
QString BgdatObject::toString(qint32 xOffset, qint32 yOffset) const { return QString("0:%1:%2:%3:%4:%5:%6").arg(id).arg(layer).arg(x+xOffset).arg(y+yOffset).arg(width).arg(height); }


// Sprite
Sprite::Sprite(qint32 x, qint32 y, qint32 id)
{
    this->x = x;
    this->y = y;
    this->id = id;
}

Sprite::Sprite(Sprite* spr)
{
    x = spr->getx();
    y = spr->gety();
    id = spr->getid();
    for (qint32 i = 0; i < 12; i++) spriteData[i] = spr->getByte(i);
    setRect();
}

void Sprite::setRect()
{
    width = 20;
    height = 20;
    offsetx = 0;
    offsety = 0;

    renderOffsetX = 0;
    renderOffsetY = 0;
    renderOffsetW = 0;
    renderOffsetH = 0;

    switch (id) {
    case 1: // Water Flow For Pipe
        if (getNybble(5) == 0)
        {
            height = 80;
            width = 40;
            offsety = -80;
        }
        else if (getNybble(5) == 1)
        {
            height = 80;
            width = 40;
            offsety = 20;
        }
        else if (getNybble(5) == 2)
        {
            height = 40;
            width = 80;
            offsetx = 20;
            offsety = -20;
        }
        else
        {
            height = 40;
            width = 80;
            offsetx = -80;
            offsety = -20;
        }
        break;
    case 2: // Downwards Water Pull
        height = 180;
        if (getNybble(10) == 1)
            width = 110;
        else
            width = 210;
        offsetx = -5;
        break;
    case 3: // Cheep Chomp
        width = 80;
        height = 80;
        offsetx = -30;
        offsety = -30;
        break;
    case 4: // Burner Right
        if(getNybble(4) == 1)
            width = 140;
        else
            width = 80;
        height = 22;
        offsety = -1;
        break;
    case 5: // Burner Down
        if(getNybble(4) == 1)
            height = 140;
        else
            height = 80;
        width = 22;
        offsetx = -1;
        break;
    case 6: // Burner Left
        if(getNybble(4) == 1)
        {
            width = 140;
            offsetx = -120;
        }
        else
        {
            width = 80;
            offsetx = -60;
        }
        height = 22;
        offsety = -1;
        break;
    case 7: // Burner Up
        if(getNybble(4) == 1)
        {
            height = 140;
            offsety = -120;
        }
        else
        {
            height = 80;
            offsety = -60;
        }
        width = 22;
        offsetx = -1;
        break;
    case 8: // Swooper
        width = 20;
        height = 25;
        break;
    case 9: // Whomp
        if (getNybble(5) == 1)
        {
            width = 140;
            height = 100;
            offsetx = -50;
            offsety = -40;
        }
        else
        {
            width = 80;
            height = 60;
            offsetx = -20;
        }
        break;
    case 10: // Switchable Conveyor Belt - Lemmy Battle
        width = 280;
        height = 20;
        break;
    case 16: // Amp
        width = 60;
        height = 60;
        offsetx = -20;
        offsety = -20;
        if (getNybble(4) == 1)
            offsetx = -10;
        break;
    case 17: // Amp Circle
        width = 30 + getNybble(8)*40;
        height = 30 + getNybble(8)*40;
        offsetx = 10 - width/2;
        offsety = +10 - height/2;
        break;
    case 18: // Tile God
        width = 20;
        height = 20;

        if (getNybble(15) > 1)
            width += (getNybble(15)-1)*20;

        if (getNybble(19) > 1)
        {
            width += (getNybble(19)-1)*20;
            offsetx = -(getNybble(19)-1)*20;
        }

        if (getNybble(13) > 1)
            height += (getNybble(13)-1)*20;

        if (getNybble(17) > 1)
        {
            height += (getNybble(17)-1)*20;
            offsety = -(getNybble(17)-1)*20;
        }
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
    case 24: case 25: // Flying Question Block
        width = 28;
        height = 22;
        offsetx = -4;
        break;
    case 26: // Roulette Block
        width = 30;
        height = 30;
        offsetx = -5;
        offsety = -5;
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
    case 34: // Sledge Bro
        width = 43;
        height = 53;
        offsetx = 44;
        offsety = 7;
        break;
    case 35: // Lava Bubble
        width = 20;
        height = 21;
        offsety = 10;
        break;
    case 36: // Arc Lava Bubble
        width = 26;
        height = 35;
        offsety = 10;
        renderOffsetH = 300;
        renderOffsetW = 300;
        renderOffsetX = -274;
        renderOffsetY = -210;
        if (getNybble(9) == 1)
        {
            offsetx = -6;
            renderOffsetX = 274;
        }
        break;
    case 38: // Reznor Wheel
        if(getNybble(6) == 1)
        {
            width = 349;
            height = 353;
            offsetx = -162;
            offsety = -177;
        }
        else
        {
            width = 201;
            height = 177;
            offsetx = -92;
            offsety = -76;
        }
        break;
    case 39: // Reznor Battle Dissapearing blocks
        if(getNybble(4) == 0)
            width = 20;
        else
            width = getNybble(4)*20;
        break;
    case 40: // !Burner Right
        if(getNybble(4) == 1)
            width = 140;
        else
            width = 80;
        height = 22;
        offsety = -1;
        break;
    case 41: // !Burner Down
        if(getNybble(4) == 1)
            height = 140;
        else
            height = 80;
        width = 22;
        offsetx = -1;
        break;
    case 42: // !Burner Left
        if(getNybble(4) == 1)
        {
            width = 140;
            offsetx = -120;
        }
        else
        {
            width = 80;
            offsetx = -60;
        }
        height = 22;
        offsety = -1;
        break;
    case 43: // !Burner Up
        if(getNybble(4) == 1)
        {
            height = 140;
            offsety = -120;
        }
        else
        {
            height = 80;
            offsety = -60;
        }
        width = 22;
        offsetx = -1;
        break;
    case 44: // Track-controlled Burner - Four Directions
        width = 21;
        height = 21;
        if((getNybble(8) == 4)||(getNybble(8) == 5)||(getNybble(8) == 6)||(getNybble(8) == 7)||(getNybble(8) == 12)||(getNybble(8) == 13)||(getNybble(8) == 14)||(getNybble(8) == 15))
        {
            if(getNybble(4) == 1)
            {
                height = height+120;
                offsety = offsety-height+21;
            }
            else
            {
                height = width+60;
                offsety = offsety-height+21;
            }
        }
        if((getNybble(8) == 8)||(getNybble(8) == 9)||(getNybble(8) == 10)||(getNybble(8) == 11)||(getNybble(8) == 12)||(getNybble(8) == 13)||(getNybble(8) == 14)||(getNybble(8) == 15))
        {
            if(getNybble(4) == 1)
                height = height+120;
            else
                height = height+60;
        }
        if((getNybble(8) == 2)||(getNybble(8) == 3)||(getNybble(8) == 6)||(getNybble(8) == 7)||(getNybble(8) == 10)||(getNybble(8) == 11)||(getNybble(8) == 14)||(getNybble(8) == 15))
        {
            if(getNybble(4) == 1)
            {
                width = width+120;
                offsetx = offsetx-width+21;
            }
            else
            {
                width = width+60;
                offsetx = offsetx-width+21;
            }
        }
        if((getNybble(8) == 1)||(getNybble(8) == 3)||(getNybble(8) == 5)||(getNybble(8) == 7)||(getNybble(8) == 9)||(getNybble(8) == 11)||(getNybble(8) == 13)||(getNybble(8) == 15))
        {
            if(getNybble(4) == 1)
                width = width+120;
            else
                width = width+60;
        }
        break;
    case 45: // Track Controlled Burner Right
        if(getNybble(4) == 1)
            width = 140;
        else
            width = 80;
        height = 22;
        offsety = -1;
        break;
    case 46: // Track Controlled Burner Down
        if(getNybble(4) == 1)
            height = 140;
        else
            height = 80;
        width = 22;
        offsetx = -1;
        break;
    case 47: // Track Controlled Burner Left
        if(getNybble(4) == 1)
        {
            width = 140;
            offsetx = -120;
        }
        else
        {
            width = 80;
            offsetx = -60;
        }
        height = 22;
        offsety = -1;
        break;
    case 48: // Track Controlled Burner Up
        if(getNybble(4) == 1)
        {
            height = 140;
            offsety = -120;
        }
        else
        {
            height = 80;
            offsety = -60;
        }
        width = 22;
        offsetx = -1;
        break;
    case 51: // Fuzzy
        if(getNybble(7) == 1)
        {
            width = 91;
            height = 83;
            offsetx = -21;
            offsety = -23;
        }
        else
        {
            width = 45;
            height = 41;
            offsetx = -2;
            offsety = -1;
        }
        break;
    case 52: // Checkpoint Flag
        width = 37;
        height = 65;
        offsetx = 1;
        offsety = -25;
        break;
    case 56: // Coin Bubble
        width = 30;
        height = 30;
        offsetx = -5;
        offsety = -5;
        break;
    case 61: // Red Coin
        if(getNybble(9) >= 1)
        {
            width = 24;
            height = 36;
            offsetx = -2;
            offsety = -16;
        }
        else
        {
            width = 20;
            height = 20;
        }
        break;
    case 63: // Skewer Left
        width = 557;
        height = 82;
        break;
    case 64: // Skewer Right
        width = 557;
        height = 82;
        offsetx = -537;
        break;
    case 65: // Morton Pipe
        width = 80;
        height = 180;
        offsetx = -40;
        offsety = -180;
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
    case 74: // Ghost House Door - Event triggered
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
    case 78: // Firebar
        if (getNybble(8) == 1)
        {
            width = 40;
            offsetx = -10;
        }
        renderOffsetH = (getNybble(5)*40) + 20;
        renderOffsetW = (getNybble(5)*40) + 20;
        renderOffsetX = offsetx - (getNybble(5)*20);
        renderOffsetY = offsety - (getNybble(5)*20);
        break;
    case 81: // Fireball Pipe - ! Junction
        width = 40;
        height = 40;
        break;
    case 82: // Fire Snake
        width = 19;
        height = 26;
        offsety = -5;
        break;
    case 83: // Fish Bone
        width = 33;
        height = 21;
        offsetx = -4;
        break;
    case 89: // porcupuffer
        width = 61;
        height = 59;
        offsetx = -21;
        offsety = -20;
        break;
    case 91: // Coin Coffer
        width = 33;
        height = 33;
        offsetx = -27;
        offsety = -13;
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
    case 94: // Flipper (One way gate)
        {
            int sizeInc = 0;
            if (getNybble(4) != 1)
                sizeInc = 7;
            switch (getNybble(5))
            {
            case 0: case 1:
                offsety = -40 - sizeInc;
                Q_FALLTHROUGH();
            case 2: case 3:
                width = 20;
                height = 60 + sizeInc;
                break;
            case 6: case 7:
                offsetx = -40 - sizeInc;
                Q_FALLTHROUGH();
            case 4: case 5:
                width = 60 + sizeInc;
                height = 20;
                break;
            default:
                break;
            }
        }
        break;
    case 95: // Blooper
        width = 28;
        height = 36;
        offsetx = -4;
        offsety = -9;
        break;
    case 96: // Blooper Nanny
        width = 50;
        height = 50;
        offsetx = -14;
        offsety = -16;
        break;
    case 97: // End of Level Flag
        width = 62;
        height = 200;
        offsetx = -22;
        renderOffsetW = 280;
        break;
    case 99: // Wiggler
        width = 89;
        height = 41;
        offsety = -20;
        break;
    case 102: // Iggy
        width = 72;
        height = 71;
        offsetx = 15;
        offsety = 89;
        break;
    case 104: // Lakitu
        width = 36;
        height = 52;
        break;
    case 107: // Path-Controlled Climbable fence
        width = 240;
        height = 240;
        offsetx = -120;
        offsety = -120;
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
    case 113: // Flame Chomp
        width = 55;
        height = 55;
        offsetx = -4;
        offsety = -32;
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
    case 116: // Bullet Bill Turret
        width = 32;
        height = 20 + (getNybble(9)*20);
        if (getNybble(9) >= 7)
            height = 160;
        offsetx = -6;
        offsety = -height+20;
        break;
    case 117: // Banzai Bill Launcher
        width = 80;
        height = 103;
        offsety = -3;
        break;
    case 118: // Bullet Bill
        if(getNybble(5) == 1)
        {
            width = 26;
            height = 20;
            offsety = 2;
            offsetx = -6;
        }
        else if(getNybble(5) == 2)
        {
            width = 20;
            height = 26;
            offsetx = -2;
        }
        else if(getNybble(5) == 3)
        {
            width = 20;
            height = 26;
            offsety = -6;
            offsetx = 2;
        }
        else if(getNybble(5) == 4)
        {
            width = 25;
            height = 25;
            offsetx = -5;
        }
        else if(getNybble(5) == 5)
        {
            width = 25;
            height = 25;
            offsety = 20;
            offsetx = -20;
        }
        else if(getNybble(5) == 6)
        {
            width = 25;
            height = 25;
            offsety = -5;
        }
        else if(getNybble(5) == 7)
        {
            width = 25;
            height = 25;
            offsety = -25;
            offsetx = 15;
        }
        else
        {
            width = 26;
            height = 20;
            offsety = 2;
        }
        break;
    case 119: // Banzai bill
        width = 108;
        height = 80;
        offsety = -20;
        offsetx = -46;
        break;
    case 120: // Up-Down Mushroom
        width = getNybble(5)*20 + 40;
        height = getNybble(4)*20 + 30;
        offsetx = 10-(width / 2);
        break;
    case 121: case 122: // Expanding Mushroom Platforms
        height = getNybble(5)*20 + 60;
        if(getNybble(4) %2 == 0)
        {
            width = 40;
            offsetx = -10;

            if(getNybble(7) %2 == 0)
            {
                renderOffsetW = 200;
                renderOffsetX = -90;
            }
            else
            {
                renderOffsetW = 120;
                renderOffsetX = -50;
            }
        }
        else
        {
            if(getNybble(7) %2 == 0)
            {
                width = 200;
                offsetx = -90;
            }
            else
            {
                width = 120;
                offsetx = -50;
            }
        }
        break;
    case 123: // Bouncy Mushroom Platform
        if(getNybble(15) == 1)
        {
            width = 160;
            height = 30;
            offsetx = -70;
            offsety = 10;
        }
        else
        {
            width = 120;
            height = 30;
            offsetx = -50;
            offsety = 10;
        }

        break;
    case 124: // Mushroom Platform
        width = 68 + 20*getNybble(6);
        height = 100 + 20*getNybble(10);
        if (getNybble(8) == 2)
        {
            switch (getNybble(9))
            {
                case 1: case 5: case 9: case 13:
                    height = 100 + 20*getNybble(10)+20;
                    offsety = -20;
                    break;
                case 2: case 6: case 10: case 14:
                    height = 100 + 20*getNybble(10)+40;
                    offsety = -40;
                    break;
                case 3: case 7: case 11: case 15:
                    height = 100 + 20*getNybble(10)+60;
                    offsety = -60;
                    break;
                default:
                    height = 100 + 20*getNybble(10);
                    break;
            }
        }
        offsetx = 10 - (width / 2);
        break;
    case 125: // Bowser
        width = 138;
        height = 108;
        offsetx = -58;
        offsety = -87;
        break;
    case 127: // Bowser Flame
        width = 57;
        height = 36;
        offsetx = 28;
        offsety = -20;
        break;
    case 128: // Dry Bowser
        width = 136;
        height = 105;
        offsetx = -55;
        offsety = -85;
        break;
    case 131: case 132: // Bowser Battle Switch and Platform Controller
        width = 53;
        height = 46;
        offsetx = -27;
        offsety = -44;
        break;
    case 133: // Bowser Shutter
        width = 36;
        height = 120;
        offsety = 20;
        break;
    case 134: // Bowser Bridge
        width = 402;
        height = 52;
        break;
    case 135: // Goomba
        width = 21;
        height = 24;
        offsetx = -1;
        offsety = -4;
        break;
    case 136: // Bone Goomba
        if (getNybble(14) != 1)
        {
            width = 22;
            height = 24;
            offsetx = -2;
            offsety = -4;
        }
        else
        {
            width = 24;
            height = 28;
            offsetx = -2;
            offsety = -8;
        }
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
        if(getNybble(5) == 0)
            height = 46;
        else
        height = getNybble(5) * 21 + 4;
        offsety = - height + 21 + 1;
        break;
    case 140: // Crowber
        width = 31;
        height = 21;
        offsetx = -5;
        offsety = -6;
        break;
    case 141: // Larry
        width = 62;
        height = 63;
        offsetx = -35;
        offsety = -33;
        break;
    case 142: // Lemmy
        width = 63;
        height = 81;
        offsetx = -34;
        offsety = -61;
        break;
    case 143: // Conveyor Belt Switch
        width = 28;
        height = 42;
        offsetx = -4;
        offsety = -21;
        break;
    case 144: case 145: // Horizontal/Vertical Lift
        if(getNybble(5) == 0)
            width = 40;
        else
            width = (getNybble(5)-1)*20 + 40;
        height = 22;
        //Down and right
        if (getNybble(9) == 1)
        {
            if(getid() == 145)
                renderOffsetH = (getNybble(7)*20);
            else
                renderOffsetW = -(getNybble(7)*20);
        }
        else
        {
            if(getid() == 145)
                renderOffsetH = -(getNybble(7)*20);
            else
                renderOffsetW = (getNybble(7)*20);
        }

        //renderOffsetX = width/2;
        //renderOffsetY = height/2;
        break;
    case 146: // Track Controlled Lift
        if(getNybble(15) == 0)
            width = 84;
        else if(getNybble(15) == 1)
            width = 35;
        else
            width = (getNybble(15)-2)*20 + 44;
        height = 22;
        offsetx = 40 -(width / 2);
        offsety = 10;
        break;
    case 147: // 3 plat rickshaw
        if (getNybble(5) != 1)
        {
            width = 128;
            height = 89;
            offsetx = -60;
            offsety = -60;
        }
        else
        {
            width = 182;
            height = 121;
            offsetx = -90;
            offsety = -80;
        }
        break;
    case 148: //move While On Lift
        width = 249;
        height = 34;
        offsetx = -6;
        offsety = -6;
        break;
    case 149: // Changeable direction platform
        width = 126;
        height = 27;
        offsetx = -3;
        break;
    case 150: // Seesaw Lift
        width = 280;
        height = 20;
        offsetx = 20;
        break;
    case 151: // Scale Lift
        if(getNybble(5) == 0)
        {
            width = 64;
            offsetx = -(width/2);
        }
        else
        {
            if(getNybble(15) < 2)
            {
                width = 64+((getNybble(5))*20);
                offsetx = -32;
            }
            else
            {
                for (int i = 0; i < getNybble(15); i++)
                {
                    width = 64+((getNybble(5))*20)+(i*20);
                    offsetx = -32-(i*10);
                }
            }
        }

        if(getNybble(7) >= getNybble(4))
            height = 40+(getNybble(7)*20);
        else
            height = 40+(getNybble(4)*20);

        offsety = -20;
        break;
    case 152: // Path Controlled Lift With Peepa
        if(getNybble(5) == 0)
            width = 46;
        else
            width = getNybble(5)*20 + 46;
        height = 22;

        offsetx = -(width/2);
        renderOffsetW = 40;
        renderOffsetH = 40;
        break;
    case 154: // 4 plat rickshaw
        if (getNybble(5) == 1)
        {
            width = 116;
            height = 90;
            offsetx = -48;
            offsety = -34;
        }
        else
        {
            width = 174;
            height = 130;
            offsetx = -80;
            offsety = -54;
        }
        break;
    case 155: // Ludwig
        width = 53;
        height = 57;
        offsetx = -52;
        offsety = -37;
        break;
    case 159: // Spike Top
        if(getNybble(4) == 1)
        {
            width = 25;
            height = 20;
            offsetx = -5;
        }
        else if(getNybble(4) == 2)
        {
            width = 20;
            height = 25;
        }
        else if(getNybble(4) == 3)
        {
            width = 25;
            height = 20;
        }
        else
        {
            width = 20;
            height = 25;
            offsety = -5;
        }
        break;
    case 162: // Morton
        width = 67;
        height = 67;
        offsetx = -38;
        offsety = 53;
        break;
    case 163: // Climbing Koopa - Vertical
        width = 27;
        height = 37;
        offsetx = -3;
        offsety = 3;
        break;
    case 164: // Climbing Koopa - Horizontal
        height = 37;
        offsety = 3;
        if(getNybble(4) == 1)
        {
            width = 26;
            offsetx = -2;
        }
        else
        {
            width = 27;
            offsetx = -3;
        }
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
    case 168: // Fire Pipe Piranha Plant - Down
        width = 30;
        height = 55;
        offsetx = 5;
        offsety = 40;
        break;
    case 169: // Fire Pipe Piranha Plant - Left
        width = 55;
        height = 30;
        offsetx = -55;
        offsety = 5;
        break;
    case 170: // Fire Pipe Piranha Plant - Right
        width = 55;
        height = 30;
        offsetx = 40;
        offsety = 5;
        break;
    case 171: // Fire Pipe Piranha Plant - Up
        width = 30;
        height = 55;
        offsetx = 5;
        offsety = -55;
        break;
    case 172: // Pipe Bone Piranha Plant - Up
        width = 30;
        height = 34;
        offsetx = 5;
        offsety = -35;
        break;
    case 173: // Pipe Bone Piranha Plant - Left
        width = 34;
        height = 30;
        offsetx = -35;
        offsety = 5;
        break;
    case 174: // Pipe Bone Piranha Plant - Right
        width = 34;
        height = 30;
        offsetx = 40;
        offsety = 5;
        break;
    case 175: // Grounded Piranha Plant
        width = 54;
        height = 33;
        offsetx = -8;
        if (getNybble(5) == 1)
            offsety = -3;
        else
            offsety = 10;
        break;
    case 176: // Big Grounded Piranha plant
        width = 110;
        height = 65;
        offsetx = -25;
        if (getNybble(5) != 1)
            offsety = 16;
        else
            offsety = 60;
        break;
    case 177: // Grounded Fire Piranha Plant
        width = 54;
        height = 33;
        offsetx = -8;
        if (getNybble(5) == 1)
            offsety = -3;
        else
            offsety = 10;
        break;
    case 178: // Big Grounded Fire Piranha plant
        width = 110;
        height = 65;
        offsetx = -25;
        if (getNybble(5) != 1)
            offsety = 16;
        else
            offsety = 60;
        break;
    case 179: // Grounded Bone Piranha Plant
        width = 53;
        height = 33;
        offsetx = -8;
        offsety = 8;
        if (getNybble(5) == 1)
            offsety = -3;
        break;
    case 180: // Big Grounded Bone Piranha plant
        width = 110;
        height = 68;
        offsetx = -25;
        if (getNybble(5) != 1)
            offsety = 14;
        else
            offsety = 58;
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
    case 186: // Paratroopa Circle
        width = 30 + getNybble(8)*40;
        height = 30 + getNybble(8)*40;
        offsetx = 10 - width/2;
        offsety = +10 - height/2;
        break;
    case 187: // Path controlled rect block
        width = 20 + getNybble(15)*20;
        height = 20 + getNybble(13)*20;
        break;
    case 189: case 190: case 191: case 192: case 193: case 282: // Rect Blocks
        {
            width = getNybble(15)*20+20;
            if (width == 20) width = 40;
            height = getNybble(13)*20+20;
            if (height == 20) height = 40;
            int distance = qMax((getNybble(16)*20), (getNybbleData(4, 5)*20));
            switch (getNybble(11))
            {
            case 1: case 5: case 9: case 13: // Left
                renderOffsetW = width + distance;
                renderOffsetX = -(width + distance);
                break;
            case 2: case 6: case 10: case 14: // Up
                renderOffsetH = height + distance;
                renderOffsetY = -(height + distance);
                break;
            case 3: case 7: case 11: case 15: // Down
                renderOffsetH = height + distance;
                break;
            default: // Right
                renderOffsetW = width + distance;
                break;
            }
            if (id == 192)
            {
                width += 6;
                offsetx -= 3;
            }
        break;
        }
    case 194: // Cheep Cheep
        if(getNybble(5) == 1 || getNybble(5) == 8)
        {
            width = 24;
            offsetx = -3;
        }
        else
            width = 22;
        height = 20;
        break;
    case 195: // Big Cheep Cheep
        if(getNybble(5) == 1)
        {
            width = 62;
            height = 58;
            offsety = -7;
            offsetx = 2;
        }
        else
        {
            width = 60;
            height = 56;
            offsety = -5;
        }
        break;
    case 199: // Cheep Cheep Circle
        width = 30 + getNybble(8)*40;
        height = 30 + getNybble(8)*40;
        offsetx = 10 - width/2;
        offsety = +10 - height/2;
        break;
    case 200: // Spiny Cheep Cheep
        width = 20;
        height = 26;
        offsety = -6;
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
    case 207: // Underwater Rock
        width = 80;
        height = 82;
        offsetx = 10;
        offsety = 1;
        break;
    case 209: // Swinging Rope
        width = 20;
        height = (getNybble(5)+4)*20;
        break;
    case 210: // Tightrope
        if (getNybble(10) == 0)
        {
            width = 20;
        }
        else
        {
            width = 20 + getNybble(10)*20;
        }
        int offset;
        if (getNybble(11) == 0)
        {
            height = 20;
            offsety = -10;
        }
        else if (getNybble(11) <= 7)
        {
            for(offset = 0; offset < getNybble(11); offset++)
            {
                height = (offset*20)+40;
                offsety = -(offset*20)-30;
            }
        }
        else
        {
            offset = 160;
            for (int i = 8; i != getNybble(11); i++) offset -= 20;
            height = offset+20;
            offsety = -10;
        }
        offsetx = -10;
        break;
    case 211: // Roy Koopa
        width = 78;
        height = 47;
        offsetx = -46;
        offsety = -27;
        break;
    case 212: // Roy Wall
        width = 99;
        height = 200;
        if(getNybble(6) == 1)
            offsetx = 40;
        else
            offsetx = -40;
        break;
    case 213: // Pokey
        width = 33;
        height = 57 + ((getNybble(5)+1)*22);
        offsetx = -7;
        offsety = -height +22;
        break;
    case 217: case 218: // SnakeBlock
        width = 20;
        height = 20;
        offsety = -20;
        break;
    case 215: // Bob-omb Cannon
        {
            switch (getNybble(4))
            {
            case 0:
                width = 31;
                height = 32;
                offsetx = -1;
                offsety = -12;
                if (getNybble(5) == 1)
                    offsetx = -10;
                break;
            case 1:
                width = 31;
                height = 32;
                offsetx = -1;
                if (getNybble(5) == 1)
                    offsetx = -10;
                break;
            case 2:
                width = 31;
                height = 34;
                offsetx  = -1;
                offsety = -9;
                if (getNybble(5) == 1)
                {
                    width = 34;
                    height = 31;
                    offsetx = 5;
                    offsety = 0;
                }
                break;
            case 3:
                width = 34;
                height = 31;
                offsetx = -19;
                if (getNybble(5) == 1)
                {
                    width = 31;
                    height = 34;
                    offsetx = -10;
                    offsety = -9;
                }
                break;
            case 4:
                width = 34;
                height = 31;
                offsetx = -19;
                break;
            case 5:
                width = 34;
                height = 31;
                offsetx = 5;
                break;
            case 6: case 10: case 11:
                width = 30;
                height = 24;
                offsetx = -20;
                offsety = 8;
                break;
            case 7: case 12: case 13:
                width = 30;
                height = 24;
                offsetx = 10;
                offsety = 8;
                break;
            case 8:
                width = 32;
                height = 31;
                offsetx = -22;
                if (getNybble(5) == 1)
                    offsety = 10;
                break;
            case 9:
                width = 32;
                height = 31;
                offsetx = 10;
                if (getNybble(5) == 1)
                    offsety = 9;
                break;
            case 14: case 15:
                width = 24;
                height = 30;
                offsetx = -1;
                offsety = -10;
                break;
            default:
                break;
            }
            break;
        }
    case 216: // Boss Shutter
        width = 34;
        height = 60;
        offsetx = 3;
        offsety = 20;
        break;
    case 219: case 220: // Star Coin
        width = 40;
        height = 40;
        break;
    case 221: case 223: case 225: // Switches
        width = 22;
        height = 24;
        offsetx = -1;
        if (getNybble(5) != 1) offsety = -4;
        if (getNybble(4) == 1) offsetx = +9;
        break;
    case 227: // Floating Barrel
        width = 60;
        height = 61;
        offsety = 10;
        break;
    case 228: // Boo
        width = 45;
        height = 44;
        offsetx = -12;
        offsety = 15;
        break;
    case 229: //Big Boo
        width = 132;
        height = 132;
        offsetx = -16;
        offsety = 15;
        break;
    case 230: // Peepa Circle
        width = 30 + getNybble(8)*40;
        height = 30 + getNybble(8)*40;
        offsetx = 10 - width/2;
        offsety = +10 - height/2;
        break;
    case 232: // Spiny
        if(getNybble(5) == 1)
        {
            width = 22;
            height = 19;
        }
        else if(getNybble(5) == 2)
        {
            width = 21;
            height = 20;
        }
        else if(getNybble(5) == 3)
        {
            width = 21;
            height = 20;
        }
        else
        {
            width = 22;
            height = 19;
        }
        break;
    case 233: // Upside Down Spiny
        width = 21;
        height = 20;
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
        offsetx = 23;
        offsety = 3;
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
    case 242: // Whirlpool
        width = 200;
        height = 150;
        offsetx = -90;
        offsety = -110;
        break;
    case 244: // Chain Chomp
        width = 63;
        height = 63;
        offsetx = -41;
        break;
    case 245: // Wendy
        width = 55;
        height = 49;
        offsetx = -40;
        offsety = -29;
        break;
    case 247: // Invisible Bouncy Block
        width = 20 + (getNybble(10)*20);
        height = 20;
        break;
    case 248: // Move Once On Ghost House Platform
        width = getNybble(5)*20 + 40;
        height = getNybble(6)*20 + 40;
        break;
    case 249: // Rotation Controlled Rectangle Lift - Tower
        width = getNybble(15)*20+60;
        height = getNybble(13)*20+60;
        break;
    case 250: // Rotation Controlled Rectangle Lift - GhostHouse (checkered)
        width = getNybble(15)*20+60;
        height = getNybble(13)*20+60;
        break;
    case 251: // Treasure Chest
        width = 48;
        height = 32;
        offsetx = -14;
        offsety = -12;
        break;
    case 252: // Item
        switch (getNybble(10))
        {
        case 2: case 5:
            width = 26;
            height = 24;
            offsetx = -2;
            offsety =-4;
            break;
        case 4:
            width = 24;
            height = 21;
            offsetx = -2;
            offsety = -1;
            break;
        case 6:
            width = 20;
            height = 20;
            break;
        case 7:
            width = 24;
            height = 23;
            offsetx = -3;
            offsety = -3;
            break;
        case 9:
            width = 49;
            height = 41;
            offsetx = -14;
            offsety = -21;
            break;
        default:
            width = 23;
            height = 22;
            offsetx = -3;
            offsety = -2;
            break;
        }
        break;
    case 253: // Larry Battle Platform
        height = 10;
        if(getNybble(10) == 0)
            width = 20;
        else
            width = (getNybble(10)*20);
        offsety = +10;
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
    case 268: // Rectangle Lift - Lava
    {
        width = 250;
        height = 80;
        if (getNybble(13) == 1)
        {
            width = 100;
            height = 603;
        }

        int distance = qMax((getNybbleData(15, 16)*20), (getNybbleData(4, 5)*20));
        switch (getNybble(11))
        {
        case 1: case 5: case 9: case 13: // Left
            renderOffsetW = width + distance;
            renderOffsetX = -(width + distance);
            break;
        case 2: case 6: case 10: case 14: // Up
            renderOffsetH = height + distance;
            renderOffsetY = -(height + distance);
            break;
        case 3: case 7: case 11: case 15: // Down
            renderOffsetH = height + distance;
            break;
        default: // Right
            renderOffsetW = width + distance;
            break;
        }
        break;
    }
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
    case 272: // Peach Cage
        width = 68;
        height = 108;
        offsety = 40;
        break;
    case 273: // Coin Roulette Block
        width = 30;
        height = 30;
        offsetx = -5;
        offsety = -5;
        if (getNybble(13) == 2)
        {
            width = 46;
            height = 46;
            offsetx = -13;
            offsety = -13;
        }
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
    case 281: // Rectangle Ice Lift
        width = 280;
        height = 60;
        break;
    case 283: // Rectangle Lift Larry Battle
        {
        width = 60;
        height = 100;
        int distance = getNybble(16)*20;
        switch (getNybble(11))
        {
        case 1: case 5: case 9: case 13: // Left
            renderOffsetW = width + distance;
            renderOffsetX = -(width + distance);
            break;
        case 2: case 6: case 10: case 14: // Up
            renderOffsetH = height + distance;
            renderOffsetY = -(height + distance);
            break;
        case 3: case 7: case 11: case 15: // Down
            renderOffsetH = height + distance;
            break;
        default: // Right
            renderOffsetW = width + distance;
            break;
        }
        break;
        }
    case 284: // Chandelier Lift - Small
        width = 170;
        height = 50;
        offsetx = -75;
        offsety = -25;
        break;
    case 285: // Chandelier Lift - Medium
        width = 270;
        height = 110;
        offsetx = -125;
        offsety = -45;
        break;
    case 286: // Chandelier Lift - Big
        width = 410;
        height = 310;
        offsetx = -195;
        offsety = -165;
        break;
    case 287: // Toad House Door
        width = 40;
        height = 60;
        break;
    case 289: // Bouncy Mushroom Platform - Castle
        if (getNybble(15) == 1)
        {
            width = 40;
            offsetx = -10;
        }
        else
        {
            width = 100;
            offsetx = -50;
        }
        height = 30;
        offsety = 15;
        break;
    case 290: // Path Controlled Fence(Small)
        width = 60;
        height = 60;
        offsetx = -30;
        offsety = -30;
        break;
    case 291: // Path Controlled Fence(Big)
        width = 120;
        height = 120;
        offsetx = -60;
        offsety = -60;
        break;
    case 292: // Warp Cannon Signboard
        width = 62;
        height = 86;
        offsetx = -20;
        offsety = -46;
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
        offsety = -19;
        break;
    case 297: // Horizontal Rail Controlled Fence
        if (getNybble(5) == 1)
        {
            width = 63;
            height = 144;
            if(getNybble(7) == 3)
                offsety = -135;
            else
                offsety = -11;
        }
        else if (getNybble(5) == 2)
        {
            width = 122;
            height = 83;
            if(getNybble(7) == 3)
                offsety = -75;
            else
                offsety = -10;
        }
        else
        {
            width = 63;
            height = 84;
            if(getNybble(7) == 3)
                offsety = -75;
            else
                offsety = -11;
        }

        break;
    case 299: // Rectangle Lift - Lemmy Battle
        width = 40;
        height = 40;
        break;
    case 302: case 303: // Moon Coin
        width = 40;
        height = 40;
        break;
    case 305: // Path Controlled Ice Lift
        if(getNybble(5) == 1)
        {
            width = 409;
            height = 140;
            offsetx = 15;
        }
        else if(getNybble(5) == 2)
        {
            width = 248;
            height = 200;
            offsetx = 36;
            offsety = -80;
        }
        else
        {
            width = 308;
            height = 140;
            offsetx = 56;
        }
        break;
    case 307: // Event Activated Rect Lift - Ruins
        {
            width = 80;
            height = 80;

            int distance = getNybbleData(4, 5)*20;
            switch (getNybble(11))
            {
            case 1: case 5: case 9: case 13: // Left
                renderOffsetW = width + distance;
                renderOffsetX = -(width + distance);
                break;
            case 2: case 6: case 10: case 14: // Up
                renderOffsetH = height + distance;
                renderOffsetY = -(height + distance);
                break;
            case 3: case 7: case 11: case 15: // Down
                renderOffsetH = height + distance;
                break;
            default: // Right
                renderOffsetW = width + distance;
                break;
            }
            break;
        }
    case 308: // Event Activated Rect Lift - Sand
        {
            width = 120;
            height = 140;

            int distance = getNybbleData(4, 5)*20;
            switch (getNybble(11))
            {
            case 1: case 5: case 9: case 13: // Left
                renderOffsetW = width + distance;
                renderOffsetX = -(width + distance);
                break;
            case 2: case 6: case 10: case 14: // Up
                renderOffsetH = height + distance;
                renderOffsetY = -(height + distance);
                break;
            case 3: case 7: case 11: case 15: // Down
                renderOffsetH = height + distance;
                break;
            default: // Right
                renderOffsetW = width + distance;
                break;
            }
            break;
        }
    case 311: // Coin Meteor
        if(getNybble(5) == 1)
        {
            width = 60;
            height = 60;
            offsetx = 30;
            offsety = -30-(getNybble(7)*60);
        }
        else
        {
            width = 30;
            height = 30;
            offsetx = 20;
            offsety = -20-(getNybble(7)*30);
        }
        break;
    case 313: // Underwater Rectangle Lift
        {
            if (getNybble(13) == 1)
            {
                width = 80;
                height = 180;
            }
            else
            {
                width = 40;
                height = 220;
            }
            int distance = getNybbleData(4, 5)*20;
            switch (getNybble(11))
            {
            case 1: case 5: case 9: case 13: // Left
                renderOffsetW = width + distance;
                renderOffsetX = -(width + distance);
                break;
            case 2: case 6: case 10: case 14: // Up
                renderOffsetH = height + distance;
                renderOffsetY = -(height + distance);
                break;
            case 3: case 7: case 11: case 15: // Down
                renderOffsetH = height + distance;
                break;
            default: // Right
                renderOffsetW = width + distance;
                break;
            }
            break;
        }
    case 314: // Ruins rickshaw
        if (getNybble(5) != 1)
        {
            width = 128;
            height = 89;
            offsetx = -60;
            offsety = -60;
        }
        else
        {
            width = 166;
            height = 110;
            offsetx = -90;
            offsety = -80;
        }
        break;
    case 315: // Peach
        width = 33;
        height = 56;
        offsetx = 5;
        offsety = 24;
        break;
    case 318: // Event Controlled Rectangle Lift
        {
            width = 320;
            switch (getNybble(13))
            {
            case 1:
                height = 40;
                break;
            case 2:
                height = 80;
                break;
            default:
                height = 60;
                break;
            }
            int distance = getNybbleData(4, 5)*20;
            switch (getNybble(11))
            {
            case 1: case 5: case 9: case 13: // Left
                renderOffsetW = width + distance;
                renderOffsetX = -(width + distance);
                break;
            case 2: case 6: case 10: case 14: // Up
                renderOffsetH = height + distance;
                renderOffsetY = -(height + distance);
                break;
            case 3: case 7: case 11: case 15: // Down
                renderOffsetH = height + distance;
                break;
            default: // Right
                renderOffsetW = width + distance;
                break;
            }
            break;
        }
    case 321: // Snow Mushroom Platform
        width = 68 + 20*getNybble(6);
        height = 100 + 20*getNybble(9);
        offsetx = 10 - (width / 2);
        break;
    case 322: // Big Grinder
        width = 86;
        height = 86;
        offsetx = -23;
        offsety = -23;
        break;
    case 326: // Mr. Sun
        width = 73;
        height = 73;
        offsetx = -26;
        offsety = -17;
        break;
    default:
        width = 20;
        height = 20;
        offsetx = 0;
        offsety = 0;
    }
}

qint32 Sprite::getType() const { return 1; }
qint32 Sprite::getid() const { return id; }

quint8 Sprite::getByte(qint32 id) const { return spriteData[id]; }
void Sprite::setByte(qint32 id, quint8 nbr) { spriteData[id] = nbr; }

quint8 Sprite::getNybble(qint32 id) const
{
    if (id%2 == 0) return spriteData[id/2] >> 4;
    else return spriteData[id/2] & 0xF;
}

void Sprite::setNybble(qint32 id, quint8 nbr)
{
    if (id%2 == 0) spriteData[id/2] = ((spriteData[id/2] & 0xF) | nbr << 4);
    else spriteData[id/2] = ((spriteData[id/2] & 0xF0) | nbr);
}

qint32 Sprite::getNybbleData(qint32 startNybble, qint32 endNybble) const
{
    qint32 data = 0;

    for (qint32 i = startNybble; i <= endNybble; i++)
        data = data << 4 | getNybble(i);

    return data;
}

void Sprite::setNybbleData(qint32 data, qint32 startNybble, qint32 endNybble)
{
    for (qint32 i = endNybble; i >= startNybble; i--)
    {
        setNybble(i, (quint8)(data & 0xF));
        data = data >> 4;
    }
}

// Format: 1:ID:X:Y:SD0:SD1:...:SD11
QString Sprite::toString(qint32 xOffset, qint32 yOffset) const
{
    QString str("1:%1:%2:%3");
    for (qint32 i=0; i<12; i++) str.append(QString(":%1").arg(spriteData[i]));
    return str.arg(id).arg(x+xOffset).arg(y+yOffset);
}


// Entrance
Entrance::Entrance(qint32 x, qint32 y, qint16 cameraX, qint16 cameraY, quint8 id, quint8 destArea, quint8 destEntr, quint8 entrType, quint8 settings, quint8 unk1, quint8 unk2)
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
    setRect();
}

void Entrance::setRect()
{
    width = 20;
    height = 20;
    offsetx = 0;
    offsety = 0;

    switch(entrType)
    {
    case 3: case 4: // Pipe up & Pipe down
        width = 40;
        break;
    case 5: case 6: // Pipe left & Pipe right
        height = 40;
        break;
    case 11: case 13: // Mispositioned Entrances
        offsetx = -10;
        offsety = -20;
        break;
    case 12: // Mispositioned Entrance
        offsety = -20;
        break;
    default:
        width = 20;
        height = 20;
        offsetx = 0;
        offsety = 0;
        break;
    }
}

// Format: 2:ID:Type:X:Y:DestArea:DestEntr:CamX:CamY:Settings
QString Entrance::toString(qint32 xOffset, qint32 yOffset) const
{
    return QString("2:%1:%2:%3:%4:%5:%6:%7:%8:%9:%10:%11").arg(id).arg(entrType).arg(x+xOffset).arg(y+yOffset).arg(destArea)
            .arg(destEntr).arg(cameraX).arg(cameraY).arg(settings).arg(unk1).arg(unk2);
}


// Zone
Zone::Zone(qint32 x, qint32 y, qint32 width, qint32 height, quint8 id, quint8 progPathId, quint8 musicId, quint8 multiplayerTracking, quint16 unk1)
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
    bgXPos = zone->getBgXPos();
    bgYPos = zone->getBgYPos();
    bgName = zone->getBgName();
    bgUnk1 = zone->getBgUnk1();
}

bool Zone::clickDetection(qint32 xcheck, qint32 ycheck)
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

    this->bgXPos = background.xPos;
    this->bgYPos = background.yPos;
    this->bgName = background.name;
    this->bgUnk1 = background.unk1;
}

QString Zone::toString(qint32 xOffset, qint32 yOffset) const
{
    return QString("3:%1:%2:%3:%4:%5:%6:%7:%8:%9:%10:%11:%12:%13:%14:%15:%16:%17:%18:%19").arg(x+xOffset).arg(y+yOffset)
            .arg(width).arg(height).arg(id).arg(progPathId).arg(musicId).arg(multiplayerTracking).arg(unk1).arg(upperBound)
            .arg(lowerBound).arg(unkUpperBound).arg(unkLowerBound).arg(upScrolling).arg(bgYPos)
            .arg(bgXPos).arg(bgName).arg(bgUnk1);
}


// Location
Location::Location(qint32 x, qint32 y, qint32 width, qint32 height, qint32 id)
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

qint32 Location::getType() const { return 4; }
qint32 Location::getid() const { return id; }

// Format: 4:ID:X:Y:Width:Height
QString Location::toString(qint32 xOffset, qint32 yOffset) const { return QString("4:%1:%2:%3:%4:%5").arg(id).arg(x+xOffset).arg(y+yOffset).arg(width).arg(height); }


// Path
Path::Path(quint16 id, quint16 loop)
{
    this->id = id;
    this->loop = loop;
}

Path::Path(Path *path)
{
    id = path->getid();
    loop = path->getLoop();
    foreach (PathNode* node, path->getNodes())
        nodes.append(new PathNode(node, this));
}

void Path::insertNode(PathNode* node, qint32 index)
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

QString Path::toString(qint32 xOffset, qint32 yOffset)
{
    QString output = QString("5:%1:%2:").arg(getid()).arg(getLoop());

    QString nodeString;

    int i = 0;
    foreach (PathNode* node, getNodes())
    {
        nodeString.append(QString::number(getIndexOfNode(node)) + ",");
        nodeString.append(QString::number(node->getx() + xOffset) + ",");
        nodeString.append(QString::number(node->gety() + yOffset) + ",");
        nodeString.append(QString::number(node->getSpeed()) + ",");
        nodeString.append(QString::number(node->getAccel()) + ",");
        nodeString.append(QString::number(node->getUnk1()));

        i++;

        if (i < getNumberOfNodes())
            nodeString.append(";");
    }

    if (xOffset > 0)
        qDebug() << nodeString;

    output.append(nodeString);

    return output;
}

QList<PathNode*> Path::getNodes() const { return nodes; }

// Path Node
PathNode::PathNode(qint32 x, qint32 y, float speed, float accel, quint32 unk1, Path* parentPath)
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

QString PathNode::toString(qint32 xOffset, qint32 yOffset) const
{
    Path path = getParentPath();
    return path.toString(xOffset, yOffset);
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

void ProgressPath::insertNode(ProgressPathNode* node, qint32 index)
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

QString ProgressPath::toString(qint32 xOffset, qint32 yOffset)
{
    QString output = QString("6:%1:%2:").arg(getid()).arg(getAlternatePathFlag());

    QString nodeString;

    int i = 0;
    foreach (ProgressPathNode* node, getNodes())
    {
        nodeString.append(QString::number(getIndexOfNode(node)) + ",");
        nodeString.append(QString::number(node->getx() + xOffset) + ",");
        nodeString.append(QString::number(node->gety() + yOffset));

        i++;

        if (i < getNumberOfNodes())
            nodeString.append(";");
    }

    if (xOffset > 0)
        qDebug() << nodeString;

    output.append(nodeString);

    return output;
}

QList<ProgressPathNode*> ProgressPath::getNodes() const { return nodes; }

// Progress Path Node
ProgressPathNode::ProgressPathNode(qint32 x, qint32 y, ProgressPath *parentPath)
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

QString ProgressPathNode::toString(qint32 xOffset, qint32 yOffset) const
{
    ProgressPath progPath = getParentPath();
    return progPath.toString(xOffset, yOffset);
}
