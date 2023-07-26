#include "deleteentrance.h"

namespace EditorCommand {

DeleteEntrance::DeleteEntrance(Level *level, Entrance *entr) :
    level(level), entr(entr)
{
    oldIndex = level->entrances.indexOf(entr);
    this->setText(QObject::tr("Deleted Entrance"));
}

DeleteEntrance::~DeleteEntrance()
{
    if (entr == nullptr || level == nullptr) {
        return;
    }

    if (!level->entrances.contains(entr)) {
        delete entr;
    }
}

void DeleteEntrance::undo()
{
    level->entrances.insert(oldIndex, entr);
}

void DeleteEntrance::redo()
{
    level->entrances.removeOne(entr);
}

} // namespace EditorCommand
