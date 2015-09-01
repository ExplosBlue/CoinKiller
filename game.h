#ifndef GAME_H
#define GAME_H

#include "filesystem.h"
#include "tileset.h"


class Game
{
public:
    Game(FilesystemBase* fs);


    Tileset* getTileset(QString name);


    FilesystemBase* fs;
};

#endif // GAME_H
