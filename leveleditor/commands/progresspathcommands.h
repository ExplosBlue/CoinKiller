#ifndef COMMANDS_PROGRESSPATHCMD_H
#define COMMANDS_PROGRESSPATHCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::ProgressPathCmd {

class InsertNode : public QUndoCommand
{
public:
    InsertNode(ProgressPath *path, ProgressPathNode *node, quint32 index = -1);
    ~InsertNode() override;

    void undo() override;
    void redo() override;

private:
    ProgressPath *path;
    ProgressPathNode *node;
    quint32 index;
    bool deletable = false;
};


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
    SetAltPathFlag(ProgressPath *path, quint8 flag);

    void undo() override;
    void redo() override;

private:
    ProgressPath *const path;
    const quint8 newFlag;
    const quint8 oldFlag;
};

} // namespace Commands::ProgressPathCmd

#endif // COMMANDS_PROGRESSPATHCMD_H
