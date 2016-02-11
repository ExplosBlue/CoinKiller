#include "editionmode.h"

ObjectsEditonMode::ObjectsEditonMode(Level *level)
{
    this->level = level;
    drawType = -1;
    selObject = 0;
    selTileset = 0;
    selLayer = 0;
}

void ObjectsEditonMode::mousePressEvent(QMouseEvent *evt)
{
    if (evt->buttons() == Qt::RightButton)
    {
        lx = evt->x();
        ly = evt->y();

        if (drawType == 0)
        {
            int id = selTileset << 12;
            id = (id & 0xF000) | selObject;
            BgdatObject* bgdatobj = new BgdatObject(toNext20(evt->x()-10), toNext20(evt->y()-10), 20, 20, id, selLayer);
            level->objects[selLayer].append(bgdatobj);
        }
    }
}

void ObjectsEditonMode::mouseReleaseEvent(QMouseEvent *evt)
{

}

void ObjectsEditonMode::mouseMoveEvent(QMouseEvent *evt)
{

}
