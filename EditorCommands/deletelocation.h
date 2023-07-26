#ifndef EDITORCOMMAND_DELETELOCATION_H
#define EDITORCOMMAND_DELETELOCATION_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeleteLocation : public QUndoCommand
{
public:
    DeleteLocation(Level *level, Location *loc);
    ~DeleteLocation();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Location *loc;
    quint32 oldIndex;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETELOCATION_H
