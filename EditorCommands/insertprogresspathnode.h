#ifndef EDITORCOMMAND_INSERTPROGRESSPATHNODE_H
#define EDITORCOMMAND_INSERTPROGRESSPATHNODE_H

#include <QUndoCommand>

#include "objects.h"

namespace EditorCommand {

class InsertProgressPathNode : public QUndoCommand
{
public:
    InsertProgressPathNode(ProgressPath *path, ProgressPathNode *node, quint32 index = -1);
    ~InsertProgressPathNode();

    void undo() override;
    void redo() override;

private:
    ProgressPath *path;
    ProgressPathNode *node;
    quint32 index;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTPROGRESSPATHNODE_H
