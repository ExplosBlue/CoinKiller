#include "tileseteditorwidgets.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>

TilesetPicker::TilesetPicker(QWidget *parent) : QWidget(parent)
{
    this->selectedTile = -1;
    tilesetImage = new QImage(420, 420, QImage::Format_RGBA8888);
    bgColor = Qt::white;
}

void TilesetPicker::setTilesetImage(QImage *tilesetImage)
{
    this->tilesetImage = tilesetImage;
}

void TilesetPicker::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.fillRect(evt->rect(), QBrush(bgColor, Qt::SolidPattern));

    for (int x = 0; x < 21; x++)
    {
        for (int y = 0; y < 21; y++)
        {
            painter.drawImage(x*21, y*21, *tilesetImage, x*20 + x*4 + 2, y*20 + y*4 + 2, 20, 20, Qt::AutoColor);
        }
    }

    if (selectedTile != -1)
        painter.fillRect(QRect(selectedTile%21*21, selectedTile/21*21, 20, 20), QBrush(QColor(160,222,255,150), Qt::SolidPattern));
}

void TilesetPicker::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    if (evt->x() % 21 != 20 && evt->x() % 21 != 20)
    {
        selectedTile = evt->x() / 21 + evt->y() / 21 * 21;

        emit selectedTileChanged(selectedTile);

        update();
    }
}


ObjectEditor::ObjectEditor(Tileset *tileset, QWidget *parent) : QWidget(parent)
{
    objNbr = -1;
    paintTileNbr = -1;
    selX = -1;
    selY = -1;
    this->tileset = tileset;
    bgColor = Qt::white;
}

void ObjectEditor::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);

    painter.fillRect(evt->rect(), QBrush(Qt::gray));

    if (objNbr == -1)
        return;

    ObjectDef* obj = tileset->getObjectDef(objNbr);

    currX = evt->rect().width()/2 - obj->width*10;
    currY = evt->rect().height()/2 - obj->height*10;
    currW = obj->width*20;
    currH = obj->height*20;

    QPixmap objPix(currW, currH);
    objPix.fill(bgColor);
    QPainter p(&objPix);

    TileGrid tileGrid;
    tileGrid.clear();
    tileGrid[0xFFFFFFFF] = 1;
    tileset->drawObject(p, tileGrid, objNbr, 0, 0, obj->width, obj->height, 1);

    p.end();

    painter.drawPixmap(currX, currY, currW, currH, objPix);

    if (selX != -1 && selX != -1)
        painter.fillRect(currX + selX*20, currY + selY*20, 20, 20, QBrush(QColor(160,222,255,150)));
}

void ObjectEditor::mousePressEvent(QMouseEvent* evt)
{
    if (objNbr == -1)
        return;

    if (evt->x() >= currX && evt->x() < currX + currW && evt->y() >= currY && evt->y() < currY + currH)
    {
        selX = (evt->x()-currX) / 20;
        selY = (evt->y()-currY) / 20;

        if (evt->button() == Qt::RightButton)
        {
            if (evt->modifiers() == Qt::ControlModifier)
            {
                tileset->setData(objNbr, selX, selY, 1, 0x00);
                tileset->setData(objNbr, selX, selY, 2, 0x00);
            }
            else if (paintTileNbr != -1)
                tileset->setData(objNbr, selX, selY, 1, paintTileNbr);

            emit tilesetChanged();
        }
        emit updateSelTileLabel(QString("Tile Data: (Repeat: 0x%1), (Tile: 0x%2), (Slot: 0x%3)").arg(tileset->getData(objNbr, selX, selY, 0), 1, 16).arg(tileset->getData(objNbr, selX, selY, 1), 1, 16).arg(tileset->getData(objNbr, selX, selY, 2), 1, 16));
    }
    else
    {
        selX = -1;
        selY = -1;
        emit updateSelTileLabel(QString("Tile Data: None"));
    }

    update();
}

void ObjectEditor::selectedObjectChanged(int objNbr)
{
    this->objNbr = objNbr;
    selX = -1;
    selY = -1;
    emit updateSelTileLabel(QString("Tile Data: None"));
    update();
}

void ObjectEditor::selectedPaintTileChanged(int paintTileNbr)
{
    this->paintTileNbr = paintTileNbr;
}
