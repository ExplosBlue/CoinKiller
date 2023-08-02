#include "pathcommands.h"

namespace Commands::PathCmd {

SetId::SetId(Path *path, qint32 id) :
    path(path),
    newId(id),
    oldId(path->getid()) {
    this->setText(QObject::tr("Changed Path ID"));
}

void SetId::undo() {
    path->setId(oldId);
}

void SetId::redo() {
    path->setId(newId);
}


SetLoop::SetLoop(Path *path, qint32 loop) :
    path(path),
    newLoop(loop),
    oldLoop(path->getLoop()) {
    this->setText(QObject::tr("Changed Path Loop"));
}

void SetLoop::undo() {
    path->setLoop(oldLoop);
}

void SetLoop::redo() {
    path->setLoop(newLoop);
}


SwapNodes::SwapNodes(Path *path, qsizetype node1, qsizetype node2) :
    path(path),
    node1(node1),
    node2(node2) {
    this->setText(QObject::tr("Swapped Path Nodes"));
}

void SwapNodes::undo() {
    path->swapNodes(node2, node1);
}

void SwapNodes::redo() {
    path->swapNodes(node1, node2);
}

} // namespace Commands::PathCmd
