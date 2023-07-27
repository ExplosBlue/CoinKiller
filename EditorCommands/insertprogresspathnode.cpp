#include "insertprogresspathnode.h"

namespace EditorCommand {

InsertProgressPathNode::InsertProgressPathNode(ProgressPath *path, ProgressPathNode *node, quint32 index) :
    path(path), node(node), index(index)
{
    this->setText(QObject::tr("Inserted Node into Progress Path %1 at X: %2 Y: %3").arg(path->getid()).arg(node->getx()).arg(node->gety()));
}

InsertProgressPathNode::~InsertProgressPathNode()
{
    if (deletable) {
        delete node;
    }
}

void InsertProgressPathNode::undo()
{
    path->removeNode(node);
    deletable = true;
}

void InsertProgressPathNode::redo()
{
    path->insertNode(node, index);
    deletable = false;
}

} // namespace EditorCommand
