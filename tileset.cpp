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

    //qDebug("LOAD TILESET %s", name.toStdString().c_str());

    archive = new SarcFilesystem(game->fs->openFile("/Unit/"+name+".sarc"));
    texture = new Ctpk(archive->openFile("/BG_tex/"+name+".ctpk"));

    texImage = texture->getTexture(0);


    // parse the object def
    FileBase* objindex = archive->openFile("/BG_unt/"+name+"_hd.bin");
    objindex->open();
    objindex->seek(0);

    FileBase* objdata = archive->openFile("/BG_unt/"+name+".bin");
    objdata->open();

    numObjects = objindex->size() / 6;
    objectDef = new ObjectDef[numObjects];
    for (int o = 0; o < numObjects; o++)
    {
        quint16 offset = objindex->read16();
        quint8 width = objindex->read8();
        quint8 height = objindex->read8();
        quint8 crap1 = objindex->read8();
        quint8 crap2 = objindex->read8();

        //qDebug("OBJECT %d -- %04X %dx%d %02X %02X", o, offset, width, height, crap1, crap2);

        ObjectDef* obj = &objectDef[o];//new ObjectDef();
        obj->width = width;
        obj->height = height;
        obj->flags1 = crap1;
        obj->flags2 = crap2;

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

            // extra
            // 00: blank tile???
            // 02: typical
            // other values also seen here: dunno
            b = objdata->read8();if (b==0) qDebug("ZERO!!! %d", o);
            //if (b != 0x00 && b != 0x02) qDebug("!!! UNUSUAL EXTRA %02X @ %d,%d OBJ %d %02X", b, curx, cury, o, offset);
            row->data.append(b); // extra shit? always 02

            curx++;
        }
    }

    objdata->close();
    delete objdata;

    objindex->close();
    delete objindex;

    // parse behaviors
    FileBase* behaviorsFile = archive->openFile("BG_chk/d_bgchk_" + name + ".bin");
    behaviorsFile->open();
    behaviorsFile->seek(0);
    for (int i = 0; i < 441; i++)
    {
        for (int j = 0; j < 8; j++)
            behaviors[i][j] = behaviorsFile->read8();
    }
    behaviorsFile->close();
    delete behaviorsFile;
}

Tileset::~Tileset()
{
    delete[] objectDef;

    delete texImage;

    delete texture;
    delete archive;
}


// x and y in tile coords
void Tileset::drawTile(QPainter& painter, TileGrid& grid, int num, int x, int y, float zoom)
{
    quint32 gridid = x | (y<<16);
    if (grid[gridid] == grid[0xFFFFFFFF])
        return;

    int tsize = (int)(20*zoom);
    x *= tsize;
    y *= tsize;

    QRect rsrc(2 + ((num%21)*24), 2 + ((num/21)*24), 20, 20);
    QRect rdst(x, y, tsize, tsize);

    painter.drawImage(rdst, *texImage, rsrc);
    grid[gridid] = grid[0xFFFFFFFF];
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
            //if (row.data[sx*3 + 2])
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom);

            dx++;
            sx++;
            if (sx >= row.xRepeatStart)
                sx = 0;
        }

        sx = row.xRepeatStart;
        while (dx < rend)
        {
            //if (row.data[sx*3 + 2])
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom);

            dx++;
            sx++;
            if (sx >= row.xRepeatEnd)
                sx = row.xRepeatStart;
        }

        sx = row.xRepeatEnd;
        while (dx < w)
        {
            //if (row.data[sx*3 + 2])
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom);

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
            //if (row.data[sx*3 + 2])
                drawTile(painter, grid, row.data[sx*3 + 1], x+dx, y, zoom);

            dx++;
            sx++;
            if (sx >= end)
                sx = 0;
        }
    }
}

void Tileset::drawObject(QPainter& painter, TileGrid& grid, int num, int x, int y, int w, int h, float zoom)
{
    if (num >= numObjects) // TODO handle properly
    {
        qDebug("!! BAD OBJ NUMBER %d\n", num);
        return;
    }

    ObjectDef& def = objectDef[num];

    //qDebug("RENDER OBJ %d | %d %d | %d,%d %d,%d\n", num, def.rows.length(), 4646445, x, y, w, h);

    if (def.rows[0].slopeFlags & 0x80)
    {
        // slope junk

        // slope 00: start from bottom-left (top-right?? checkme)
        // slope 01: start from top-left
        // slope 02: same as 00, reverse Y
        // slope 03: same as 01, reverse Y

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
