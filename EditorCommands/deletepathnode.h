#ifndef EDITORCOMMAND_DELETEPATHNODE_H
#define EDITORCOMMAND_DELETEPATHNODE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeletePathNode : public QUndoCommand
{
public:
    DeletePathNode(Level *level, PathNode *node);
    ~DeletePathNode();

    void undo() override;
    void redo() override;

private:
    Level *level;
    PathNode *node;
    Path *path;
    bool wasLastNode = false;
    quint32 oldNodeIndex;
    quint32 oldPathIndex;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETEPATHNODE_H
