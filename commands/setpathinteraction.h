#ifndef EDITORCOMMAND_SETPATHINTERACTION_H
#define EDITORCOMMAND_SETPATHINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class setPathInteraction : public QUndoCommand
{
public:
    setPathInteraction(bool *pathInteraction, bool state);
    ~setPathInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *pathInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETPATHINTERACTION_H
