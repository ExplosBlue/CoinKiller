#ifndef EDITORCOMMAND_DELETEENTRANCE_H
#define EDITORCOMMAND_DELETEENTRANCE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeleteEntrance : public QUndoCommand
{
public:
    DeleteEntrance(Level *level, Entrance *entr);
    ~DeleteEntrance();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Entrance *entr;
    quint32 oldIndex;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETEENTRANCE_H
