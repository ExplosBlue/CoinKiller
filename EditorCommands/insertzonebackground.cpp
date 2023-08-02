#include "insertzonebackground.h"

namespace EditorCommand {

InsertZoneBackground::InsertZoneBackground(Level *level, ZoneBackground *background) :
    level(level),
    background(background) {
    this->setText(QObject::tr("Inserted Zone Background"));
}

InsertZoneBackground::~InsertZoneBackground() {
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        delete background;
    }
}

void InsertZoneBackground::undo() {
    level->backgrounds.removeOne(background);
    deletable = true;
}

void InsertZoneBackground::redo() {
    level->backgrounds.append(background);
    deletable = false;
}


RemoveZoneBackground::RemoveZoneBackground(Level *level, ZoneBackground *background) :
    level(level),
    background(background) {
    this->setText(QObject::tr("Removed Zone Background"));
}

RemoveZoneBackground::~RemoveZoneBackground() {
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        delete background;
    }
}

void RemoveZoneBackground::undo() {
    level->backgrounds.append(background);
    deletable = false;
}

void RemoveZoneBackground::redo() {
    level->backgrounds.removeOne(background);
    deletable = true;
}

} // namespace EditorCommand
