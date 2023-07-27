#include "insertlocation.h"

namespace EditorCommand {

InsertLocation::InsertLocation(Level *level, Location *location) :
    level(level), location(location)
{
    this->setText(QObject::tr("Inserted Location at X: %1 Y: %2").arg(location->getx()).arg(location->gety()));
}

InsertLocation::~InsertLocation()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertLocation - Deleting Location";
        delete location;
    }
}

void InsertLocation::undo()
{
    level->locations.removeOne(location);
    deletable = true;
}

void InsertLocation::redo()
{
    level->locations.append(location);
    deletable = false;
}

} // namespace EditorCommand
