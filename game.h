#ifndef GAME_H
#define GAME_H

#include "filesystem.h"
#include "tileset.h"
#include "level.h"


class Game
{
public:
    Game(FilesystemBase* fs);


    Tileset* getTileset(QString name);
    Level* getLevel(int world, int level, int area);


    FilesystemBase* fs;
};

#endif // GAME_H
