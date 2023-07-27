#ifndef EDITORCOMMAND_INSERTBGDATOBJ_H
#define EDITORCOMMAND_INSERTBGDATOBJ_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertBgdatObj : public QUndoCommand
{
public:
    InsertBgdatObj(Level *level, BgdatObject *obj);
    ~InsertBgdatObj();

    void undo() override;
    void redo() override;

private:
    Level *level;
    BgdatObject *obj;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTBGDATOBJ_H
