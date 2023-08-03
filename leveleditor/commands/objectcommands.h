#ifndef COMMANDS_OBJECTCMD_H
#define COMMANDS_OBJECTCMD_H

#include <QUndoCommand>

#include "commandids.h"
#include "objects.h"

namespace Commands::ObjectCmd {

class IncreasePosition : public QUndoCommand
{
public:
    IncreasePosition(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap = 0);

    void undo() override;
    void redo() override;
    int id() const override { return CommandID::Cmd_IncreasePosition; }
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    Object *obj;
    quint32 deltaX;
    quint32 deltaY;
    quint32 snap;
    quint32 oldX;
    quint32 oldY;
};


class Move : public QUndoCommand
{
public:
    Move(Object *obj, quint32 x, quint32 y);

    void undo() override;
    void redo() override;
    int id() const override { return CommandID::Cmd_Move; }
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    Object *obj;
    quint32 newX;
    quint32 newY;
    quint32 oldX;
    quint32 oldY;
};


class Resize : public QUndoCommand
{
public:
    Resize(Object *obj, quint32 w, quint32 h);

    void undo() override;
    void redo() override;
    int id() const override { return CommandID::Cmd_Resize; }
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    Object *obj;
    quint32 oldW;
    quint32 oldH;
    quint32 newW;
    quint32 newH;
};


class IncreaseSize : public QUndoCommand
{
public:
    IncreaseSize(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap = 0);

    void undo() override;
    void redo() override;

private:
    Object *obj;
    quint32 deltaX;
    quint32 deltaY;
    quint32 snap;
    quint32 oldW;
    quint32 oldH;
};

} // namespace Commands::ObjectCmd

#endif // COMMANDS_OBJECTCMD_H
