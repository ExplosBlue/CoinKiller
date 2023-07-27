#include "insertprogresspath.h"

namespace EditorCommand {

InsertProgressPath::InsertProgressPath(Level *level, ProgressPath *path) :
    level(level), path(path)
{
    this->setText(QObject::tr("Inserted Progress Path %1").arg(path->getid()));
}

InsertProgressPath::~InsertProgressPath()
{
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        delete path;
    }
}

void InsertProgressPath::undo()
{
    level->progressPaths.removeOne(path);
    deletable = true;
}

void InsertProgressPath::redo()
{
    level->progressPaths.append(path);
    deletable = false;
}

} // namespace EditorCommand
