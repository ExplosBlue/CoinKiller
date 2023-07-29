#include "changespriteid.h"

namespace EditorCommand {

ChangeSpriteID::ChangeSpriteID(Sprite *spr, quint16 id) :
    spr(spr), newId(id) {

    this->oldId = spr->getid();
    this->setText(QObject::tr("Swapped Sprite"));

}

void ChangeSpriteID::undo() {
    spr->setid(oldId);
}

void ChangeSpriteID::redo() {
    spr->setid(newId);
}

} // namespace EditorCommand
