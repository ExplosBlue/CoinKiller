#ifndef COMMANDS_LOCATIONCMD_H
#define COMMANDS_LOCATIONCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::LocationCmd {

class SetId : public QUndoCommand
{
public:
    SetId(Location *loc, quint8 id);

    void undo() override;
    void redo() override;

private:
    Location *const loc;
    const quint8 newId;
    const quint8 oldId;
};

} // namespace Commands::LocationCmd

#endif // COMMANDS_LOCATIONCMD_H
