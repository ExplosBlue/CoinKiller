#ifndef EDITORCOMMAND_SETLAYERMASK_H
#define EDITORCOMMAND_SETLAYERMASK_H

#include <QUndoCommand>

#include "layermask.h"

namespace EditorCommand {

class setLayerMask : public QUndoCommand
{
public:
    setLayerMask(quint8 *layerMask, LAYER_MASK layer, bool state);
    ~setLayerMask() {};

    void undo() override;
    void redo() override;

private:
    quint8 *layerMask;
    LAYER_MASK layer;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETLAYERMASK_H
