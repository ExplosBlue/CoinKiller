#ifndef COMMANDS_EDITORCMD_H
#define COMMANDS_EDITORCMD_H

#include <QUndoCommand>

#include "leveleditor/layermask.h"

namespace Commands::EditorCmd {

class SetLayerMask : public QUndoCommand
{
public:
    SetLayerMask(quint8 *layerMask, LAYER_MASK layer, bool state);

    void undo() override;
    void redo() override;

private:
    quint8 *layerMask;
    LAYER_MASK layer;
    bool state;
};


class SetSpriteInteraction : public QUndoCommand
{
public:
    SetSpriteInteraction(bool *spriteInteraction, bool state);

    void undo() override;
    void redo() override;

private:
    bool *spriteInteraction;
    bool state;
};


class SetEntranceInteraction : public QUndoCommand
{
public:
    SetEntranceInteraction(bool *entranceInteraction, bool state);

    void undo() override;
    void redo() override;

private:
    bool *entranceInteraction;
    bool state;
};


class SetPathInteraction : public QUndoCommand
{
public:
    SetPathInteraction(bool *pathInteraction, bool state);

    void undo() override;
    void redo() override;

private:
    bool *pathInteraction;
    bool state;
};


class SetLocationInteraction : public QUndoCommand
{
public:
    SetLocationInteraction(bool *locationInteraction, bool state);

    void undo() override;
    void redo() override;

private:
    bool *locationInteraction;
    bool state;
};

} // namespace Commands::EditorCmd

#endif // COMMANDS_EDITORCMD_H
