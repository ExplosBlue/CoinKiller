#ifndef EDITORCOMMAND_INSERTENTRANCE_H
#define EDITORCOMMAND_INSERTENTRANCE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertEntrance : public QUndoCommand
{
public:
    InsertEntrance(Level *level, Entrance *entr);
    ~InsertEntrance();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Entrance *entr;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTENTRANCE_H
