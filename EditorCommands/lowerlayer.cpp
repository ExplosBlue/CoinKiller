#include "lowerlayer.h"

namespace EditorCommand {

LowerLayer::LowerLayer(Level *level, BgdatObject *obj) :
    level(level), obj(obj)
{
    prevLayer = obj->getLayer();
    prevIndex = level->objects[prevLayer].indexOf(obj);
    this->setText(QObject::tr("Lowered Layer of Object"));
}

void LowerLayer::undo()
{
    level->objects[newLayer].removeOne(obj);
    level->objects[prevLayer].insert(prevIndex, obj);
    obj->setLayer(prevLayer);
}

void LowerLayer::redo()
{
    level->lowerLayer(obj);
    newLayer = obj->getLayer();
}

} // namespace EditorCommand
