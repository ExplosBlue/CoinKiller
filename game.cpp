#include "game.h"

Game::Game(FilesystemBase* fs)
{
    this->fs = fs;
}


Tileset* Game::getTileset(QString name)
{
    return new Tileset(this, name);
}

