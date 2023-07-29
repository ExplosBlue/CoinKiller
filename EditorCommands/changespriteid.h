#ifndef EDITORCOMMAND_CHANGESPRITEID_H
#define EDITORCOMMAND_CHANGESPRITEID_H

#include <QUndoCommand>

#include "objects.h"

namespace EditorCommand {

class ChangeSpriteID : public QUndoCommand
{
public:
    ChangeSpriteID(Sprite *spr, quint16 id);
    ~ChangeSpriteID() {}

    void undo() override;
    void redo() override;

private:
    Sprite *spr;
    quint16 newId;
    quint16 oldId;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_CHANGESPRITEID_H
