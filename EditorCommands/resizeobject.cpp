#include "resizeobject.h"

namespace EditorCommand {

ResizeObject::ResizeObject(Object *obj, quint32 w, quint32 h) :
    obj(obj), newW(w), newH(h)
{
    oldW = obj->getwidth();
    oldH = obj->getheight();

    this->setText(QObject::tr("Object Resized to W: %1 H: %2").arg(newW).arg(newH));
}

void ResizeObject::undo()
{
    obj->resize(oldW, oldH);
}

void ResizeObject::redo()
{
    obj->resize(newW, newH);
}

bool ResizeObject::mergeWith(const QUndoCommand *otherCommand)
{
    if (otherCommand->id() != this->id())
        return false;

    const ResizeObject* otherResize = static_cast<const ResizeObject*>(otherCommand);

    if (otherResize->obj != this->obj) {
        return false;
    }

    newW = otherResize->newW;
    newH = otherResize->newH;
    return true;
}

} // namespace EditorCommand
