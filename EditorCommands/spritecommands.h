#ifndef EDITORCOMMAND_SPRITE_SETLAYER_H
#define EDITORCOMMAND_SPRITE_SETLAYER_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::SpriteCmd {

class SetLayer : public QUndoCommand
{
public:
    SetLayer(Sprite *spr, quint8 layer);

    void undo() override;
    void redo() override;

private:
    Sprite *const spr;
    const quint8 newLayer;
    const quint8 oldLayer;
};


class SetBits : public QUndoCommand
{
public:
    SetBits(Sprite *spr, qint32 data, qint32 start, qint32 end);

    void undo() override;
    void redo() override;

private:
    Sprite *const spr;
    const qint32 newData;
    const qint32 oldData;
    const qint32 start;
    const qint32 end;
};


class SetByte : public QUndoCommand
{
public:
    SetByte(Sprite *spr, qint32 byteIndex, quint8 value);

    void undo() override;
    void redo() override;

private:
    Sprite *const spr;
    const qint32 byteIndex;
    const quint8 newValue;
    const quint8 oldValue;
};


class SetNybbleData : public QUndoCommand
{
public:
    SetNybbleData(Sprite *spr, qint32 data, qint32 start, qint32 end);

    void undo() override;
    void redo() override;

private:
    Sprite *const spr;
    const qint32 newData;
    const qint32 oldData;
    const qint32 start;
    const qint32 end;
};

} // namespace Commands::SpriteCmd

#endif // EDITORCOMMAND_SPRITE_SETLAYER_H
