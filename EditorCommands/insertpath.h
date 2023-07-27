#ifndef EDITORCOMMAND_INSERTPATH_H
#define EDITORCOMMAND_INSERTPATH_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertPath : public QUndoCommand
{
public:
    InsertPath(Level *level, Path *path);
    ~InsertPath();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Path *path;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTPATH_H
