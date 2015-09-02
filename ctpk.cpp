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

#include "ctpk.h"

Ctpk::Ctpk(FileBase* file)
{
    this->file = file;

    file->open();

    file->seek(6);
    numTextures = file->read16();
    texSectionOffset = file->read32();
    texSectionSize = file->read32();
    hashSectionOffset = file->read32();
    infoSectionOffset = file->read32();

    file->close();
}

Ctpk::~Ctpk()
{
    delete file;
}


QImage* Ctpk::getTexture(quint32 num)
{
    const qint32 etc1_mod[8][2] =
    {
        {2, 8}, {5, 17}, {9, 29}, {13, 42},
        {18, 60}, {24, 80}, {33, 106}, {47, 183}
    };

    file->open();

    file->seek(0x20 + (0x20*num) + 4);
    quint32 size = file->read32();
    quint32 offset = file->read32();
    quint32 format = file->read32();
    quint16 width = file->read16();
    quint16 height = file->read16();
    // well whatever. We don't care about the remaining crap

    if (format != 0xD) // hax!!
    {
        qDebug("BAD TEXTURE FORMAT!!");
        return NULL;
    }

    file->seek(texSectionOffset + offset);

    //quint8* data = new quint8[width*height*4];
    //QImage* tex = new QImage(width, height, QImage::Format_ARGB32); // 0xAARRGGBB
    QImage* tex = new QImage(width, height, QImage::Format_RGBA8888); // R,G,B,A
    quint8* data = tex->scanLine(0);

    //for (quint32 t = 0; t < (size>>4); t++)
    for (quint32 y = 0; y < height; y += 8)
    {
        for (quint32 x = 0; x < width; x += 8)
        {
            for (quint32 ty = 0; ty < 8; ty += 4)
            {
                for (quint32 tx = 0; tx < 8; tx += 4)
                {
                   /* data[((y*width)+x)*4 + 0] = 0;
                    data[((y*width)+x)*4 + 1] = 255;
                    data[((y*width)+x)*4 + 2] = 255;
                    data[((y*width)+x)*4 + 3] = 255;*/

                    quint64 alpha = (quint64)file->read32();
                    alpha |= ((quint64)file->read32() << 32);

                    quint16 subindexes = file->read16();
                    quint16 negative = file->read16();
                    quint32 flags_col = file->read32();

                    for (quint32 sx = 0; sx < 4; sx++)
                    {
                        for (quint32 sy = 0; sy < 4; sy++)
                        {
                            qint32 r, g, b;
                            quint32 tsx;

                            // dirty
                            if (flags_col & 0x1)
                                tsx = sy;
                            else
                                tsx = sx;

                            if (flags_col & 0x2)
                            {
                                r = (flags_col & 0xF8000000) >> 24;
                                g = (flags_col & 0x00F80000) >> 16;
                                b = (flags_col & 0x0000F800) >> 8;
                                if (tsx >= 2)
                                {
                                    qint32 dr = (flags_col & 0x07000000) >> 21;
                                    qint32 dg = (flags_col & 0x00070000) >> 13;
                                    qint32 db = (flags_col & 0x00000700) >> 5;
                                    if (dr & 0x20) dr -= 0x40;
                                    if (dg & 0x20) dg -= 0x40;
                                    if (db & 0x20) db -= 0x40;
                                    r += dr;
                                    g += dg;
                                    b += db;
                                }

                                r |= (r >> 5);
                                g |= (g >> 5);
                                b |= (b >> 5);
                            }
                            else
                            {
                                if (tsx >= 2)
                                {
                                    r = (flags_col & 0x0F000000) >> 20;
                                    g = (flags_col & 0x000F0000) >> 12;
                                    b = (flags_col & 0x00000F00) >> 4;
                                }
                                else
                                {
                                    r = (flags_col & 0xF0000000) >> 24;
                                    g = (flags_col & 0x00F00000) >> 16;
                                    b = (flags_col & 0x0000F000) >> 8;
                                }

                                r |= (r >> 4);
                                g |= (g >> 4);
                                b |= (b >> 4);
                            }

                            quint32 mod_index = (flags_col >> (tsx>=2 ? 2:5)) & 0x7;
                            qint32 mod = etc1_mod[mod_index][subindexes & 0x1];
                            if (negative & 0x1) mod = -mod;

                            r = clampColor(r + mod);
                            g = clampColor(g + mod);
                            b = clampColor(b + mod);

                            // TODO: premultiplied alpha
                            quint8 a = alpha & 0xF;
                            a |= (a << 4);

                            quint32 dstpos = ((sy + ty + y) * width + sx + tx + x) * 4;
                            data[dstpos + 0] = r;
                            data[dstpos + 1] = g;
                            data[dstpos + 2] = b;
                            data[dstpos + 3] = a;

                            subindexes >>= 1;
                            negative >>= 1;
                            alpha >>= 4ULL;
                        }
                    }
                }
            }
        }
    }


    file->close();
    return tex;
}

