#include "insertpath.h"

namespace EditorCommand {

InsertPath::InsertPath(Level *level, Path *path) :
    level(level), path(path)
{
    this->setText(QObject::tr("Inserted Path %1").arg(path->getid()));
}

InsertPath::~InsertPath()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        delete path;
    }
}

void InsertPath::undo()
{
    level->paths.removeOne(path);
    deletable = true;
}

void InsertPath::redo()
{
    level->paths.append(path);
    deletable = false;
}

} // namespace EditorCommand
