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

    layerMask = 0x7; // failsafe
    selType = 0;
}


void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect drawrect = evt->rect();

    //qDebug("draw %d,%d %d,%d", drawrect.x(), drawrect.y(), drawrect.width(), drawrect.height());

    painter.fillRect(drawrect, QColor(119,136,153));
    tileGrid.clear();

    // Render Tiles
    for (int l = 1; l >= 0; l--)
    {
        if (!(layerMask & (1<<l)))
            continue;

        tileGrid[0xFFFFFFFF] = l+1;

        for (int i = level->objects[l].size()-1; i >= 0; i--)
        {
            const BgdatObject& obj = level->objects[l].at(i);

            // don't draw shit that is outside of the view
            // (TODO: also eliminate individual out-of-view tiles)
            if (!drawrect.intersects(QRect(obj.getx()*20, obj.gety()*20, obj.getwidth()*20, obj.getheight()*20)))
                continue;

            quint16 tsid = (obj.getid() >> 12) & 0x3;
            if (level->tilesets[tsid])
            {
                level->tilesets[tsid]->drawObject(painter, tileGrid, obj.getid()&0x0FFF, obj.getx(), obj.gety(), obj.getwidth(), obj.getheight(), 1);
            }
            else
            {
                // TODO fallback
                qDebug("attempt to draw obj %04X with non-existing tileset", obj.getid());
            }
        }
    }


    // Render Sprites
    for (int i = 0; i < level->sprites.size(); i++)
    {
        const Sprite& spr = level->sprites.at(i);

        QRect sprrect(spr.getx()/16*20, spr.gety()/16*20, 20, 20);

        painter.setPen(QColor(0,0,0));

        QPainterPath path;
        path.addRoundedRect(sprrect, 2.0, 2.0);
        QColor color(0,90,150,200);
        painter.fillPath(path, color);
        painter.drawPath(path);

        QString spriteText = QString("%1").arg(spr.getid());
        painter.setFont(QFont("Arial", 7, QFont::Normal));
        painter.drawText(sprrect, spriteText, Qt::AlignHCenter | Qt::AlignVCenter);
    }

    if (selType == 1)
    {
        QRect objrect(selObject->getx()*20, selObject->gety()*20, selObject->getwidth()*20, selObject->getheight()*20);

        objrect.adjust(-1, -1, 0, 0);
        painter.setPen(QColor(0,0,0));
        painter.drawRect(objrect);

        objrect.adjust(1, 1, -1, -1);
        painter.setPen(QColor(255,255,255));
        painter.drawRect(objrect);
    }
}


void LevelView::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    int x = evt->x() / 20;
    int y = evt->y() / 20;

    selType = 0;

    for (int l = 0; l < 2; l++)
    {
        if (!(layerMask & (1<<l)))
            continue;

        for (int i = level->objects[l].size()-1; i >= 0; i--)
        {
            BgdatObject& obj = level->objects[l][i];

            if (x >= obj.getx() && x < obj.getx()+obj.getwidth() && y >= obj.gety() && y < obj.gety()+obj.getheight())
            {
                // hit!
                selType = 1;
                selObject = &obj;

                //dragX = evt->x() - (obj.x*20);
                //dragY = evt->y() - (obj.y*20);
                dragX = x - obj.getx();
                dragY = y - obj.gety();

                qDebug("OBJ %04X", obj.getid());

                break;
            }
        }

        if (selType) break;
    }

    update();
}


void LevelView::mouseMoveEvent(QMouseEvent* evt)
{
    if (evt->buttons() != Qt::LeftButton) // checkme?
        return;

    if (selType == 1)
    {
        //selObject->x = (evt->x() - dragX) / 20;
        //selObject->y = (evt->y() - dragY) / 20;
        int finalX = (evt->x() / 20) - dragX;
        int finalY = (evt->y() / 20) - dragY;

        // clamp coords
        if (finalX < 0) finalX = 0;
        else if (finalX > 0xFFFF) finalX = 0xFFFF;
        if (finalY < 0) finalY = 0;
        else if (finalY > 0xFFFF) finalY = 0xFFFF;

        selObject->setPosition(finalX, finalY);
    }

    update();
}

