#ifndef EDITORCOMMAND_RAISELAYER_H
#define EDITORCOMMAND_RAISELAYER_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class RaiseLayer : public QUndoCommand
{
public:
    RaiseLayer(Level *level, BgdatObject *obj);
    ~RaiseLayer() {}

    void undo() override;
    void redo() override;

private:
    Level *level;
    BgdatObject *obj;
    quint32 prevLayer;
    quint32 newLayer;
    quint32 prevIndex;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_RAISELAYER_H