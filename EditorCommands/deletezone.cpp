#include "deletezone.h"

namespace EditorCommand {

DeleteZone::DeleteZone(Level *level, Zone *zone) :
    level(level), zone(zone)
{
    oldIndex = level->zones.indexOf(zone);
    this->setText(QObject::tr("Deleted Zone"));
}

DeleteZone::~DeleteZone()
{
    if (zone == nullptr || level == nullptr) {
        return;
    }

    if (!level->zones.contains(zone)) {
        delete zone;
    }
}

void DeleteZone::undo()
{
    level->zones.insert(oldIndex, zone);
}

void DeleteZone::redo()
{
    level->zones.removeOne(zone);
}

} // namespace EditorCommand
