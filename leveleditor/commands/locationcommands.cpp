#include "locationcommands.h"

namespace Commands::LocationCmd {

SetId::SetId(Location *loc, quint8 id) :
    loc(loc),
    newId(id),
    oldId(loc->getid()) {
    this->setText(QObject::tr("Location ID Changed"));
}

void SetId::undo() {
    loc->setId(oldId);
}

void SetId::redo() {
    loc->setId(newId);
}

} // namespace Commands::LocationCmd
