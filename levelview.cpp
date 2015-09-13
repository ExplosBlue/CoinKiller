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

LevelView::LevelView(QWidget *parent, Level* level) : QWidget(parent)
{
    this->level = level;
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

        quint16 tsid = (obj.id >> 12) & 0x3;
        if (level->tilesets[tsid])
        {
            level->tilesets[tsid]->drawObject(painter, obj.id&0x0FFF, obj.x, obj.y, obj.width, obj.height, 1);
        }
        else
        {
            // TODO fallback
            qDebug("attempt to draw obj %04X with non-existing tileset", obj.id);
        }
    }
}

