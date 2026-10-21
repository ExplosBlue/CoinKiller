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
#include "settingsmanager.h"

#include <utility>
#include <QDomDocument>
#include <QObject>
#include <QSet>
#include <QDir>
#include <QFile>

Tileset::Tileset(Game *game, QString name)
{
    this->game = game;
    this->name = name;



    archive = new SarcFilesystem(game->fs->openFile("/Unit/"+name+".sarc"));
    ctpk = new Ctpk(archive->openFile("/BG_tex/"+name+".ctpk"));

    texImage = ctpk->getTexture((quint32)0);
    texPixmap = QPixmap::fromImage(texImage);

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

        ObjectDef* obj = new ObjectDef();
        obj->width = width;
        obj->height = height;
        obj->randomisation = randomisation;
        obj->unkFlag = crap;

        if (crap != 0)
            qDebug() << "Found unkFlag value != 0:" << crap << "for Object" << o;

        obj->yRepeatStart = 0xFF;

        // data format:
        // [marker] [flags] [tile] [slot] ...
        // 0xFE -> next row
        // 0xFF -> end
        //
        // markers:
        // 0x80-0x83, 0x90-0x93 -> slope type (first row)
        // 0x84 -> section 1 start
        // 0x85 -> section 2 start
        //
        // flags:
        // 0x01 -> repeat X
        // 0x02 -> repeat Y
        // 0x04 -> slope base tile

        quint8 b;
        objdata->seek(offset);

        int curx = 0, cury = 0;

        ObjectRow* row = new ObjectRow();
        bool xrepeat = false;
        int flag4State = 0;
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
                xrepeat = false;
                flag4State = 0;

                curx = 0;
                cury++;
                continue;
            }

            if (b & 0x80) // slope marker
            {
                row->slopeFlags = b;
                b = objdata->read8();
            }

            if (b & 0x04)
            {
                if (flag4State == 2)
                {
                    flag4State = 3;
                    row->xRepeatEnd = curx;
                }
                else if (!flag4State)
                    flag4State = 1;
            }
            else if (flag4State == 1)
            {
                flag4State = 2;
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

    slot = TilesetList::slotForFileName(name);

    if (slot == -1)
    {
        slot = 0;
        bool foundSlot = false;

        for (int o = 0; o < objectDefs.size() && !foundSlot; o++)
        {
            ObjectDef& obj = *objectDefs[o];

            for (int r = 0; r < obj.rows.size() && !foundSlot; r++)
            {
                for (int t = 0; t < obj.rows[r].data.size(); t += 3)
                {
                    if (obj.rows[r].data[t+1] == 0 && (obj.rows[r].data[t+2] & 6) == 0)
                        continue;

                    slot = (obj.rows[r].data[t+2] & 6) >> 1;
                    foundSlot = true;
                    break;
                }
            }
        }
    }

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


void Tileset::clearScaledTiles()
{
    scaledTiles.clear();
    scaledTilesScale = 1.0;
}

const QPixmap& Tileset::getScaledTile(int num, qreal scale)
{
    if (qAbs(scale - scaledTilesScale) > 0.001)
    {
        scaledTiles.clear();
        scaledTilesScale = scale;
    }

    auto it = scaledTiles.constFind(num);
    if (it != scaledTiles.constEnd())
        return it.value();

    QRect rsrc(2 + ((num%21)*24), 2 + ((num/21)*24), 20, 20);
    int size = qMax(1, qRound(20.0 * scale));
    QImage tile = texImage.copy(rsrc).scaled(size, size, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    QPixmap pixmap = QPixmap::fromImage(tile);
    pixmap.setDevicePixelRatio(size / 20.0);

    return scaledTiles.insert(num, pixmap).value();
}

// x and y in tile coords
void Tileset::drawTile(QPainter& painter, TileGrid& grid, int num, int x, int y, float zoom, int item)
{
    drawTile(painter, grid, num, x, y, zoom, item, grid.value(0xFFFFFFFF));
}

void Tileset::drawTile(QPainter& painter, TileGrid& grid, int num, int x, int y, float zoom, int item, quint8 gridLayer)
{
    quint32 gridid = x | (y<<16);
    auto gridIt = grid.constFind(gridid);
    if (gridIt != grid.constEnd() && gridIt.value() == gridLayer)
        return;

    int tsize = (int)(20*zoom);
    x *= tsize;
    y *= tsize;

    QRect rdst(x, y, tsize, tsize);

    qreal tileScale = 1.0;
    if (tsize == 20)
    {
        QTransform trans = painter.transform();
        if (trans.m11() > 0.0 && qAbs(trans.m11() - trans.m22()) < 0.001 &&
            qAbs(trans.m12()) < 0.001 && qAbs(trans.m21()) < 0.001)
            tileScale = trans.m11() * painter.device()->devicePixelRatioF();
    }
    bool useScaled = tileScale > 1.001;

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
        if (useScaled)
            painter.drawPixmap(rdst, getScaledTile(num, tileScale));
        else
        {
            QRect rsrc(2 + ((num%21)*24), 2 + ((num/21)*24), 20, 20);
            painter.drawPixmap(rdst, texPixmap, rsrc);
        }
    }

    if (draw3D && (getOverlayTile(num) !=0))
    {
        int overlay = getOverlayTile(num);
        if (useScaled)
            painter.drawPixmap(rdst, getScaledTile(overlay, tileScale));
        else
        {
            QRect overlaysrc(2 + ((overlay%21)*24), 2 + ((overlay/21)*24), 20, 20);
            painter.drawPixmap(rdst, texPixmap, overlaysrc);
        }
    }

    grid.insert(gridid, gridLayer);

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

template<typename F>
static void forEachRowCell(const ObjectRow& row, int w, F fn)
{
    int end = row.data.length()/3;
    if (end <= 0 || w <= 0)
        return;

    int dx = 0;

    if (row.xRepeatStart != 0xFF && row.xRepeatStart < end)
    {
        int rstart = row.xRepeatStart;
        int repeatEnd = qMin((int)row.xRepeatEnd, end);
        if (repeatEnd <= rstart)
            repeatEnd = end;
        int rend = w - (end - repeatEnd);

        int sx = 0;
        while (dx < rstart)
        {
            fn(sx, dx);
            dx++;
            sx++;
            if (sx >= rstart)
                sx = 0;
        }

        sx = rstart;
        while (dx < rend)
        {
            fn(sx, dx);
            dx++;
            sx++;
            if (sx >= repeatEnd)
                sx = rstart;
        }

        sx = qMin(repeatEnd, end - 1);
        while (dx < w)
        {
            fn(sx, dx);
            dx++;
            sx++;
            if (sx >= end)
                sx = qMin(repeatEnd, end - 1);
        }
    }
    else
    {
        int drawEnd = (row.data[0] == 4) ? qMin(w, end) : w;
        int sx = 0;
        while (dx < drawEnd)
        {
            fn(sx, dx);
            dx++;
            sx++;
            if (sx >= end)
                sx = 0;
        }
    }
}

template<typename F>
static void forEachRowCellRange(const ObjectRow& row, int w, int dxStart, int dxEnd, F fn)
{
    int end = row.data.length()/3;
    if (end <= 0 || w <= 0)
        return;

    dxStart = qMax(dxStart, 0);
    dxEnd = qMin(dxEnd, w);
    if (dxStart >= dxEnd)
        return;

    if (row.xRepeatStart != 0xFF && row.xRepeatStart < end)
    {
        int rstart = row.xRepeatStart;
        int repeatEnd = qMin((int)row.xRepeatEnd, end);
        if (repeatEnd <= rstart)
            repeatEnd = end;
        int rend = w - (end - repeatEnd);
        int thirdStart = qMax(rstart, rend);

        for (int dx = dxStart; dx < dxEnd; dx++)
        {
            int sx;
            if (dx < rstart)
                sx = dx;
            else if (dx < rend)
                sx = rstart + (dx-rstart) % (repeatEnd-rstart);
            else
                sx = repeatEnd + (dx-thirdStart) % (end-repeatEnd);
            fn(sx, dx);
        }
    }
    else
    {
        int drawEnd = (row.data[0] == 4) ? qMin(w, end) : w;
        dxEnd = qMin(dxEnd, drawEnd);
        if (dxStart >= dxEnd)
            return;

        int sx = dxStart % end;
        for (int dx = dxStart; dx < dxEnd; dx++)
        {
            fn(sx, dx);
            if (++sx >= end)
                sx = 0;
        }
    }
}

template<typename F>
static void forEachSlopeTile(const ObjectDef& def, int w, int h, F fn)
{
    if (def.rows.isEmpty())
        return;

    quint8 slopeflag = def.rows[0].slopeFlags;

    if (!(slopeflag & 0x80) || slopeflag == SlopeSection1 || slopeflag == SlopeSection2)
        return;

    QList<int> sections[3];
    int section = 0;

    for (int r = 0; r < def.rows.size(); r++)
    {
        quint8 flags = def.rows[r].slopeFlags;

        if (flags == SlopeSection1)
            section = 1;
        else if (flags == SlopeSection2)
            section = 2;

        sections[section].append(r);
    }

    int secW[3] = {0, 0, 0};
    int secH[3] = {0, 0, 0};

    for (int s = 0; s < 3; s++)
    {
        secH[s] = sections[s].size();

        for (int r : sections[s])
            secW[s] = qMax(secW[s], def.rows[r].data.size() / 3);
    }

    if (secW[0] <= 0 || secH[0] <= 0)
        return;

    int w0 = secW[0], h0 = secH[0];
    int w1 = secW[1], h1 = secH[1];
    int h2 = secH[2];

    auto normRow = [&](int s, int rowNbr)
    {
        int n = sections[s].size();
        return ((rowNbr % n) + n) % n;
    };

    auto emitTile = [&](int s, int rowNbr, int col, int px, int py)
    {
        if (sections[s].isEmpty() || px < 0 || px >= w || py >= h)
            return;

        int flatRow = sections[s][normRow(s, rowNbr)];
        int rowW = def.rows[flatRow].data.size() / 3;

        if (col < 0 || col >= rowW)
            return;

        fn(flatRow, col, px, py);
    };

    auto emitRow = [&](int s, int rowNbr, int px, int py, int rowWidth)
    {
        if (sections[s].isEmpty() || px < 0 || py >= h || rowWidth <= 0)
            return;

        int flatRow = sections[s][normRow(s, rowNbr)];
        const ObjectRow& row = def.rows[flatRow];

        forEachRowCell(row, qMin(rowWidth, w - px), [&](int col, int dx)
        {
            fn(flatRow, col, px + dx, py);
        });
    };

    auto drawSlopeUpSide = [&](int cx, int cy)
    {
        for (int r = 0; r < h0; r++)
        {
            int ty = cy - r;
            if (ty < 0) break;

            for (int c = 0; c < w0; c++)
            {
                if (cx + c >= w) break;
                emitTile(0, h0 - 1 - r, c, cx + c, ty);
            }
        }

        if (cx + w0 < w && h2 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - r;
                if (ty < 0) break;

                emitRow(2, h2 - 1 - r, cx + w0, ty, w - cx - w0);
            }
        }

        if (cx > 0 && h1 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - r;
                if (ty < 0) break;

                emitRow(1, h1 - 1 - r, 0, ty, qMin(cx, w));
            }
        }
    };

    auto drawSlopeDownSide = [&](int cx, int cy)
    {
        for (int r = 0; r < h0; r++)
        {
            int ty = cy - r;
            if (ty < 0) break;

            for (int c = 0; c < w0; c++)
            {
                if (cx + c >= w) break;
                emitTile(0, h0 - 1 - r, c, cx + c, ty);
            }
        }

        if (cx > 0 && h2 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - r;
                if (ty < 0) break;

                int rowNbr = h2 - 1 - r;
                int rowW = def.rows[sections[2][normRow(2, rowNbr)]].data.size() / 3;

                for (int c = 0; c < cx && c < w; c++)
                {
                    int col = rowW - cx + c;
                    if (col < 0 || col >= rowW) continue;
                    emitTile(2, rowNbr, col, c, ty);
                }
            }
        }

        if (cx + w0 < w && h1 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - r;
                if (ty < 0) break;

                emitRow(1, h1 - 1 - r, cx + w0, ty, w - cx - w0);
            }
        }
    };

    auto drawSlopeUpsideDownSide = [&](int cx, int cy)
    {
        for (int r = 0; r < h0; r++)
        {
            int ty = cy + r;
            if (ty >= h) break;

            for (int c = 0; c < w0; c++)
            {
                if (cx + c >= w) break;
                emitTile(0, r, c, cx + c, ty);
            }
        }

        if (cx + w0 < w && h2 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                emitRow(2, r, cx + w0, ty, w - cx - w0);
            }
        }

        if (cx > 0 && h1 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                emitRow(1, r, 0, ty, qMin(cx, w));
            }
        }
    };

    auto drawSlopeUpsideUpSide = [&](int cx, int cy)
    {
        for (int r = 0; r < h0; r++)
        {
            int ty = cy + r;
            if (ty >= h) break;

            for (int c = 0; c < w0; c++)
            {
                if (cx + c >= w) break;
                emitTile(0, r, c, cx + c, ty);
            }
        }

        if (cx + w0 < w && h1 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                emitRow(1, r, cx + w0, ty, w - cx - w0);
            }
        }

        if (cx > 0 && h2 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                int rowNbr = normRow(2, r);
                int rowW = def.rows[sections[2][rowNbr]].data.size() / 3;

                for (int c = 0; c < cx && c < w; c++)
                {
                    int col = rowW - cx + c;
                    if (col < 0 || col >= rowW) continue;
                    emitTile(2, rowNbr, col, c, ty);
                }
            }
        }
    };

    auto drawSlopeUpBase = [&](int cx, int cy)
    {
        if (cx + w1 <= w && h1 > 0)
        {
            for (int r = 0; r < h1; r++)
            {
                int ty = cy - (h1 - 1 - r);
                if (ty < 0) break;

                emitRow(1, r, cx, ty, w1);
            }
        }

        if (w >= cx + w0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - h1 - r;
                if (ty < 0) break;

                for (int c = 0; c < w0; c++)
                {
                    if (cx + c >= w) break;
                    emitTile(0, h0 - 1 - r, c, cx + c, ty);
                }
            }
        }

        if (cx + w0 < w && h2 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - h1 - r;
                if (ty < 0) break;

                emitRow(2, h2 - 1 - r, cx + w0, ty, w0);
            }
        }
    };

    auto drawSlopeDownBase = [&](int cx, int cy)
    {
        if (w >= cx + w0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                for (int c = 0; c < w0; c++)
                {
                    if (cx + c >= w) break;
                    emitTile(0, r, c, cx + c, ty);
                }
            }
        }

        if (w1 + cx <= w && h1 > 0)
        {
            for (int r = 0; r < h1; r++)
            {
                int ty = cy + h0 + r;
                if (ty >= h) break;

                emitRow(1, r, cx, ty, w1);
            }
        }
    };

    auto drawSlopeUpsideDownBase = [&](int cx, int cy)
    {
        if (w >= cx + w0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + h1 + r;
                if (ty >= h) break;

                for (int c = 0; c < w0; c++)
                {
                    if (cx + c >= w) break;
                    emitTile(0, r, c, cx + c, ty);
                }
            }
        }

        if (cx + w0 <= w && h1 > 0)
        {
            for (int r = 0; r < h1; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                emitRow(1, r, cx, ty, w1);
            }
        }

        if (cx + w0 <= w && h2 > 0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy + r;
                if (ty >= h) break;

                emitRow(2, r, cx + w0, ty, w1);
            }
        }
    };

    auto drawSlopeUpsideUpBase = [&](int cx, int cy)
    {
        if (w1 + cx <= w && h1 > 0)
        {
            for (int r = 0; r < h1; r++)
            {
                int ty = cy - h0 - (h1 - 1 - r);
                if (ty < 0) break;

                emitRow(1, r, cx, ty, w1);
            }
        }

        if (w >= cx + w0)
        {
            for (int r = 0; r < h0; r++)
            {
                int ty = cy - r;
                if (ty < 0) break;

                for (int c = 0; c < w0; c++)
                {
                    if (cx + c >= w) break;
                    emitTile(0, h0 - 1 - r, c, cx + c, ty);
                }
            }
        }
    };

    switch (slopeflag)
    {
    case SlopeUpSide:
        for (int cx = 0, cy = h - 1; cy >= 0; cx += w0, cy -= h0)
            drawSlopeUpSide(cx, cy);
        break;
    case SlopeDownSide:
        for (int cx = w - w0, cy = h - 1; cy >= 0; cx -= w0, cy -= h0)
            drawSlopeDownSide(cx, cy);
        break;
    case SlopeUpsideDownSide:
        for (int cx = 0, cy = 0; cy < h; cx += w0, cy += h0)
            drawSlopeUpsideDownSide(cx, cy);
        break;
    case SlopeUpsideUpSide:
        for (int cx = w - w0, cy = 0; cy < h; cx -= w0, cy += h0)
            drawSlopeUpsideUpSide(cx, cy);
        break;
    case SlopeUpBase:
        for (int cx = 0, cy = h - 1; cy >= 0; cx += w0, cy -= h0)
            drawSlopeUpBase(cx, cy);
        break;
    case SlopeDownBase:
        for (int cx = 0, cy = 0; cy < h; cx += w0, cy += h0)
            drawSlopeDownBase(cx, cy);
        break;
    case SlopeUpsideDownBase:
        for (int cx = 0, cy = 0; cy < h; cx += w0, cy += h0)
            drawSlopeUpsideDownBase(cx, cy);
        break;
    case SlopeUpsideUpBase:
        for (int cx = 0, cy = h - 1; cy >= 0; cx += w0, cy -= h0)
            drawSlopeUpsideUpBase(cx, cy);
        break;
    default:
        break;
    }
}

void Tileset::drawRow(QPainter& painter, TileGrid& grid, const ObjectRow& row, int x, int y, int w, float zoom, quint8 gridLayer, const QRect& clip)
{
    if (clip.isValid() && (y < clip.top() || y > clip.bottom()))
        return;

    int dxStart = 0;
    int dxEnd = w;
    if (clip.isValid())
    {
        dxStart = qMax(0, clip.left() - x);
        dxEnd = qMin(w, clip.right() - x + 1);
    }

    forEachRowCellRange(row, w, dxStart, dxEnd, [&](int col, int dx)
    {
        if (row.data[col*3 + 1] || (row.data[col*3 + 2] & 6) >> 1) // skip empty tiles
            drawTile(painter, grid, row.data[col*3 + 1], x + dx, y, zoom, (row.data[col*3 + 2] & 120) >> 3, gridLayer);
    });
}

void Tileset::drawObject(QPainter& painter, TileGrid& grid, int num, int x, int y, int w, int h, float zoom, const QRect& clip)
{
    if (num >= objectDefs.size())
    {
        int xStart = 0, xEnd = w, yStart = 0, yEnd = h;
        if (clip.isValid())
        {
            xStart = qMax(0, clip.left() - x);
            xEnd = qMin(w, clip.right() - x + 1);
            yStart = qMax(0, clip.top() - y);
            yEnd = qMin(h, clip.bottom() - y + 1);
        }

        for (int xx = xStart; xx < xEnd; xx++)
            for (int yy = yStart; yy < yEnd; yy++)
                painter.drawPixmap(QRect((x+xx)*20, (y+yy)*20, 20, 20), ImageCache::getInstance()->get(TileOverride, "error.png"));

        return;
    }

    ObjectDef& def = *objectDefs[num];

    if (def.rows.isEmpty())
        return;

    quint8 gridLayer = grid.value(0xFFFFFFFF);

    quint8 slopeflag = def.rows[0].slopeFlags;
    if ((slopeflag & 0x80) && slopeflag != SlopeSection1 && slopeflag != SlopeSection2)
    {
        forEachSlopeTile(def, w, h, [&](int rowIdx, int col, int px, int py)
        {
            int tx = x + px;
            int ty = y + py;
            if (clip.isValid() && (tx < clip.left() || tx > clip.right() || ty < clip.top() || ty > clip.bottom()))
                return;

            ObjectRow& row = def.rows[rowIdx];

            if (row.data[col*3 + 1] || (row.data[col*3 + 2] & 6) >> 1)
                drawTile(painter, grid, row.data[col*3 + 1], tx, ty, zoom, (row.data[col*3 + 2] & 120) >> 3, gridLayer);
        });

        return;
    }

    int sy = 0, dy = 0;
    int end = def.rows.length();

    int dyStart = 0;
    int dyEnd = h;
    if (clip.isValid())
    {
        dyStart = qMax(0, clip.top() - y);
        dyEnd = qMin(h, clip.bottom() - y + 1);
    }

    if (dyStart >= dyEnd)
        return;

    if (def.yRepeatStart != 0xFF)
    {
        // with repeat

        int rstart = def.yRepeatStart;
        int rend = h - (def.height - def.yRepeatEnd);

        dy = dyStart;
        if (dy < rstart)
        {
            sy = dy % (def.yRepeatStart > 0 ? def.yRepeatStart : 1);
            while (dy < rstart && dy < dyEnd)
            {
                if (sy < def.rows.length() && sy < def.yRepeatStart)
                {
                    ObjectRow& row = def.rows[sy];
                    drawRow(painter, grid, row, x, y+dy, w, zoom, gridLayer, clip);
                }
                dy++;
                sy++;
                if (sy >= def.yRepeatStart)
                    sy = 0;
            }
        }

        dy = qMax(dy, rstart);
        if (dy < rend && dy < dyEnd)
        {
            sy = def.yRepeatStart + (dy - rstart) % (def.yRepeatEnd > def.yRepeatStart ? (def.yRepeatEnd - def.yRepeatStart) : 1);
            if (sy >= def.yRepeatEnd && def.yRepeatEnd > def.yRepeatStart)
                sy = def.yRepeatStart;
            while (dy < rend && dy < dyEnd)
            {
                if (sy < def.rows.length() && sy >= def.yRepeatStart && sy < def.yRepeatEnd)
                {
                    ObjectRow& row = def.rows[sy];
                    drawRow(painter, grid, row, x, y+dy, w, zoom, gridLayer, clip);
                }
                dy++;
                sy++;
                if (sy >= def.yRepeatEnd)
                    sy = def.yRepeatStart;
            }
        }

        dy = qMax(dy, rend);
        if (dy < h && dy < dyEnd)
        {
            sy = def.yRepeatEnd + (dy - rend) % (end > def.yRepeatEnd ? (end - def.yRepeatEnd) : 1);
            if (sy >= end)
                sy = def.yRepeatEnd;
            while (dy < h && dy < dyEnd)
            {
                if (sy < def.rows.length())
                {
                    ObjectRow& row = def.rows[sy];
                    drawRow(painter, grid, row, x, y+dy, w, zoom, gridLayer, clip);
                }
                dy++;
                sy++;
                if (sy >= end)
                    sy = def.yRepeatEnd;
            }
        }
    }
    else
    {
        // without repeat

        if (dyStart < dyEnd)
        {
            dy = dyStart;
            sy = end > 0 ? dyStart % end : 0;
            while (dy < dyEnd)
            {
                if (sy < def.rows.length())
                {
                    ObjectRow& row = def.rows[sy];
                    drawRow(painter, grid, row, x, y+dy, w, zoom, gridLayer, clip);
                }
                dy++;
                sy++;
                if (sy >= end)
                    sy = 0;
            }
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

bool Tileset::getObjectCell(int objNbr, int x, int y, int& row, int& col)
{
    const ObjectDef& def = *objectDefs[objNbr];
    row = y;
    col = x;

    if (def.rows.isEmpty())
        return false;

    quint8 flags = def.rows[0].slopeFlags;
    if (!(flags & 0x80) || flags == SlopeSection1 || flags == SlopeSection2)
    {
        if (row < 0 || row >= def.rows.size())
            return false;

        int rowW = def.rows[row].data.size()/3;
        return col >= 0 && col < rowW;
    }

    bool found = false;
    bool foundVisible = false;
    forEachSlopeTile(def, def.width, def.height, [&](int rowIdx, int colIdx, int px, int py)
    {
        if (px != x || py != y)
            return;

        const ObjectRow& r = def.rows[rowIdx];
        bool visible = r.data[colIdx*3 + 1] || (r.data[colIdx*3 + 2] & 6) >> 1;

        if (visible || !foundVisible)
        {
            row = rowIdx;
            col = colIdx;
            found = true;
            foundVisible = visible;
        }
    });

    return found;
}

quint8 Tileset::getData(int objNbr, int x, int y, int byte)
{
    int row, col;
    if (!getObjectCell(objNbr, x, y, row, col))
        return 0;

    return objectDefs[objNbr]->rows[row].data[col*3 + byte];
}

void Tileset::setData(int objNbr, int x, int y, int byte, int value)
{
    int row, col;
    if (!getObjectCell(objNbr, x, y, row, col))
        return;

    objectDefs[objNbr]->rows[row].data[col*3 + byte] = value;
}

QImage& Tileset::getImage()
{
    return texImage;
}

void Tileset::setImage(QImage &img, uint quality, bool dither)
{
    ctpk->setTextureEtc1(0, img, true, quality, dither);
    texImage = ctpk->getTexture((quint32)0);
    texPixmap = QPixmap::fromImage(texImage);
    clearScaledTiles();
}

static void paintPaddedTile(QImage& dst, const QImage& sheet, int i, int tilew)
{
    const int spx = i % tilew * 20;
    const int spy = i / tilew * 20;
    const int dpx = i % tilew * 20 + i % tilew * 4;
    const int dpy = i / tilew * 20 + i / tilew * 4;

    QPainter p(&dst);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(QRect(dpx, dpy, 24, 24), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Margins
    p.drawImage(QRect(dpx, 2 + dpy, 2, 20), sheet.copy(spx, spy, 1, 20));                        // Left
    p.drawImage(QRect(22 + dpx, 2 + dpy, 2, 20), sheet.copy(19 + spx, spy, 1, 20));              // Right
    p.drawImage(QRect(2 + dpx, dpy, 20, 2), sheet.copy(spx, spy, 20, 1));                        // Top
    p.drawImage(QRect(2 + dpx, 22 + dpy, 20, 2), sheet.copy(spx, 19 + spy, 20, 1));              // Bottom
    p.drawImage(QRect(dpx, dpy, 2, 2), sheet.copy(spx, spy, 1, 1));                              // Top-Left
    p.drawImage(QRect(22 + dpx, dpy, 2, 2), sheet.copy(19 + spx, spy, 1, 1));                    // Top-Right
    p.drawImage(QRect(dpx, 22 + dpy, 2, 2), sheet.copy(spx, 19 + spy, 1, 1));                    // Bottom-Left
    p.drawImage(QRect(22 + dpx, 22 + dpy, 2, 2), sheet.copy(19 + spx, 19 + spy, 1, 1));          // Bottom-Right
    // Core Tile
    p.drawImage(QRect(2 + dpx, 2 + dpy, 20, 20), sheet.copy(spx, spy, 20, 20));
    p.end();
}

void Tileset::setImageRegion(QImage &sheet, const QList<int> &tiles, uint quality, bool dither)
{
    QSet<int> selected;

    for (int tile : tiles)
    {
        if (tile >= 0 && tile < 441)
            selected.insert(tile);
    }

    if (selected.isEmpty())
        return;

    QImage newImg = texImage.copy();

    QList<QRect> regions;
    regions.reserve(selected.size());

    for (int tile : selected)
    {
        paintPaddedTile(newImg, sheet, tile, 21);

        regions.append(QRect(tile % 21 * 24, tile / 21 * 24, 24, 24));
    }

    ctpk->setTextureEtc1Region(0, newImg, regions, quality, dither);
    texImage = ctpk->getTexture((quint32)0);
    texPixmap = QPixmap::fromImage(texImage);
    clearScaledTiles();
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
            dataSize++; // 0xFE
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

    // save the (possibly in-memory edited) texture
    ctpk->save();
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

    ObjectRow* row = new ObjectRow();
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

    for (int i = 0; i < 441; i++)
        if (getOverlayTile(i) != 0)
            overlays3D[i] = (overlays3D[i] & 61951) | (slot << 9);
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
        obj.rows[0].slopeFlags = quint8(SlopeUpBase + (type - 4));

        if (obj.rows.size() < 2)
            return;

        for (int r = 1; r < obj.rows.size(); r++)
        {
            obj.rows[r].slopeFlags = (r == 1) ? SlopeSection1 : SlopeNone;

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


void Tileset::setInternalName(QString newName)
{
    qDebug() << "setting filename";

    ctpk->setFilename(newName + ".tga");
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

namespace {

QString tilesetDisplayNameFromFile(const QString& fileName)
{
    QString name = fileName;

    if (name.startsWith("S1_") || name.startsWith("S2_"))
        name = name.mid(3);
    else if (name.startsWith("M_") || name.startsWith("J_"))
        name = name.mid(2);

    return name.replace('_', ' ');
}

bool parseTilesetsXml(const QString& path, QList<TilesetGroup>& groups)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument xml;
    xml.setContent(&file);
    file.close();

    QDomElement root = xml.documentElement();
    if (root.isNull())
        return false;

    for (QDomElement groupElement = root.firstChildElement("group"); !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("group"))
    {
        TilesetGroup group;
        group.name = groupElement.attribute("name");

        for (QDomElement child = groupElement.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        {
            if (child.tagName() == "tileset")
            {
                TilesetEntry entry;
                entry.fileName = child.attribute("file");
                entry.name = child.attribute("name", entry.fileName);

                if (entry.fileName.isEmpty())
                    continue;

                group.tilesets.append(entry);
            }
            else if (child.tagName() == "subgroup")
            {
                TilesetSubgroup subgroup;
                subgroup.name = child.attribute("name");

                for (QDomElement tsElement = child.firstChildElement("tileset"); !tsElement.isNull(); tsElement = tsElement.nextSiblingElement("tileset"))
                {
                    TilesetEntry entry;
                    entry.fileName = tsElement.attribute("file");
                    entry.name = tsElement.attribute("name", entry.fileName);

                    if (entry.fileName.isEmpty())
                        continue;

                    subgroup.tilesets.append(entry);
                }

                if (!subgroup.tilesets.isEmpty())
                    group.subgroups.append(subgroup);
            }
        }

        groups.append(group);
    }

    return true;
}

}

QList<TilesetEntry> TilesetGroup::allTilesets() const
{
    QList<TilesetEntry> entries = tilesets;

    for (const TilesetSubgroup& subgroup : subgroups)
        entries.append(subgroup.tilesets);

    return entries;
}

TilesetEntry* TilesetGroup::findTileset(const QString& fileName)
{
    for (TilesetEntry& entry : tilesets)
        if (entry.fileName == fileName)
            return &entry;

    for (TilesetSubgroup& subgroup : subgroups)
        for (TilesetEntry& entry : subgroup.tilesets)
            if (entry.fileName == fileName)
                return &entry;

    return nullptr;
}

TilesetList::TilesetList(FilesystemBase* fs) :
    fs(fs)
{
}

QList<TilesetGroup> TilesetList::groups() const
{
    QList<TilesetGroup> groups;

    QString basePath = SettingsManager::getInstance()->dataPath("tilesets.xml");
    parseTilesetsXml(basePath, groups);

    QString translatedPath = SettingsManager::getInstance()->getFilePath("tilesets.xml");
    if (!translatedPath.isEmpty() && translatedPath != basePath)
    {
        QList<TilesetGroup> translatedGroups;
        if (parseTilesetsXml(translatedPath, translatedGroups))
        {
            for (const TilesetGroup& translatedGroup : translatedGroups)
            {
                TilesetGroup* targetGroup = nullptr;
                for (TilesetGroup& group : groups)
                {
                    if (group.name == translatedGroup.name)
                    {
                        targetGroup = &group;
                        break;
                    }
                }

                if (targetGroup == nullptr)
                {
                    groups.append(translatedGroup);
                    continue;
                }

                for (const TilesetEntry& translatedEntry : translatedGroup.tilesets)
                {
                    TilesetEntry* entry = targetGroup->findTileset(translatedEntry.fileName);
                    if (entry)
                        entry->name = translatedEntry.name;
                    else
                        targetGroup->tilesets.append(translatedEntry);
                }

                for (const TilesetSubgroup& translatedSubgroup : translatedGroup.subgroups)
                {
                    TilesetSubgroup* targetSubgroup = nullptr;
                    for (TilesetSubgroup& subgroup : targetGroup->subgroups)
                    {
                        if (subgroup.name == translatedSubgroup.name)
                        {
                            targetSubgroup = &subgroup;
                            break;
                        }
                    }

                    if (targetSubgroup == nullptr)
                    {
                        targetGroup->subgroups.append(translatedSubgroup);
                        continue;
                    }

                    for (const TilesetEntry& translatedEntry : translatedSubgroup.tilesets)
                    {
                        TilesetEntry* entry = targetGroup->findTileset(translatedEntry.fileName);
                        if (entry)
                            entry->name = translatedEntry.name;
                        else
                            targetSubgroup->tilesets.append(translatedEntry);
                    }
                }
            }
        }
    }

    QList<QString> tilesetFiles;
    fs->directoryContents("/Unit", QDir::Files, tilesetFiles);

    QList<QString> existingFiles;
    for (const QString& file : tilesetFiles)
        existingFiles.append(stripExtension(file));

    QSet<QString> mapped;
    for (TilesetGroup& group : groups)
    {
        for (int i = group.tilesets.size() - 1; i >= 0; i--)
        {
            if (!existingFiles.contains(group.tilesets[i].fileName))
            {
                group.tilesets.removeAt(i);
                continue;
            }

            mapped.insert(group.tilesets[i].fileName);
        }

        for (TilesetSubgroup& subgroup : group.subgroups)
        {
            for (int i = subgroup.tilesets.size() - 1; i >= 0; i--)
            {
                if (!existingFiles.contains(subgroup.tilesets[i].fileName))
                {
                    subgroup.tilesets.removeAt(i);
                    continue;
                }

                mapped.insert(subgroup.tilesets[i].fileName);
            }
        }

        for (int i = group.subgroups.size() - 1; i >= 0; i--)
            if (group.subgroups[i].tilesets.isEmpty())
                group.subgroups.removeAt(i);
    }

    for (int i = groups.size() - 1; i >= 0; i--)
        if (groups[i].allTilesets().isEmpty())
            groups.removeAt(i);

    TilesetGroup otherGroup;
    otherGroup.name = QObject::tr("Other");

    for (const QString& fileName : existingFiles)
    {
        if (mapped.contains(fileName))
            continue;

        TilesetEntry entry;
        entry.fileName = fileName;
        entry.name = tilesetDisplayNameFromFile(fileName);
        otherGroup.tilesets.append(entry);
    }

    if (!otherGroup.tilesets.isEmpty())
        groups.append(otherGroup);

    return groups;
}

bool TilesetList::matchesSlot(const QString& fileName, int slot)
{
    switch (slot)
    {
    case 0: return fileName.startsWith("J_");
    case 1: return fileName.startsWith("M_");
    case 2: return fileName.startsWith("S1_");
    case 3: return fileName.startsWith("S2_");
    }

    return true;
}

int TilesetList::slotForFileName(const QString& fileName)
{
    for (int slot = 0; slot < 4; slot++)
    {
        if (matchesSlot(fileName, slot))
            return slot;
    }

    return -1;
}

QString TilesetList::stripExtension(const QString& fileName)
{
    int dot = fileName.lastIndexOf('.');
    if (dot > 0)
        return fileName.left(dot);

    return fileName;
}

QString TilesetList::displayName(const QString& fileName) const
{
    for (const TilesetGroup& group : groups())
        for (const TilesetEntry& entry : group.allTilesets())
            if (entry.fileName == fileName)
                return entry.name;

    return fileName;
}

QString TilesetList::groupName(const QString& fileName) const
{
    for (const TilesetGroup& group : groups())
        for (const TilesetEntry& entry : group.allTilesets())
            if (entry.fileName == fileName)
                return group.name;

    return QString();
}
