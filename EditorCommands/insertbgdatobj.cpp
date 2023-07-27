#include "insertbgdatobj.h"

namespace EditorCommand {

InsertBgdatObj::InsertBgdatObj(Level *level, BgdatObject *obj) :
    level(level), obj(obj)
{
    this->setText(QObject::tr("Inserted Object at X: %1 Y: %2").arg(obj->getx()).arg(obj->gety()));
}

InsertBgdatObj::~InsertBgdatObj()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertBgdatObj - Deleting bgdat";
        delete obj;
    }
}

void InsertBgdatObj::undo()
{
    level->objects[obj->getLayer()].removeOne(obj);
    deletable = true;
}

void InsertBgdatObj::redo()
{
    level->objects[obj->getLayer()].append(obj);
    deletable = false;
}

} // namespace EditorCommand
