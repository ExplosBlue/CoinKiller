#include "increaseobjectposition.h"

namespace EditorCommand {

IncreaseObjectPosition::IncreaseObjectPosition(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap) :
    obj(obj), deltaX(deltaX), deltaY(deltaY), snap(snap)
{
    oldX = obj->getx();
    oldY = obj->gety();

    this->setText(QObject::tr("Increased object position"));
}

void IncreaseObjectPosition::undo()
{
    obj->setPosition(oldX, oldY);
}

void IncreaseObjectPosition::redo()
{
    obj->increasePosition(deltaX, deltaY, snap);
}

} // namespace EditorCommand
