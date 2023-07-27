#include "insertzonebounding.h"

namespace EditorCommand {

InsertZoneBounding::InsertZoneBounding(Level *level, ZoneBounding *bounding) :
    level(level), bounding(bounding)
{
    this->setText(QObject::tr("Inserted ZoneBackground"));
}

InsertZoneBounding::~InsertZoneBounding()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::InsertZoneBounding - Deleting ZoneBounding";
        delete bounding;
    }
}

void InsertZoneBounding::undo()
{
    level->boundings.removeOne(bounding);
    deletable = true;
}

void InsertZoneBounding::redo()
{
    level->boundings.append(bounding);
    deletable = false;
}

} // namespace EditorCommand
