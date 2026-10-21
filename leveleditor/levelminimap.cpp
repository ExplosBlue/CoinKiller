#include "levelminimap.h"

#include "QPaintEvent"

LevelMiniMap::LevelMiniMap(QWidget *parent, Level *level) : QWidget(parent)
{
    this->level = level;
    setMinimumSize(100, 100);

    updateBounds();
}

void LevelMiniMap::paintEvent(QPaintEvent*)
{
    float zoomX = this->width()/(float)maxX;
    float zoomY = this->height()/(float)maxY;

    zoom = qMin(zoomX, zoomY);

    qreal dpr = devicePixelRatioF();
    QSize cachePixels(qRound(width()*dpr), qRound(height()*dpr));

    if (cacheDirty || cache.size() != cachePixels || cache.devicePixelRatio() != dpr)
    {
        cache = QPixmap(cachePixels);
        cache.setDevicePixelRatio(dpr);
        cache.fill(QColor(119,136,153));

        QPainter cachePainter(&cache);
        cachePainter.scale(zoom,zoom);

        foreach (BgdatObject* obj, level->objects[1])
            cachePainter.fillRect(QRect(obj->getx(), obj->gety(), obj->getwidth(), obj->getheight()), QColor(200,200,200));

        foreach (BgdatObject* obj, level->objects[0])
            cachePainter.fillRect(QRect(obj->getx(), obj->gety(), obj->getwidth(), obj->getheight()), Qt::white);

        foreach (Location* loc, level->locations)
        {
            QRect locrect(loc->getx(), loc->gety(), loc->getwidth(), loc->getheight());

            cachePainter.fillRect(locrect, QBrush(QColor(85,80,185,120)));

            QPen pen(Qt::black);
            pen.setWidth(5);

            cachePainter.setPen(pen);
            cachePainter.drawRect(locrect);
        }

        foreach (Sprite* spr, level->sprites)
        {
            if (!spr->getSelectionRects()->empty())
            {
                foreach (QRect rect, *spr->getSelectionRects())
                {
                    cachePainter.fillRect(QRect(spr->getx()+rect.x(), spr->gety()+rect.y(), rect.width(), rect.height()), QColor(0,90,150,150));
                }
                continue;
            }

            cachePainter.fillRect(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight()), QColor(0,90,150,150));
        }

        foreach (Entrance* entr, level->entrances)
             cachePainter.fillRect(QRect(entr->getx()+entr->getOffsetX(), entr->gety()+entr->getOffsetY(), entr->getwidth(), entr->getheight()), QColor(255,0,0,150));

        foreach (Zone* zone, level->zones)
        {
            QRect zonerect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight());

            cachePainter.fillRect(zonerect, QBrush(QColor(0,220,255,25)));

            QPen pen(Qt::black);
            pen.setWidth(5);

            cachePainter.setPen(pen);
            cachePainter.drawRect(zonerect);
        }

        cacheDirty = false;
    }

    QPainter painter(this);
    painter.drawPixmap(0, 0, cache);

    painter.scale(zoom,zoom);
    QPen pen(Qt::blue);
    pen.setWidth(8);
    painter.setPen(pen);
    painter.drawRect(drawRect);
}

void LevelMiniMap::updateBounds()
{
    maxX = 0;
    maxY = 0;

    for (int l = 1; l >= 0; l--) foreach (BgdatObject* bgdat, level->objects[l]) checkBounds(bgdat);
    foreach (Location* loc, level->locations) checkBounds(loc);
    foreach (Sprite* spr, level->sprites) checkBounds(spr);
    foreach (Entrance* entr, level->entrances) checkBounds(entr);
    foreach (Zone* zone, level->zones) checkBounds(zone);

    maxX += 400;
    maxY += 400;
    cacheDirty = true;
}

void LevelMiniMap::invalidateCache()
{
    cacheDirty = true;
    update();
}

void LevelMiniMap::checkBounds(Object* obj)
{
    if (obj->getx() > maxX)
        maxX = obj->getx();
    if (obj->gety() > maxY)
        maxY = obj->gety();
}

void LevelMiniMap::update_(QRect drawRect)
{
    this->drawRect = drawRect;
    update();
}

void LevelMiniMap::mousePressEvent(QMouseEvent* evt)
{
    emit scrollTo(evt->x()/zoom - drawRect.width()/2, evt->y()/zoom - drawRect.height()/2);
}

void LevelMiniMap::mouseMoveEvent(QMouseEvent* evt)
{
    emit scrollTo(evt->x()/zoom - drawRect.width()/2, evt->y()/zoom - drawRect.height()/2);
}
