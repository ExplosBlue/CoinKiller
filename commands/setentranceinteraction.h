#ifndef EDITORCOMMAND_SETENTRANCEINTERACTION_H
#define EDITORCOMMAND_SETENTRANCEINTERACTION_H

#include <QUndoCommand>

namespace EditorCommand {

class setEntranceInteraction : public QUndoCommand
{
public:
    setEntranceInteraction(bool *entranceInteraction, bool state);
    ~setEntranceInteraction() {};

    void undo() override;
    void redo() override;

private:
    bool *entranceInteraction;
    bool state;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETENTRANCEINTERACTION_H
