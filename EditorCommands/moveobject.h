#ifndef EDITORCOMMAND_MOVEOBJECT_H
#define EDITORCOMMAND_MOVEOBJECT_H

#include <QUndoCommand>

#include "objects.h"
#include "commandids.h"

namespace EditorCommand {

class MoveObject : public QUndoCommand
{
public:
    MoveObject(Object *obj, quint32 x, quint32 y);
    ~MoveObject() {}

    void undo() override;
    void redo() override;
    int id() const override { return CommandID::Cmd_Move; }
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    Object *obj;
    quint32 oldX;
    quint32 oldY;
    quint32 newX;
    quint32 newY;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_MOVEOBJECT_H
