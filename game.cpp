#include "game.h"

Game::Game(FilesystemBase* fs)
{
    this->fs = fs;
}


Tileset* Game::getTileset(QString name)
{
    return new Tileset(this, name);
}

Level* Game::getLevel(int world, int level, int area)
{
    return new Level(this, world, level, area);
}

