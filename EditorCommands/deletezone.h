#ifndef EDITORCOMMAND_DELETEZONE_H
#define EDITORCOMMAND_DELETEZONE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeleteZone : public QUndoCommand
{
public:
    DeleteZone(Level *level, Zone *zone);
    ~DeleteZone();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Zone *zone;
    quint32 oldIndex;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETEZONE_H
