#ifndef EDITORCOMMAND_SETLOCATIONINTERACTION_H
#define EDITORCOMMAND_SETLOCATIONINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class setLocationInteraction : public QUndoCommand
{
public:
    setLocationInteraction(bool *locationInteraction, bool state);
    ~setLocationInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *locationInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETLOCATIONINTERACTION_H
