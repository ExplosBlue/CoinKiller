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

#include <QFile>
#include <QMessageBox>
#include <limits>
#include <QMessageBox>
#include <algorithm>

Level::Level(Game *game, SarcFilesystem* archive, int area, QString lvlName)
{
    this->game = game;

    this->archive = archive;
    this->lvlName = lvlName;
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

    // Block 0: Tilesets
    for (int t = 0; t < 4; t++)
    {
        header->seek(blockOffsets[0] + (t*32));

        QString tilesetname;
        header->readStringASCII(tilesetname, 32);

        if (tilesetname.isEmpty())
        {
            tilesets[t] = nullptr;
            continue;
        }

        try
        {
            tilesets[t] = game->getTileset(tilesetname);
        }
        catch (const std::exception &e)
        {
            QMessageBox::warning(nullptr, "CoinKiller", QObject::tr("Tileset %1 could not be found!").arg(tilesetname));
            tilesets[t] = nullptr;
        }
    }

    // Block 1: Area Settings
    header->seek(blockOffsets[1]);
    eventState = header->read64();
    unk1 = header->read16();
    timeLimit = header->read16();
    header->skip(4);
    levelEntranceID = header->read8();
    unk2 = header->read8();
    specialLevelFlag = header->read8();
    specialLevelFlag2 = header->read8();
    coinRushTimeLimit = header->read16();

    // Block 2: Zone Boundings
    header->seek(blockOffsets[2]);
    for (int i = 0; i < (int)(blockSizes[2]/28); i++)
    {
        quint32 primaryUpperBound = header->read32();
        quint32 primaryLowerBound = header->read32();
        quint32 secondaryUpperBound = header->read32();
        quint32 secondaryLowerBound = header->read32();
        quint16 id = header->read16();
        quint16 upScrolling = header->read16();
        header->skip(8);

        ZoneBounding* bounding = new ZoneBounding(id, primaryUpperBound, primaryLowerBound, secondaryUpperBound, secondaryLowerBound, upScrolling);

        boundings.append(bounding);
    }

    // Block 3: Unknown

    // Block 4: ZoneBackground Information
    header->seek(blockOffsets[4]);
    for (int i = 0; i < (int)(blockSizes[4]/28); i++)
    {
        quint16 id = header->read16();
        qint16 yPos = header->read16();
        qint16 xPos = header->read16();
        header->skip(2);
        QString name;
        header->readStringASCII(name, 16);
        quint16 parallaxMode = header->read16();

        header->skip(2);

        ZoneBackground* background = new ZoneBackground(id, yPos, xPos, name, parallaxMode);

        backgrounds.append(background);
    }

    // Block 5: Static / Dummy?

    // Block 6: Entrances
    header->seek(blockOffsets[6]);
    for (int e = 0; e < (int)(blockSizes[6]/24); e++)
    {
        quint16 x = header->read16();
        quint16 y = header->read16();
        qint16 cameraX = header->read16();
        qint16 cameraY = header->read16();
        quint8 id = header->read8();
        quint8 destArea = header->read8();
        quint8 destEntr = header->read8();
        quint8 entrType = header->read8();
        header->skip(4);
        quint16 settings = header->read16();
        header->skip(2);
        quint8 entrUnk1 = header->read8();
        quint8 entrUnk2 = header->read8();
        header->skip(2);

        Entrance* entr = new Entrance(to20(x), to20(y), cameraX, cameraY, id, destArea, destEntr, entrType, settings, entrUnk1, entrUnk2);
        entr->setRect();
        entrances.append(entr);
    }

    // Block 7: Sprites
    header->seek(blockOffsets[7]);
    for (;;)
    {
        quint16 id = header->read16();
        if (id == 0xFFFF) break;

        qint32 x = to20(header->read16());
        qint32 y = to20(header->read16());

        Sprite* spr = new Sprite(x, y, id);

        for (int i=0; i<2; i++) spr->setByte(i, header->read8());
        spr->setNybbleData(header->read32(), 4, 11);
        spr->setNybbleData(header->read32(), 12, 19);
        header->skip(1);
        spr->setLayer(header->read8());
        for (int i=10; i<12; i++) spr->setByte(i, header->read8());

        header->skip(4);

        spr->setRect();
        sprites.append(spr);
        if (isCameraLimit(spr)) {
            insertCameraLimit(spr);
        }
    }

    // Block 8: Sprites Used List (no need to read this)


    // Block 9: Zones
    header->seek(blockOffsets[9]);
    for (int z = 0; z < (int)(blockSizes[9]/28); z++)
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
        quint8 cameraFlags = header->read8();
        header->skip(2);

        Zone* zone = new Zone(to20(x), to20(y), to20(width), to20(height), id, progPathId, musicId, multiplayerTracking, zoneUnk1, boundingId, backgroundId, cameraFlags);
        zones.append(zone);
    }

    // Block 10: Locations
    header->seek(blockOffsets[10]);
    for (int l = 0; l < (int)(blockSizes[10]/12); l++)
    {
        qint32 x = to20(header->read16());
        qint32 y = to20(header->read16());
        qint32 w = to20(header->read16());
        qint32 h = to20(header->read16());
        qint32 id = header->read8();

        Location* loc = new Location(x, y, w, h, id);
        locations.append(loc);

        header->skip(3);
    }

    // Block 11/12: Empty

    // Block 13/14: Paths
    for (int p = 0; p < (int)(blockSizes[13]/12); p++)
    {
        header->seek(blockOffsets[13]+p*12);
        quint8 id = header->read8();
        header->skip(1);
        quint16 nodeOffset = header->read16();
        quint16 nodeCount = header->read16();
        quint16 loopFlag = header->read16();
        header->skip(4);

        Path* path = new Path(id, loopFlag);

        for (quint16 i = 0; i < nodeCount; i++)
        {
            header->seek(blockOffsets[14] + i*20 + nodeOffset*20);

            qint32 x = to20(header->read16());
            qint32 y = to20(header->read16());
            float speed = header->readFloat();
            float accel = header->readFloat();
            quint16 delay = header->read16();
            qint16 rotation = header->read16();

            quint8 variableField = header->read8();
            quint8 nextPathID = header->read8();
            header->skip(2);

            PathNode* pathN = new PathNode(x, y, speed, accel, delay, rotation, variableField, nextPathID, path);
            path->insertNode(pathN);
        }
        paths.append(path);
    }

    // Block: 15/16 Progress Paths
    for (int p = 0; p < (int)(blockSizes[15]/12); p++)
    {
        header->seek(blockOffsets[15]+p*12);
        quint16 id = header->read16();
        quint16 nodeOffset = header->read16();
        quint16 nodeCount = header->read16();
        header->skip(3);
        quint8 alternatePathFlag = header->read8();

        ProgressPath* pPath = new ProgressPath(id, alternatePathFlag);

        for (int i = 0; i < nodeCount; i++)
        {
            header->seek(blockOffsets[16] + i*20 + nodeOffset*20);

            qint32 x = to20(header->read16());
            qint32 y = to20(header->read16());

            ProgressPathNode* pPathN = new ProgressPathNode(x, y, pPath);
            pPath->insertNode(pPathN);
        }
        progressPaths.append(pPath);
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

            qint32 x = bgdat->read16()*20;
            qint32 y = bgdat->read16()*20;
            qint32 w = bgdat->read16()*20;
            qint32 h = bgdat->read16()*20;

            BgdatObject* obj = new BgdatObject(x, y, w, h, id, l);

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

    qDeleteAll(sprites);
    qDeleteAll(entrances);
    qDeleteAll(locations);
    qDeleteAll(zones);
    qDeleteAll(paths);
    qDeleteAll(progressPaths);
    qDeleteAll(boundings);
    qDeleteAll(backgrounds);
}

qint8 Level::save()
{
    // before we even do anything, make sure that this level will load
    // take very good note, however. The only possible scenarios that are coded are ones
    // that the editor can create, not by external hex editing.
    // start out with a null string, add shit to it if things went wrong
    /*QString wrongShit;

    // no zones
    if (zones.size() == 0)
        wrongShit += "- Needs at least one zone\n";
    // no entrances
    if (entrances.size() == 0)
        wrongShit += "- Needs at least one entrance\n";

    // crashing entrances
    foreach (Entrance* entr, entrances)
        if (entr->getEntrType() == 28 || entr->getEntrType() == 29)
            wrongShit += "- An invalid/crashing entrance type on ID " + QString::number(entr->getid()) + "\n";

    if (wrongShit != NULL)
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Level Errors");
        msgBox->setWindowModality(Qt::NonModal);
        msgBox->setInformativeText(
                    "This level has errors that WILL crash the game.\n"
                    "The following errors were found:\n" + wrongShit +
                    " Do you still wish to save this file?");
        msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int ret = msgBox->exec();

        // if they selected No, terminate the save
        // it's asked "Yes" or "No" in case there's something that needs to be tested or whatever
        if (ret == QMessageBox::No)
            return 1;

        delete msgBox;
    }*/

    // Save BGDat
    QString bgdatfiletemp = QString("/course/course%1_bgdatL%2.bin").arg(area);
    for (int l = 0; l < 2; l++)
    {
        QString bgdatfile = bgdatfiletemp.arg(l+1);

        if (objects[l].length() == 0)
        {
            if (archive->fileExists(bgdatfile)) archive->deleteFile(bgdatfile);
            continue;
        }

        FileBase* bgdat;
        if (archive->fileExists(bgdatfile)) bgdat = archive->openFile(bgdatfile);
        else
        {
            bgdat = new MemoryFile(archive);
            bgdat->setIdPath(bgdatfile);
        }

        bgdat->open();
        bgdat->resize(objects[l].size()*16+2);
        bgdat->seek(0);

        foreach (BgdatObject* obj, objects[l])
        {
            bgdat->write16(obj->getid());
            bgdat->write16(obj->getx()/20);
            bgdat->write16(obj->gety()/20);
            bgdat->write16(obj->getwidth()/20);
            bgdat->write16(obj->getheight()/20);
            for (int i = 0; i < 6; i++) bgdat->write8(0);
        }
        bgdat->write16(0xFFFF);

        bgdat->save();
        bgdat->close();
        delete bgdat;
    }


    // Save Level Header

    // Generate Sprites Used List
    QList<quint16> spritesUsed;
    foreach (Sprite* spr, sprites)
    {
        if (!spritesUsed.contains(spr->getid())) spritesUsed.append(spr->getid());
    }
    std::sort(spritesUsed.begin(), spritesUsed.end());

    // Sort Sprites
    if (zones.size() > 0)
    {
        QList<int> zoneIDs;
        foreach (Zone* z, zones) zoneIDs.append(z->getid());
        std::sort(zoneIDs.begin(), zoneIDs.end());

        QList<Sprite*> sortedSprites;
        for (int i = 0; i < zoneIDs.count(); i++)
        {
            foreach (Sprite* spr, sprites)
            {
                if (getNextZoneID(spr) == zones[i]->getid())
                {
                    sprites.removeOne(spr);
                    sortedSprites.append(spr);
                }
            }
        }
        sprites = sortedSprites;
    }

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
    headersize += boundings.size()*28;
    blockSizes[2] = headersize-blockOffsets[2];

    blockOffsets[3] = headersize;                   // Block 3: Unknown
    headersize += 8;
    blockSizes[3] = headersize-blockOffsets[3];

    blockOffsets[4] = headersize;                   // Block 4: Zone Backgrounds
    headersize += backgrounds.size()*28;
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
    foreach (Path* p, paths)
    {
        headersize += p->getNumberOfNodes()*20;
    }
    blockSizes[14] = headersize-blockOffsets[14];

    blockOffsets[15] = headersize;                   // Block 13: Progress Paths
    headersize += progressPaths.size()*12;
    blockSizes[15] = headersize-blockOffsets[15];

    blockOffsets[16] = headersize;                   // Block 14: Progress Path Nodes
    foreach (ProgressPath* p, progressPaths)
    {
        headersize += p->getNumberOfNodes()*20;
    }
    blockSizes[16] = headersize-blockOffsets[16];

    QString headerfile = QString("/course/course%1.bin").arg(area);
    FileBase* header = archive->openFile(headerfile);
    header->open();
    header->resize(headersize);
    header->seek(0);

    // Block Offsets/Sizes
    for (int i = 0; i < 17; i++)
    {
        header->write32(blockOffsets[i]);
        header->write32(blockSizes[i]);;
    }

    // Block 0: Tileset Names
    header->seek(blockOffsets[0]);
    for (int i = 0; i < 4; i++)
    {
        if (!tilesets[i])
            for (int j = 0; j < 32; j++)
                header->write8(0);
        else
            header->writeStringASCII(tilesets[i]->getName(), 32);
    }

    // Block 1: Area Settings
    header->seek(blockOffsets[1]);
    header->write64(eventState);
    header->write16(unk1);
    header->write16(timeLimit);
    header->write8(0);
    for (int j = 0; j < 3; j++) header->write8(0x64);
    header->write8(levelEntranceID);
    header->write8(unk2);
    header->write8(specialLevelFlag);
    header->write8(specialLevelFlag2);
    header->write16(coinRushTimeLimit);
    header->write16(0);

    // Block 2: Zone Boundings
    header->seek(blockOffsets[2]);
    foreach (ZoneBounding* bounding, boundings)
    {
        header->write32(bounding->getPrimaryUpperBound());
        header->write32(bounding->getPrimaryLowerBound());
        header->write32(bounding->getSecondaryUpperBound());
        header->write32(bounding->getSecondaryLowerBound());
        header->write16(bounding->getId());
        header->write16(bounding->getUpScrolling());
        for (int j = 0; j < 8; j++) header->write8(0);
    }

    // Block 3: Unknown / Useless
    header->seek(blockOffsets[3]);
    for (int i = 0; i < 8; i++) header->write8(0);  // There is an unknown value, probably useless

    // Block 4: Zone Backgrounds
    header->seek(blockOffsets[4]);
    foreach (ZoneBackground* background, backgrounds)
    {
        header->write16(background->getId());
        header->write16(background->getYPos());
        header->write16(background->getXPos());
        header->write16(0);
        header->writeStringASCII(background->getName(), 16);
        header->write16(background->getParallaxMode());
        header->write16(0);
    }

    // Block 5: Unknown / Dummy
    header->seek(blockOffsets[5]);
    {
        header->write16(0);
        header->write32(0xFFFFFFFF);
        for (int i = 0; i < 14; i++) header->write8(0);
    }

    // Block 6: Entrances
    header->seek(blockOffsets[6]);
    foreach (Entrance* entr, entrances)
    {
        header->write16(to16(entr->getx()));
        header->write16(to16(entr->gety()));
        header->write16(entr->getCameraX());
        header->write16(entr->getCameraY());
        header->write8(entr->getid());
        header->write8(entr->getDestArea());
        header->write8(entr->getDestEntr());
        header->write8(entr->getEntrType());
        header->write8(0);
        header->write8(getNextZoneID(entr));
        for (int i = 0; i < 2; i++) header->write8(0);
        header->write16(entr->getSettings());
        for (int i = 0; i < 2; i++) header->write8(0);
        header->write8(entr->getUnk1());
        header->write8(entr->getUnk2());
        for (int i = 0; i < 2; i++) header->write8(0);
    }

    // Block 7: Sprites
    header->seek(blockOffsets[7]);
    foreach (Sprite* spr, sprites)
    {
        header->write16(spr->getid());
        header->write16(to16(spr->getx()));
        header->write16(to16(spr->gety()));
        header->write8(spr->getByte(0));
        header->write8(spr->getByte(1));
        header->write32(spr->getNybbleData(4, 11));
        header->write32(spr->getNybbleData(12, 19));
        header->write8(getNextZoneID(spr));
        header->write8(spr->getLayer());
        header->write8(spr->getByte(10));
        header->write8(spr->getByte(11));
        for (int i = 0; i < 4; i++) header->write8(0);
    }
    header->write32(0xFFFFFFFF);

    // Block 8: Sprites Used
    header->seek(blockOffsets[8]);
    foreach (quint16 sprite, spritesUsed)
    {
        header->write16(sprite);
        header->write16(0);
    }

    // Block 9: Zones
    header->seek(blockOffsets[9]);
    for (quint8 i = 0; i < zones.size(); i++)
    {
        Zone* z = zones[i];
        header->write16(to16(z->getx()));
        header->write16(to16(z->gety()));
        header->write16(to16(z->getwidth()));
        header->write16(to16(z->getheight()));
        header->write16(z->getUnk1());
        header->write16(0);
        header->write8(z->getid());
        header->write8(z->getBoundingId());
        for (int j = 0; j < 6; j++) header->write8(0);
        header->write8(z->getMultiplayerTracking());
        header->write8(z->getProgPathId());
        header->write8(z->getMusicId());
        header->write8(0);
        header->write8(z->getBackgroundId());
        header->write8(z->getCameraFlags());
        for (int j = 0; j < 2; j++) header->write8(0);
    }


    // Block 10: Locations
    header->seek(blockOffsets[10]);
    foreach (Location* loc, locations)
    {
        header->write16(to16(loc->getx()));
        header->write16(to16(loc->gety()));
        header->write16(to16(loc->getwidth()));
        header->write16(to16(loc->getheight()));
        header->write8(loc->getid());
        for (int i = 0; i < 3; i++) header->write8(0);
    }

    // Block 11/12: Empty

    // Block 13/14: Paths
    quint16 actualNodeCount1 = 0;
    for (int i = 0; i < paths.size(); i++)
    {
        Path* p = paths[i];
        header->seek(blockOffsets[13] + i*12);
        header->write8(p->getid());
        header->write8(0);
        header->write16(actualNodeCount1);
        header->write16(p->getNumberOfNodes());
        header->write16(p->getLoop());
        header->write32(0);

        foreach (PathNode* pNode, p->getNodes())
        {
            header->seek(blockOffsets[14] + actualNodeCount1*20);
            header->write16(to16(pNode->getx()));
            header->write16(to16(pNode->gety()));
            header->writeFloat(pNode->getSpeed());
            header->writeFloat(pNode->getAccel());
            header->write16(pNode->getDelay());
            header->write16(pNode->getRotation());
            header->write8(pNode->getVariableField());
            header->write8(pNode->getNextPathID());
            header->write16(0);

            actualNodeCount1++;
        }
    }

    // Block 15/16: Progress Paths
    quint16 actualNodeCount2 = 0;
    for (int i = 0; i < progressPaths.size(); i++)
    {
        ProgressPath* p = progressPaths[i];
        header->seek(blockOffsets[15] + i*12);
        header->write16(p->getid());
        header->write16(actualNodeCount2);
        header->write16(p->getNumberOfNodes());
        for (int j = 0; j < 3; j++) header->write8(0);
        header->write8(p->getAlternatePathFlag());
        header->write16(0);

        foreach (ProgressPathNode* pNode, p->getNodes())
        {
            header->seek(blockOffsets[16] + actualNodeCount2*20);
            header->write16(to16(pNode->getx()));
            header->write16(to16(pNode->gety()));
            for (int j = 0; j < 16; j++) header->write8(0);
            actualNodeCount2++;
        }
    }

    header->save();
    header->close();
    delete header;

    return 0;
}



quint8 Level::getNextZoneID(Object* obj)
{
    quint8 zoneID = 0;
    int actualDistance = 2147483647;
    foreach (Zone* zone, zones)
    {
        if (QRect(zone->getx(), zone->gety(), zone->getwidth(), zone->getheight()).contains(obj->getx(), obj->gety()))
            return zone->getid();

        int distance = 0;
        int dx = qMax(qAbs(obj->getx() - zone->getx()) - zone->getwidth() / 2, 0);
        int dy = qMax(qAbs(obj->gety() - zone->gety()) - zone->getheight() / 2, 0);
        distance = dx * dx + dy * dy;

        if (distance < actualDistance)
        {
            zoneID = zone->getid();
            actualDistance = distance;
        }
    }
    return zoneID;
}

void Level::add(QList<Object *> objs)
{
    foreach (Object* obj, objs)
        add(obj);
}

void Level::add(Object *obj)
{
    if (is<BgdatObject*>(obj))
    {
        BgdatObject* bgdatobj = dynamic_cast<BgdatObject*>(obj);
        objects[bgdatobj->getLayer()].append(bgdatobj);
    }
    else if (is<Sprite*>(obj))
        sprites.append(dynamic_cast<Sprite*>(obj));
    else if (is<Entrance*>(obj))
        entrances.append(dynamic_cast<Entrance*>(obj));
    else if (is<Zone*>(obj))
        zones.append(dynamic_cast<Zone*>(obj));
    else if (is<Location*>(obj))
        locations.append(dynamic_cast<Location*>(obj));
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
void Level::raise(Object* obj)
{
    if (is<BgdatObject*>(obj))
    {
        BgdatObject* bgdat = dynamic_cast<BgdatObject*>(obj);
        objects[bgdat->getLayer()].move(objects[bgdat->getLayer()].indexOf(bgdat), objects[bgdat->getLayer()].size()-1);
    }
    else if (is<Sprite*>(obj))
    {
        Sprite* spr = dynamic_cast<Sprite*>(obj);
        sprites.move(sprites.indexOf(spr), sprites.size()-1);
    }
}

void Level::lower(Object *obj)
{
    if (is<BgdatObject*>(obj))
    {
        BgdatObject* bgdat = dynamic_cast<BgdatObject*>(obj);
        objects[bgdat->getLayer()].move(objects[bgdat->getLayer()].indexOf(bgdat), 0);
    }
    else if (is<Sprite*>(obj))
    {
        Sprite* spr = dynamic_cast<Sprite*>(obj);
        sprites.move(sprites.indexOf(spr), 0);
    }
}

void Level::raiseLayer(BgdatObject *obj)
{
    int currLayer = obj->getLayer();

    if (currLayer < 1)
        return;

    objects[currLayer].removeOne(obj);
    objects[currLayer-1].append(obj);
    obj->setLayer(currLayer-1);
}

void Level::lowerLayer(BgdatObject *obj)
{
    int currLayer = obj->getLayer();

    if (currLayer > 0)
        return;

    objects[currLayer].removeOne(obj);
    objects[currLayer+1].append(obj);
    obj->setLayer(currLayer+1);
}

Entrance* Level::newEntrance(int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (Entrance* entr, entrances)
            if (entr->getid() == id) check = true;
        if (!check) break;
        else if (id == 255)
        {
            id = 0;
            break;
        }
    }
    return new Entrance(toNext16Compatible(x), toNext16Compatible(y), 0, 0, id, 0, 0, 0, 0, 0, 0);
}

Zone* Level::newZone(int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (Zone* zone, zones)
            if (zone->getid() == id) check = true;
        if (!check) break;
        else if (id == 255)
        {
            id = 0;
            break;
        }
    }
    return new Zone(toNext16Compatible(x), toNext16Compatible(y), 400, 240, id, 0, 0, 0, 0, 0, 0, 0);
}

Location* Level::newLocation(int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (Location* loc, locations)
            if (loc->getid() == id) check = true;
        if (!check) break;
        else if (id == 255)
        {
            id = 0;
            break;
        }
    }
    return new Location(toNext16Compatible(x), toNext16Compatible(y), 40, 40, id);
}

Path* Level::newPath()
{
    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (Path* path, paths)
            if (path->getid() == id) check = true;
        if (!check) break;
        else if (id == 255)
        {
            id = 0;
            break;
        }
    }
    return new Path(id, 0);
}

ProgressPath* Level::newProgressPath()
{
    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (ProgressPath* path, progressPaths)
            if (path->getid() == id) check = true;
        if (!check) break;
        else if (id == 255)
        {
            id = 0;
            break;
        }
    }
    return new ProgressPath(id, 0);
}

bool Level::isCameraLimit(Sprite *spr)
{
    int id = spr->getid();

    if (id != 156 && id != 157 && id != 160 && id != 161)
        return false;

    return true;
}

void Level::insertCameraLimit(Sprite *spr)
{
    if (!isCameraLimit(spr))
        return;

    switch (spr->getid()) {
    case 156:
        leftCamLimits.append(spr);
        break;
    case 157:
        rightCamLimits.append(spr);
        break;
    case 160:
        bottomCamLimits.append(spr);
        break;
    case 161:
        topCamLimits.append(spr);
        break;
    default:
        break;
    }

    sortCameraLimits();
}

void Level::removeCameraLimit(Sprite *spr)
{
    if (!isCameraLimit(spr))
        return;

    switch (spr->getid()) {
    case 156:
        leftCamLimits.removeOne(spr);
        break;
    case 157:
        rightCamLimits.removeOne(spr);
        break;
    case 160:
        bottomCamLimits.removeOne(spr);
        break;
    case 161:
        topCamLimits.removeOne(spr);
        break;
    default:
        break;
    }
}

void Level::sortCameraLimits()
{
    // Sort right limits from smallest to largest xPos
    std::sort(rightCamLimits.begin(), rightCamLimits.end(), [](const Sprite* a, const Sprite* b) -> bool { return a->getx() < b->getx(); });

    // Sort bottom limits from smallest to largest yPos
    std::sort(bottomCamLimits.begin(), bottomCamLimits.end(), [](const Sprite* a, const Sprite* b) -> bool { return a->gety() < b->gety(); });
}
