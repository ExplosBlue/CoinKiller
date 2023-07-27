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
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeleteZone - Deleting Zone";
        delete zone;
    }
}

void DeleteZone::undo()
{
    level->zones.insert(oldIndex, zone);
    deletable = false;
}

void DeleteZone::redo()
{
    level->zones.removeOne(zone);
    deletable = true;
}

} // namespace EditorCommand
