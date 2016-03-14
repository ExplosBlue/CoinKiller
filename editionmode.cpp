#include "editionmode.h"
#include "unitsconvert.h"
#include "is.h"

ObjectsEditonMode::ObjectsEditonMode(Level *level)
{
    this->level = level;
    selectionMode = false;
}

void ObjectsEditonMode::mouseDown(int x, int y, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
{
    dx = x;
    dy = y;
    lx = x;
    ly = y;

    if (buttons == Qt::RightButton)
    {
        selectedObjects.clear();

        if (drawType == 0)
        {
            if (selTileset == -1 || selObject == -1) return;
            int id = selTileset << 12;
            id = (id & 0xF000) | selObject;
            BgdatObject* bgdatobj = new BgdatObject(toNext20(x-10), toNext20(y-10), 20, 20, id, selLayer);
            level->objects[selLayer].append(bgdatobj);
            newObject = bgdatobj;
            creatNewObject = true;
            selectedObjects.append(bgdatobj);
        }
        else if (drawType == 1)
        {
            if (selSprite == -1) return;
            Sprite* spr = new Sprite(0, 0, selSprite);
            spr->setRect();
            int xpos = toNext10(dx-spr->getwidth()/2-spr->getOffsetX());
            int ypos = toNext10(dy-spr->getheight()/2-spr->getOffsetY());
            if (xpos < 0) xpos = 0;
            if (ypos < 0) ypos = 0;
            spr->setPosition(xpos, ypos);
            level->sprites.append(spr);
            selectedObjects.append(spr);
        }
        else if (drawType == 2)
        {
            Entrance* entr = level->newEntrance(x-10, y-10);
            level->entrances.append(entr);
            selectedObjects.append(entr);
        }
        else if (drawType == 3)
        {
            Zone* zone = level->newZone(x-200, y-120);
            level->zones.append(zone);
            selectedObjects.append(zone);
        }
        else if (drawType == 4)
        {
            Location* loc = level->newLocation(x, y);
            level->locations.append(loc);
            newObject = loc;
            creatNewObject = true;
            selectedObjects.append(loc);
        }

        checkEmits();
        emit updateEditors();
        if (selectedObjects.size() == 1) emit selectdObjectChanged(selectedObjects[0]);
    }

    if (buttons == Qt::LeftButton)
    {
        mouseAct = getActionAtPos(x, y);

        if (!mouseAct.drag || (mouseAct.hor == ResizeNone && mouseAct.vert == ResizeNone))
        {
            QList<Object*> tempSelObjects = selectedObjects;
            bool shift = false;

            if (modifiers != Qt::ShiftModifier)
                selectedObjects = getObjectsAtPos(dx, dy, dx, dy, true);
            else
            {
                shift = true;
                QList<Object*> newObjList = getObjectsAtPos(dx, dy, dx, dy, true);
                if (newObjList.size() == 1)
                {
                    if(!selectedObjects.contains(newObjList[0]))
                        selectedObjects.append(newObjList[0]);
                    else
                        selectedObjects.removeOne(newObjList[0]);
                }
            }

            if (!shift && selectedObjects.size() != 0 && tempSelObjects.contains(selectedObjects[0]))
            {
                selectedObjects = tempSelObjects;
            }

            if (selectedObjects.size() == 0)
            {
                selectionMode = true;
            }

            checkEmits();
        }

        mouseAct = getActionAtPos(x, y);
        actualCursor = getCursorAtPos(x, y);
        updateSelectionBounds();
    }
}

void ObjectsEditonMode::mouseDrag(int x, int y, Qt::KeyboardModifiers modifieres)
{
    if (creatNewObject)
    {
        lx = x;
        ly = y;

        int xpos = typeRound(dx, newObject->getType());
        int ypos = typeRound(dy, newObject->getType());

        int mX = typeRound(qMax(0, lx), newObject->getType());
        int mY = typeRound(qMax(0, ly), newObject->getType());

        int w = qAbs(mX-xpos);
        int h = qAbs(mY-ypos);

        if (newObject->getType() == 0)
        {
            if (mX < 0) mX = 0;
            else w += 20;
            if (mY < 0) mY = 0;
            else h += 20;
        }

        newObject->setPosition(qMin(xpos, mX), qMin(ypos, mY));
        newObject->resize(w, h);

        emit updateEditors();

        return;
    }

    if (selectionMode)
    {
        lx = x;
        ly = y;
        selectedObjects.clear();
        selectedObjects = getObjectsAtPos(lx, ly, dx, dy, false);
    }
    else
    {
        updateSelectionBounds();

        // Drag
        if (mouseAct.hor == ResizeNone && mouseAct.vert == ResizeNone)
        {
            int xDelta = x-lx;
            int yDelta = y-ly;

            // Quick and dirty fix to prevent the dragging objects from wiggling sometimes
            if (xDelta == -xDeltaL) xDelta = 0;
            else xDeltaL = xDelta;
            if (yDelta == -yDeltaL) yDelta = 0;
            else yDeltaL = yDelta;

            if (xDelta < -minBoundX) xDelta = -minBoundX;
            if (yDelta < -minBoundY) yDelta = -minBoundY;
            if (xDelta == 0 && yDelta == 0) return;

            int snap = 0;
            if (selectionHasBGDats)
            {
                xDelta = toNext20(xDelta);
                yDelta = toNext20(yDelta);
                snap = 20;
            }
            else if (modifieres == Qt::AltModifier)
            {
                xDelta = toNext16Compatible(xDelta);
                xDelta = toNext16Compatible(xDelta);
            }
            else
            {
                xDelta = toNext10(xDelta);
                yDelta = toNext10(yDelta);
                snap = 10;
            }

            foreach (Object* obj, selectedObjects)
            {
                obj->increasePosition(xDelta, yDelta, snap);
            }

            minBoundX += xDelta;
            minBoundY += yDelta;
            lx += xDelta;
            ly += yDelta;

            emit updateEditors();
        }
        // Resize
        else
        {
            int xDelta = x-lx;
            int yDelta = y-ly;
            int minSize = 0;

            int snap = 0;
            if (selectionHasBGDats)
            {
                xDelta = toNext20(xDelta);
                yDelta = toNext20(yDelta);
                minSize = 20;
                snap = 0;
            }
            else if (modifieres == Qt::AltModifier)
            {
                xDelta = toNext16Compatible(xDelta);
                xDelta = toNext16Compatible(xDelta);
            }
            else
            {
                xDelta = toNext10(xDelta);
                yDelta = toNext10(yDelta);
                snap = 10;
            }

            int xMoveDelta = 0;
            int xResizeDelta = 0;
            int yMoveDelta = 0;
            int yResizeDelta = 0;

            if (xDelta == 0 && yDelta == 0) return;

            if (mouseAct.hor == ResizeBegin)
            {
                if (-xDelta <= -minSizeX) xDelta = -(-minSizeX) - minSize;
                if (xDelta < -minBoundX) xDelta = -minBoundX;
                xMoveDelta = xDelta;
                xResizeDelta = -xDelta;
            }
            if (mouseAct.vert == ResizeBegin)
            {
                if (-yDelta <= -minSizeY) yDelta = -(-minSizeY) - minSize;
                if (yDelta < -minBoundY) yDelta = -minBoundY;
                yMoveDelta = yDelta;
                yResizeDelta = -yDelta;
            }
            if (mouseAct.hor == ResizeEnd)
            {
                if (xDelta <= -minSizeX) xDelta = -minSizeX + minSize;
                xResizeDelta = xDelta;
            }
            if (mouseAct.vert == ResizeEnd)
            {
                if (yDelta <= -minSizeY) yDelta = -minSizeY + minSize;
                yResizeDelta = yDelta;
            }

            if (xMoveDelta == 0 && yMoveDelta == 0 && xResizeDelta == 0 && yResizeDelta == 0) return;

            minBoundX += xMoveDelta;
            minBoundY += yMoveDelta;
            minSizeX += xResizeDelta;
            minSizeY += yResizeDelta;

            foreach (Object* obj, selectedObjects)
            {
                obj->increasePosition(xMoveDelta, yMoveDelta);
                if (obj->isResizable()) obj->increaseSize(xResizeDelta, yResizeDelta, snap);
            }

            lx += xDelta;
            ly += yDelta;

            emit updateEditors();
        }
    }
}

void ObjectsEditonMode::mouseMove(int x, int y)
{
    actualCursor = getCursorAtPos(x, y);
}

void ObjectsEditonMode::mouseUp(int x, int y)
{
    mouseAct = getActionAtPos(x, y);
    actualCursor = getCursorAtPos(x, y);
    if (selectionMode) checkEmits();
    selectionMode = false;
    creatNewObject = false;
}

void ObjectsEditonMode::render(QPainter *painter)
{
    foreach (Object* obj, selectedObjects)
    {
        QRect objrect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());

        painter->setPen(QPen(QColor(255,255,255,200), 1, Qt::DotLine));
        painter->drawRect(objrect);
        painter->fillRect(objrect, QColor(255,255,255,75));

        if (obj->isResizable())
        {
            drawResizeKnob(obj->getx(), obj->gety(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth(), obj->gety(), painter);
            drawResizeKnob(obj->getx(), obj->gety() + obj->getheight(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth(), obj->gety() + obj->getheight(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth()/2, obj->gety(), painter);
            drawResizeKnob(obj->getx(), obj->gety() + obj->getheight()/2, painter);
            drawResizeKnob(obj->getx() + obj->getwidth()/2, obj->gety() + obj->getheight(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth(), obj->gety() + obj->getheight()/2, painter);
        }
    }

    if (selectionMode)
    {
        QRect selArea(qMin(dx, lx), qMin(dy, ly), qAbs(lx-dx), qAbs(ly-dy));
        painter->setPen(QPen(QColor(0,80,180), 0.5));
        painter->fillRect(selArea, QColor(160,222,255,50));
        painter->drawRect(selArea);
    }
}

void ObjectsEditonMode::drawResizeKnob(int x, int y, QPainter *painter)
{
    painter->fillRect(x-2, y-2, 4, 4, QBrush(Qt::white));
}

QList<Object*> ObjectsEditonMode::getObjectsAtPos(int x1, int y1, int x2, int y2, bool firstOnly)
{
    QList<Object*> objects;

    if (x1>x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1>y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    QRect area = QRect(QPoint(x1, y1), QPoint(x2, y2));

    for (int l = 1; l >= 0; l--) foreach (BgdatObject* bgdat, level->objects[l]) if (bgdat->clickDetection(area)) objects.append(bgdat);
    foreach (Location* loc, level->locations) if (loc->clickDetection(area)) objects.append(loc);
    foreach (Sprite* spr, level->sprites) if (spr->clickDetection(area)) objects.append(spr);
    foreach (Entrance* entr, level->entrances) if (entr->clickDetection(area)) objects.append(entr);
    foreach (Zone* zone, level->zones) if (zone->clickDetection(area)) objects.append(zone);

    if (firstOnly && objects.size() > 1)
    {
        Object* object = objects[objects.size()-1];
        objects.clear();
        objects.append(object);
    }

    return objects;
}

ObjectsEditonMode::mouseAction ObjectsEditonMode::getActionAtPos(int x, int y)
{
    mouseAction act;

    for (int i = selectedObjects.size()-1; i >= 0; i--)
    {
        Object* o = selectedObjects[i];

        if (o->isResizable())
        {
            act.drag = true;

            if (x >= o->getx() - 8 && x <= o->getx() + 4)
                act.hor = ResizeBegin;
            else if (x >= o->getx() + o->getwidth() - 4 && x <= o->getx() + o->getwidth() + 8)
                act.hor = ResizeEnd;
            else if (x >= o->getx() && x <= o->getx() + o->getwidth())
                act.hor = ResizeNone;
            else act.drag = false;

            if (y >= o->gety() - 8 && y <= o->gety() + 4)
                act.vert = ResizeBegin;
            else if (y >= o->gety() + o->getheight() - 4 && y <= o->gety() + o->getheight() + 8)
                act.vert = ResizeEnd;
            else if (y >= o->gety() && y <= o->gety() + o->getheight())
                act.vert = ResizeNone;
            else act.drag = false;

            if (act.hor == ResizeNone && act.vert == ResizeNone && is<Zone*>(o) && !dynamic_cast<Zone*>(o)->clickDetection(x, y))
                act.drag = false;
        }
        else
        {
            act.hor = ResizeNone;
            act.vert = ResizeNone;

            act.drag = false;
            if (x >= o->getx() + o->getOffsetX() && x < o->getx() + o->getOffsetX() + o->getwidth() && y >= o->gety() + o->getOffsetY() && y < o->gety() + o->getOffsetY() + o->getheight())
                act.drag = true;
        }

        if (act.drag) return act;
    }

    return act;
}

Qt::CursorShape ObjectsEditonMode::getCursorAtPos(int x, int y)
{
    mouseAction act = getActionAtPos(x, y);

    if (!act.drag) return Qt::ArrowCursor;

    if (act.vert == ResizeBegin && act.hor == ResizeBegin) return Qt::SizeFDiagCursor;
    if (act.vert == ResizeEnd && act.hor == ResizeEnd) return Qt::SizeFDiagCursor;
    if (act.vert == ResizeBegin && act.hor == ResizeEnd) return Qt::SizeBDiagCursor;
    if (act.vert == ResizeEnd && act.hor == ResizeBegin) return Qt::SizeBDiagCursor;
    if (act.vert == ResizeNone && act.hor == ResizeNone) return Qt::SizeAllCursor;
    if (act.vert == ResizeNone) return Qt::SizeHorCursor;
    if (act.hor == ResizeNone) return Qt::SizeVerCursor;

    return Qt::ArrowCursor;
}

void ObjectsEditonMode::updateSelectionBounds()
{
    minBoundX = 2147483647;
    minBoundY = 2147483647;
    maxBoundX = 0;
    maxBoundY = 0;
    minSizeX = 2147483647;
    minSizeY = 2147483647;
    selectionHasBGDats = false;

    foreach (Object* o, selectedObjects)
    {
        if (o->getx() < minBoundX) minBoundX = o->getx();
        if (o->gety() < minBoundY) minBoundY = o->gety();
        if (o->getx() + o->getwidth() > maxBoundX) maxBoundX = o->getx() + o->getwidth();
        if (o->gety() + o->getheight() > maxBoundY) maxBoundY = o->gety() + o->getheight();

        if (o->isResizable())
        {
            if (is<Zone*>(o))
            {
                if (o->getwidth() - 400 < minSizeX) minSizeX = o->getwidth() - 400;
                if (o->getheight() - 240 < minSizeY) minSizeY = o->getheight() - 240;
            }
            else
            {
                if (o->getwidth() < minSizeX) minSizeX = o->getwidth();
                if (o->getheight() < minSizeY) minSizeY = o->getheight();
            }
        }

        if (is<BgdatObject*>(o))
            selectionHasBGDats = true;
    }
}

void ObjectsEditonMode::deleteSelection()
{
    level->remove(selectedObjects);
    selectedObjects.clear();
    actualCursor = Qt::ArrowCursor;
    mouseAction act;
    mouseAct = act;
    checkEmits();
    updateEditors();
}

void ObjectsEditonMode::checkEmits()
{
    if (selectedObjects.size() != 1)
        emit deselected();
    else
        emit selectdObjectChanged(selectedObjects[0]);
}
