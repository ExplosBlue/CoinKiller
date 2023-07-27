#include "insertentrance.h"

namespace EditorCommand {

InsertEntrance::InsertEntrance(Level *level, Entrance *entr) :
    level(level), entr(entr)
{
    this->setText(QObject::tr("Inserted Entrance at X: %1 Y: %2").arg(entr->getx()).arg(entr->gety()));
}

InsertEntrance::~InsertEntrance()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertEntrance - Deleting Entrance";
        delete entr;
    }
}

void InsertEntrance::undo()
{
    level->entrances.removeOne(entr);
    deletable = true;
}

void InsertEntrance::redo()
{
    level->entrances.append(entr);
    deletable = false;
}

} // namespace EditorCommand
