#ifndef EDITORCOMMAND_RESIZEOBJECT_H
#define EDITORCOMMAND_RESIZEOBJECT_H

#include <QUndoCommand>

#include "objects.h"
#include "commandids.h"

namespace EditorCommand {

class ResizeObject : public QUndoCommand
{
public:
    ResizeObject(Object *obj, quint32 w, quint32 h);
    ~ResizeObject() {}

    void undo() override;
    void redo() override;
    int id() const override { return CommandID::Cmd_Resize; }
    bool mergeWith(const QUndoCommand *otherCommand) override;

private:
    Object *obj;
    quint32 oldW;
    quint32 oldH;
    quint32 newW;
    quint32 newH;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_RESIZEOBJECT_H
