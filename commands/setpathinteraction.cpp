#include "setpathinteraction.h"

namespace EditorCommand {

setPathInteraction::setPathInteraction(bool *pathInteraction, bool state) :
    pathInteraction(pathInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Paths").arg(enabledStr));
}

void setPathInteraction::undo()
{
    *pathInteraction = !state;
}

void setPathInteraction::redo()
{
    *pathInteraction = state;
}

} // namespace EditorCommand
