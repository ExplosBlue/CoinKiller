#ifndef EDITORCOMMAND_SETSPRITEINTERACTION_H
#define EDITORCOMMAND_SETSPRITEINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class SetSpriteInteraction : public QUndoCommand
{
public:
    SetSpriteInteraction(bool *spriteInteraction, bool state);
    ~SetSpriteInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *spriteInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETSPRITEINTERACTION_H
