#include "deletelocation.h"

namespace EditorCommand {

DeleteLocation::DeleteLocation(Level *level, Location *loc) :
    level(level), loc(loc)
{
    oldIndex = level->locations.indexOf(loc);
    this->setText(QObject::tr("Deleted Location"));
}

DeleteLocation::~DeleteLocation()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeleteLocation - Deleting Location";
        delete loc;
    }
}

void DeleteLocation::undo()
{
    level->locations.insert(oldIndex, loc);
    deletable = false;
}

void DeleteLocation::redo()
{
    level->locations.removeOne(loc);
    deletable = true;
}

} // namespace EditorCommand
