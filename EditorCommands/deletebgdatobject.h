#ifndef EDITORCOMMAND_DELETEBGDATOBJECT_H
#define EDITORCOMMAND_DELETEBGDATOBJECT_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeleteBgdatObject : public QUndoCommand
{
public:
    DeleteBgdatObject(Level *level, BgdatObject *obj);
    ~DeleteBgdatObject();

    void undo() override;
    void redo() override;

private:
    Level *level;
    BgdatObject *obj;
    quint32 oldIndex;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETEBGDATOBJECT_H
