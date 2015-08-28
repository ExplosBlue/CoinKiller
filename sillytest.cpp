#include "sillytest.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QLine>
#include <QPen>





SillyTest::SillyTest(QWidget *parent) : QMainWindow(parent)
{
    resize(600, 600);
    this->setWindowTitle("silly test!");

    Crapomatic* crap = new Crapomatic(this);
    setCentralWidget(crap);
    crap->setMouseTracking(true);
}



Crapomatic::Crapomatic(QWidget* parent)
    : QWidget(parent)
{
    // derp!
}

void Crapomatic::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);

    QBrush casablanca(QColor(255, 255, 255));
    QBrush cursor(QColor(255, 0, 0));
    QBrush snape(QColor(0, 0, 255));

    painter.fillRect(0, 0, width()-1, height()-1, casablanca);


    painter.setPen(QColor(0, 0, 0));

    for (int x = 20; x < width(); x += 20)
        painter.drawLine(x, 0, x, height()-1);
    for (int y = 20; y < height(); y += 20)
        painter.drawLine(0, y, width()-1, y);


    // serious shit!!

    painter.setPen(QPen(snape, 2));

    for (int i = 0; i < lines.length(); i++)
    {
        painter.drawLine(lines.at(i));
    }


    if (drawing)
    {
        painter.setPen(QPen(cursor, 2));
        painter.drawLine(lastX, lastY, snapX, snapY);
    }

    painter.setPen(QColor(0, 0, 0));

    painter.setBrush(cursor);
    painter.drawRect(mouseX-3, mouseY-3, 7, 7);

    painter.setBrush(snape);
    painter.drawRect(snapX-3, snapY-3, 7, 7);
}

void Crapomatic::mouseMoveEvent(QMouseEvent *evt)
{
    mouseX = evt->x();
    mouseY = evt->y();

    snapX = ((mouseX + 10) / 20) * 20;
    snapY = ((mouseY + 10) / 20) * 20;

    update();
}

void Crapomatic::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() != Qt::LeftButton)
    {
        drawing = false;
        lines.append(QLine(lastX, lastY, lines.at(0).p1().x(), lines.at(0).p1().y()));
        return;
    }


    if (drawing)
    {
        // police!!

        lines.append(QLine(lastX, lastY, snapX, snapY));
    }


    lastX = snapX;
    lastY = snapY;
    drawing = true;

    update();
}

