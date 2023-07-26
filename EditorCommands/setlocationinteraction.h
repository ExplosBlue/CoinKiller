#ifndef EDITORCOMMAND_SETLOCATIONINTERACTION_H
#define EDITORCOMMAND_SETLOCATIONINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class SetLocationInteraction : public QUndoCommand
{
public:
    SetLocationInteraction(bool *locationInteraction, bool state);
    ~SetLocationInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *locationInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETLOCATIONINTERACTION_H
