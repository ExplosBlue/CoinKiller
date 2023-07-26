#include "raiseobject.h"

namespace EditorCommand {

raiseObject::raiseObject(Level *level, Object *obj) :
    level(level), obj(obj)
{
    this->setText(QObject::tr("Raised object"));
}

void raiseObject::undo()
{
    level->lower(obj);
}

void raiseObject::redo()
{
    level->raise(obj);
}

} // namespace EditorCommand
