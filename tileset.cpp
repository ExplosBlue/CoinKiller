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

    archive = new SarcFilesystem(game->fs->openFile("/Unit/"+name+".sarc"));
    texture = new Ctpk(archive->openFile("/BG_tex/"+name+".ctpk"));

    texImage = texture->getTexture(0);
}

Tileset::~Tileset()
{
    delete texImage;

    delete texture;
    delete archive;
}


// x and y in tile coords
void Tileset::drawTile(QPainter& painter, int num, int x, int y, float zoom)
{
    int tsize = (int)(20*zoom);
    x *= tsize;
    y *= tsize;

    QRect rsrc(2 + ((num%21)*24), 2 + ((num/21)*24), 20, 20);
    QRect rdst(x, y, tsize, tsize);

    painter.drawImage(rdst, *texImage, rsrc);
}

