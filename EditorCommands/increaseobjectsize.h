#ifndef EDITORCOMMAND_INCREASEOBJECTSIZE_H
#define EDITORCOMMAND_INCREASEOBJECTSIZE_H

#include <QUndoCommand>

#include "objects.h"

namespace EditorCommand {

class IncreaseObjectSize : public QUndoCommand
{
public:
    IncreaseObjectSize(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap = 0);
    ~IncreaseObjectSize() {}

    void undo() override;
    void redo() override;

private:
    Object *obj;
    quint32 deltaX;
    quint32 deltaY;
    quint32 snap;
    quint32 oldW;
    quint32 oldH;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INCREASEOBJECTSIZE_H
