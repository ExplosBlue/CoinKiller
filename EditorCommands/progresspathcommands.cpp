#include "progresspathcommands.h"

namespace Commands::ProgressPathCmd {

SetId::SetId(ProgressPath *path, qint32 id) :
    path(path),
    newId(id),
    oldId(path->getid()) {
    this->setText(QObject::tr("Changed Progress Path ID"));
}

void SetId::undo() {
    path->setId(oldId);
}

void SetId::redo() {
    path->setId(newId);
}


SetAltPathFlag::SetAltPathFlag(ProgressPath *path, bool flag) :
    path(path),
    flag(flag) {
    this->setText(QObject::tr("Changed Progress Alt Flag"));
    if (flag == path->getAlternatePathFlag()) {
        this->setObsolete(true);
    }
}

void SetAltPathFlag::undo() {
    path->setAlternatePathFlag(!flag);
}

void SetAltPathFlag::redo() {
    path->setAlternatePathFlag(flag);
}

} // namespace Commands::ProgressPathCmd
