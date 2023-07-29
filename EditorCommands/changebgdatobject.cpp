#include "changebgdatobject.h"

namespace EditorCommand {

ChangeBgdatObject::ChangeBgdatObject(BgdatObject *obj, qint32 objID, qint32 tilesetID) :
    obj(obj), newObjID(objID), newTilesetID(tilesetID) {

    oldObjID = obj->getObjID();
    oldTilesetID = obj->getTsID();

    this->setText(QObject::tr("Changed Object"));
}

void ChangeBgdatObject::undo() {
    obj->setObjID(oldObjID);
    obj->setTsID(oldTilesetID);
}

void ChangeBgdatObject::redo() {
    obj->setObjID(newObjID);
    obj->setTsID(newTilesetID);
}

} // namespace EditorCommand
