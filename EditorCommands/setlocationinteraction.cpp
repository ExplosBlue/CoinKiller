#include "setlocationinteraction.h"

namespace EditorCommand {

SetLocationInteraction::SetLocationInteraction(bool *locationInteraction, bool state) :
    locationInteraction(locationInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Locations").arg(enabledStr));
}

void SetLocationInteraction::undo()
{
    *locationInteraction = !state;
}

void SetLocationInteraction::redo()
{
    *locationInteraction = state;
}

} // namespace EditorCommand
