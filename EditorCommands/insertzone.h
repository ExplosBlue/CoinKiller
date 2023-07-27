#ifndef EDITORCOMMAND_INSERTZONE_H
#define EDITORCOMMAND_INSERTZONE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertZone : public QUndoCommand
{
public:
    InsertZone(Level *level, Zone *zone);
    ~InsertZone();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Zone *zone;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTZONE_H
