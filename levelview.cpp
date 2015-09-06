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

#include "leveleditorwindow.h"
#include "levelview.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QPaintEvent>

LevelView::LevelView(QWidget *parent, Level* level, Tileset* tileset) : QWidget(parent)
{
    this->level = level;
    this->tileset = tileset;
}


void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    QRect drawrect = evt->rect();

    //qDebug("draw %d,%d %d,%d", drawrect.x(), drawrect.y(), drawrect.width(), drawrect.height());

    painter.fillRect(drawrect, QColor(200,220,255));


    for (int i = 0; i < level->objects[0].size(); i++)
    {
        const BgdatObject& obj = level->objects[0].at(i);

        // don't draw shit that is outside of the view
        // (TODO: also eliminate individual out-of-view tiles)
        if (!drawrect.intersects(QRect(obj.x*20, obj.y*20, obj.width*20, obj.height*20)))
            continue;

        /*switch (obj.id & 0xF000)
        {
        case 0x0000: painter.setBrush(mariobrosse); break;
        case 0x1000: painter.setBrush(luigibrosse); break;
        case 0x2000: painter.setBrush(peachbrosse); break;
        case 0x3000: painter.setBrush(yoshibrosse); break;
        default: painter.setBrush(wariobrosse); break;
        }*/

        //painter.drawRect(QRect(obj.x*20, obj.y*20, obj.width*20 - 1, obj.height*20 - 1));
        /*for (int y = 0; y < obj.height; y++)
        {
            for (int x = 0; x < obj.width; x++)
            {
                tileset->drawTile(painter, x, obj.x+x, obj.y+y, 1);
            }
        }*/
        tileset->drawObject(painter, obj.id&0x0FFF, obj.x, obj.y, obj.width, obj.height, 1);
    }
}

