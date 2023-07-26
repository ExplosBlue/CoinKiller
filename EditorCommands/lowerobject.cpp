#include "lowerobject.h"

namespace EditorCommand {

LowerObject::LowerObject(Level *level, Object *obj) :
    level(level), obj(obj)
{
    this->setText(QObject::tr("Lowered Object"));
}

void LowerObject::undo()
{
    level->raise(obj);
}

void LowerObject::redo()
{
    level->lower(obj);
}

} // namespace EditorCommand
