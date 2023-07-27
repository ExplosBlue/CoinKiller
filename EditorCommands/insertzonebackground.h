#ifndef EDITORCOMMAND_INSERTZONEBACKGROUND_H
#define EDITORCOMMAND_INSERTZONEBACKGROUND_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertZoneBackground : public QUndoCommand
{
public:
    InsertZoneBackground(Level *level, ZoneBackground *background);
    ~InsertZoneBackground();

    void undo() override;
    void redo() override;

private:
    Level *level;
    ZoneBackground *background;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTZONEBACKGROUND_H
