#ifndef EDITORCOMMAND_SETSPRITEINTERACTION_H
#define EDITORCOMMAND_SETSPRITEINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class setSpriteInteraction : public QUndoCommand
{
public:
    setSpriteInteraction(bool *spriteInteraction, bool state);
    ~setSpriteInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *spriteInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETSPRITEINTERACTION_H
