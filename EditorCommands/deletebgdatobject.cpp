#include "deletebgdatobject.h"


namespace EditorCommand {

DeleteBgdatObject::DeleteBgdatObject(Level *level, BgdatObject *obj) :
    level(level), obj(obj)
{
    oldIndex = level->objects[obj->getLayer()].indexOf(obj);
    this->setText(QObject::tr("Deleted BgdatObject"));
}

DeleteBgdatObject::~DeleteBgdatObject()
{
    if (obj == nullptr || level == nullptr) {
        return;
    }

    if (!level->objects[obj->getLayer()].contains(obj)) {
        delete obj;
    }
}

void DeleteBgdatObject::undo()
{
    level->objects[obj->getLayer()].insert(oldIndex, obj);
}

void DeleteBgdatObject::redo()
{
    level->objects[obj->getLayer()].removeOne(obj);
}

} // namespace EditorCommand
