#ifndef EDITORCOMMAND_SETPATHINTERACTION_H
#define EDITORCOMMAND_SETPATHINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class SetPathInteraction : public QUndoCommand
{
public:
    SetPathInteraction(bool *pathInteraction, bool state);
    ~SetPathInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *pathInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETPATHINTERACTION_H
