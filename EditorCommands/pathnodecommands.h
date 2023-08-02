#ifndef COMMANDS_PATHNODECMD_H
#define COMMANDS_PATHNODECMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::PathNodeCmd {

class SetSpeed : public QUndoCommand
{
public:
    SetSpeed(PathNode *node, float speed);

    void undo() override;
    void redo() override;

private:
    PathNode *const node;
    const float newSpeed;
    const float oldSpeed;
};


class SetAccel : public QUndoCommand
{
public:
    SetAccel(PathNode *node, float accel);

    void undo() override;
    void redo() override;

private:
    PathNode *const node;
    const float newAccel;
    const float oldAccel;
};


class SetDelay : public QUndoCommand
{
public:
    SetDelay(PathNode *node, quint16 delay);

    void undo() override;
    void redo() override;

private:
    PathNode *const node;
    const quint16 newDelay;
    const quint16 oldDelay;
};


class SetRotation : public QUndoCommand
{
public:
    SetRotation(PathNode *node, qint16 rotation);

    void undo() override;
    void redo() override;

private:
    PathNode *const node;
    const qint16 newRotation;
    const qint16 oldRotation;
};


class SetVariableField : public QUndoCommand
{
public:
    SetVariableField(PathNode *node, quint8 value);

    void undo() override;
    void redo() override;

private:
    PathNode *const node;
    const quint8 newValue;
    const quint8 oldValue;
};


class SetNextPathID : public QUndoCommand
{
public:
    SetNextPathID(PathNode *node, quint8 id);

    void undo() override;
    void redo() override;

private:
    PathNode *const node;
    const quint8 newId;
    const quint8 oldId;
};

} // namespace Commands::PathNodeCmd

#endif // COMMANDS_PATHNODECMD_H
