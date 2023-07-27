#ifndef EDITORCOMMAND_INSERTLOCATION_H
#define EDITORCOMMAND_INSERTLOCATION_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertLocation : public QUndoCommand
{
public:
    InsertLocation(Level *level, Location *location);
    ~InsertLocation();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Location *location;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTLOCATION_H
