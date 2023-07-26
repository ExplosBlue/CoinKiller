#include "setentranceinteraction.h"

namespace EditorCommand {

SetEntranceInteraction::SetEntranceInteraction(bool *entranceInteraction, bool state) :
    entranceInteraction(entranceInteraction), state(state)
{
    QString enabledStr = (state) ? "Enabled" : "Disabled";
    this->setText(QObject::tr("%1 Entrances").arg(enabledStr));
}

void SetEntranceInteraction::undo()
{
    *entranceInteraction = !state;
}

void SetEntranceInteraction::redo()
{
    *entranceInteraction = state;
}

} // namespace EditorCommand
