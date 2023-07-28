#include "increaseobjectsize.h"

namespace EditorCommand {

IncreaseObjectSize::IncreaseObjectSize(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap) :
    obj(obj), deltaX(deltaX), deltaY(deltaY), snap(snap)
{
    oldW = obj->getwidth();
    oldH = obj->getheight();

    this->setText(QObject::tr("Increased object size"));
}

void IncreaseObjectSize::undo()
{
    obj->resize(oldW, oldH);
}

void IncreaseObjectSize::redo()
{
    obj->increaseSize(deltaX, deltaY, snap);
}

} // namespace EditorCommand
