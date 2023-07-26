#ifndef EDITORCOMMAND_SETENTRANCEINTERACTION_H
#define EDITORCOMMAND_SETENTRANCEINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class SetEntranceInteraction : public QUndoCommand
{
public:
    SetEntranceInteraction(bool *entranceInteraction, bool state);
    ~SetEntranceInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *entranceInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETENTRANCEINTERACTION_H
