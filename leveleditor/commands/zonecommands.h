#ifndef COMMANDS_ZONECMD_H
#define COMMANDS_ZONECMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::ZoneCmd {

class SetId : public QUndoCommand
{
public:
    SetId(Zone *zone, quint8 id);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newId;
    const quint8 oldId;
};


class SetProgPathId : public QUndoCommand
{
public:
    SetProgPathId(Zone *zone, quint8 id);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newId;
    const quint8 oldId;
};


class SetMusicId : public QUndoCommand
{
public:
    SetMusicId(Zone *zone, quint8 id);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newId;
    const quint8 oldId;
};


class SetMultiplayerTracking : public QUndoCommand
{
public:
    SetMultiplayerTracking(Zone *zone, quint8 tracking);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newTracking;
    const quint8 oldTracking;
};


class SetUnk1 : public QUndoCommand
{
public:
    SetUnk1(Zone *zone, quint32 value);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint32 newValue;
    const quint32 oldValue;
};


class SetBoundingId : public QUndoCommand
{
public:
    SetBoundingId(Zone *zone, quint8 id);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newId;
    const quint8 oldId;
};


class SetBackgroundId : public QUndoCommand
{
public:
    SetBackgroundId(Zone *zone, quint8 id);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newId;
    const quint8 oldId;
};


class SetCameraFlags : public QUndoCommand
{
public:
    SetCameraFlags(Zone *zone, quint8 unk19);

    void undo() override;
    void redo() override;

private:
    Zone *const zone;
    const quint8 newValue;
    const quint8 oldValue;
};

} // namespace Commands::ZoneCmd

#endif // COMMANDS_ZONECMD_H
