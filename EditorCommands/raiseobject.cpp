#include "raiseobject.h"

namespace EditorCommand {

RaiseObject::RaiseObject(Level *level, Object *obj) :
    level(level), obj(obj)
{
    this->setText(QObject::tr("Raised Object"));
}

void RaiseObject::undo()
{
    level->lower(obj);
}

void RaiseObject::redo()
{
    level->raise(obj);
}

} // namespace EditorCommand
