#ifndef COMMANDS_ENTRANCECMD_H
#define COMMANDS_ENTRANCECMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::EntranceCmd {

class SetType : public QUndoCommand
{
public:
    SetType(Entrance *entr, quint8 type);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const quint8 newType;
    const quint8 oldType;
};


class SetId : public QUndoCommand
{
public:
    SetId(Entrance *entr, quint8 id);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const quint8 newId;
    const quint8 oldId;
};


class SetDestEntr : public QUndoCommand
{
public:
    SetDestEntr(Entrance *entr, quint8 destEntrId);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const quint8 newDest;
    const quint8 oldDest;
};


class SetDestArea : public QUndoCommand
{
public:
    SetDestArea(Entrance *entr, quint8 destAreaId);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const quint8 newDest;
    const quint8 oldDest;
};


class SetCamXOffset : public QUndoCommand
{
public:
    SetCamXOffset(Entrance *entr, qint16 xOffset);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const qint16 newXOffset;
    const qint16 oldXOffset;
};


class SetCamYOffset : public QUndoCommand
{
public:
    SetCamYOffset(Entrance *entr, qint16 yOffset);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const qint16 newYOffset;
    const qint16 oldYOffset;
};


class SetUnk1 : public QUndoCommand
{
public:
    SetUnk1(Entrance *entr, quint8 unk1);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const quint8 newUnk1;
    const quint8 oldUnk1;
};


class SetUnk2 : public QUndoCommand
{
public:
    SetUnk2(Entrance *entr, quint8 unk2);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const quint8 newUnk2;
    const quint8 oldUnk2;
};


class SetSettingsBit : public QUndoCommand
{
public:
    SetSettingsBit(Entrance *entr, bool value, qint32 bit);

    void undo() override;
    void redo() override;

private:
    Entrance *const entr;
    const bool bitValue;
    const qint32 bitIndex;
};

} // namespace Commands::EntranceCmd

#endif // COMMANDS_ENTRANCECMD_H
