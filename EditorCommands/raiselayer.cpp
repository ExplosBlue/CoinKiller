#include "raiselayer.h"

namespace EditorCommand {

RaiseLayer::RaiseLayer(Level *level, Object *obj) :
    level(level), obj(obj)
{
    prevLayer = dynamic_cast<BgdatObject*>(obj)->getLayer();
    this->setText(QObject::tr("Raised Layer of Object"));
}

void RaiseLayer::undo()
{
    dynamic_cast<BgdatObject*>(obj)->setLayer(prevLayer);
}

void RaiseLayer::redo()
{
    level->raiseLayer(dynamic_cast<BgdatObject*>(obj));
}

} // namespace EditorCommand
