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
#include "imagecache.h"

Tileset::Tileset(Game *game, QString name)
{
    this->game = game;
    this->name = name;

    //qDebug("LOAD TILESET %s", name.toStdString().c_str());

    archive = new SarcFilesystem(game->fs->openFile("/Unit/"+name+".sarc"));
    ctpk = new Ctpk(archive->openFile("/BG_tex/"+name+".ctpk"));

    texImage = ctpk->getTexture((quint32)0);

    if (name.startsWith("J_"))
        drawOverrides = true;

    // parse the object def
    FileBase* objindex = archive->openFile("/BG_unt/"+name+"_hd.bin");
    objindex->open();
    objindex->seek(0);

    FileBase* objdata = archive->openFile("/BG_unt/"+name+".bin");
    objdata->open();

    int numObjects = objindex->size() / 6;

    for (int o = 0; o < numObjects; o++)
    {
        quint16 offset = objindex->read16();
        quint8 width = objindex->read8();
        quint8 height = objindex->read8();
        quint8 randomisation = objindex->read8();
        quint8 crap = objindex->read8();

        //qDebug("OBJECT %d -- %04X %dx%d %02X %02X", o, offset, width, height, crap1, crap2);

        ObjectDef* obj = new ObjectDef();
        obj->width = width;
        obj->height = height;
        obj->randomisation = randomisation;
        obj->unkFlag = crap;

        if (crap != 0)
            qDebug() << "Found unkFlag value != 0:" << crap << "for Object" << o;

        //obj->xRepeatStart = 0xFF;
        obj->yRepeatStart = 0xFF;

        obj->slopeY = obj->height; // failsafe default

        // data format:
        // [optional slope marker, bit7 set]
        // [repeat] [Tile#] 02
        // FE -> next line
        // FF -> end

        // repeat:
        // 01 -> repeat X
        // 02 -> repeat Y (in NSMBDS, flag for first tile applies to whole line, maybe same here?)
        // 04 -> slope thing (used for slope second row)

        quint8 b;
        objdata->seek(offset);

        int curx = 0, cury = 0;

        ObjectRow* row = new ObjectRow();
        row->slopeFlags = 0;
        row->xRepeatStart = 0xFF;
        bool xrepeat = false;
        int shittyxrepeat = 0;
        bool yrepeat = false;

        for (;;)
        {
            b = objdata->read8();
            if (b == 0xFF) // end
            {
                delete row;
                break;
            }

            if (b == 0xFE) // start new row
            {
                obj->rows.append(*row);
                delete row;

                row = new ObjectRow();
                row->slopeFlags = 0;
                row->xRepeatStart = 0xFF;
                xrepeat = false;
                shittyxrepeat = 0;

                curx = 0;
                cury++;
                continue;
            }

            if (b & 0x80) // slope shit
            {
                if (b == 0x84) // checkme
                    obj->slopeY = cury;

                row->slopeFlags = b;
                b = objdata->read8();
            }


            if (b & 0x04)
            {
                if (shittyxrepeat == 2)
                {
                    shittyxrepeat = 3;
                    row->xRepeatEnd = curx;
                }
                else if (!shittyxrepeat)
                    shittyxrepeat = 1;
            }
            else if (shittyxrepeat == 1)
            {
                shittyxrepeat = 2;
                row->xRepeatStart = curx;
                row->xRepeatEnd = width; // failsafe
            }


            if ((b & 0x01) && !xrepeat)
            {
                xrepeat = true;
                row->xRepeatStart = curx;
                row->xRepeatEnd = width; // failsafe
            }
            else if ((!(b & 0x01)) && xrepeat)
            {
                xrepeat = false;
                row->xRepeatEnd = curx;
            }

            if (curx == 0)
            {
                if ((b & 0x02) && !yrepeat)
                {
                    yrepeat = true;
                    obj->yRepeatStart = cury;
                    obj->yRepeatEnd = height;
                }
                else if ((!(b & 0x02)) && yrepeat)
                {
                    yrepeat = false;
                    obj->yRepeatEnd = cury;
                }
            }

            row->data.append(b); // repeat/etc flags

            b = objdata->read8();
            row->data.append(b); // tile #

            b = objdata->read8(); // item (0XXX X000) / slot (0000 0XX0)
            row->data.append(b);

            curx++;
        }

        objectDefs.append(obj);
    }

    objdata->close();
    delete objdata;

    objindex->close();
    delete objindex;

    if (objectDefs.size() > 0)
        slot = (objectDefs[0]->rows[0].data[2] & 6) >> 1;

    // parse behaviors
    FileBase* behaviorsFile = archive->openFile("BG_chk/d_bgchk_" + name + ".bin");
    behaviorsFile->open();
    behaviorsFile->seek(0);
    for (int i = 0; i < 441; i++) // TODO ensure the file has the right size!
    {
        for (int j = 0; j < 8; j++)
            behaviors[i][j] = behaviorsFile->read8();
    }
    behaviorsFile->close();
    delete behaviorsFile;

    // parse 3D overlays
    if (archive->fileExists("BG_unt/" + name + "_add.bin"))
    {
        FileBase* overlaysFile = archive->openFile("BG_unt/" + name + "_add.bin");
        overlaysFile->open();
        overlaysFile->seek(0);
        for (int i = 0; i < 441; i++) // TODO ensure the file has the right size!
        {
            overlays3D[i] = overlaysFile->read16();
        }
        overlaysFile->close();
        delete overlaysFile;
    }
}

Tileset::~Tileset()
{
    delete ctpk;
    delete archive;
    qDeleteAll(objectDefs);
}


// x and y in tile coords
void Tileset::drawTile(QPainter& painter, TileGrid& grid, int num, int x, int y, float zoom, int item)
{
    quint32 gridid = x | (y<<16);
    if (grid[gridid] == grid[0xFFFFFFFF])
        return;

    int tsize = (int)(20*zoom);
    x *= tsize;
    y *= tsize;

    QRect rdst(x, y, tsize, tsize);

    if (drawOverrides)
    {
        int xx = num % 21;
        int yy = num / 21;

        if (xx == 15 && yy == 0) { painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverride, "coin.png")); return; }
        if (xx == 16 && yy == 0) { painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverride, "blue_coin.png")); return; }
        if (xx == 10 && yy == 3) { painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverride, "vine.png")); return; }
        if (xx == 11 && yy == 0) { painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverride, "solid.png")); return; }
    }

    if (draw2D)
    {
        QRect rsrc(2 + ((num%21)*24), 2 + ((num/21)*24), 20, 20);
        painter.drawImage(rdst, texImage, rsrc);
    }

    if (draw3D && (getOverlayTile(num) !=0))
    {
        QRect overlaysrc(2 + ((getOverlayTile(num)%21)*24), 2 + ((getOverlayTile(num)/21)*24), 20, 20);
        painter.drawImage(rdst, texImage, overlaysrc);
    }

    grid[gridid] = grid[0xFFFFFFFF];

    // Draw Overlays
    if (behaviors[num][0] == 0 && behaviors[num][2] == 1) // Beanstalk Stopper
        painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "beanstalk_stopper.png"));

    if (behaviors[num][0] == 6) // Brick Block
    {
        switch (item)
        {
            case 1: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "coin.png")); break;
            case 2: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "10_coins.png")); break;
            case 3: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "fire_flower.png")); break;
            case 4: case 9: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "super_star.png")); break;
            case 5: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "1up_mushroom.png")); break;
            case 6: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "beanstalk.png")); break;
            case 7: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "mini_mushroom.png")); break;
            case 8: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "coin_super_mushroom.png")); break;
            case 10: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "super_leaf.png")); break;
            case 11: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "trampoline.png")); break;
            case 12: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "gold_flower.png")); break;
            case 13: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "pow_coin.png")); break;
            default: break;
        }
    }
    if (behaviors[num][0] == 7) // ? Block
    {
        switch (item)
        {
            case 0: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "coin.png")); break;
            case 1: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "fire_flower.png")); break;
            case 2: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "super_star.png")); break;
            case 3: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "1up_mushroom.png")); break;
            case 4: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "beanstalk.png")); break;
            case 5: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "trampoline.png")); break;
            case 6: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "mini_mushroom.png")); break;
            case 7: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "coin_super_mushroom.png")); break;
            case 8: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "mega_mushroom.png")); break;
            case 9: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "super_leaf.png")); break;
            case 10: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "gold_flower.png")); break;
            default: break;
        }
    }
    if (behaviors[num][0] == 10) // Invisible Block
    {
        switch (behaviors[num][2])
        {
            case 0: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "fire_flower_invisible.png")); break;
            case 1: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "super_star_invisible.png")); break;
            case 2: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "coin_invisible.png")); break;
            case 3: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "beanstalk_invisible.png")); break;
            case 4: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "1up_mushroom_invisible.png")); break;
            case 5: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "mini_mushroom_invisible.png")); break;
            case 6: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "super_leaf_invisible.png")); break;
            case 7: painter.drawPixmap(rdst, ImageCache::getInstance()->get(TileOverlay, "gold_flower_invisible.png")); break;
            default: break;
        }
    }
}

void Tileset::drawRow(QPainter& painter, TileGrid& grid, ObjectDef& def, ObjectRow& row, int x, int y, int w, float zoom)
{
    int sx = 0, dx = 0;
    int end = row.data.length()/3;

    if (row.xRepeatStart != 0xFF)
    {
        // with repeat
        int rstart = row.xRepeatStart;
        int rend = w - (def.width - row.xRepeatEnd);

        sx = 0;
        while (dx < rstart)
        {
            if (row.data[sx*3 + 1] || (row.data[sx*3 + 2] & 6) >> 1) // Lame work arround
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom, (row.data[sx*3 + 2] & 120) >> 3);

            dx++;
            sx++;
            if (sx >= row.xRepeatStart)
                sx = 0;
        }

        sx = row.xRepeatStart;
        while (dx < rend)
        {
            if (row.data[sx*3 + 1] || (row.data[sx*3 + 2] & 6) >> 1) // Lame work arround
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom, (row.data[sx*3 + 2] & 120) >> 3);

            dx++;
            sx++;
            if (sx >= row.xRepeatEnd)
                sx = row.xRepeatStart;
        }

        sx = row.xRepeatEnd;
        while (dx < w)
        {
            if (row.data[sx*3 + 1] || (row.data[sx*3 + 2] & 6) >> 1) // Lame work arround
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom, (row.data[sx*3 + 2] & 120) >> 3);

            dx++;
            sx++;
            if (sx >= end)
                sx = row.xRepeatEnd;
        }
    }
    else
    {
        // without repeat

        sx = 0;
        while (dx < w)
        {
            if (row.data[sx*3 + 1] || (row.data[sx*3 + 2] & 6) >> 1) // Lame work arround
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom, (row.data[sx*3 + 2] & 120) >> 3);

            dx++;
            sx++;
            if (sx >= end)
                sx = 0;
        }
    }
}

void Tileset::drawObject(QPainter& painter, TileGrid& grid, int num, int x, int y, int w, int h, float zoom)
{
    if (num >= objectDefs.size())
    {
        for (int xx = 0; xx < w; xx++)
            for (int yy = 0; yy < h; yy++)
                painter.drawPixmap(QRect((x+xx)*20, (y+yy)*20, 20, 20), ImageCache::getInstance()->get(TileOverride, "error.png"));

        return;
    }

    ObjectDef& def = *objectDefs[num];

    //qDebug("RENDER OBJ %d | %d %d | %d,%d %d,%d\n", num, def.rows.length(), 4646445, x, y, w, h);

    if (def.rows[0].slopeFlags & 0x80)
    {
        // slope junk

        // slope 00: start from bottom-left (top-right?? checkme)
        // slope 01: start from top-left
        // slope 02: same as 00, reverse Y
        // slope 03: same as 01, reverse Y
        // FIXME: not proper! it swaps top and bottom blocks, instead of reversing Y

        quint8 slopeflag = def.rows[0].slopeFlags;

        int cury, yinc;
        if (slopeflag & 0x01)
        {
            cury = 0;
            yinc = def.slopeY;
        }
        else
        {
            cury = h - def.height;
            yinc = -def.slopeY;
        }
// This causes heavy lagging in huge levels
//qDebug("SLOPE Y %d %02X %d/%d", def.slopeY, slopeflag, def.width, def.height);
        int curx = 0;
        while (curx < w)
        {
            if ((curx+def.width) > w)
                break;

            for (int by = 0; by < def.height; by++)
            {
                ObjectRow& row = def.rows[by];

                int tiley = cury+by;
                if (slopeflag & 0x02) tiley = h - 1 - tiley;

                if (tiley < 0 || tiley >= h)
                    break;

                drawRow(painter, grid, def, row, x+curx, y+tiley, def.width, zoom);
            }

            curx += def.width;
            cury += yinc;
        }

        return;
    }

    int sy = 0, dy = 0;
    int end = def.rows.length();

    if (def.yRepeatStart != 0xFF)
    {
        // with repeat

        int rstart = def.yRepeatStart;
        int rend = h - (def.height - def.yRepeatEnd);

        sy = 0;
        while (dy < rstart)
        {
            ObjectRow& row = def.rows[sy];
            drawRow(painter, grid, def, row, x, y+dy, w, zoom);

            dy++;
            sy++;
            if (sy >= def.yRepeatStart)
                sy = 0;
        }

        sy = def.yRepeatStart;
        while (dy < rend)
        {
            ObjectRow& row = def.rows[sy];
            drawRow(painter, grid, def, row, x, y+dy, w, zoom);

            dy++;
            sy++;
            if (sy >= def.yRepeatEnd)
                sy = def.yRepeatStart;
        }

        sy = def.yRepeatEnd;
        while (dy < h)
        {
            ObjectRow& row = def.rows[sy];
            drawRow(painter, grid, def, row, x, y+dy, w, zoom);

            dy++;
            sy++;
            if (sy >= end)
                sy = def.yRepeatEnd;
        }
    }
    else
    {
        // without repeat

        sy = 0;
        while (dy < h)
        {
            ObjectRow& row = def.rows[sy];
            drawRow(painter, grid, def, row, x, y+dy, w, zoom);

            dy++;
            sy++;
            if (sy >= end)
                sy = 0;
        }
    }
}


quint8 Tileset::getBehaviorByte(int tile, int byte)
{
    return behaviors[tile][byte];
}

void Tileset::setBehaviorByte(int tile, int byte, quint8 value)
{
    behaviors[tile][byte] = value;
}

quint8 Tileset::getData(int objNbr, int x, int y, int byte)
{
    ObjectDef& def = *objectDefs[objNbr];
    return def.rows[y].data[x*3 + byte];
}

void Tileset::setData(int objNbr, int x, int y, int byte, int value)
{
    objectDefs[objNbr]->rows[y].data[x*3 + byte] = value;
}

QImage& Tileset::getImage()
{
    return texImage;
}

void Tileset::setImage(QImage &img, uint quality, bool dither)
{
    ctpk->setTextureEtc1(0, img, true, quality, dither);
    texImage = ctpk->getTexture((quint32)0);
}

void Tileset::save()
{
    // Save Behaviors
    FileBase* behaviorsFile = archive->openFile("BG_chk/d_bgchk_" + name + ".bin");
    behaviorsFile->open();
    behaviorsFile->seek(0);
    for (int i = 0; i < 441; i++)
    {
        for (int j = 0; j < 8; j++)
            behaviorsFile->write8(behaviors[i][j]);
    }
    behaviorsFile->save();
    behaviorsFile->close();
    delete behaviorsFile;


    // Save 3D Overlays
    if (archive->fileExists("BG_unt/" + name + "_add.bin"))
    {
        FileBase* overlaysFile = archive->openFile("BG_unt/" + name + "_add.bin");
        overlaysFile->open();
        overlaysFile->seek(0);
        for (int i = 0; i < 441; i++)
        {
            overlaysFile->write16(overlays3D[i]);
        }
        overlaysFile->save();
        overlaysFile->close();
        delete overlaysFile;
    }


    // Save Object Def
    FileBase* objindex = archive->openFile("/BG_unt/"+name+"_hd.bin");
    objindex->open();
    objindex->seek(0);

    FileBase* objdata = archive->openFile("/BG_unt/"+name+".bin");
    objdata->open();
    objdata->seek(0);

    // Calc new File Sizes
    objindex->resize(6 * objectDefs.size());

    quint64 dataSize = 0;
    for (int o = 0; o < objectDefs.size(); o++)
    {
        for (int r = 0; r < objectDefs[o]->rows.size(); r++)
        {
            if (objectDefs[o]->rows[r].slopeFlags != 0)
                dataSize++;
            dataSize += objectDefs[o]->rows[r].data.size();
            dataSize++; // 0xEF
        }
        dataSize++; // 0xFF
    }

    objdata->resize(dataSize);

    for (int o = 0; o < objectDefs.size(); o++)
    {
        ObjectDef& obj = *objectDefs[o];

        objindex->write16((quint16)objdata->pos());
        objindex->write8(obj.width);
        objindex->write8(obj.height);
        objindex->write8(obj.randomisation);
        objindex->write8(obj.unkFlag);

        for (int r = 0; r < obj.rows.size(); r++)
        {
            if (obj.rows[r].slopeFlags != 0)
                objdata->write8(obj.rows[r].slopeFlags);
            for (int d = 0; d < obj.rows[r].data.size(); d++)
                objdata->write8(obj.rows[r].data[d]);
            objdata->write8(0xFE);
        }
        objdata->write8(0xFF);
    }

    objindex->save();
    objdata->save();

    objindex->close();
    objdata->close();

    delete objindex;
    delete objdata;
}

void Tileset::addObject(int objNbr)
{
    // Create an empty object
    ObjectDef* obj = new ObjectDef();
    obj->width = 1;
    obj->height = 1;
    obj->randomisation = 0;
    obj->unkFlag = 0;
    obj->yRepeatStart = 0xFF;
    obj->slopeY = obj->height;

    ObjectRow* row = new ObjectRow();
    row->slopeFlags = 0;
    row->xRepeatStart = 0xFF;
    row->data.append(0x00); // no repeating
    row->data.append(0x00); // make it use no tile
    row->data.append(0x00); // make it blank
    obj->rows.append(*row);

    objectDefs.insert(objNbr+1, obj);
}

void Tileset::removeObject(int objNbr)
{
    objectDefs.removeAt(objNbr);
}

void Tileset::moveObjectDown(int objNbr)
{
    if (objNbr < objectDefs.size())
        objectDefs.move(objNbr, objNbr+1);
}


void Tileset::moveObjectUp(int objNbr)
{
    if (objNbr > 0)
        objectDefs.move(objNbr, objNbr-1);
}

void Tileset::resizeObject(int objNbr, int width, int height)
{
    ObjectDef& obj = *objectDefs[objNbr];

    if (width != -1)
    {
        int wDelta = width - obj.width;
        if (wDelta >= 0)
        {
            for (int i = 0; i < wDelta; i++)
            {
                for (int j = 0; j < obj.rows.size(); j++)
                {
                    obj.rows[j].data.append(0x00);
                    obj.rows[j].data.append(0x00);
                    obj.rows[j].data.append(0x00);
                }
            }
        }
        else
        {
            wDelta *= -1;
            for (int i = 0; i < wDelta; i++)
            {
                for (int j = 0; j < obj.rows.size(); j++)
                {
                    obj.rows[j].data.removeLast();
                    obj.rows[j].data.removeLast();
                    obj.rows[j].data.removeLast();
                }
            }
        }
        obj.width = width;
    }

    if (height != -1)
    {
        int hDelta = height - obj.height;

        if (hDelta >= 0)
        {
            for (int i = 0; i < hDelta; i++)
            {
                ObjectRow* row = new ObjectRow();
                row->slopeFlags = 0;    // TODO: Handle properly
                for (int j = 0; j < obj.rows[0].data.size(); j++)
                    row->data.append(0x00);
                obj.rows.append(*row);
            }
        }
        else
        {
            hDelta *= -1;
            for (int i = 0; i < hDelta; i++)
                obj.rows.removeLast();
        }
        obj.height = height;
    }
}

void Tileset::setSlot(int slot)
{
    this->slot = slot;

    for (int o = 0; o < objectDefs.size(); o++)
    {
        ObjectDef& obj = *objectDefs[o];
        for (int r = 0; r < obj.rows.size(); r++)
        {
            for (int t = 0; t < obj.rows[r].data.size(); t += 3)
                if (!(obj.rows[r].data[t+1] == 0 && (obj.rows[r].data[t+2] & 6) >> 1 == 0)) // Don't change empty tiles
                    obj.rows[r].data[t+2] = (obj.rows[r].data[t+2] & 249) | slot << 1;
        }
    }
}

quint16 Tileset::getOverlayTile(int selTile)
{
    return overlays3D[selTile] & 511;
}

quint16 Tileset::getOverlayTileSlot(int selTile)
{
    return (overlays3D[selTile] & 3584) >> 9;
}

void Tileset::setOverlayTile(int selTile, int ovTile, int slot)
{
    overlays3D[selTile] = (overlays3D[selTile] & 65024) | ovTile;
    overlays3D[selTile] = (overlays3D[selTile] & 61951) | (slot << 9);
}

bool Tileset::getRandomizeH(int tile)
{
    return (objectDefs[tile]->randomisation & 16) >> 4;
}

void Tileset::setRandomizeH(int tile, bool value)
{
    objectDefs[tile]->randomisation = (objectDefs[tile]->randomisation & 239) | value << 4;
}

bool Tileset::getRandomizeV(int tile)
{
    return (objectDefs[tile]->randomisation & 32) >> 5;
}

void Tileset::setRandomizeV(int tile, bool value)
{
    objectDefs[tile]->randomisation = (objectDefs[tile]->randomisation & 223) | value << 5;
}

quint8 Tileset::getRandomizeTiles(int tile)
{
    return (objectDefs[tile]->randomisation & 15);
}

void Tileset::setRandomizeTiles(int tile, quint8 value)
{
    objectDefs[tile]->randomisation = (objectDefs[tile]->randomisation & 240) | value;
}

void Tileset::setObjectBehavior(int selObj, int type, int hStart, int hEnd, int vStart, int vEnd)
{
    // Types
    //  0: Tile
    //  1: Repeat Horizontally
    //  2: Repeat Vertically
    //  3: Repeat Horizontally and Vertically
    //  4: Slope (Up)
    //  5: Slope (Down)
    //  6: Upside-Down Slope (Down)
    //  7: Upside-Down Slope (Up)

    ObjectDef& obj = *objectDefs[selObj];

    obj.yRepeatStart = 255;
    obj.yRepeatEnd = 0;
    for (int r = 0; r < obj.rows.size(); r++) // Tile / Clean Up
    {
        obj.rows[r].xRepeatStart = 255;
        obj.rows[r].xRepeatEnd = 0;
        obj.rows[r].slopeFlags = 0;
        for (int d = 0; d < obj.rows[r].data.size(); d += 3)
            obj.rows[r].data[d] = 0;
    }

    if (type >= 4 && type <= 7) // Slope
    {
        obj.rows[0].slopeFlags = 144 + type-4;

        if (obj.rows.size() < 2)
            return;

        for (int r = 1; r < obj.rows.size(); r++)
        {
            obj.rows[r].slopeFlags = 132;

            for (int d = 0; d < obj.rows[r].data.size(); d += 3)
                obj.rows[r].data[d] = 4;
        }
        return;
    }

    if (type == 1 || type == 3) // Repeat Horizontally
    {
        for (int r = 0; r < obj.rows.size(); r++)
        {
            obj.rows[r].xRepeatStart = hStart;
            obj.rows[r].xRepeatEnd = hEnd;

            for (int d = 0; d < obj.rows[r].data.size(); d += 3)
                if (d/3 >= hStart && d/3 < hEnd) obj.rows[r].data[d] = (obj.rows[r].data[d] & 254) | true;
        }
    }

    if (type == 2 || type == 3) // Repeat Vertically
    {
        obj.yRepeatStart = vStart;
        obj.yRepeatEnd = vEnd;

        for (int r = 0; r < obj.rows.size(); r++)
        {
            for (int d = 0; d < obj.rows[r].data.size(); d += 3)
                if (r >= vStart && r < vEnd) obj.rows[r].data[d] = (obj.rows[r].data[d] & 253) | true << 1;
        }
    }
}


void Tileset::replaceCTPK(QString filename)
{
    QFile newCtpk(filename);

    if(!newCtpk.open(QIODevice::ReadOnly))
        return;

    newCtpk.seek(0);
    char* data = new char[newCtpk.size()];
    newCtpk.read(data, newCtpk.size());
    newCtpk.close();

    delete ctpk;

    FileBase* texFile = archive->openFile("/BG_tex/"+name+".ctpk");
    texFile->open();
    texFile->resize(newCtpk.size());
    texFile->seek(0);
    texFile->writeData((quint8*)data, texFile->size());
    texFile->save();
    texFile->close();

    ctpk = new Ctpk(archive->openFile("/BG_tex/"+name+".ctpk"));

    texImage = ctpk->getTexture(name + ".tga");
}

void Tileset::setInternalName(QString newName)
{
    qDebug() << "setting filename";

    ctpk->setFilename(name + ".tga");
}

QImage Tileset::padTilesetImage(const QImage& img, quint32 outWidth, quint32 outHeight)
{
    QImage ret(outWidth, outHeight, QImage::Format_RGBA8888);
    ret.fill(Qt::transparent);

    QPainter painter(&ret);

    quint32 tilew = img.width() / 20;
    quint32 tileh = img.height() / 20;
    quint32 tilenum = tilew * tileh;

    for (quint32 i = 0; i < tilenum; i++)
    {
        // Margins
        painter.drawImage(QRect(i%tilew*20 + i%tilew*4, 2 + i/tilew*20 + i/tilew*4, 2, 20), img.copy(i%tilew*20, i/tilew*20, 1, 20));               // Left
        painter.drawImage(QRect(22 + i%tilew*20 + i%tilew*4, 2 + i/tilew*20 + i/tilew*4, 2, 20), img.copy(19 + i%tilew*20, i/tilew*20, 1, 20));     // Right
        painter.drawImage(QRect(2 + i%tilew*20 + i%tilew*4, i/tilew*20 + i/tilew*4, 20, 2), img.copy(i%tilew*20, i/tilew*20, 20, 1));               // Top
        painter.drawImage(QRect(2 + i%tilew*20 + i%tilew*4, 22 + i/tilew*20 + i/tilew*4, 20, 2), img.copy(i%tilew*20, 19 + i/tilew*20, 20, 1));     // Bottom
        painter.drawImage(QRect(i%tilew*20 + i%tilew*4, i/tilew*20 + i/tilew*4, 2, 2), img.copy(i%tilew*20, i/tilew*20, 1, 1));                     // Top-Left
        painter.drawImage(QRect(22 + i%tilew*20 + i%tilew*4, i/tilew*20 + i/tilew*4, 2, 2), img.copy(19 + i%tilew*20, i/tilew*20, 1, 1));           // Top-Right
        painter.drawImage(QRect(i%tilew*20 + i%tilew*4, 22 + i/tilew*20 + i/tilew*4, 2, 2), img.copy(i%tilew*20, 19 + i/tilew*20, 1, 1));           // Bottom-Left
        painter.drawImage(QRect(22 + i%tilew*20 + i%tilew*4, 22 + i/tilew*20 + i/tilew*4, 2, 2), img.copy(19 + i%tilew*20, 19 + i/tilew*20, 1, 1)); // Bottom-Right

        // Core Tiles
        painter.drawImage(QRect(2 + i%tilew*20 + i%tilew*4, 2 + i/tilew*20 + i/tilew*4, 20, 20), img.copy(i%tilew*20, i/tilew*20, 20, 20));
    }

    return ret;
}
