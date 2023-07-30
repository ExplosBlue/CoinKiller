#include "settileset.h"

#include <utility>

namespace EditorCommand {

SetTileset::SetTileset(Tileset *&tileset, Game *game, QString tsName) :
    tileset(tileset), game(game), newTsName(std::move(tsName))
{
    if (tileset == nullptr) {
        oldTsName = "";
    } else {
        oldTsName = tileset->getName();
    }

    this->setText(QObject::tr("Changed Tileset"));
}

void SetTileset::undo()
{
    delete tileset;

    if (oldTsName == "") {
        tileset = nullptr;
    } else {
        tileset = game->getTileset(oldTsName);
    }
}

void SetTileset::redo()
{
    delete tileset;

    if (newTsName == "") {
        tileset = nullptr;
    } else {
        tileset = game->getTileset(newTsName);
    }
}

} // namespace EditorCommand
