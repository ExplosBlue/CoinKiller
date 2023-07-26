#include "setpathinteraction.h"

namespace EditorCommand {

SetPathInteraction::SetPathInteraction(bool *pathInteraction, bool state) :
    pathInteraction(pathInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Paths").arg(enabledStr));
}

void SetPathInteraction::undo()
{
    *pathInteraction = !state;
}

void SetPathInteraction::redo()
{
    *pathInteraction = state;
}

} // namespace EditorCommand
