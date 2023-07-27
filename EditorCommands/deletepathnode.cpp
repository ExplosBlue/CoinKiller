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
    if (level == nullptr) {
        return;
    }

    if (deletable) {
        qDebug() << "EditorCommand::DeletePathNode - Deleting PathNode";
        delete node;

        if (wasLastNode) {
            qDebug() << "EditorCommand::DeletePathNode - Deleting Path";
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
    deletable = false;
}

void DeletePathNode::redo()
{
    path->removeNode(node);

    if (wasLastNode) {
        level->paths.removeOne(path);
    }
    deletable = true;
}

} // namespace EditorCommand
