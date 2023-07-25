#include "setentranceinteraction.h"

namespace EditorCommand {

setEntranceInteraction::setEntranceInteraction(bool *entranceInteraction, bool state) :
    entranceInteraction(entranceInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Entrances").arg(enabledStr));
}

void setEntranceInteraction::undo()
{
    *entranceInteraction = !state;
}

void setEntranceInteraction::redo()
{
    *entranceInteraction = state;
}

} // namespace EditorCommand
