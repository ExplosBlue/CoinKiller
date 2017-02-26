#include "tileseteditorwidgets.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QToolTip>

TilesetPicker::TilesetPicker(QWidget *parent) : QWidget(parent)
{
    this->selectedTileTL = -1;
    this->selectedTileBR = -1;
    this->selectedOvTile = -1;
    this->selectDrag = false;
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

    if (selectedTileTL != -1 || selectedTileBR != -1)
        for (int i = selectedTileTL; i <= selectedTileBR; i++)
            if (i % 21 >= selectedTileTL % 21  && i % 21 <= selectedTileBR % 21)
                painter.fillRect(QRect(i%21*21, i/21*21, 20, 20), QBrush(QColor(160,222,255,150), Qt::SolidPattern));

    if (selectedOvTile != -1 && selectedTileTL == selectedTileBR)
        painter.fillRect(QRect(selectedOvTile%21*21, selectedOvTile/21*21, 20, 20), QBrush(QColor(255,40,0,150), Qt::SolidPattern));
}

void TilesetPicker::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        if (evt->x() % 21 != 20 && evt->x() % 21 != 20)
        {
            int tempSelTile = evt->x() / 21 + evt->y() / 21 * 21;

            if (tempSelTile > 0xFF)
            {
                QToolTip::showText(evt->globalPos(), "You can only select the first 256 tiles due to limitations", this, rect(), 5000);
                // TODO: Somehow this disappears after releasing the mouse button
                return;
            }

            selectedTileTL = tempSelTile;
            selectedTileBR = tempSelTile;
            initSelectTile = tempSelTile;
            selectDrag = true;

            emit selectedTileChanged(selectedTileTL, selectedTileBR);

            update();
        }
    }
    else if (evt->button() == Qt::RightButton && selectedTileTL != -1 && selectedTileBR == selectedTileTL)
    {
        if (evt->modifiers() == Qt::ControlModifier)
        {
            selectedOvTile = -1;
            emit selectedOvTileChanged(-1);
            update();
        }
        else if (evt->x() % 21 != 20 && evt->x() % 21 != 20)
        {
            selectedOvTile = evt->x() / 21 + evt->y() / 21 * 21;
            emit selectedOvTileChanged(selectedOvTile);
            update();
        }
    }
}

void TilesetPicker::mouseReleaseEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::LeftButton)
    {
        selectDrag = false;
    }
}

void TilesetPicker::mouseMoveEvent(QMouseEvent* evt)
{
    if (!selectDrag)
        return;

    if (!this->rect().contains(evt->pos()))
        return;

    if (evt->x() % 21 == 20 || evt->x() % 21 == 20)
        return;

    int tempSelTile = evt->x() / 21 + evt->y() / 21 * 21;

    int currX = tempSelTile % 21;
    int currY = tempSelTile / 21;

    int initX = initSelectTile % 21;
    int initY = initSelectTile / 21;

    int tmpSelectedTileTL = qMin(initX, currX) + qMin(initY, currY) * 21;
    int tmpSelectedTileBR = qMax(initX, currX) + qMax(initY, currY) * 21;

    if (tmpSelectedTileBR > 0xFF)
    {
        QToolTip::showText(evt->globalPos(), "You can only select the first 256 tiles due to limitations", this, rect(), 2000);
        // TODO: Somehow this disappears after releasing the mouse button
        return;
    }

    selectedTileTL = tmpSelectedTileTL;
    selectedTileBR = tmpSelectedTileBR;

    emit selectedTileChanged(selectedTileTL, selectedTileBR);

    repaint();
}

ObjectEditor::ObjectEditor(Tileset *tileset, QWidget *parent) : QWidget(parent)
{
    objNbr = -1;
    paintTileNbrTL = -1;
    paintTileNbrBR = -1;
    selX = -1;
    selY = -1;
    this->tileset = tileset;
    bgColor = Qt::white;
    showMarkers = true;
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

    if (selX >= obj->width || selY >= obj->height)
    {
        selX = -1;
        selY = -1;
        emit updateSelTileLabel(SettingsManager::getInstance()->getTranslation("TilesetEditor", "noneData"));
    }

    QPixmap objPix(currW, currH);
    objPix.fill(bgColor);
    QPainter p(&objPix);

    TileGrid tileGrid;
    tileGrid.clear();
    tileGrid[0xFFFFFFFF] = 1;
    tileset->drawObject(p, tileGrid, objNbr, 0, 0, obj->width, obj->height, 1);

    p.end();

    painter.drawPixmap(currX, currY, currW, currH, objPix);

    if (showMarkers)
    {
        if (obj->yRepeatStart != 255)
        {
            QPen blue(Qt::blue);
            QPen blueDashed(Qt::blue);
            blueDashed.setStyle(Qt::DashLine);

            painter.setPen(blue);
            painter.drawLine(currX, currY+obj->yRepeatStart*20, currX-10, currY+obj->yRepeatStart*20);
            painter.drawLine(currX, currY+obj->yRepeatEnd*20, currX-10, currY+obj->yRepeatEnd*20);
            painter.setPen(blueDashed);
            painter.drawLine(currX, currY+obj->yRepeatStart*20, currX+currW, currY+obj->yRepeatStart*20);
            painter.drawLine(currX, currY+obj->yRepeatEnd*20, currX+currW, currY+obj->yRepeatEnd*20);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(QRect(currX-62, currY+obj->yRepeatStart*20-5,50,10), "Start", Qt::AlignRight | Qt::AlignCenter);
            painter.drawText(QRect(currX-62, currY+obj->yRepeatEnd*20-5,50,10), "End", Qt::AlignRight | Qt::AlignCenter);
        }
        if (obj->rows[0].xRepeatStart != 255) // Render Horizontal Repeat Start/End Markers
        {
            QPen red(Qt::red);
            QPen redDashed(Qt::red);
            redDashed.setStyle(Qt::DashLine);

            painter.setPen(red);
            painter.drawLine(currX + obj->rows[0].xRepeatStart*20, currY, currX + obj->rows[0].xRepeatStart*20, currY-10);
            painter.drawLine(currX + obj->rows[0].xRepeatEnd*20, currY, currX + obj->rows[0].xRepeatEnd*20, currY-10);
            painter.setPen(redDashed);
            painter.drawLine(currX + obj->rows[0].xRepeatStart*20, currY, currX + obj->rows[0].xRepeatStart*20, currY+currH);
            painter.drawLine(currX + obj->rows[0].xRepeatEnd*20, currY, currX + obj->rows[0].xRepeatEnd*20, currY+currH);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(QRect(currX + obj->rows[0].xRepeatStart*20-43, currY-20,50,10), "Start", Qt::AlignRight | Qt::AlignTop);
            painter.drawText(QRect(currX + obj->rows[0].xRepeatEnd*20-7, currY-20,50,10), "End", Qt::AlignLeft | Qt::AlignTop);
        }
        if (obj->rows[0].slopeFlags != 0)
        {
            QPen black(Qt::black);
            QPen blackDashed(Qt::black);
            blackDashed.setStyle(Qt::DashLine);

            int y = 20;

            if ((obj->rows[0].slopeFlags & 15) > 1)
                y = obj->height*20 - 20;

            painter.setPen(black);
            painter.drawLine(currX, currY+y, currX-65, currY+y);
            painter.setPen(blackDashed);
            painter.drawLine(currX, currY+y, currX+currW, currY+y);

            if ((obj->rows[0].slopeFlags & 15) < 2)
            {
                painter.drawText(QRect(currX-65, currY, 65, y), "Slope Top", Qt::AlignLeft | Qt::AlignVCenter);
                painter.drawText(QRect(currX-65, currY+y, 65, currH-y), "Slope Base", Qt::AlignLeft | Qt::AlignVCenter);
            }
            else
            {
                painter.drawText(QRect(currX-65, currY, 65, y), "Slope Bottom", Qt::AlignLeft | Qt::AlignVCenter);
                painter.drawText(QRect(currX-65, currY+y, 65, currH-y), "Slope Base", Qt::AlignLeft | Qt::AlignVCenter);
            }
        }
    }

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
            else if (paintTileNbrTL != -1)
            {
                for (int i = paintTileNbrTL; i <= paintTileNbrBR; i++)
                {
                    int iX = i % 21;
                    int iY = i / 21;

                    if (iX < paintTileNbrTL % 21 || iX > paintTileNbrBR % 21)
                        continue;

                    if (iY < paintTileNbrTL / 21 || iY > paintTileNbrBR / 21)
                        continue;

                    int sX = iX - paintTileNbrTL % 21;
                    int sY = iY - paintTileNbrTL / 21;

                    if (selX + sX > tileset->getObjectDef(objNbr)->width-1)
                        continue;

                    if (selY + sY > tileset->getObjectDef(objNbr)->height-1)
                        continue;



                    tileset->setData(objNbr, selX + sX, selY + sY, 1, i);
                    tileset->setData(objNbr, selX + sX, selY + sY, 2, tileset->getSlot() << 1);
                }
            }

            emit tilesetChanged();
        }
        emit updateSelTileLabel(QString("(Repeat: 0x%1), (Tile: 0x%2), (Slot: 0x%3), (Item: 0x%4)").arg(tileset->getData(objNbr, selX, selY, 0), 1, 16).arg(tileset->getData(objNbr, selX, selY, 1), 1, 16).arg((tileset->getData(objNbr, selX, selY, 2) & 6) >> 1, 1, 16).arg((tileset->getData(objNbr, selX, selY, 2) & 120) >> 3, 1, 16));
    }
    else
    {
        selX = -1;
        selY = -1;
        emit updateSelTileLabel(SettingsManager::getInstance()->getTranslation("TilesetEditor", "noneData"));
    }

    update();
}

void ObjectEditor::selectedObjectChanged(int objNbr)
{
    this->objNbr = objNbr;
    selX = -1;
    selY = -1;
    emit updateSelTileLabel(SettingsManager::getInstance()->getTranslation("TilesetEditor", "noneData"));
    update();
}

void ObjectEditor::selectedPaintTileChanged(int tileTL, int tileBR)
{
    this->paintTileNbrTL = tileTL;
    this->paintTileNbrBR = tileBR;
}
