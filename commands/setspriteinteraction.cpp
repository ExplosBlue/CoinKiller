#include "setspriteinteraction.h"

namespace EditorCommand {

setSpriteInteraction::setSpriteInteraction(bool *spriteInteraction, bool state) :
    spriteInteraction(spriteInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Sprites").arg(enabledStr));
}

void setSpriteInteraction::undo()
{
    *spriteInteraction = !state;
}

void setSpriteInteraction::redo()
{
    *spriteInteraction = state;
}

} // namespace EditorCommand
