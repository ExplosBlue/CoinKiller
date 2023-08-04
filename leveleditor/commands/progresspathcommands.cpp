#include "progresspathcommands.h"

namespace Commands::ProgressPathCmd {

InsertNode::InsertNode(ProgressPath *path, ProgressPathNode *node, quint32 index) :
    path(path),
    node(node),
    index(index) {
    this->setText(QObject::tr("Inserted Node into Progress Path %1 at X: %2 Y: %3").arg(path->getid()).arg(node->getx()).arg(node->gety()));
}

InsertNode::~InsertNode() {
    if (deletable) {
        delete node;
    }
}

void InsertNode::undo() {
    path->removeNode(node);
    deletable = true;
}

void InsertNode::redo() {
    path->insertNode(node, index);
    deletable = false;
}


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


SetAltPathFlag::SetAltPathFlag(ProgressPath *path, quint8 flag) :
    path(path),
    newFlag(flag),
    oldFlag(path->getAlternatePathFlag()) {
    this->setText(QObject::tr("Changed Progress Alt Flag"));
    if (flag == path->getAlternatePathFlag()) {
        this->setObsolete(true);
    }
}

void SetAltPathFlag::undo() {
    path->setAlternatePathFlag(oldFlag);
}

void SetAltPathFlag::redo() {
    path->setAlternatePathFlag(newFlag);
}

} // namespace Commands::ProgressPathCmd
