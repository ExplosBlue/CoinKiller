#include "setlocationinteraction.h"

namespace EditorCommand {

setLocationInteraction::setLocationInteraction(bool *locationInteraction, bool state) :
    locationInteraction(locationInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Locations").arg(enabledStr));
}

void setLocationInteraction::undo()
{
    *locationInteraction = !state;
}

void setLocationInteraction::redo()
{
    *locationInteraction = state;
}

} // namespace EditorCommand
