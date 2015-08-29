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

#include "leveleditorwindow.h"
#include "levelview.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QPaintEvent>

LevelView::LevelView(QWidget *parent, SarcFilesystem* level) : QWidget(parent)
{
    //imgtest = crapo;
    levelArchive = level;

    FileBase* bgdat = level->openFile("/course/course1_bgdatL1.bin");
    bgdat->open();
    bgdat->seek(0);
    for (;;)
    {
        quint16 id = bgdat->read16();
        if (id == 0xFFFF) break;

        BgdatObject* obj = new BgdatObject;
        obj->id = id;
        obj->x = bgdat->read16();
        obj->y = bgdat->read16();
        obj->width = bgdat->read16();
        obj->height = bgdat->read16();
        objects.append(*obj);

        bgdat->skip(6);
    }
    bgdat->close();
}


void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);

    qDebug("draw %d,%d %d,%d", evt->rect().left(), evt->rect().top(), evt->rect().right(), evt->rect().bottom());

    QBrush mariobrosse(QColor(0, 255, 0));
    QBrush luigibrosse(QColor(0, 0, 255));
    QBrush peachbrosse(QColor(255, 0, 0));
    QBrush yoshibrosse(QColor(255, 255, 0));
    QBrush wariobrosse(QColor(255, 0, 255));
    painter.setBrush(mariobrosse);

    /*QBrush mariobrosse(QColor(0, 255, 0));
    painter.setBrush(mariobrosse);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));*/

    //painter.drawImage(0, 0, *imgtest);
    for (int i = 0; i < objects.size(); i++)
    {
        const BgdatObject& obj = objects.at(i);

        switch (obj.id & 0xF000)
        {
        case 0x0000: painter.setBrush(mariobrosse); break;
        case 0x1000: painter.setBrush(luigibrosse); break;
        case 0x2000: painter.setBrush(peachbrosse); break;
        case 0x3000: painter.setBrush(yoshibrosse); break;
        default: painter.setBrush(wariobrosse); break;
        }

        painter.drawRect(QRect(obj.x*20, obj.y*20, obj.width*20 - 1, obj.height*20 - 1));
    }
}

