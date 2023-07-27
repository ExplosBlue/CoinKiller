#ifndef EDITORCOMMAND_INSERTPATHNODE_H
#define EDITORCOMMAND_INSERTPATHNODE_H

#include <QUndoCommand>

#include "objects.h"

namespace EditorCommand {

class InsertPathNode : public QUndoCommand
{
public:
    InsertPathNode(Path *path, PathNode *node, quint32 index = -1);
    ~InsertPathNode();

    void undo() override;
    void redo() override;

private:
    Path *path;
    PathNode *node;
    quint32 index;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTPATHNODE_H
