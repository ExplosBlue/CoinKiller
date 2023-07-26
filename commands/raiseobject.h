#ifndef EDITORCOMMAND_RAISEOBJECT_H
#define EDITORCOMMAND_RAISEOBJECT_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class raiseObject : public QUndoCommand
{
public:
    raiseObject(Level *level, Object *obj);
    ~raiseObject() {}

    void undo() override;
    void redo() override;

private:
    Level *level;
    Object *obj;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_RAISEOBJECT_H
