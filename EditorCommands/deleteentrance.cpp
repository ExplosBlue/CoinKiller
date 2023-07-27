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
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeleteEntrance - Deleting Entrance";
        delete entr;
    }
}

void DeleteEntrance::undo()
{
    level->entrances.insert(oldIndex, entr);
    deletable = false;
}

void DeleteEntrance::redo()
{
    level->entrances.removeOne(entr);
    deletable = true;
}

} // namespace EditorCommand
