#include "insertpathnode.h"

namespace EditorCommand {

InsertPathNode::InsertPathNode(Path *path, PathNode *node, quint32 index) :
    path(path), node(node), index(index)
{
    this->setText(QObject::tr("Inserted Node into Path %1 at X: %2 Y: %3").arg(path->getid()).arg(node->getx()).arg(node->gety()));
}

InsertPathNode::~InsertPathNode()
{
    if (deletable) {
        delete node;
    }
}

void InsertPathNode::undo()
{
    path->removeNode(node);
    deletable = true;
}

void InsertPathNode::redo()
{
    path->insertNode(node, index);
    deletable = false;
}

} // namespace EditorCommand
