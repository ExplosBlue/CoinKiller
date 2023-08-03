#ifndef COMMANDS_PROGRESSPATHCMD_H
#define COMMANDS_PROGRESSPATHCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::ProgressPathCmd {

class SetId : public QUndoCommand
{
public:
    SetId(ProgressPath *path, qint32 id);

    void undo() override;
    void redo() override;

private:
    ProgressPath *const path;
    const qint32 newId;
    const qint32 oldId;
};


class SetAltPathFlag : public QUndoCommand
{
public:
    SetAltPathFlag(ProgressPath *path, bool flag);

    void undo() override;
    void redo() override;

private:
    ProgressPath *const path;
    const bool flag;
};

} // namespace Commands::ProgressPathCmd

#endif // COMMANDS_PROGRESSPATHCMD_H
