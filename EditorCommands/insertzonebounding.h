#ifndef EDITORCOMMAND_INSERTZONEBOUNDING_H
#define EDITORCOMMAND_INSERTZONEBOUNDING_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertZoneBounding : public QUndoCommand
{
public:
    InsertZoneBounding(Level *level, ZoneBounding *bounding);
    ~InsertZoneBounding() override;

    void undo() override;
    void redo() override;

private:
    Level *level;
    ZoneBounding *bounding;
    bool deletable = false;
};


class RemoveZoneBounding : public QUndoCommand
{
public:
    RemoveZoneBounding(Level *level, ZoneBounding *bounding);
    ~RemoveZoneBounding() override;

    void undo() override;
    void redo() override;

private:
    Level *level;
    ZoneBounding *bounding;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTZONEBOUNDING_H
