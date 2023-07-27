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
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeleteProgressPathNode - Deleting ProgressPathNode";
        delete node;

        if (wasLastNode) {
            qDebug() << "EditorCommand::DeleteProgressPathNode - Deleting ProgressPath";
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
    deletable = false;
}

void DeleteProgressPathNode::redo()
{
    path->removeNode(node);

    if (wasLastNode) {
        level->progressPaths.removeOne(path);
    }
    deletable = true;
}

} // namespace EditorCommand
