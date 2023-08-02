#ifndef EDITORCOMMAND_INSERTZONEBACKGROUND_H
#define EDITORCOMMAND_INSERTZONEBACKGROUND_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertZoneBackground : public QUndoCommand
{
public:
    InsertZoneBackground(Level *level, ZoneBackground *background);
    ~InsertZoneBackground() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ZoneBackground *background;
    bool deletable = false;
};


class RemoveZoneBackground : public QUndoCommand
{
public:
    RemoveZoneBackground(Level *level, ZoneBackground *background);
    ~RemoveZoneBackground() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ZoneBackground *background;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTZONEBACKGROUND_H
