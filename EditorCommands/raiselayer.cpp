#include "raiselayer.h"

namespace EditorCommand {

RaiseLayer::RaiseLayer(Level *level, BgdatObject *obj) :
    level(level), obj(obj)
{
    prevLayer = obj->getLayer();
    prevIndex = level->objects[prevLayer].indexOf(obj);
    this->setText(QObject::tr("Raised Layer of Object"));
}

void RaiseLayer::undo()
{
    level->objects[newLayer].removeOne(obj);
    level->objects[prevLayer].insert(prevIndex, obj);
    obj->setLayer(prevLayer);
}

void RaiseLayer::redo()
{
    level->raiseLayer(obj);
    newLayer = obj->getLayer();
}

} // namespace EditorCommand