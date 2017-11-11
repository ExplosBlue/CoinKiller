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
#include "is.h"

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

    setEditonMode(EditMode_ObjectsMode, true);

    zoom = 1;
    grid = false;
    checkerboard = false;
    renderLiquids = false;
    renderSprites = true;
    renderPaths = true;

}

LevelView::~LevelView()
{
    mode->deactivate();
    delete objectEditionMode;
}

void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.scale(zoom,zoom);

    drawrect = QRect(evt->rect().x()/zoom, evt->rect().y()/zoom, evt->rect().width()/zoom+20, evt->rect().height()/zoom+20);

    //qDebug("draw %d,%d %d,%d", drawrect.x(), drawrect.y(), drawrect.width(), drawrect.height());

    painter.fillRect(drawrect, backgroundColor);
    tileGrid.clear();

    // Render Checkerboard
    if (checkerboard)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);

        int startx = drawrect.x() - drawrect.x() %160;
        int endx = startx + drawrect.width() + 160;

        int starty = drawrect.y() - drawrect.y() %160;
        int endy = starty + drawrect.height() + 160;

        int x = startx;
        int y = starty;

        int county = 0;
        bool xoffset = 0;

        painter.setPen(Qt::NoPen);
        QBrush brush(Qt::SolidPattern);
        brush.setColor(QColor(50,50,50));
        painter.setBrush(brush);


        // Big Squares
        while (y <= endy)
        {
            while (x <= endx)
            {
                if (xoffset == false)
                {
                    painter.setOpacity(0.2);
                    painter.drawRect(x, y, 80, 80);
                    x += 80;
                }
                x += 80;
                xoffset = false;
            }
            x = 0;
            y += 80;
            county += 1;
            if (county %2)
                xoffset = true;
        }
        x = 0;
        y = 0;
        // Small Squares
        if (zoom  > 0.5)
        {
            while (y <= endy)
            {
                while (x <= endx)
                {
                    if (xoffset == false)
                    {
                        painter.setOpacity(0.1);
                        painter.drawRect(x, y, 20, 20);
                        x += 20;
                    }
                    x += 20;
                    xoffset = false;
                }
                x = 0;
                y += 20;
                county += 1;
                if (county %2)
                    xoffset = true;
            }
        }
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);
        painter.setOpacity(1);
    }

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

    if (renderSprites)
    {
        // Render Liquids
        if (renderLiquids)
        {
            for (int i = 0; i < level->zones.size(); i++)
            {
                const Zone* zone = level->zones.at(i);

                QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

                if (!drawrect.intersects(zonerect))
                    continue;

                foreach (Sprite* s, level->sprites)
                {
                    if (s->getid() != 12 && s->getid() != 13 && s->getid() != 14 && s->getid() != 15 && s->getid() != 246)
                        continue;

                    if (zonerect.contains(s->getx(), s->gety(), false))
                    {
                        LiquidRenderer liquidRend(s, zone);
                        liquidRend.render(&painter, &drawrect);
                    }
                }
            }
        }

        // Render Scuttlebug Strings
        for (int i = 0; i < level->zones.size(); i++)
        {
            const Zone* zone = level->zones.at(i);

            QRect zonerect(zone->getx(), zone->gety()+30, zone->getwidth(), zone->getheight());

            if (!drawrect.intersects(zonerect))
                continue;

            foreach (Sprite* s, level->sprites)
            {
                if (s->getid() != 93)
                    continue;

                if (zonerect.contains(s->getx(), s->gety(), false))
                {
                    ScuttlebugRenderer ScuttleRender(s, zone);
                    ScuttleRender.render(&painter, &drawrect);
                }
            }
        }

        // Render Path Controlled Sprites
        for (int i = 0; i < level->paths.size(); i++)
        {
            const Path* path = level->paths.at(i);

            foreach (Sprite* s, level->sprites)
            {
                if (s->getid() != 107 && s->getid() != 152 && s->getid() != 290 && s->getid() != 291)
                    continue;

                int pathID = 0;
                if (s->getid() == 152)
                    pathID = (s->getNybble(4)+(s->getNybble(7)*16));
                else
                    pathID = (s->getNybble(4));

                if (path->getid() == pathID)
                {
                    s->setPosition(path->getNode(0)->getx(), path->getNode(0)->gety());
                }
            }
        }

        // Render SnakeBlocks
        for (int i = 0; i < level->paths.size(); i++)
        {
            const Path* path = level->paths.at(i);

            foreach (Sprite* s, level->sprites)
            {
                if (s->getid() != 217 && s->getid() != 218)
                    continue;

                int pathID = s->getNybble(4)+(s->getNybble(7)*16);

                if (path->getid() == pathID)
                {

                    SnakeBlockRenderer SnakeBlockRenderer(s, path);
                    SnakeBlockRenderer.render(&painter, &drawrect);
                }
            }
        }

        // Render Sprites
        for (int i = 0; i < level->sprites.size(); i++)
        {
            Sprite* spr = level->sprites.at(i);

            if (!spr->doRender(drawrect))
                continue;

            SpriteRenderer sprRend(spr, level->tilesets);
            sprRend.render(&painter, &drawrect);
        }
    }
    // Render Entrances
    for (int i = 0; i < level->entrances.size(); i++)
    {
        const Entrance* entr = level->entrances.at(i);

        QRect entrrect(entr->getx(), entr->gety(), 20, 20);

        if (!drawrect.intersects(entrrect))
            continue;

        EntranceRenderer entrRend(entr);
        entrRend.render(&painter, &drawrect);
    }

    // Render Paths
    if (renderPaths)
        {
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

        int adjustX = 3;
        int adjustY = 3;
        if (zonerect.x() < drawrect.x())
            adjustX += drawrect.x()-zonerect.x();
        if (zonerect.y() < drawrect.y())
            adjustY += drawrect.y()-zonerect.y();

        painter.drawText(zonerect.adjusted(adjustX,adjustY,100,20), zoneText);
    }

    // Render Edition Mode Stuff
    mode->render(&painter);

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

    emit updateMinimap(drawrect);
}


void LevelView::mousePressEvent(QMouseEvent* evt)
{
    setFocus();

    if (evt->buttons() & Qt::MiddleButton)
    {
        dragX = evt->x();
        dragY = evt->y();
    }

    if (mode != NULL)
    {
        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
            mode->mouseDown(evt->x()/zoom, evt->y()/zoom, evt->buttons(), evt->modifiers(), drawrect);
        setCursor(QCursor(mode->getActualCursor()));
    }
    update();

    emit updateMinimapBounds();
}


void LevelView::mouseMoveEvent(QMouseEvent* evt)
{    
    if (evt->buttons() & Qt::MiddleButton)
    {
        int x = evt->x();
        int y = evt->y();

        emit scrollTo(visibleRegion().boundingRect().x() - x + dragX, visibleRegion().boundingRect().y() - y + dragY);
    }

    if (mode != NULL)
    {
        int x = evt->x()/zoom;
        int y = evt->y()/zoom;

        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
        {
            mode->mouseDrag(x, y, evt->modifiers(), drawrect);
        }
        else
            mode->mouseMove(x, y);
    }
    setCursor(QCursor(mode->getActualCursor()));
    update();

    emit updateMinimapBounds();
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

void LevelView::keyPressEvent(QKeyEvent* evt)
{
    mode->keyPress(evt);
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
    emit updateMinimapBounds();
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
    emit updateMinimapBounds();
}

void LevelView::selectAll()
{
    editionModePtr()->selectAll();
    update();
}

void LevelView::deleteSel()
{
    mode->deleteSelection();
    setCursor(QCursor(mode->getActualCursor()));
    update();
    emit updateMinimapBounds();
}

void LevelView::selectObj(Object *obj)
{
    mode->select(obj);

    int x = obj->getx()+obj->getOffsetX();
    int y = obj->gety()+obj->getOffsetY();
    int w = obj->getwidth();
    int h = obj->getheight();
    int wR = visibleRegion().boundingRect().width()/zoom;
    int hR = visibleRegion().boundingRect().height()/zoom;

    if (!QRect(visibleRegion().boundingRect().x()/zoom, visibleRegion().boundingRect().y()/zoom, wR, hR).intersects(QRect(x, y, w, h)))
    {
        int sX = qMax(x+w/2-wR/2, 0);
        int sY = qMax(y+h/2-hR/2, 0);
        emit scrollTo(sX, sY);
    }

    update();
}

void LevelView::selectZoneContents(Zone* zone)
{
    if (is<ObjectsEditonMode*>(mode))
    {
        objectEditionMode->selectZoneContents(zone);
        update();
    }
}

void LevelView::setEditonMode(EditMode newMode, bool init)
{
    if (init)
    {
        objectEditionMode = new ObjectsEditonMode(level);
        connect(objectEditionMode, SIGNAL(updateLevelView()), this, SLOT(update()));
    }
    else
        mode->deactivate();

    switch (newMode)
    {
        case EditMode_ObjectsMode:
        default:
            mode = objectEditionMode;
            break;
    }

    mode->activate();
    update();
}
