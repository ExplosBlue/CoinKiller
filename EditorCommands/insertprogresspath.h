#ifndef EDITORCOMMAND_INSERTPROGRESSPATH_H
#define EDITORCOMMAND_INSERTPROGRESSPATH_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertProgressPath : public QUndoCommand
{
public:
    InsertProgressPath(Level *level, ProgressPath *path);
    ~InsertProgressPath();

    void undo() override;
    void redo() override;

private:
    Level *level;
    ProgressPath *path;
    bool deletable = false;
};
} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTPROGRESSPATH_H
