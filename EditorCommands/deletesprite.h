#ifndef EDITORCOMMAND_DELETESPRITE_H
#define EDITORCOMMAND_DELETESPRITE_H

#include <QUndoCommand>

#include "level.h"

namespace EditorCommand {

class DeleteSprite : public QUndoCommand
{
public:
    DeleteSprite(Level *level, Sprite *spr);
    ~DeleteSprite();

    void undo() override;
    void redo() override;

private:
    Level *level;
    Sprite *spr;
    quint32 oldIndex;
    bool deletable;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_DELETESPRITE_H
