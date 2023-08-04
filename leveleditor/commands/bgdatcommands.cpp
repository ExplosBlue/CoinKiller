#include "bgdatcommands.h"

namespace Commands::BgdatCmd {

ChangeObject::ChangeObject(BgdatObject *obj, qint32 objId, qint32 tilesetId) :
    obj(obj),
    newObjId(objId),
    oldObjId(obj->getObjID()),
    newTilesetId(tilesetId),
    oldTilesetId(obj->getTsID()) {
    this->setText(QObject::tr("Changed Object"));
}

void ChangeObject::undo() {
    obj->setObjID(oldObjId);
    obj->setTsID(oldTilesetId);
}

void ChangeObject::redo() {
    obj->setObjID(newObjId);
    obj->setTsID(newTilesetId);
}

} // namespace Commands::BgdatCmd
