#ifndef EDITORCOMMAND_CHANGEBGDATOBJECT_H
#define EDITORCOMMAND_CHANGEBGDATOBJECT_H

#include <QUndoCommand>

#include "objects.h"

namespace EditorCommand {

class ChangeBgdatObject : public QUndoCommand
{
public:
    ChangeBgdatObject(BgdatObject *obj, qint32 objID, qint32 tilesetID);
    ~ChangeBgdatObject() {}

    void undo() override;
    void redo() override;

private:
    BgdatObject *obj;
    qint32 oldObjID;
    qint32 newObjID;
    qint32 oldTilesetID;
    qint32 newTilesetID;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_CHANGEBGDATOBJECT_H
