#include "deleteprogresspathnode.h"

namespace EditorCommand {

DeleteProgressPathNode::DeleteProgressPathNode(Level *level, ProgressPathNode *node) :
    level(level), node(node)
{
    path = node->getParentPath();
    oldNodeIndex = path->getIndexOfNode(node);

    if (path->getNumberOfNodes() <= 1) {
        wasLastNode = true;
        oldPathIndex = level->progressPaths.indexOf(path);
    }

    this->setText(QObject::tr("Deleted ProgressPathNode"));
}

DeleteProgressPathNode::~DeleteProgressPathNode()
{
    if (node == nullptr || path == nullptr || level == nullptr) {
        qDebug() << "Deleted Nothing";
        return;
    }

    if (!level->progressPaths.contains(path)) {
        qDebug() << "Deleted ProgressPathNode";
        delete node;

        if (path->getNumberOfNodes() <= 0) {
            qDebug() << "Deleted ProgressPath";
            delete path;
        }
    }
}

void DeleteProgressPathNode::undo()
{
    if (wasLastNode) {
        level->progressPaths.insert(oldPathIndex, path);
    }

    path->insertNode(node, oldNodeIndex);
}

void DeleteProgressPathNode::redo()
{
    path->removeNode(node);

    if (wasLastNode) {
        level->progressPaths.removeOne(path);
    }
}

} // namespace EditorCommand
