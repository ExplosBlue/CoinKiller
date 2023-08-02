#include "insertzonebounding.h"

namespace EditorCommand {

InsertZoneBounding::InsertZoneBounding(Level *level, ZoneBounding *bounding) :
    level(level),
    bounding(bounding) {
    this->setText(QObject::tr("Inserted ZoneBackground"));
}

InsertZoneBounding::~InsertZoneBounding() {
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        delete bounding;
    }
}

void InsertZoneBounding::undo() {
    level->boundings.removeOne(bounding);
    deletable = true;
}

void InsertZoneBounding::redo() {
    level->boundings.append(bounding);
    deletable = false;
}


RemoveZoneBounding::RemoveZoneBounding(Level *level, ZoneBounding *bounding) :
    level(level),
    bounding(bounding) {
    this->setText(QObject::tr("Inserted ZoneBackground"));
}

RemoveZoneBounding::~RemoveZoneBounding() {
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        delete bounding;
    }
}

void RemoveZoneBounding::undo() {
    level->boundings.append(bounding);
    deletable = false;
}

void RemoveZoneBounding::redo() {
    level->boundings.removeOne(bounding);
    deletable = true;
}

} // namespace EditorCommand
