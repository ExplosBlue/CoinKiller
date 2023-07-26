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
    if (loc == nullptr || level == nullptr) {
        return;
    }

    if (!level->locations.contains(loc)) {
        delete loc;
    }
}

void DeleteLocation::undo()
{
    level->locations.insert(oldIndex, loc);
}

void DeleteLocation::redo()
{
    level->locations.removeOne(loc);
}

} // namespace EditorCommand
