#ifndef EDITORCOMMAND_LOWEROBJECT_H
#define EDITORCOMMAND_LOWEROBJECT_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class LowerObject : public QUndoCommand
{
public:
    LowerObject(Level *level, Object *obj);
    ~LowerObject() {}

    void undo() override;
    void redo() override;

private:
    Level *level;
    Object *obj;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_LOWEROBJECT_H
