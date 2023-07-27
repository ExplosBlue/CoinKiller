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
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeleteBgdatObject - Deleting bgdat";
        delete obj;
    }
}

void DeleteBgdatObject::undo()
{
    level->objects[obj->getLayer()].insert(oldIndex, obj);
    deletable = false;
}

void DeleteBgdatObject::redo()
{
    level->objects[obj->getLayer()].removeOne(obj);
    deletable = true;
}

} // namespace EditorCommand
