#ifndef EDITORCOMMAND_SETLAYERMASK_H
#define EDITORCOMMAND_SETLAYERMASK_H

#include <QUndoCommand>

#include "layermask.h"

namespace EditorCommand {

class SetLayerMask : public QUndoCommand
{
public:
    SetLayerMask(quint8 *layerMask, LAYER_MASK layer, bool state);
    ~SetLayerMask() {};

    void undo() override;
    void redo() override;

private:
    quint8 *layerMask;
    LAYER_MASK layer;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETLAYERMASK_H
