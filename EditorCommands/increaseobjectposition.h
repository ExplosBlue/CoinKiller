#ifndef EDITORCOMMAND_INCREASEOBJECTPOSITION_H
#define EDITORCOMMAND_INCREASEOBJECTPOSITION_H

#include <QUndoCommand>

#include "objects.h"
#include "commandids.h"

namespace EditorCommand {

class IncreaseObjectPosition : public QUndoCommand
{
public:
    IncreaseObjectPosition(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap = 0);
    ~IncreaseObjectPosition() {}

    void undo() override;
    void redo() override;
    int id() const override { return CommandID::IncreasePosition; }
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    Object *obj;
    quint32 deltaX;
    quint32 deltaY;
    quint32 snap;
    quint32 oldX;
    quint32 oldY;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INCREASEOBJECTPOSITION_H
