#ifndef COMMANDS_ZONEBOUNDINGCMD_H
#define COMMANDS_ZONEBOUNDINGCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::ZoneBoundingCmd {

class SetUpScrolling : public QUndoCommand
{
public:
    SetUpScrolling(ZoneBounding *bounding, quint16 value);

    void undo() override;
    void redo() override;

private:
    ZoneBounding *const bounding;
    const quint16 newValue;
    const quint16 oldValue;
};


class SetPrimaryUpperBound : public QUndoCommand
{
public:
    SetPrimaryUpperBound(ZoneBounding *bounding, qint32 value);

    void undo() override;
    void redo() override;

private:
    ZoneBounding *const bounding;
    const qint32 newValue;
    const qint32 oldValue;
};


class SetPrimaryLowerBound : public QUndoCommand
{
public:
    SetPrimaryLowerBound(ZoneBounding *bounding, qint32 value);

    void undo() override;
    void redo() override;

private:
    ZoneBounding *const bounding;
    const qint32 newValue;
    const qint32 oldValue;
};


class SetSecondaryUpperBound : public QUndoCommand
{
public:
    SetSecondaryUpperBound(ZoneBounding *bounding, qint32 value);

    void undo() override;
    void redo() override;

private:
    ZoneBounding *const bounding;
    const qint32 newValue;
    const qint32 oldValue;
};


class SetSecondaryLowerBound : public QUndoCommand
{
public:
    SetSecondaryLowerBound(ZoneBounding *bounding, qint32 value);

    void undo() override;
    void redo() override;

private:
    ZoneBounding *const bounding;
    const qint32 newValue;
    const qint32 oldValue;
};


class SetId : public QUndoCommand
{
public:
    SetId(ZoneBounding *bounding, quint16 id);

    void undo() override;
    void redo() override;

private:
    ZoneBounding *const bounding;
    const quint16 newId;
    const quint16 oldId;
};


} // namespace Commands::ZoneBoundingCmd

#endif // COMMANDS_ZONEBOUNDINGCMD_H
