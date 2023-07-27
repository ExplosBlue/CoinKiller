#include "insertsprite.h"

namespace EditorCommand {

InsertSprite::InsertSprite(Level *level, Sprite *spr) :
    level(level), spr(spr)
{
    this->setText(QObject::tr("Inserted Sprite %1 at X: %2 Y: %3").arg(spr->getid()).arg(spr->getx()).arg(spr->gety()));
}

InsertSprite::~InsertSprite()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertSprite - Deleting sprite";
        delete spr;
    }
}

void InsertSprite::undo()
{
    level->sprites.removeOne(spr);
    if (level->isCameraLimit(spr)) {
        level->removeCameraLimit(spr);
    }
    deletable = true;
}

void InsertSprite::redo()
{
    level->sprites.append(spr);
    if (level->isCameraLimit(spr)) {
        level->insertCameraLimit(spr);
    }
    deletable = false;
}

} // namespace EditorCommand
