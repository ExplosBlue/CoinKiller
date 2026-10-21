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

#include "is.h"
#include "leveleditorwindow.h"
#include "levelview.h"
#include "unitsconvert.h"
#include "objectrenderer.h"
#include "settingsmanager.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QPaintEvent>
#include <QPainterPath>
#include <QMessageBox>
#include <QSet>
#include <QGesture>
#include <QScrollArea>
#include <QScrollBar>
#include <QScreen>
#include <QTimer>


static int refreshIntervalMs(const QWidget* widget)
{
    QScreen* screen = widget ? widget->screen() : nullptr;
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    qreal hz = screen ? screen->refreshRate() : 60.0;
    if (hz <= 0.0)
        hz = 60.0;
    return qMax(1, qRound(1000.0 / hz));
}

static const QPixmap& checkerboardPattern(bool smallSquares)
{
    static QPixmap patterns[2];
    QPixmap& pattern = patterns[smallSquares ? 1 : 0];
    if (pattern.isNull())
    {
        pattern = QPixmap(160, 160);
        pattern.fill(Qt::transparent);

        QPainter painter(&pattern);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(50, 50, 50));
        painter.setOpacity(0.2);
        for (int j = 0; j < 2; j++)
        {
            for (int i = j % 2; i < 2; i += 2)
                painter.drawRect(i * 80, j * 80, 80, 80);
        }

        if (smallSquares)
        {
            painter.setOpacity(0.1);
            for (int j = 0; j < 8; j++)
            {
                for (int i = j % 2; i < 8; i += 2)
                    painter.drawRect(i * 20, j * 20, 20, 20);
            }
        }
    }
    return pattern;
}

LevelView::LevelView(QWidget *parent, Level *level, QUndoStack *undoStack) :
    QWidget(parent), level(level), undoStack(undoStack)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_AcceptTouchEvents);
#if defined(Q_OS_MACOS)
    grabGesture(Qt::PinchGesture);
#endif

    editManager = new EditManager(level, undoStack);
    connect(editManager, SIGNAL(updateLevelView()), this, SLOT(requestUpdate()));
    connect(undoStack, &QUndoStack::indexChanged, this, [](int) { SpriteRenderer::invalidateCache(); });

    zoom = 1;
    editManager->setZoom(zoom);
    grid = false;
    checkerboard = false;
    renderLiquids = true;
    renderControllers = true;
    renderCameraLimits = true;
    render2DTile = true;
    render3DOverlay = true;

    panDirty = false;

    panTimer = new QTimer(this);
    panTimer->setTimerType(Qt::PreciseTimer);
    panTimer->setInterval(refreshIntervalMs(this));
    connect(panTimer, &QTimer::timeout, this, [this] {
        if (!panDirty)
            return;
        panDirty = false;
        emit scrollTo(panTarget.x(), panTarget.y());
    });

#ifdef USE_KDE_BLUR
    setBackgroundColor(QColor(0,0,0,0));
#endif
}

LevelView::~LevelView()
{
    SpriteRenderer::invalidateCache();
    delete editManager;
}

void LevelView::invalidateRenderCaches()
{
    SpriteRenderer::invalidateCache();
}

void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    paint(painter, evt->rect().adjusted(0,0,20,20), zoom, true);
    emit updateMinimap(drawrect);
}

void LevelView::screenshot(QRect rect)
{
    QPixmap pix(rect.size());
    QPainter painter(&pix);
    painter.translate(-rect.x(), -rect.y());
    paint(painter, rect, 1.0f, false);
    QApplication::clipboard()->setPixmap(pix);
}

void LevelView::paint(QPainter& painter, QRect rect, float zoomLvl, bool selections)
{
    static const QFont locationFont("Arial", 10, QFont::Bold);
    static const QFont zoneFont("Arial", 8, QFont::Normal);
    static const QFont pathFont("Arial", 7, QFont::Normal);

    painter.scale(zoomLvl,zoomLvl);

    drawrect = QRect(rect.x()/zoomLvl, rect.y()/zoomLvl, rect.width()/zoomLvl, rect.height()/zoomLvl);

    painter.fillRect(drawrect, backgroundColor);
    //painter.fillRect(drawrect, QColor(0,0,0,0));
    tileGrid.clear();

    // Render Checkerboard
    if (checkerboard)
    {
        const int startx = drawrect.x() - drawrect.x() % 160;
        const int endx = startx + drawrect.width() + 160;

        const int starty = drawrect.y() - drawrect.y() % 160;
        const int endy = starty + drawrect.height() + 160;

        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setOpacity(1.0);
        painter.drawTiledPixmap(QRect(startx, starty, endx - startx, endy - starty), checkerboardPattern(zoom > 0.5f));
        painter.restore();
    }

    // Render Translucent Liquid Indicators (if set to appear under tiles)
    if (renderLiquids && editManager->spriteInteractionEnabled() && !SettingsManager::getInstance()->getLERenderTransparentLiquidAboveTiles())
    {
        for (int i = 0; i < level->zones.size(); i++)
        {
            const Zone* zone = level->zones.at(i);

            QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

            if (!drawrect.intersects(zonerect))
                continue;

            foreach (Sprite* s, level->sprites)
            {
                if (s->getid() != 12 && s->getid() != 13 && s->getid() != 15)
                    continue;

                if (zonerect.contains(s->getx(), s->gety(), false))
                {
                    LiquidRenderer liquidRend(s, zone);
                    liquidRend.renderTranslucent(&painter, &drawrect);
                }
            }
        }
    }

    // Render Tiles
    QRect tileClip(drawrect.x()/20 - 1, drawrect.y()/20 - 1, drawrect.width()/20 + 3, drawrect.height()/20 + 3);
    for (int l = 1; l >= 0; l--)
    {
        if (!(editManager->getLayerMask() & (1<<l)))
            continue;

        tileGrid[0xFFFFFFFF] = l+1;

        for (int i = level->objects[l].size()-1; i >= 0; i--)
        {
            const BgdatObject* obj = level->objects[l].at(i);

            // don't draw shit that is outside of the view
            if (!drawrect.intersects(QRect(obj->getx(), obj->gety(), obj->getwidth(), obj->getheight())))
                continue;

            quint16 tsid = (obj->getid() >> 12) & 0x3;
            if (level->tilesets[tsid])
            {
                level->tilesets[tsid]->Render2DTiles(render2DTile);
                level->tilesets[tsid]->Render3DOverlay(render3DOverlay);
                level->tilesets[tsid]->drawObject(painter, tileGrid, obj->getid()&0x0FFF, obj->getx()/20, obj->gety()/20, obj->getwidth()/20, obj->getheight()/20, 1, tileClip);
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
    if (editManager->locationInteractionEnabled())
    {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, zoomLvl >= 0.5f);
        for (int i = 0; i < level->locations.size(); i++)
        {
            const Location* loc = level->locations.at(i);

            QRect locrect(loc->getx(), loc->gety(), loc->getwidth(), loc->getheight());

            if (!drawrect.intersects(locrect))
                continue;

            painter.fillRect(locrect, QBrush(QColor(85,80,185,50)));

            painter.setPen(QColor(0,0,0));
            painter.drawRect(locrect);

            if (zoomLvl >= 0.5f)
            {
                QString locText = QString("%1").arg(loc->getid());
                painter.setFont(locationFont);
                painter.setPen(QColor(255,255,255));
                painter.drawText(locrect.adjusted(5,5,0,0), locText);
            }
        }
        painter.restore();
    }

    // Render anything that falls under "sprites"
    if (editManager->spriteInteractionEnabled())
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
                        if (SettingsManager::getInstance()->getLERenderTransparentLiquidAboveTiles())
                            liquidRend.renderTranslucent(&painter, &drawrect);
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

        // Render Boss Cutscene Controllers
        for (int i = 0; i < level->zones.size(); i++)
        {
            const Zone* zone = level->zones.at(i);

            QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

            if (!drawrect.intersects(zonerect))
                continue;

            foreach (Sprite* s, level->sprites)
            {
                if (s->getid() != 260 && s->getid() != 261 && s->getid() != 262
                        && s->getid() != 263 && s->getid() != 264 && s->getid() != 265)
                    continue;

                if (zonerect.contains(s->getx(), s->gety(), false))
                {
                    BossControllerRenderer BossControllerRenderer(s, zone);
                    BossControllerRenderer.render(&painter, &drawrect);
                }
            }
        }

        // Render Path Controlled Sprites
        for (int i = 0; i < level->paths.size(); i++)
        {
            const Path* path = level->paths.at(i);

            foreach (Sprite* s, level->sprites)
            {
                if (s->getid() != 107 && s->getid() != 152 && s->getid() != 290 && s->getid() != 291 && s->getid() != 320)
                    continue;

                int pathID = 0;
                int nodeID = 0;

                if (s->getid() == 152 || s->getid() == 320)
                {
                    pathID = s->getNybbleData(9, 10);
                    nodeID = s->getNybbleData(7,8);
                }
                else
                    pathID = (s->getNybble(10));

                if (path->getid() == pathID)
                {
                    if (nodeID > path->getNumberOfNodes())
                        nodeID = 0;

                    s->setPosition(path->getNode(nodeID)->getx(), path->getNode(nodeID)->gety());
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

                int pathID = s->getNybble(10)+(s->getNybble(9)*16);

                if (path->getid() == pathID)
                {
                    SnakeBlockRenderer SnakeBlockRenderer(s, path);
                    SnakeBlockRenderer.render(&painter, &drawrect);
                }
            }
        }

        // Render Movement Controller - Back & Forth Indicators
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, zoomLvl >= 0.5f);
        static const QSet<int> supportsMovBackForth{28, 60, 101, 204, 293};
        for (int i = 0; renderControllers && i < level->zones.size(); i++)
        {
            const Zone* zone = level->zones.at(i);
            QRect zonerect(zone->getx() - 80, zone->gety() - 80, zone->getwidth() + 160, zone->getheight() + 160);

            if (!drawrect.intersects(zonerect))
                continue;

            foreach (Sprite* s, level->sprites)
            {
                if (!zonerect.contains(s->getx(), s->gety(), false))
                    continue;

                if (!supportsMovBackForth.contains(s->getid()))
                    continue;

                int distance;
                int direction = -1;

                // Find the correct associated Back & Forth movement controller & note its data
                foreach (Sprite* m, level->sprites)
                {
                    if (!zonerect.contains(m->getx(), m->gety(), false))
                        continue;

                    if (m->getid() != 166 || m->getNybbleData(20, 21) != s->getNybbleData(20, 21))
                        continue;

                    distance = m->getNybble(14) * 20;
                    direction = m->getNybble(7) % 4;
                    break;
                }

                if (direction == -1 || distance == 0)
                    continue;

                // Render a movement indicator for the sprite
                QColor color = QColor(244, 250, 255);
                int x = s->getx() + s->getOffsetX();
                int y = s->gety() + s->getOffsetY();

                switch (direction)
                {
                case 0: // Right
                {
                    MovIndicatorRenderer indicator(x + s->getwidth(), y + s->getheight()/2, x + s->getwidth() + distance, y + s->getheight()/2, 2, 6, false, color);
                    indicator.render(&painter);
                    break;
                }
                case 1: // Left
                {
                    MovIndicatorRenderer indicator(x, y + s->getheight()/2, x - distance, y + s->getheight()/2, 2, 6, false, color);
                    indicator.render(&painter);
                    break;
                }
                case 2: // Up
                {
                    MovIndicatorRenderer indicator(x + s->getwidth()/2, y, x + s->getwidth()/2, y - distance, 2, 6, true, color);
                    indicator.render(&painter);
                    break;
                }
                default: // Down
                {
                    MovIndicatorRenderer indicator(x + s->getwidth()/2, y + s->getheight(), x + s->getwidth()/2, y + s->getheight() + distance, 2, 6, true, color);
                    indicator.render(&painter);
                    break;
                }
                }
            }
        }
        painter.restore();

        // Render Sprites
        for (int i = 0; i < level->sprites.size(); i++)
        {
            Sprite* spr = level->sprites.at(i);

            if (!spr->doRender(drawrect))
                continue;

            SpriteRenderer sprRend(spr, level->tilesets);
            sprRend.render(&painter, &drawrect);
        }

        // Render Camera Limit Boundries
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, zoomLvl >= 0.5f);
        if (renderCameraLimits)
        {
            QList<Sprite*> leftCamLimits = level->leftCamLimits;
            QList<Sprite*> rightCamLimits = level->rightCamLimits;
            QList<Sprite*> topCamLimits = level->topCamLimits;
            QList<Sprite*> bottomCamLimits = level->bottomCamLimits;

            foreach (Sprite* left, leftCamLimits)
            {
                int leftX = left->getx();
                int rightX = 0;
                int yPos = left->gety();
                bool noPair = true;
                bool permiable = false;
                bool isBelow = false;
                int yRenderOffset = 0;
                foreach (Sprite* right, rightCamLimits)
                {
                    if (!Level::cameraLimitCanPair(left, right))
                        continue;

                    noPair = false;
                    rightX = right->getx();

                    if (right->getNybble(8) == 1 && left->getNybble(8) == 1)
                        permiable = true;

                    if (right->getNybble(4) == 1 && left->getNybble(4) == 1)
                        isBelow = true;

                    if (right->getNybbleData(10, 11) == left->getNybbleData(10, 11))
                        yRenderOffset = to20(right->getNybbleData(10, 11));

                    break;
                }

                if (!noPair)
                {
                    if (!drawrect.intersects(QRect(leftX, yPos, rightX, yPos+yRenderOffset)))
                        continue;

                    VCameraLimitRenderer VCameraLimitRenderer(leftX, rightX, yPos, yRenderOffset, permiable, isBelow);
                    VCameraLimitRenderer.render(&painter, &drawrect);
                }
            }

            foreach (Sprite* top, topCamLimits)
            {
                int topY = top->gety();
                int bottomY = 0;
                int xPos = top->getx();
                bool noPair = true;
                bool permiable = false;
                bool isRight = false;
                int xRenderOffset = 0;
                foreach (Sprite* bottom, bottomCamLimits)
                {
                    if (!Level::cameraLimitCanPair(top, bottom))
                        continue;

                    noPair = false;
                    bottomY = bottom->gety();

                    if (bottom->getNybble(8) == 1 && top->getNybble(8) == 1)
                        permiable = true;

                    if (bottom->getNybble(4) == 1 && top->getNybble(4) == 1)
                        isRight = true;

                    if (bottom->getNybbleData(10, 11) == top->getNybbleData(10, 11))
                        xRenderOffset = to20(bottom->getNybbleData(10, 11));

                    break;
                }

                if (!noPair)
                {
                    if (!drawrect.intersects(QRect(xPos, topY, xPos+xRenderOffset, bottomY)))
                        continue;

                    HCameraLimitRenderer HCameraLimitRenderer(topY, bottomY, xPos, xRenderOffset, permiable, isRight);
                    HCameraLimitRenderer.render(&painter, &drawrect);
                }
            }
        }
        painter.restore();
    }

    // Render Entrances
    if (editManager->entranceInteractionEnabled())
    {
        for (int i = 0; i < level->entrances.size(); i++)
        {
            const Entrance* entr = level->entrances.at(i);

            QRect entrrect(entr->getx(), entr->gety(), entr->getwidth(), entr->getheight());

            if (!drawrect.intersects(entrrect))
                continue;

            EntranceRenderer entrRend(entr);
            entrRend.render(&painter, &drawrect);
        }
    }
    // Render Paths
    if (editManager->pathInteractionEnabled())
    {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, zoomLvl >= 0.5f);
        for (int i = 0; i < level->paths.size(); i++)
        {
            const Path* path = level->paths.at(i);
            QList<PathNode*> nodes  = path->getNodes();

            for (int j = 0; j < nodes.size() - 1; j++)
            {
                QLine pathLine(QPoint(nodes[j]->getx(), nodes[j]->gety()), QPoint(nodes[j+1]->getx(), nodes[j+1]->gety()));

                if (!drawrect.intersects(QRect(pathLine.x1(), pathLine.y1(), pathLine.x2()-pathLine.x1(), pathLine.y2()-pathLine.y1())))
                    continue;

                QPen pen(QColor(0,255,20));
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawLine(pathLine);
            }

            // Connect path end to start if loop flag is enabled
            if (((path->getLoop() & 2) != 0) && (nodes.size() > 2))
            {
                QLine pathLine(QPoint(nodes[0]->getx(), nodes[0]->gety()), QPoint(nodes[nodes.size()-1]->getx(), nodes[nodes.size()-1]->gety()));

                QPen pen(QColor(0,255,20));
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawLine(pathLine);
            }

            for (int j = 0; j < nodes.size(); j++)
            {
                QRect pathrect(nodes[j]->getx()-10, nodes[j]->gety()-10, 20, 20);

                if (!drawrect.intersects(pathrect))
                    continue;

                painter.setPen(QColor(0,0,0));

                QPainterPath painterPath;
                painterPath.addRoundedRect(pathrect, 2.0, 2.0);
                QColor color(0,255,20,200);
                painter.fillPath(painterPath, color);
                painter.drawPath(painterPath);

                if (zoomLvl >= 0.5f)
                {
                    QString pathText = QString("%1-%2").arg(path->getid()).arg(j);
                    painter.setFont(pathFont);
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
                QLine ppathLine(QPoint(nodes[j]->getx(), nodes[j]->gety()), QPoint(nodes[j+1]->getx(), nodes[j+1]->gety()));

                if (!drawrect.intersects(QRect(ppathLine.x1(), ppathLine.y1(), ppathLine.x2()-ppathLine.x1(), ppathLine.y2()-ppathLine.y1())))
                    continue;

                QPen pen(QColor(220,255,0));
                pen.setWidth(2);
                painter.setPen(pen);
                painter.drawLine(ppathLine);
            }

            for (int j = 0; j < nodes.size(); j++)
            {
                QRect ppathrect(nodes[j]->getx()-10, nodes[j]->gety()-10, 20, 20);

                if (!drawrect.intersects(ppathrect))
                    continue;

                painter.setPen(QColor(0,0,0));

                QPainterPath path;
                path.addRoundedRect(ppathrect, 2.0, 2.0);
                QColor color(220,255,0,200);
                painter.fillPath(path, color);
                painter.drawPath(path);

                if (zoomLvl >= 0.5f)
                {
                    QString pPathText = QString("%1-%2").arg(pPath->getid()).arg(j);
                    painter.setFont(pathFont);
                    painter.drawText(ppathrect, pPathText, Qt::AlignHCenter | Qt::AlignVCenter);
                }
            }
        }

        painter.restore();
    }

    // Render Zones
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, zoomLvl >= 0.5f);
    for (int i = 0; i < level->zones.size(); i++)
    {
        const Zone* zone = level->zones.at(i);

        QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

        if (!drawrect.intersects(zonerect))
            continue;

        painter.setPen(QColor(255,255,255));

        painter.drawRect(zonerect);

        if (zoomLvl >= 0.5f)
        {
            QString zoneText = QString("Zone %1").arg(zone->getid());
            painter.setFont(zoneFont);

            int adjustX = 3;
            int adjustY = 3;
            if (zonerect.x() < drawrect.x())
                adjustX += drawrect.x()-zonerect.x();
            if (zonerect.y() < drawrect.y())
                adjustY += drawrect.y()-zonerect.y();

            painter.drawText(zonerect.adjusted(adjustX,adjustY,100,20), zoneText);
        }
    }
    painter.restore();

    // Render Stuff
    if (selections)
        editManager->render(&painter, drawrect);

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
    setFocus();

    if (evt->buttons() & Qt::MiddleButton)
    {
        dragX = evt->position().x();
        dragY = evt->position().y();
        panDirty = false;
        panTarget = QPoint(-1, -1);
        panTimer->setInterval(refreshIntervalMs(this));
        panTimer->start();
    }

    if (editManager != NULL)
    {
        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
            editManager->mouseDown(evt->position().x()/zoom, evt->position().y()/zoom, evt->buttons(), evt->modifiers(), drawrect);
        setCursor(QCursor(editManager->getActualCursor()));
    }
    requestUpdate();

    emit updateMinimapBounds();
}


void LevelView::mouseMoveEvent(QMouseEvent* evt)
{
    bool panning = evt->buttons() & Qt::MiddleButton;

    if (panning)
    {
        int x = evt->position().x();
        int y = evt->position().y();

        QPoint target((visibleRegion().boundingRect().x() - x + dragX)/zoom,
                      (visibleRegion().boundingRect().y() - y + dragY)/zoom);
        if (target != panTarget)
        {
            panTarget = target;
            panDirty = true;
        }
    }

    if (editManager != nullptr && !panning)
    {
        int x = evt->position().x()/zoom;
        int y = evt->position().y()/zoom;

        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
        {
            SpriteRenderer::invalidateCache();
            editManager->mouseDrag(x, y, evt->modifiers(), drawrect);
        }
        else
            editManager->mouseMove(x, y);

        setCursor(QCursor(editManager->getActualCursor()));
    }

    if (!panning)
    {
        requestUpdate();
        if (evt->buttons() & (Qt::LeftButton | Qt::RightButton))
            emit updateMinimapBounds();
    }
}

void LevelView::mouseReleaseEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::MiddleButton)
    {
        panTimer->stop();
        if (panDirty)
        {
            panDirty = false;
            emit scrollTo(panTarget.x(), panTarget.y());
        }
    }

    editManager->mouseUp(evt->position().x()/zoom, evt->position().y()/zoom, evt->button());

    if (evt->button() == Qt::LeftButton || evt->button() == Qt::RightButton)
        emit updateMinimapBounds();

    setCursor(QCursor(editManager->getActualCursor()));
    requestUpdate();
}

void LevelView::keyPressEvent(QKeyEvent* evt)
{
    if (editManager->hasSelection() &&
        (evt->key() == Qt::Key_Up || evt->key() == Qt::Key_Down ||
         evt->key() == Qt::Key_Left || evt->key() == Qt::Key_Right))
    {
        SpriteRenderer::invalidateCache();
        editManager->keyPress(evt);
        return;
    }

    // Allow scrolling on ScrollView
    if (evt->key() == Qt::Key_Up || evt->key() == Qt::Key_Down ||
        evt->key() == Qt::Key_Left || evt->key() == Qt::Key_Right)
    {
        evt->ignore();
        return;
    }

    SpriteRenderer::invalidateCache();
    editManager->keyPress(evt);
}

bool LevelView::event(QEvent* evt)
{
#if defined(Q_OS_MACOS)
    if (evt->type() == QEvent::Gesture)
    {
        QGestureEvent* gestureEvt = static_cast<QGestureEvent*>(evt);
        QPinchGesture* pinch = static_cast<QPinchGesture*>(gestureEvt->gesture(Qt::PinchGesture));
        if (pinch)
        {
            handlePinchZoom(pinch);
            return true;
        }
    }
#endif

    if (evt->type() == QEvent::NativeGesture)
    {
        QNativeGestureEvent* nativeEvt = static_cast<QNativeGestureEvent*>(evt);
        if (nativeEvt->gestureType() == Qt::ZoomNativeGesture ||
            nativeEvt->gestureType() == Qt::BeginNativeGesture ||
            nativeEvt->gestureType() == Qt::EndNativeGesture)
        {
            handleNativeZoom(nativeEvt);
            return true;
        }
    }

    return QWidget::event(evt);
}

void LevelView::wheelEvent(QWheelEvent* evt)
{
    bool ctrlHeld = (evt->modifiers() & Qt::ControlModifier) ||
                    (QApplication::keyboardModifiers() & Qt::ControlModifier);

    if (ctrlHeld)
        handleWheelZoom(evt);
    else
        handleWheelScroll(evt);

    evt->accept();
}

void LevelView::handleWheelZoom(QWheelEvent* evt)
{
    if (evt->phase() == Qt::ScrollMomentum)
        return;

    int delta = evt->angleDelta().y();
    if (delta == 0)
        delta = evt->angleDelta().x();
    if (delta == 0 && !evt->pixelDelta().isNull())
    {
        delta = evt->pixelDelta().y() * 6;
        if (delta == 0)
            delta = evt->pixelDelta().x() * 6;
    }

    scrollAccumulator += delta;

    int steps = scrollAccumulator / 120;
    scrollAccumulator %= 120;

    if (steps == 0)
        return;

    float oldZoom = zoom;
    float newZoom = zoom;

    if (steps > 0)
    {
        for (int i = 0; i < steps && newZoom < 4; i++)
        {
            if (newZoom < 1) newZoom += 0.1f;
            else newZoom += 0.25f;
        }
    }
    else
    {
        for (int i = 0; i < -steps && newZoom > 0.3f; i++)
        {
            if (newZoom <= 1.5f) newZoom -= 0.1f;
            else newZoom -= 0.25f;
        }
    }

    if (newZoom != oldZoom)
    {
        float mx = evt->position().x();
        float my = evt->position().y();
        float scrollX = visibleRegion().boundingRect().x();
        float scrollY = visibleRegion().boundingRect().y();

        float ratio = newZoom / oldZoom;

        setZoom(newZoom);

        float newScrollX = qMax(0.0f, scrollX + mx * (ratio - 1.0f));
        float newScrollY = qMax(0.0f, scrollY + my * (ratio - 1.0f));

        emit zoomLevelChanged(newZoom);
        emit scrollTo(newScrollX / newZoom, newScrollY / newZoom);
    }
}

void LevelView::handleWheelScroll(QWheelEvent* evt)
{
    if (evt->phase() == Qt::ScrollMomentum)
        return;

    QPoint pixelD = evt->pixelDelta();
    QPoint angleD = evt->angleDelta();
    float speed = 2.5f;

    float dx = 0, dy = 0;

    if (!pixelD.isNull() && pixelD != QPoint(0, 0))
    {
        dx = static_cast<float>(-pixelD.x());
        dy = static_cast<float>(-pixelD.y());
    }
    else
    {
        float ax = angleD.x() * 20.0f / 120.0f;
        float ay = angleD.y() * 20.0f / 120.0f;

        if (evt->inverted())
        {
            dx = ax;
            dy = ay;
        }
        else
        {
            dx = -ax;
            dy = -ay;
        }
    }

    if (evt->modifiers() & Qt::ShiftModifier)
    {
        float tmp = dx;
        dx = dy;
        dy = tmp;
    }

    dx *= speed;
    dy *= speed;

    if (dx == 0 && dy == 0)
        return;

    QScrollArea* area = qobject_cast<QScrollArea*>(parent()->parent());
    if (area)
    {
        float curX = static_cast<float>(area->horizontalScrollBar()->value());
        float curY = static_cast<float>(area->verticalScrollBar()->value());
        float newX = qMax(0.0f, curX + dx);
        float newY = qMax(0.0f, curY + dy);
        area->horizontalScrollBar()->setValue(qRound(newX));
        area->verticalScrollBar()->setValue(qRound(newY));
    }
}

void LevelView::handlePinchZoom(QPinchGesture* gesture)
{
    if (gesture->changeFlags() & QPinchGesture::ScaleFactorChanged)
    {
        float oldZoom = zoom;
        float newZoom = qBound(0.3f, oldZoom * static_cast<float>(gesture->scaleFactor()), 4.0f);

        if (newZoom != oldZoom)
        {
            int vpW = visibleRegion().boundingRect().width();
            int vpH = visibleRegion().boundingRect().height();

            float centerX = (visibleRegion().boundingRect().x() + vpW / 2.0f) / oldZoom;
            float centerY = (visibleRegion().boundingRect().y() + vpH / 2.0f) / oldZoom;

            setZoom(newZoom);

            float newScrollX = qMax(0.0f, centerX * newZoom - vpW / 2.0f);
            float newScrollY = qMax(0.0f, centerY * newZoom - vpH / 2.0f);

            emit zoomLevelChanged(newZoom);
            emit scrollTo(newScrollX / newZoom, newScrollY / newZoom);
        }
    }
}




void LevelView::handleNativeZoom(QNativeGestureEvent* evt)
{
    if (evt->gestureType() == Qt::ZoomNativeGesture)
    {
        float newZoom = qBound(0.3f, zoom * (1.0f + static_cast<float>(evt->value())), 4.0f);

        if (newZoom != zoom)
        {
            float oldZoom = zoom;
            int vpW = visibleRegion().boundingRect().width();
            int vpH = visibleRegion().boundingRect().height();
            float centerX = (visibleRegion().boundingRect().x() + vpW / 2.0f) / oldZoom;
            float centerY = (visibleRegion().boundingRect().y() + vpH / 2.0f) / oldZoom;

            setZoom(newZoom);

            float newScrollX = qMax(0.0f, centerX * newZoom - vpW / 2.0f);
            float newScrollY = qMax(0.0f, centerY * newZoom - vpH / 2.0f);

            emit zoomLevelChanged(newZoom);
            emit scrollTo(newScrollX / newZoom, newScrollY / newZoom);
        }

        evt->accept();
    }
}

void LevelView::requestUpdate()
{
    if (!updatePending)
    {
        updatePending = true;
        QTimer::singleShot(refreshIntervalMs(this), this, [this] {
            update();
            updatePending = false;

            scrollAccumulator = 0;
        });
    }
}

qint8 LevelView::saveLevel()
{
    return level->save();
}

void LevelView::copy()
{
    editManager->copy();
}

void LevelView::paste()
{
    int x = (-pos().x())/zoom;
    int y = (-pos().y())/zoom;
    int w = visibleRegion().boundingRect().width()/zoom;
    int h = visibleRegion().boundingRect().height()/zoom;

    editManager->paste(x, y, w, h);
    requestUpdate();
    emit updateMinimapBounds();
}

void LevelView::raise()
{
    editManager->raise();
    requestUpdate();
}

void LevelView::lower()
{
    editManager->lower();
    requestUpdate();
}

void LevelView::raiseLayer()
{
    editManager->raiseLayer();
    requestUpdate();
}

void LevelView::lowerLayer()
{
    editManager->lowerLayer();
    requestUpdate();
}

void LevelView::cut()
{
    editManager->cut();
    requestUpdate();
    emit updateMinimapBounds();
}

void LevelView::selectAll()
{
    editManager->selectAll();
    requestUpdate();
}

void LevelView::deselect()
{
    editManager->deselect();
    requestUpdate();
}

void LevelView::deleteSel()
{
    editManager->deleteSelection();
    setCursor(QCursor(editManager->getActualCursor()));
    requestUpdate();
    emit updateMinimapBounds();
}

void LevelView::selectObj(Object *obj)
{
    editManager->select(obj);

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

    requestUpdate();
}

void LevelView::selectZoneContents(Zone* zone)
{
    editManager->selectZoneContents(zone);
    requestUpdate();
}
