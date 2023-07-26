#ifndef EDITORCOMMAND_LOWERLAYER_H
#define EDITORCOMMAND_LOWERLAYER_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class LowerLayer : public QUndoCommand
{
public:
    LowerLayer(Level *level, BgdatObject *obj);
    ~LowerLayer() {}

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

#endif // EDITORCOMMAND_LOWERLAYER_H
