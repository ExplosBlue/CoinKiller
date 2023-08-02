#ifndef COMMANDS_PATHCMD_H
#define COMMANDS_PATHCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::PathCmd {

class SetId : public QUndoCommand
{
public:
    SetId(Path *path, qint32 id);

    void undo() override;
    void redo() override;

private:
    Path *const path;
    const qint32 newId;
    const qint32 oldId;
};


class SetLoop : public QUndoCommand
{
public:
    SetLoop(Path *path, qint32 loop);

    void undo() override;
    void redo() override;

private:
    Path *const path;
    const qint32 newLoop;
    const qint32 oldLoop;
};


class SwapNodes : public QUndoCommand
{
public:
    SwapNodes(Path *path, qsizetype node1, qsizetype node2);

    void undo() override;
    void redo() override;

private:
    Path *const path;
    const qsizetype node1;
    const qsizetype node2;
};

} // namespace Commands::PathCmd

#endif // COMMANDS_PATHCMD_H
