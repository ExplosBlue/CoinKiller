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
#include <utility>
#include <QScreen>
#include <QTimer>
#include <QFontMetrics>
#include <QRegion>
#include <QHash>
#include <cmath>


bool RenderState::smoothZooming = false;

static qreal elapsedSeconds(QElapsedTimer& clock)
{
    const qreal dt = clock.isValid() ? clock.nsecsElapsed() / 1000000000.0 : -1.0;
    clock.restart();

    return dt;
}

static qreal easeFactor(qreal dt, qreal duration)
{
    return 1.0 - std::exp(-dt / duration);
}

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

static const QPixmap& scaledCheckerboardPattern(bool smallSquares, int cellPixels, qreal cell)
{
    static QPixmap patterns[2];
    static int sizes[2] = {-1, -1};

    const int index = smallSquares ? 1 : 0;
    QPixmap& pattern = patterns[index];

    if (sizes[index] != cellPixels)
    {
        pattern = checkerboardPattern(smallSquares).scaled(cellPixels, cellPixels,
                                                           Qt::IgnoreAspectRatio,
                                                           Qt::FastTransformation);
        sizes[index] = cellPixels;
    }

    pattern.setDevicePixelRatio(cellPixels / cell);

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

    panTarget = QPointF(-1, -1);
    panClock.start();

    const auto makeTimer = [this](void (LevelView::*step)()) {
        QTimer* timer = new QTimer(this);
        timer->setTimerType(Qt::PreciseTimer);
        connect(timer, &QTimer::timeout, this, step);
        return timer;
    };

    smoothScrollTimer = makeTimer(&LevelView::smoothScrollStep);
    smoothZoomTimer = makeTimer(&LevelView::smoothZoomStep);
    inertiaTimer = makeTimer(&LevelView::inertiaStep);

    smoothZoomTarget = zoom;

    scrollMomentumTimer = new QTimer(this);
    scrollMomentumTimer->setSingleShot(true);
    scrollMomentumTimer->setTimerType(Qt::PreciseTimer);
    scrollMomentumTimer->setInterval(30);
    connect(scrollMomentumTimer, &QTimer::timeout, this, &LevelView::startMomentum);

#ifdef USE_KDE_BLUR
    setBackgroundColor(QColor(0,0,0,0));
#endif
}

LevelView::~LevelView()
{
    RenderState::smoothZooming = false;
    SpriteRenderer::invalidateCache();
    delete editManager;
}

void LevelView::setZoom(float zoom)
{
    this->zoom = qBound(MinZoom, zoom, MaxZoom);

    if (editManager)
        editManager->setZoom(this->zoom);

    const int canvasSize = int(4096 * 20 * this->zoom);
    setMinimumSize(canvasSize, canvasSize);
    setMaximumSize(canvasSize, canvasSize);

    lastPinnedLabelRects.clear();
    update();
}

float LevelView::steppedZoom(float from, int steps)
{
    float result = qBound(MinZoom, from, MaxZoom);

    for (int i = 0; i < qAbs(steps); i++)
    {
        const float delta = (steps > 0)
                ? (result < 1.0f ? 0.1f : 0.25f)
                : (result <= 1.5f ? -0.1f : -0.25f);

        result = qBound(MinZoom, result + delta, MaxZoom);
    }

    return result;
}

void LevelView::zoomTo(float newZoom)
{
    finishSmoothZoom();
    applyZoom(newZoom, viewportCenter());
}

void LevelView::zoomBy(int steps)
{
    zoomTo(steppedZoom(zoom, steps));
}

QPointF LevelView::viewportCenter() const
{
    const QRect visible = visibleRegion().boundingRect();

    return QPointF(visible.x() + visible.width() / 2.0f,
                   visible.y() + visible.height() / 2.0f);
}

void LevelView::applyZoom(float newZoom, const QPointF& focalPoint)
{
    stopScrollAnimations();

    newZoom = qBound(MinZoom, newZoom, MaxZoom);

    const float oldZoom = zoom;
    if (qFuzzyCompare(newZoom, oldZoom))
        return;

    const QPointF levelPoint = focalPoint / oldZoom;
    const QPointF focalOffset = focalPoint - viewportTopLeft();

    setZoom(newZoom);

    const QPointF scroll = levelPoint * newZoom - focalOffset;

    emit scrollTo(qMax(0.0f, float(scroll.x())) / newZoom,
                  qMax(0.0f, float(scroll.y())) / newZoom);
}

void LevelView::invalidateRenderCaches()
{
    SpriteRenderer::invalidateCache();
}

static const QFont& zoneLabelFont()
{
    static const QFont font("Arial", 8, QFont::Normal);

    return font;
}

static QPoint zoneLabelAnchor(const QRect& zonerect, const QRect& viewport)
{
    return zonerect.intersected(viewport).topLeft() + QPoint(3, 3);
}

void LevelView::paintEvent(QPaintEvent* evt)
{
    const QRect viewport = visibleRect();

    QPainter painter(this);
    paint(painter, evt->rect().adjusted(0,0,20,20), viewport, zoom, true);
    emit updateMinimap(viewport);

    lastPinnedLabelRects = pinnedZoneLabelRects();
}

void LevelView::screenshot(QRect rect)
{
    QPixmap pix(rect.size());
    QPainter painter(&pix);
    painter.translate(-rect.x(), -rect.y());
    paint(painter, rect, rect, 1.0f, false);
    QApplication::clipboard()->setPixmap(pix);
}

QRect LevelView::visibleRect() const
{
    const QRect visible = visibleRegion().boundingRect();

    return QRect(visible.x()/zoom, visible.y()/zoom, visible.width()/zoom, visible.height()/zoom);
}

QList<QRect> LevelView::pinnedZoneLabelRects() const
{
    QList<QRect> rects;

    if (zoom < 0.5f)
        return rects;

    const QRect viewport = visibleRect();
    const QFontMetrics metrics(zoneLabelFont());

    for (const Zone* zone : level->zones)
    {
        const QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

        if (!zonerect.intersects(viewport) || viewport.contains(zonerect.topLeft()))
            continue;

        const QPoint anchor = zoneLabelAnchor(zonerect, viewport);
        const int width = metrics.horizontalAdvance(QString("Zone %1").arg(zone->getid()));

        rects.append(QRect(QPoint(int(anchor.x()*zoom), int(anchor.y()*zoom)),
                           QSize(int(width*zoom) + 2, int(metrics.height()*zoom) + 2))
                     .adjusted(-2, -2, 2, 2));
    }

    return rects;
}

void LevelView::onLevelScrolled()
{
    const QList<QRect> currentRects = pinnedZoneLabelRects();

    if (lastPinnedLabelRects.isEmpty() && currentRects.isEmpty())
        return;

    QRegion dirty;

    for (const QRect& rect : lastPinnedLabelRects)
        dirty += rect;

    for (const QRect& rect : currentRects)
        dirty += rect;

    update(dirty);
}

template <typename PathType>
static void drawPathOverlay(QPainter& painter, const PathType* path, const QRect& drawrect,
                            float zoomLvl, const QColor& color, const QFont& font, bool closeLoop)
{
    const auto nodes = path->getNodes();

    for (int j = 0; j < nodes.size() - 1; j++)
    {
        const QLine line(QPoint(nodes[j]->getx(), nodes[j]->gety()),
                         QPoint(nodes[j+1]->getx(), nodes[j+1]->gety()));

        if (!drawrect.intersects(QRect(line.x1(), line.y1(), line.x2()-line.x1(), line.y2()-line.y1())))
            continue;

        painter.setPen(QPen(color, 2));
        painter.drawLine(line);
    }

    if (closeLoop && nodes.size() > 2)
    {
        painter.setPen(QPen(color, 2));
        painter.drawLine(QPoint(nodes[0]->getx(), nodes[0]->gety()),
                         QPoint(nodes[nodes.size()-1]->getx(), nodes[nodes.size()-1]->gety()));
    }

    for (int j = 0; j < nodes.size(); j++)
    {
        const QRect nodeRect(nodes[j]->getx()-10, nodes[j]->gety()-10, 20, 20);

        if (!drawrect.intersects(nodeRect))
            continue;

        painter.setPen(QColor(0,0,0));

        QPainterPath nodePath;
        nodePath.addRoundedRect(nodeRect, 2.0, 2.0);
        painter.fillPath(nodePath, QColor(color.red(), color.green(), color.blue(), 200));
        painter.drawPath(nodePath);

        if (zoomLvl >= 0.5f)
        {
            painter.setFont(font);
            painter.drawText(nodeRect, QString("%1-%2").arg(path->getid()).arg(j),
                             Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void LevelView::paint(QPainter& painter, QRect rect, QRect viewport, float zoomLvl, bool selections)
{
    static const QFont locationFont("Arial", 10, QFont::Bold);
    static const QFont pathFont("Arial", 7, QFont::Normal);

    painter.scale(zoomLvl,zoomLvl);

    drawrect = QRect(rect.x()/zoomLvl, rect.y()/zoomLvl, rect.width()/zoomLvl, rect.height()/zoomLvl);

    painter.fillRect(drawrect, backgroundColor);
    //painter.fillRect(drawrect, QColor(0,0,0,0));
    tileGrid.clear();

    // Render Checkerboard
    if (checkerboard)
    {
        const QTransform toDevice = painter.worldTransform();
        const QRect visible = toDevice.mapRect(QRectF(drawrect)).toAlignedRect();
        const QPointF levelOrigin = toDevice.map(QPointF(0, 0));
        const qreal cell = 160.0 * zoomLvl;
        const bool small = zoomLvl > 0.5f;
        const int cellPixels = qMax(2, qRound(cell * painter.device()->devicePixelRatioF()));

        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setOpacity(1.0);
        painter.resetTransform();

        const QPointF origin(std::fmod(visible.left() - levelOrigin.x(), cell),
                             std::fmod(visible.top() - levelOrigin.y(), cell));

        painter.drawTiledPixmap(visible, scaledCheckerboardPattern(small, cellPixels, cell), origin);
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
        QHash<int, QList<Sprite*>> movControllers;
        for (Sprite* spr : level->sprites)
            if (spr->getid() == 166)
                movControllers[spr->getNybbleData(20, 21)].append(spr);

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

                int distance = 0;
                int direction = -1;

                const auto controllers = movControllers.constFind(s->getNybbleData(20, 21));
                if (controllers != movControllers.constEnd())
                {
                    for (Sprite* m : *controllers)
                    {
                        if (!zonerect.contains(m->getx(), m->gety(), false))
                            continue;

                        distance = m->getNybble(14) * 20;
                        direction = m->getNybble(7) % 4;
                        break;
                    }
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

        for (const Path* path : level->paths)
            drawPathOverlay(painter, path, drawrect, zoomLvl, QColor(0,255,20), pathFont, (path->getLoop() & 2) != 0);

        for (const ProgressPath* pPath : level->progressPaths)
            drawPathOverlay(painter, pPath, drawrect, zoomLvl, QColor(220,255,0), pathFont, false);

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
            painter.setFont(zoneLabelFont());

            const QPoint anchor = zoneLabelAnchor(zonerect, viewport);
            painter.drawText(QPoint(anchor.x(), anchor.y() + painter.fontMetrics().ascent()),
                             QString("Zone %1").arg(zone->getid()));
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
    stopScrollAnimations();
    finishSmoothZoom();

    if (evt->buttons() & Qt::MiddleButton)
    {
        panLastGlobal = evt->globalPosition();
        panTarget = QPointF(-1, -1);

        panFastVelocity = QPointF();
        panSlowVelocity = QPointF();
        panClock.start();
    }

    if (editManager != NULL)
    {
        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
            editManager->mouseDown(evt->position().x()/zoom, evt->position().y()/zoom, evt->buttons(), evt->modifiers(), visibleRect());
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
        const QPointF screenDelta = evt->globalPosition() - panLastGlobal;
        panLastGlobal = evt->globalPosition();

        const QPointF target = viewportTopLeft() - screenDelta;
        if (target != panTarget)
        {
            panTarget = target;
            scrollToPos(target);
        }

        const qreal dt = qBound(0.001, elapsedSeconds(panClock), 0.06);
        const QPointF instVelocity = -screenDelta / dt;

        panFastVelocity += (instVelocity - panFastVelocity) * easeFactor(dt, 0.03);
        panSlowVelocity += (instVelocity - panSlowVelocity) * easeFactor(dt, 0.12);
    }

    if (editManager != nullptr && !panning)
    {
        int x = evt->position().x()/zoom;
        int y = evt->position().y()/zoom;

        if (evt->buttons() == Qt::LeftButton || evt->buttons() == Qt::RightButton)
        {
            SpriteRenderer::invalidateCache();
            editManager->mouseDrag(x, y, evt->modifiers(), visibleRect());
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
        const qreal fastSpeed = panFastVelocity.manhattanLength();
        const qreal slowSpeed = panSlowVelocity.manhattanLength();

        if (panClock.nsecsElapsed() < 80000000 && fastSpeed > 200.0)
        {
            const qreal flick = qBound(0.0, (fastSpeed - slowSpeed) / fastSpeed * 3.0, 1.0);

            if (flick > 0.1)
                startInertia(panFastVelocity * (0.2 + 0.8 * flick),
                             3200.0 + 4800.0 * (1.0 - flick), 0.0);
        }

        panTarget = QPointF(-1, -1);
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

    const int steps = scrollAccumulator / 120;
    scrollAccumulator %= 120;

    if (steps != 0)
    {
        const float from = smoothZoomTimer->isActive() ? smoothZoomTarget : zoom;

        startSmoothZoom(steppedZoom(from, steps), evt->position());
    }
}

void LevelView::handleWheelScroll(QWheelEvent* evt)
{
    if (evt->phase() == Qt::ScrollMomentum)
        return;

    const QPoint pixelDelta = evt->pixelDelta();
    const QPoint angleDelta = evt->angleDelta();
    const bool hasPixelDelta = !pixelDelta.isNull() && pixelDelta != QPoint(0, 0);
    const bool fractionalAngle = angleDelta.x() % 120 != 0 || angleDelta.y() % 120 != 0;
    const bool touchpad = evt->source() == Qt::MouseEventSynthesizedBySystem
            || (hasPixelDelta && fractionalAngle);

    if (evt->phase() == Qt::ScrollEnd)
    {
        startMomentum();
        return;
    }

    scrollFromTouchpad = touchpad;

    inertiaTimer->stop();
    finishSmoothZoom();

    float dx = 0.0f, dy = 0.0f;

    if (hasPixelDelta)
    {
        dx = static_cast<float>(-pixelDelta.x());
        dy = static_cast<float>(-pixelDelta.y());
    }
    else
    {
        dx = static_cast<float>(-angleDelta.x()) * 20.0f / 120.0f;
        dy = static_cast<float>(-angleDelta.y()) * 20.0f / 120.0f;

        if (evt->inverted())
        {
            dx = -dx;
            dy = -dy;
        }
    }

    if (evt->modifiers() & Qt::ShiftModifier)
        std::swap(dx, dy);

    static const float scrollSpeed = 2.5f;
    dx *= scrollSpeed;
    dy *= scrollSpeed;

    if (dx == 0.0f && dy == 0.0f)
        return;

    if (touchpad)
    {
        smoothScrollTimer->stop();

        const qreal gap = elapsedSeconds(scrollEventClock);
        const qreal dt = gap > 0.0 ? qBound(0.004, gap, 0.05) : 0.008;

        if (gap <= 0.0 || gap > 0.2)
            scrollVelocity = QPointF();

        scrollVelocity = scrollVelocity * 0.5 + QPointF(dx, dy) / dt * 0.5;
        scrollMomentumTimer->start();

        scrollToPos(clampScroll(viewportTopLeft() + QPointF(dx, dy)));
    }
    else
    {
        scrollMomentumTimer->stop();

        startSmoothScroll(dx, dy);
    }
}

QPointF LevelView::viewportTopLeft() const
{
    return visibleRegion().boundingRect().topLeft();
}

void LevelView::scrollToPos(const QPointF& pos)
{
    emit scrollTo(pos.x() / zoom, pos.y() / zoom);
}

void LevelView::startAnimationTimer(QTimer* timer)
{
    timer->setInterval(refreshIntervalMs(this));
    timer->start();
}

void LevelView::stopScrollAnimations()
{
    smoothScrollTimer->stop();
    inertiaTimer->stop();
    scrollMomentumTimer->stop();
}

QPointF LevelView::clampScroll(const QPointF& pos) const
{
    const QSize viewport = visibleRegion().boundingRect().size();
    const qreal maxX = qMax(0, size().width() - viewport.width());
    const qreal maxY = qMax(0, size().height() - viewport.height());

    return QPointF(qBound(0.0, pos.x(), maxX), qBound(0.0, pos.y(), maxY));
}

void LevelView::startSmoothScroll(float dx, float dy)
{
    inertiaTimer->stop();

    const QPointF origin = smoothScrollTimer->isActive() ? smoothScrollTarget : viewportTopLeft();

    smoothScrollTarget = clampScroll(origin + QPointF(dx, dy));

    if (!smoothScrollTimer->isActive())
    {
        lastSmoothScrollPos = QPointF(-1, -1);
        startAnimationTimer(smoothScrollTimer);
    }
}

void LevelView::smoothScrollStep()
{
    const QPointF current(viewportTopLeft());
    const QPointF remaining = smoothScrollTarget - current;

    if ((qAbs(remaining.x()) <= 1.0 && qAbs(remaining.y()) <= 1.0) || current == lastSmoothScrollPos)
    {
        smoothScrollTimer->stop();
        smoothScrollTarget = current;
        scrollToPos(current);
        return;
    }

    QPointF next = current + remaining * 0.35;

    if (qAbs(next.x() - current.x()) < 1.0 && qAbs(next.y() - current.y()) < 1.0)
        next = smoothScrollTarget;

    lastSmoothScrollPos = current;
    scrollToPos(next);
}

void LevelView::startMomentum()
{
    scrollMomentumTimer->stop();

    if (scrollFromTouchpad && !inertiaTimer->isActive() && scrollVelocity.manhattanLength() > 80.0)
        startInertia(scrollVelocity, 0.0, 2.4);
}

void LevelView::startInertia(const QPointF& velocity, qreal decel, qreal damping)
{
    const qreal speed = velocity.manhattanLength();
    if (speed < 50.0)
        return;

    stopScrollAnimations();
    finishSmoothZoom();

    inertiaVelocity = speed > 4000.0 ? velocity * (4000.0 / speed) : velocity;
    inertiaDecel = decel;
    inertiaDamping = damping;
    inertiaPos = viewportTopLeft();

    inertiaClock.start();
    startAnimationTimer(inertiaTimer);
}

void LevelView::inertiaStep()
{
    const qreal dt = qBound(0.001, elapsedSeconds(inertiaClock), 0.05);
    const qreal speed = inertiaVelocity.manhattanLength();
    const qreal newSpeed = speed - (inertiaDecel + inertiaDamping * speed) * dt;

    if (newSpeed <= 15.0)
    {
        inertiaTimer->stop();
        return;
    }

    const QPointF next = clampScroll(inertiaPos + inertiaVelocity * (dt * (speed + newSpeed) * 0.5 / speed));

    if (next == inertiaPos)
    {
        inertiaTimer->stop();
        return;
    }

    inertiaPos = next;
    inertiaVelocity *= newSpeed / speed;
    scrollToPos(next);
}

void LevelView::finishSmoothZoom()
{
    smoothZoomTimer->stop();

    if (!RenderState::smoothZooming)
        return;

    RenderState::smoothZooming = false;

    for (Tileset* tileset : level->tilesets)
    {
        if (tileset)
            tileset->clearScaledTiles();
    }

    SpriteRenderer::invalidateCache();
    requestUpdate();
}

void LevelView::startSmoothZoom(float targetZoom, const QPointF& focalPoint)
{
    stopScrollAnimations();

    smoothZoomTarget = qBound(MinZoom, targetZoom, MaxZoom);
    smoothZoomFocalOffset = focalPoint - viewportTopLeft();

    if (smoothZoomTimer->isActive() || qFuzzyCompare(smoothZoomTarget, zoom))
        return;

    RenderState::smoothZooming = true;
    startAnimationTimer(smoothZoomTimer);
}

void LevelView::smoothZoomStep()
{
    const float remaining = smoothZoomTarget - zoom;
    const QPointF focalPoint = viewportTopLeft() + smoothZoomFocalOffset;

    if (qAbs(remaining) < 0.002f * qMax(1.0f, smoothZoomTarget))
    {
        applyZoom(smoothZoomTarget, focalPoint);
        finishSmoothZoom();
        return;
    }

    applyZoom(zoom + remaining * 0.35f, focalPoint);
}

void LevelView::handlePinchZoom(QPinchGesture* gesture)
{
    if (gesture->changeFlags() & QPinchGesture::ScaleFactorChanged)
    {
        finishSmoothZoom();
        applyZoom(zoom * static_cast<float>(gesture->scaleFactor()), viewportCenter());
    }
}

void LevelView::handleNativeZoom(QNativeGestureEvent* evt)
{
    if (evt->gestureType() != Qt::ZoomNativeGesture)
        return;

    finishSmoothZoom();
    applyZoom(zoom * (1.0f + static_cast<float>(evt->value())), viewportCenter());
    evt->accept();
}

void LevelView::requestUpdate()
{
    if (!updatePending)
    {
        updatePending = true;
        QTimer::singleShot(refreshIntervalMs(this), Qt::PreciseTimer, this, [this] {
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
