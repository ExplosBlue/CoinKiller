#ifndef EDITORCOMMAND_RAISEOBJECT_H
#define EDITORCOMMAND_RAISEOBJECT_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class RaiseObject : public QUndoCommand
{
public:
    RaiseObject(Level *level, Object *obj);
    ~RaiseObject() {}

    void undo() override;
    void redo() override;

private:
    Level *level;
    Object *obj;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_RAISEOBJECT_H
