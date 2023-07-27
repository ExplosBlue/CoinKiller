#ifndef EDITORCOMMAND_DELETEPROGRESSPATHNODE_H
#define EDITORCOMMAND_DELETEPROGRESSPATHNODE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeleteProgressPathNode : public QUndoCommand
{
public:
    DeleteProgressPathNode(Level *level, ProgressPathNode *node);
    ~DeleteProgressPathNode();

    void undo() override;
    void redo() override;

private:
    Level *level;
    ProgressPathNode *node;
    ProgressPath *path;
    bool wasLastNode = false;
    quint32 oldNodeIndex;
    quint32 oldPathIndex;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETEPROGRESSPATHNODE_H
