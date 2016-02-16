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
#include "objects.h"
#include "unitsconvert.h"
#include "is.h"

Level::Level(Game *game, int world, int level, int area)
{
    this->game = game;

    this->world = world;
    this->level = level;

    QString arcpath = QString("/Course/%1-%2.sarc").arg(world).arg(level);
    qDebug(arcpath.toStdString().c_str());
    archive = new SarcFilesystem(game->fs->openFile(arcpath));
    this->area = area;


    QString headerfile = QString("/course/course%1.bin").arg(area);
    if (!archive->fileExists(headerfile))
        throw std::runtime_error("Level has no header!!");

    // read header blocks
    FileBase* header = archive->openFile(headerfile);
    header->open();
    header->seek(0);

    quint32 blockOffsets[17];
    quint32 blockSizes[17];
    for (int b = 0; b < 17; b++)
    {
        blockOffsets[b] = header->read32();
        blockSizes[b] = header->read32();
    }

    QList<tempZoneBounding> boundings;
    QList<tempZoneBackground> backgrounds;

    // Block 0: Tilesets
    for (int t = 0; t < 4; t++)
    {
        header->seek(blockOffsets[0] + (t*32));

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

    // TEST!!
    qDebug("SAVING TEST!!!!!!");
    header->save();
    qDebug("SAVING TEST END!!!!!!");

    // Block 1: Area Settings
    header->seek(blockOffsets[1]);
    header->skip(8); // 8 Zeros
    unk1 = header->read16();
    timeLimit = header->read16();
    header->skip(4);
    if (header->read8() == 2) toadHouseFlag = true;
    unk2 = header->read8();
    specialLevelFlag = header->read8();
    specialLevelFlag2 = header->read8();
    unk3 = header->read8();

    // Block 2: Zone Boundings
    header->seek(blockOffsets[2]);
    for (int i = 0; i < (int)(blockSizes[2]/28); i++)
    {
        tempZoneBounding bounding;
        bounding.upperBound = header->read32();
        bounding.lowerBound = header->read32();
        bounding.unkUpperBound = header->read32();
        bounding.unkUpperBound = header->read32();
        bounding.id = header->read16();
        bounding.upScrolling = header->read16();
        header->skip(8);
        boundings.append(bounding);
    }

    // Block 3: Unknown

    // Block 4: ZoneBackground Information
    header->seek(blockOffsets[4]);
    for (int i = 0; i < (int)(blockSizes[4]/28); i++)
    {
        tempZoneBackground background;
        background.id = header->read16();
        background.xScrollRate = header->read8();
        background.yScrollRate = header->read8();
        background.xPos = header->read8();
        background.yPos = header->read8();
        header->skip(2);
        header->readStringASCII(background.name, 15);
        background.unk1 = header->read16();
        header->skip(2);
        backgrounds.append(background);
    }

    // Block 5: Static / Dummy?

    // Block 6: Entrances
    header->seek(blockOffsets[6]);
    for (int e = 0; e < (int)(blockSizes[6]/24); e++)
    {
        Entrance* entr = new Entrance(to20(header->read16()), to20(header->read16()), header->read16(), header->read16(), header->read8(), header->read8(), header->read8(), header->read8());
        qDebug("Found Entrance with x: %d, y: %d and ID %d", entr->getx(), entr->gety(), entr->getid());
        entrances.append(entr);

        header->skip(12);
    }

    // Block 7: Sprites
    header->seek(blockOffsets[7]);
    for (;;)
    {
        quint16 id = header->read16();
        if (id == 0xFFFF) break;

        Sprite* spr = new Sprite(to20(header->read16()), to20(header->read16()), id);

        for (int i=0; i<8; i++) spr->setByte(i, header->read8());

        spr->setRect();
        sprites.append(spr);

        header->skip(10); // Unused Sprite Data and Zone
    }

    // Block 8: Sprites Used List
    header->seek(blockOffsets[8]);
    for (int i = 0; i < (int)(blockSizes[8]/4); i++)
    {
        spritesUsed.append(header->read16());
        qDebug("Sprites Used [%d]: ID %d", i, spritesUsed[i]);
        header->skip(2);
    }

    // Block 9: Zones
    header->seek(blockOffsets[9]);
    for (int z = 0; z < (int)(blockSizes[9]/24); z++)
    {
        quint16 x = header->read16();
        quint16 y = header->read16();
        quint16 width = header->read16();
        quint16 height = header->read16();
        quint16 zoneUnk1 = header->read16();
        header->skip(2);
        quint8 id = header->read8();
        quint8 boundingId = header->read8();
        header->skip(6);
        quint8 multiplayerTracking = header->read8();
        quint8 progPathId = header->read8();
        quint8 musicId = header->read8();
        header->skip(1);
        quint8 backgroundId = header->read8();
        header->skip(3);

        Zone* zone = new Zone(to20(x), to20(y), to20(width), to20(height), id, progPathId, musicId, multiplayerTracking, zoneUnk1);
        foreach(tempZoneBounding bounding, boundings)
        {
            if (boundingId == bounding.id)
            {
                zone->setBounding(bounding);
                qDebug() << "Found Bounding for Zone" << z;
                break;
            }
        }
        foreach(tempZoneBackground background, backgrounds)
        {
            if (backgroundId == background.id)
            {
                zone->setBackground(background);
                qDebug() << "Found Background for Zone" << z;
                break;
            }
        }

        zones.append(*zone);
    }

    // Block 10: Locations
    header->seek(blockOffsets[10]);
    for (int l = 0; l < (int)(blockSizes[10]/12); l++)
    {
        Location* loc = new Location(to20(header->read16()), to20(header->read16()), to20(header->read16()), to20(header->read16()), header->read8());
        qDebug("Found Location with x: %d, y: %d, width: %d, height: %d", loc->getx(), loc->gety(), loc->getwidth(), loc->getheight());
        locations.append(loc);

        header->skip(3);
    }

    // Block 11/12: Empty

    // Block 13/14: Paths
    for (int p = 0; p < (int)(blockSizes[13]/12); p++)
    {
        header->seek(blockOffsets[13]+p*12);
        int id = header->read8();
        header->skip(1);
        Path* path = new Path(id, header->read16(), header->read16());
        qDebug("Found Path with ID %d, %d Nodes, Node Offset: %d", path->getid(), path->getNumberOfNodes(), path->getNodesOffset());
        for (int i = 0; i < path->getNumberOfNodes(); i++)
        {
            header->seek(blockOffsets[14]+i*20+path->getNodesOffset()*20);
            PathNode* pathN = new PathNode(to20(header->read16()), to20(header->read16()), header->read32(), header->read32());
            path->insertNode(*pathN);
        }
        paths.append(*path);
    }

    // Block: 15/16 Progress Paths
    for (int p = 0; p < (int)(blockSizes[15]/12); p++)
    {
        header->seek(blockOffsets[15]+p*12);
        ProgressPath* pPath = new ProgressPath(header->read16(), header->read16(), header->read16());
        qDebug("Found Progress Path with ID %d, %d Nodes, Node Offset: %d", pPath->getid(), pPath->getNumberOfNodes(), pPath->getNodesOffset());
        for (int i = 0; i < pPath->getNumberOfNodes(); i++)
        {
            header->seek(blockOffsets[16]+i*20+pPath->getNodesOffset()*20);
            ProgressPathNode* pPathN = new ProgressPathNode(to20(header->read16()), to20(header->read16()));
            pPath->insertNode(*pPathN);
        }
        progressPaths.append(*pPath);
    }

    header->close();
    delete header;

    // read bgdat
    QString bgdatfiletemp = QString("/course/course%1_bgdatL%2.bin").arg(area);
    for (int l = 0; l < 2; l++)
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

            BgdatObject* obj = new BgdatObject(bgdat->read16()*20, bgdat->read16()*20, bgdat->read16()*20, bgdat->read16()*20, id, l);
            objects[l].append(obj);

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

    for (int l = 0; l < 2; l++)
    {
        for (int o = 0; o < objects[l].size(); o++)
            delete objects[l][o];
    }

    for (int s = 0; s < sprites.size(); s++)
        delete sprites[s];

    for (int e = 0; e < entrances.size(); e++)
        delete entrances[e];

    for (int l = 0; l < locations.size(); l++)
        delete locations[l];

    delete archive;
}

void Level::save()
{
    // Save Level Header

    // Calc Block Offsets/Sizes and File Size
    quint32 blockOffsets[17];
    quint32 blockSizes[17];

    int headersize = 136;                           // Block Offsets and Sizes

    blockOffsets[0] = headersize;                   // Block 0: Tileset Names
    headersize += 128;
    blockSizes[0] = headersize-blockOffsets[0];

    blockOffsets[1] = headersize;                   // Block 1: Area Settings
    headersize += 24;
    blockSizes[1] = headersize-blockOffsets[1];

    blockOffsets[2] = headersize;                   // Block 2: Zone Boundings
    headersize += zones.size()*28;
    blockSizes[2] = headersize-blockOffsets[2];

    blockOffsets[3] = headersize;                   // Block 3: Unknown
    headersize += 8;
    blockSizes[3] = headersize-blockOffsets[3];

    blockOffsets[4] = headersize;                   // Block 4: Zone Backgrounds
    headersize += zones.size()*28;
    blockSizes[4] = headersize-blockOffsets[4];

    blockOffsets[5] = headersize;                   // Block 5: Unknown
    headersize += 20;
    blockSizes[5] = headersize-blockOffsets[5];

    blockOffsets[6] = headersize;                   // Block 6: Entrances
    headersize += entrances.size()*24;
    blockSizes[6] = headersize-blockOffsets[6];

    blockOffsets[7] = headersize;                   // Block 7: Sprites
    headersize += sprites.size()*24+4;
    blockSizes[7] = headersize-blockOffsets[7];

    blockOffsets[8] = headersize;                   // Block 8: Sprites Used
    headersize += spritesUsed.size()*4;
    blockSizes[8] = headersize-blockOffsets[8];

    blockOffsets[9] = headersize;                   // Block 9: Zone Settings
    headersize += zones.size()*28;
    blockSizes[9] = headersize-blockOffsets[9];

    blockOffsets[10] = headersize;                   // Block 10: Locations
    headersize += locations.size()*12;
    blockSizes[10] = headersize-blockOffsets[10];

    blockOffsets[11] = headersize;                   // Block 11: Empty
    blockSizes[11] = headersize-blockOffsets[11];

    blockOffsets[12] = headersize;                   // Block 12: Empty
    blockSizes[12] = headersize-blockOffsets[12];

    blockOffsets[13] = headersize;                   // Block 13: Paths
    headersize += paths.size()*12;
    blockSizes[13] = headersize-blockOffsets[13];

    blockOffsets[14] = headersize;                   // Block 14: Path Nodes
    foreach (Path p, paths)
    {
        headersize += p.getNumberOfNodes()*20;
    }
    blockSizes[14] = headersize-blockOffsets[14];

    blockOffsets[15] = headersize;                   // Block 13: Progress Paths
    headersize += progressPaths.size()*12;
    blockSizes[15] = headersize-blockOffsets[15];

    blockOffsets[16] = headersize;                   // Block 14: Progress Path Nodes
    foreach (ProgressPath p, progressPaths)
    {
        headersize += p.getNumberOfNodes()*20;
    }
    blockSizes[16] = headersize-blockOffsets[16];

    QString headerfile = QString("/course/course%1.bin").arg(area);
    FileBase* header = archive->openFile(headerfile);
    header->open();

    //header->resize(headersize);
    //header->save();

    header->close();
    delete header;

}

void Level::remove(QList<Object*> objs)
{
    for (int i = 0; i < objs.size(); i++)
        remove(objs[i]);
}

void Level::remove(Object* obj)
{
    // Only if the object is removed from the corresponding list, delete it.
    bool check = false;

    if (is<BgdatObject*>(obj))
    {
        BgdatObject* bgdatobj = dynamic_cast<BgdatObject*>(obj);
        objects[bgdatobj->getLayer()].removeOne(bgdatobj);
        check = true;
    }
    else if (is<Sprite*>(obj))
    {
        sprites.removeOne(dynamic_cast<Sprite*>(obj));
        check = true;
    }
    else if (is<Entrance*>(obj))
    {
        entrances.removeOne(dynamic_cast<Entrance*>(obj));
        check = true;
    }
    else if (is<Location*>(obj))
    {
        locations.removeOne(dynamic_cast<Location*>(obj));
        check = true;
    }

    if (check)
        delete obj;
    else
        qDebug() << "Unhandled Object Remove";
}

void Level::move(QList<Object*> objs, int deltax, int deltay)
{
    int minX = 4096*20;
    int minY = 4096*20;
    int maxX = 0;
    int maxY = 0;

    foreach (Object* obj, objs)
    {
        if (obj->getx() < minX) minX = obj->getx();
        if (obj->gety() < minY) minY = obj->gety();
        if (obj->getx()+obj->getwidth() > maxX) maxX = obj->getx()+obj->getwidth();
        if (obj->gety()+obj->getheight() > maxY) maxY = obj->gety()+obj->getheight();
    }

    if (minX - deltax < 0)
        deltax = -minX;
    if (minY - deltay < 0)
        deltay = -minY;
    /*if (minX - deltaX < 0)
        deltaX = X;
    if (minX - deltaX < 0)
        deltaX = -minX;*/

    foreach (Object* obj, objs)
    {
        obj->increasePosition(deltax, deltay);
    }
}
void Level::raise(BgdatObject* obj)
{
    objects[obj->getLayer()].move(objects[obj->getLayer()].indexOf(obj), objects[obj->getLayer()].size()-1);
}

void Level::lower(BgdatObject* obj)
{
    objects[obj->getLayer()].move(objects[obj->getLayer()].indexOf(obj), 0);
}
