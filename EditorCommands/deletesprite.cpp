#include "deletesprite.h"

namespace EditorCommand {

DeleteSprite::DeleteSprite(Level *level, Sprite *spr) :
    level(level), spr(spr)
{
    oldIndex = level->sprites.indexOf(spr);
    this->setText(QObject::tr("Deleted Sprite"));
}

DeleteSprite::~DeleteSprite()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeleteSprite - Deleting sprite";
        delete spr;
    }
}

void DeleteSprite::undo()
{
    level->sprites.insert(oldIndex, spr);

    if (level->isCameraLimit(spr)) {
        level->insertCameraLimit(spr);
    }

    deletable = false;
}

void DeleteSprite::redo()
{
    level->sprites.removeOne(spr);

    if (level->isCameraLimit(spr)) {
        level->removeCameraLimit(spr);
    }

    deletable = true;
}

} // namespace EditorCommand
