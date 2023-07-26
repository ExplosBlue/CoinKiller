#ifndef EDITORCOMMAND_RAISELAYER_H
#define EDITORCOMMAND_RAISELAYER_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class RaiseLayer : public QUndoCommand
{
public:
    RaiseLayer(Level *level, Object *obj);
    ~RaiseLayer() {}

    void undo() override;
    void redo() override;

private:
    Level *level;
    Object *obj;
    quint32 prevLayer;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_RAISELAYER_H
