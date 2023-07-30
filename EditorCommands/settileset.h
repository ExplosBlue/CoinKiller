#ifndef EDITORCOMMAND_SETTILESET_H
#define EDITORCOMMAND_SETTILESET_H

#include <QUndoCommand>

#include "tileset.h"
#include "game.h"

namespace EditorCommand {

class SetTileset : public QUndoCommand
{
public:
    SetTileset(Tileset *&tileset, Game *game, QString tsName);

    void undo() override;
    void redo() override;

private:
    Tileset *&tileset;
    Game *game;
    QString oldTsName;
    QString newTsName;
};

} // namespace EditorCommand

#endif // EDITORCOMMAND_SETTILESET_H
