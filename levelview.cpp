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

    QBrush mariobrosse(QColor(0, 255, 0));
    painter.setBrush(mariobrosse);

    /*QBrush mariobrosse(QColor(0, 255, 0));
    painter.setBrush(mariobrosse);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));*/

    //painter.drawImage(0, 0, *imgtest);
    for (int i = 0; i < objects.size(); i++)
    {
        const BgdatObject& obj = objects.at(i);

        painter.drawRect(QRect(obj.x*20, obj.y*20, obj.width*20 - 1, obj.height*20 - 1));
    }
}

