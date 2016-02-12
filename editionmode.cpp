#include "editionmode.h"

ObjectsEditonMode::ObjectsEditonMode(Level *level)
{
    this->level = level;
    drawType = -1;
    selObject = 0;
    selTileset = 0;
    selLayer = 0;
    paintingNewObject = false;
}

void ObjectsEditonMode::mousePressEvent(QMouseEvent *evt)
{
    dx = evt->x();
    dy = evt->y();

    selectedObjects.clear();

    if (evt->buttons() == Qt::RightButton)
    {
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
        // Select Shit
    }
}

void ObjectsEditonMode::mouseReleaseEvent(QMouseEvent *evt)
{
    if (paintingNewObject)
        paintingNewObject = false;
}

void ObjectsEditonMode::mouseMoveEvent(QMouseEvent *evt)
{
    lx = evt->x();
    ly = evt->y();

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
}
