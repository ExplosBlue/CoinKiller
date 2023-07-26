#include "deletepathnode.h"

namespace EditorCommand {

DeletePathNode::DeletePathNode(Level *level, PathNode *node) :
    level(level), node(node)
{
    path = node->getParentPath();
    oldNodeIndex = path->getIndexOfNode(node);

    if (path->getNumberOfNodes() <= 1) {
        wasLastNode = true;
        oldPathIndex = level->paths.indexOf(path);
    }

    this->setText(QObject::tr("Deleted PathNode"));
}

DeletePathNode::~DeletePathNode()
{
    if (node == nullptr || level == nullptr) {
        return;
    }

    if (path == nullptr) {
        qDebug() << "deleted path node";
        delete node;
        return;
    }

    if (!level->paths.contains(path)) {
        qDebug() << "deleted path node";
        delete node;

        if (wasLastNode) {
            qDebug() << "deleted path";
            delete path;
        }
    }
}

void DeletePathNode::undo()
{
    if (wasLastNode) {
        level->paths.insert(oldPathIndex, path);
    }

    path->insertNode(node, oldNodeIndex);
}

void DeletePathNode::redo()
{
    path->removeNode(node);

    if (wasLastNode) {
        level->paths.removeOne(path);
    }
}

} // namespace EditorCommand
