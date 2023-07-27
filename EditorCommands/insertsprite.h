#ifndef EDITORCOMMAND_INSERTSPRITE_H
#define EDITORCOMMAND_INSERTSPRITE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class InsertSprite : public QUndoCommand
{
public:
    InsertSprite(Level *level, Sprite *spr);
    ~InsertSprite();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Sprite *spr;
    bool deletable = false;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_INSERTSPRITE_H
