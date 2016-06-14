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
#include "unitsconvert.h"
#include "objectrenderer.h"

#include <QApplication>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QPaintEvent>
#include <QClipboard>
#include <QMessageBox>


LevelView::LevelView(QWidget *parent, Level* level) : QWidget(parent)
{
    this->level = level;

    layerMask = 0x7; // failsafe

    setMouseTracking(true);
    objectEditionMode = new ObjectsEditonMode(level);
    mode = objectEditionMode;

    zoom = 1;
    grid = false;
}

LevelView::~LevelView()
{
    delete objectEditionMode;
}

void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.scale(zoom,zoom);

    drawrect = QRect(evt->rect().x()/zoom, evt->rect().y()/zoom, evt->rect().width()/zoom+20, evt->rect().height()/zoom+20);

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
            const BgdatObject* obj = level->objects[l].at(i);

            // don't draw shit that is outside of the view
            // (TODO: also eliminate individual out-of-view tiles)
            if (!drawrect.intersects(QRect(obj->getx(), obj->gety(), obj->getwidth(), obj->getheight())))
                continue;

            quint16 tsid = (obj->getid() >> 12) & 0x3;
            if (level->tilesets[tsid])
            {
                level->tilesets[tsid]->drawObject(painter, tileGrid, obj->getid()&0x0FFF, obj->getx()/20, obj->gety()/20, obj->getwidth()/20, obj->getheight()/20, 1);
            }
            else
            {
                // TODO fallback
                qDebug("attempt to draw obj %04X with non-existing tileset", obj->getid());
            }
        }
    }

    painter.setRenderHint(QPainter::Antialiasing);

    // Render Locations
    for (int i = 0; i < level->locations.size(); i++)
    {
        const Location* loc = level->locations.at(i);

        QRect locrect(loc->getx(), loc->gety(), loc->getwidth(), loc->getheight());

        if (!drawrect.intersects(locrect))
            continue;

        painter.fillRect(locrect, QBrush(QColor(85,80,185,50)));

        painter.setPen(QColor(0,0,0));
        painter.drawRect(locrect);

        QString locText = QString("%1").arg(loc->getid());
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(QColor(255,255,255));
        painter.drawText(locrect.adjusted(5,5,0,0), locText);
    }

    // Render Sprites
    for (int i = 0; i < level->sprites.size(); i++)
    {
        const Sprite* spr = level->sprites.at(i);

        QRect sprRect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight());

        if (!drawrect.intersects(sprRect))
            continue;

        SpriteRenderer sprRend(spr);
        sprRend.render(&painter);
    }

    // Render Entrances
    for (int i = 0; i < level->entrances.size(); i++)
    {
        const Entrance* entr = level->entrances.at(i);

        QRect entrrect(entr->getx(), entr->gety(), 20, 20);

        if (!drawrect.intersects(entrrect))
            continue;

        EntranceRenderer entrRend(entr);
        entrRend.render(&painter);
    }

    // Render Paths
    for (int i = 0; i < level->paths.size(); i++)
    {
        const Path* path = level->paths.at(i);
        QList<PathNode*> nodes  = path->getNodes();

        for (int j = 0; j < nodes.size() - 1; j++)
        {
            QLine pathLine(QPoint(nodes[j]->getx()+10, nodes[j]->gety()+10), QPoint(nodes[j+1]->getx()+10, nodes[j+1]->gety()+10));

            if (!drawrect.intersects(QRect(pathLine.x1(), pathLine.y1(), pathLine.x2()-pathLine.x1(), pathLine.y2()-pathLine.y1())))
                continue;

            QPen pen(QColor(0,255,20));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(pathLine);
        }

        for (int j = 0; j < nodes.size(); j++)
        {
            QRect pathrect(nodes[j]->getx(), nodes[j]->gety(), 20, 20);

            if (!drawrect.intersects(pathrect))
                continue;

            painter.setPen(QColor(0,0,0));

            QPainterPath painterPath;
            painterPath.addRoundedRect(pathrect, 2.0, 2.0);
            QColor color(0,255,20,200);
            painter.fillPath(painterPath, color);
            painter.drawPath(painterPath);

            QString pathText = QString("%1-%2").arg(path->getid()).arg(j+1);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(pathrect, pathText, Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // Render Progress Paths
    for (int i = 0; i < level->progressPaths.size(); i++)
    {
        const ProgressPath* pPath = level->progressPaths.at(i);
        QList<ProgressPathNode*> nodes  = pPath->getNodes();

        for (int j = 0; j < nodes.size() - 1; j++)
        {
            QLine ppathLine(QPoint(nodes[j]->getx()+10, nodes[j]->gety()+10), QPoint(nodes[j+1]->getx()+10, nodes[j+1]->gety()+10));

            if (!drawrect.intersects(QRect(ppathLine.x1(), ppathLine.y1(), ppathLine.x2()-ppathLine.x1(), ppathLine.y2()-ppathLine.y1())))
                continue;

            QPen pen(QColor(220,255,0));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(ppathLine);
        }

        for (int j = 0; j < nodes.size(); j++)
        {
            QRect ppathrect(nodes[j]->getx(), nodes[j]->gety(), 20, 20);

            if (!drawrect.intersects(ppathrect))
                continue;

            painter.setPen(QColor(0,0,0));

            QPainterPath path;
            path.addRoundedRect(ppathrect, 2.0, 2.0);
            QColor color(220,255,0,200);
            painter.fillPath(path, color);
            painter.drawPath(path);

            QString pPathText = QString("%1-%2").arg(pPath->getid()).arg(j+1);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(ppathrect, pPathText, Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // Render Zones
    for (int i = 0; i < level->zones.size(); i++)
    {
        const Zone* zone = level->zones.at(i);

        QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

        if (!drawrect.intersects(zonerect))
            continue;

        painter.setPen(QColor(255,255,255));

        painter.drawRect(zonerect);

        QString zoneText = QString("Zone %1").arg(zone->getid());
        painter.setFont(QFont("Arial", 8, QFont::Normal));
        painter.drawText(zonerect.adjusted(3,3,0,0), zoneText);
    }

    // Render Selection
    objectEditionMode->render(&painter);

    // Render Grid
    if (grid)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);

        int startx = drawrect.x() - drawrect.x() % 20;
        int endx = startx + drawrect.width() + 20;

        int starty = drawrect.y() - drawrect.y() % 20;
        int endy = starty + drawrect.height() + 20;

        int x = startx - 20;
        while (x <= endx)
        {
            x += 20;
            if (x % 160 == 0)
            {
                painter.setPen(QPen(QColor(255,255,255,75), 2, Qt::DashLine));
                painter.drawLine(x, starty, x, endy);
            }
            else if (x % 80 == 0)
            {
                if (zoom  < 0.3) continue;
                painter.setPen(QPen(QColor(255,255,255,75), 1, Qt::DashLine));
                painter.drawLine(x, starty, x, endy);
            }
            else
            {
                if (zoom  < 0.5) continue;
                painter.setPen(QPen(QColor(255,255,255,75), 1, Qt::DotLine));
                painter.drawLine(x, starty, x, endy);
            }
        }

        int y = starty - 20;
        while (y <= endy)
        {
            y += 20;
            if (y % 160 == 0)
            {
                painter.setPen(QPen(QColor(255,255,255,75), 2, Qt::DashLine));
                painter.drawLine(startx, y, endx, y);
            }
            else if (y % 80 == 0)
            {
                if (zoom  < 0.3) continue;
                painter.setPen(QPen(QColor(255,255,255,75), 1, Qt::DashLine));
                painter.drawLine(startx, y, endx, y);
            }
            else
            {
                if (zoom  < 0.5) continue;
                painter.setPen(QPen(QColor(255,255,255,75), 1, Qt::DotLine));
                painter.drawLine(startx, y, endx, y);
            }
        }

        painter.setRenderHint(QPainter::Antialiasing);
    }
}


void LevelView::mousePressEvent(QMouseEvent* evt)
{    
    if (mode != NULL)
    {
        mode->mouseDown(evt->x()/zoom, evt->y()/zoom, evt->buttons(), evt->modifiers());
    }
    setCursor(QCursor(mode->getActualCursor()));
    update();
}


void LevelView::mouseMoveEvent(QMouseEvent* evt)
{    
    if (mode != NULL)
    {
        int x = evt->x()/zoom;
        int y = evt->y()/zoom;

        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
        {
            mode->mouseDrag(x, y, evt->modifiers());
        }
        else
            mode->mouseMove(x, y);
    }
    setCursor(QCursor(mode->getActualCursor()));
    update();
}

void LevelView::mouseReleaseEvent(QMouseEvent *evt)
{
    mode->mouseUp(evt->x()/zoom, evt->y()/zoom);
    setCursor(QCursor(mode->getActualCursor()));
    update();
}

void LevelView::moveEvent(QMoveEvent *)
{
    update();
}

void LevelView::saveLevel()
{
    level->save();
}

void LevelView::copy()
{
    editionModePtr()->copy();
}

void LevelView::paste()
{
    int x = (-pos().x())/zoom;
    int y = (-pos().y())/zoom;
    int w = visibleRegion().boundingRect().width()/zoom;
    int h = visibleRegion().boundingRect().height()/zoom;

    editionModePtr()->paste(x, y, w, h);
    update();
}

void LevelView::raise()
{
    objEditionModePtr()->raise();
    update();
}

void LevelView::lower()
{
    objEditionModePtr()->lower();
    update();
}

void LevelView::raiseLayer()
{
    objEditionModePtr()->raiseLayer();
    update();
}

void LevelView::lowerLayer()
{
    objEditionModePtr()->lowerLayer();
    update();
}

void LevelView::cut()
{
    editionModePtr()->cut();
    update();
}

void LevelView::deleteSel()
{
    mode->deleteSelection();
    setCursor(QCursor(mode->getActualCursor()));
    update();
}
