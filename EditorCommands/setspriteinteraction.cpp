#include "setspriteinteraction.h"

namespace EditorCommand {

SetSpriteInteraction::SetSpriteInteraction(bool *spriteInteraction, bool state) :
    spriteInteraction(spriteInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Sprites").arg(enabledStr));
}

void SetSpriteInteraction::undo()
{
    *spriteInteraction = !state;
}

void SetSpriteInteraction::redo()
{
    *spriteInteraction = state;
}

} // namespace EditorCommand
