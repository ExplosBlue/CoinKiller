#include "editionmode.h"
#include "is.h"

ObjectsEditonMode::ObjectsEditonMode(Level *level)
{
    this->level = level;
    drawType = -1;
    selObject = 0;
    selTileset = 0;
    selLayer = 0;
    paintingNewObject = false;
    selectionMode = false;
    selectionHasBGDats = false;
}

void ObjectsEditonMode::mousePressEvent(QMouseEvent *evt)
{
    dx = evt->x();
    dy = evt->y();
    lx = dx;
    ly = dy;

    if (evt->buttons() == Qt::RightButton)
    {
        selectionHasBGDats = false;
        selectedObjects.clear();

        if (drawType == 0)
        {
            int id = selTileset << 12;
            id = (id & 0xF000) | selObject;
            BgdatObject* bgdatobj = new BgdatObject(toNext20(evt->x()-10), toNext20(evt->y()-10), 20, 20, id, selLayer);
            level->objects[selLayer].append(bgdatobj);
            newObject = bgdatobj;
            paintingNewObject = true;
        }

        else if (drawType == 4)
        {
            Location* loc = new Location(toNext16Compatible(evt->x()), toNext16Compatible(evt->y()), 4, 4, 0);
            level->locations.append(loc);
            newObject = loc;
            paintingNewObject = true;
        }
    }

    else if (evt->buttons() == Qt::LeftButton)
    {
        dragMode = false;

        foreach (Object* obj, selectedObjects)
        {
            if (obj->clickDetection(dx, dy))
            {
                setDrags();
                return;
            }
        }

        selectionHasBGDats = false;
        selectedObjects.clear();

        findSelectedObjects(dx, dy, dx, dy, true, true);

        if (selectedObjects.size() == 0)
        {
            selectionMode = true;
        }
        else
        {
            setDrags();
        }
    }
}

void ObjectsEditonMode::mouseReleaseEvent(QMouseEvent *evt)
{
    dragMode = false;
    paintingNewObject = false;
    selectionMode = false;
}

void ObjectsEditonMode::mouseMoveEvent(QMouseEvent *evt)
{
    lx = evt->x();
    ly = evt->y();

    if (dragMode)
    {
        int deltax = lx - dx;
        int deltay = ly - dy;

        if (selectionHasBGDats)
        {
            deltax = toNext20(deltax);
            deltay = toNext20(deltay);
        }
        else if (evt->modifiers() == Qt::AltModifier)
        {
            deltax = toNext16Compatible(deltax);
            deltay = toNext16Compatible(deltay);
        }
        else
        {
            deltax = toNext10(deltax);
            deltay = toNext10(deltay);
        }

        foreach (Object* obj, selectedObjects)
        {
            obj->setPosition(obj->getDragX() + deltax, obj->getDragY() + deltay);
        }

        return;
    }

    if (paintingNewObject)
    {
        int x = typeRound(dx, newObject->getType());
        int y = typeRound(dy, newObject->getType());

        int mX = typeRound(qMax(0, lx), newObject->getType());
        int mY = typeRound(qMax(0, ly), newObject->getType());

        int w = qAbs(mX-x);
        int h = qAbs(mY-y);

        if (newObject->getType() == 0)
        {
            w += 20;
            h += 20;
        }

        newObject->setPosition(qMin(x, mX), qMin(y, mY));
        newObject->resize(w,h);

        return;
    }

    if (selectionMode)
    {
        findSelectedObjects(dx, dy, lx, ly, false, true);
    }
}

void ObjectsEditonMode::findSelectedObjects(int x1, int y1, int x2, int y2, bool firstOnly, bool clearSelection)
{
    if (clearSelection)
    {
        selectionHasBGDats = false;
        selectedObjects.clear();
    }


    if (x1 > x2) { int tmp = x1; x1 = x2; x2 = tmp; }
    if (y1 > y2) { int tmp = y1; y1 = y2; y2 = tmp; }

    QRect rect(QPoint(x1, y1), QPoint(x2, y2));

    for (int l = 1; l >= 0; l--) foreach (BgdatObject* o, level->objects[l]) if (o->clickDetection(rect)) selectedObjects.append(o);
    if (selectedObjects.size() != 0) selectionHasBGDats = true;
    foreach (Location* l, level->locations) if (l->clickDetection(rect)) selectedObjects.append(l);
    foreach (Sprite* s, level->sprites) if (s->clickDetection(rect)) selectedObjects.append(s);
    foreach (Entrance* e, level->entrances) if (e->clickDetection(rect)) selectedObjects.append(e);

    if (firstOnly && selectedObjects.size() > 1)
    {
        Object* obj = selectedObjects[selectedObjects.size()-1];
        selectedObjects.clear();
        selectionHasBGDats = is<BgdatObject*>(obj);
        selectedObjects.append(obj);
    }
}

void ObjectsEditonMode::render(QPainter *painter)
{
    foreach (Object* obj, selectedObjects)
    {
        QRect objrect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());

        painter->setPen(QPen(QColor(255,255,255,200), 1, Qt::DotLine));
        painter->drawRect(objrect);
        painter->fillRect(objrect, QColor(255,255,255,75));
    }

    if (selectionMode)
    {
        QRect selArea(qMin(dx, lx), qMin(dy, ly), qAbs(lx-dx), qAbs(ly-dy));
        painter->setPen(QPen(QColor(0,80,180), 0.5));
        painter->fillRect(selArea, QColor(160,222,255,50));
        painter->drawRect(selArea);
    }
}

void ObjectsEditonMode::setDrags()
{
    dragMode = true;

    foreach(Object* obj, selectedObjects)
    {
        obj->setDrag(obj->getx(), obj->gety());
    }
}
