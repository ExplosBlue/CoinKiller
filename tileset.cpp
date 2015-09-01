/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

#include "tileset.h"
#include "game.h"

Tileset::Tileset(Game *game, QString name)
{
    this->game = game;

    archive = new SarcFilesystem(game->fs->openFile("/Unit/"+name+".arc"));
    texture = new Ctpk(archive->openFile("/BG_tex/"+name+".ctpk"));
}

