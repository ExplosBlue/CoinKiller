#ifndef COMMANDS_BGDATOBJCMD_H
#define COMMANDS_BGDATOBJCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::BgdatCmd {

class ChangeObject : public QUndoCommand
{
public:
    ChangeObject(BgdatObject *obj, qint32 objId, qint32 tilesetId);

    void undo() override;
    void redo() override;

private:
    BgdatObject *obj;
    qint32 newObjId;
    qint32 oldObjId;
    qint32 newTilesetId;
    qint32 oldTilesetId;
};

} // namespace Commands::BgdatCmd

#endif // COMMANDS_BGDATOBJCMD_H
