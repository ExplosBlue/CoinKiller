#include "deletesprite.h"

namespace EditorCommand {

DeleteSprite::DeleteSprite(Level *level, Sprite *spr) :
    level(level), spr(spr)
{
    oldIndex = level->sprites.indexOf(spr);
    int id = spr->getid();

    switch (id) {
    case 156:
        oldCamLimitIndex = level->leftCamLimits.indexOf(spr);
        camLimitListPtr = &level->leftCamLimits;
        break;
    case 157:
        oldCamLimitIndex = level->rightCamLimits.indexOf(spr);
        camLimitListPtr = &level->rightCamLimits;
        break;
    case 160:
        oldCamLimitIndex = level->bottomCamLimits.indexOf(spr);
        camLimitListPtr = &level->bottomCamLimits;
        break;
    case 161:
        oldCamLimitIndex = level->topCamLimits.indexOf(spr);
        camLimitListPtr = &level->topCamLimits;
        break;
    }
    this->setText(QObject::tr("Deleted Sprite"));
}

DeleteSprite::~DeleteSprite()
{
    if (spr == nullptr || level == nullptr) {
        return;
    }

    if (!level->sprites.contains(spr)) {
        delete spr;
    }
}

void DeleteSprite::undo()
{
    level->sprites.insert(oldIndex, spr);

    if (camLimitListPtr != nullptr) {
        camLimitListPtr->insert(oldCamLimitIndex, spr);
    }
}

void DeleteSprite::redo()
{
    level->sprites.removeOne(spr);

    if (camLimitListPtr != nullptr) {
        camLimitListPtr->removeOne(spr);
    }
}

} // namespace EditorCommand
