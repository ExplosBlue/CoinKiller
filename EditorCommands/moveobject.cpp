#include "moveobject.h"

namespace EditorCommand {

MoveObject::MoveObject(Object *obj, quint32 x, quint32 y) :
    obj(obj), newX(x), newY(y)
{
    oldX = obj->getx();
    oldY = obj->gety();

    this->setText(QObject::tr("Object Moved to X: %1 Y: %2").arg(newX).arg(newY));
}

void MoveObject::undo()
{
    obj->setPosition(oldX, oldY);
}

void MoveObject::redo()
{
    obj->setPosition(newX, newY);
}

bool MoveObject::mergeWith(const QUndoCommand *otherCommand)
{
    if (otherCommand->id() != this->id())
        return false;

    const MoveObject* otherMove = static_cast<const MoveObject*>(otherCommand);

    if (otherMove->obj != this->obj) {
        return false;
    }

    newX = otherMove->newX;
    newY = otherMove->newY;
    return true;
}

} // namespace EditorCommand
