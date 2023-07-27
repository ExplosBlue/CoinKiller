#include "insertzone.h"

namespace EditorCommand {

InsertZone::InsertZone(Level *level, Zone *zone) :
    level(level), zone(zone)
{
    this->setText(QObject::tr("Inserted Zone at X: %1 Y: %2").arg(zone->getx()).arg(zone->gety()));
}

InsertZone::~InsertZone()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertZone - Deleting Zone";
        delete zone;
    }
}

void InsertZone::undo()
{
    level->zones.removeOne(zone);
    deletable = true;
}

void InsertZone::redo()
{
    level->zones.append(zone);
    deletable = false;
}

} // namespace EditorCommand
