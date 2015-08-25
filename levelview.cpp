#include "leveleditorwindow.h"
#include "levelview.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QPaintEvent>

LevelView::LevelView(QWidget *parent, QImage* crapo) : QWidget(parent)
{
    imgtest = crapo;
}


void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);

    /*QBrush mariobrosse(QColor(0, 255, 0));
    painter.setBrush(mariobrosse);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));*/

    painter.drawImage(0, 0, *imgtest);
}

