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

#include "level.h"
#include "game.h"

Level::Level(Game *game, int world, int level, int area)
{
    this->game = game;

    QString arcpath = QString("/Course/%1-%2.sarc").arg(world).arg(level);
    qDebug(arcpath.toStdString().c_str());
    archive = new SarcFilesystem(game->fs->openFile(arcpath));
    this->area = area;


    QString headerfile = QString("/course/course%1.bin").arg(area);
    if (!archive->fileExists(headerfile))
        throw std::runtime_error("Level has no header!!");

    // read header
    FileBase* header = archive->openFile(headerfile);
    header->open();
    header->seek(0);

    quint32 block0offset = header->read32();
    for (int t = 0; t < 4; t++)
    {
        header->seek(block0offset + (t*32));

        QString tilesetname;
        header->readStringASCII(tilesetname, 32);

        if (tilesetname.isEmpty())
        {
            tilesets[t] = NULL;
            continue;
        }

        try
        {
            tilesets[t] = game->getTileset(tilesetname);
        }
        catch ( const std::exception & e )
        {
            qDebug("Tileset %s not found", tilesetname.toStdString().c_str());
            tilesets[t] = NULL;
        }
    }

    header->close();
    delete header;


    // read bgdat
    QString bgdatfiletemp = QString("/course/course%1_bgdatL%2.bin").arg(area);
    for (int l = 0; l < 3; l++)
    {
        QString bgdatfile = bgdatfiletemp.arg(l+1);
        if (!archive->fileExists(bgdatfile)) continue;

        FileBase* bgdat = archive->openFile(bgdatfile);
        bgdat->open();
        bgdat->seek(0);
        for (;;)
        {
            if (bgdat->pos() >= bgdat->size()) break;
            quint16 id = bgdat->read16();
            if (id == 0xFFFF) break;

            BgdatObject* obj = new BgdatObject;
            obj->id = id;
            obj->x = bgdat->read16();
            obj->y = bgdat->read16();
            obj->width = bgdat->read16();
            obj->height = bgdat->read16();
            objects[l].append(*obj);

            bgdat->skip(6);
        }
        bgdat->close();
        delete bgdat;
    }
}

Level::~Level()
{
    for (int t = 0; t < 4; t++)
    {
        if (tilesets[t])
            delete tilesets[t];
    }

    /*for (int l = 0; l < 3; l++)
    {
        for (int o = 0; o < objects[l].size(); o++)
            //
    }*/

    delete archive;
}

