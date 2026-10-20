#include "tileseteditorwidgets.h"
#include "imagecache.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QToolTip>
#include <QScrollArea>
#include <QScrollBar>

TilesetPicker::TilesetPicker(Tileset *tileset, QWidget *parent) : QWidget(parent)
{
    this->selectedTileTL = -1;
    this->selectedTileBR = -1;
    this->selectedOvTile = -1;
    this->selectDrag = false;
    this->tileset = tileset;
    tilesetImage = QImage(420, 420, QImage::Format_RGBA8888);
    bgColor = Qt::white;
}

void TilesetPicker::setTilesetImage(QImage& tilesetImage)
{
    this->tilesetImage = tilesetImage;
    this->update();
}

static constexpr int PickerTileCount = 21;
static constexpr int PickerTilePitch = 21;
static constexpr int PickerContentPixels = PickerTileCount * PickerTilePitch;
static constexpr qreal PickerContentSize = PickerContentPixels;
static constexpr qreal PickerMinZoom = 0.25;
static constexpr qreal PickerMaxZoom = 8.0;

int TilesetPicker::contentPixels() const
{
    return qMax(1, qRound(PickerContentSize * zoom));
}

qreal TilesetPicker::pickerScale() const
{
    return zoom;
}

QPointF TilesetPicker::pickerOffset() const
{
    int content = contentPixels();
    return QPointF((width() - content) / 2.0, (height() - content) / 2.0);
}

QPoint TilesetPicker::pickerPos(const QPointF& pos) const
{
    qreal scale = pickerScale();
    if (scale <= 0.0)
        return QPoint(-1, -1);

    QPointF offset = pickerOffset();
    return QPoint(int((pos.x() - offset.x()) / scale), int((pos.y() - offset.y()) / scale));
}

QSize TilesetPicker::sizeHint() const
{
    int content = contentPixels();
    return QSize(content, content);
}

QScrollArea *TilesetPicker::scrollArea() const
{
    return viewportWidget ? qobject_cast<QScrollArea *>(viewportWidget->parentWidget()) : nullptr;
}

void TilesetPicker::attachViewport(QWidget *viewport)
{
    if (viewportWidget)
        viewportWidget->removeEventFilter(this);

    viewportWidget = viewport;

    if (viewportWidget)
        viewportWidget->installEventFilter(this);

    if (autoFit)
        fitToViewport();
}

void TilesetPicker::fitToViewport()
{
    QSize vp = viewportWidget ? viewportWidget->size() : size();
    if (vp.width() <= 0 || vp.height() <= 0)
        return;

    qreal fit = qMin(vp.width(), vp.height()) / PickerContentSize;
    if (qFuzzyCompare(fit, zoom))
        return;

    zoom = fit;
    setMinimumSize(contentPixels(), contentPixels());
    updateGeometry();
    update();
    emit zoomChanged(zoom);
}

void TilesetPicker::setZoom(qreal zoom)
{
    zoom = qBound(PickerMinZoom, zoom, PickerMaxZoom);
    if (qFuzzyCompare(zoom, this->zoom))
        return;

    this->zoom = zoom;
    autoFit = false;

    setMinimumSize(contentPixels(), contentPixels());
    updateGeometry();
    update();
    emit zoomChanged(zoom);
}

void TilesetPicker::zoomIn()
{
    setZoom(zoom * 1.25);
}

void TilesetPicker::zoomOut()
{
    setZoom(zoom / 1.25);
}

void TilesetPicker::resetZoom()
{
    autoFit = true;
    fitToViewport();

    if (QScrollArea *area = scrollArea())
    {
        area->horizontalScrollBar()->setValue(0);
        area->verticalScrollBar()->setValue(0);
    }

    update();
}

bool TilesetPicker::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == viewportWidget && evt->type() == QEvent::Resize && autoFit)
        fitToViewport();

    return QWidget::eventFilter(obj, evt);
}

void TilesetPicker::showEvent(QShowEvent *evt)
{
    QWidget::showEvent(evt);

    if (autoFit)
        fitToViewport();
}

void TilesetPicker::wheelEvent(QWheelEvent *evt)
{
    bool ctrlHeld = (evt->modifiers() & Qt::ControlModifier) ||
                    (QApplication::keyboardModifiers() & Qt::ControlModifier);

    if (!ctrlHeld)
    {
        evt->ignore();
        return;
    }

    int delta = evt->angleDelta().y();
    if (delta == 0)
        delta = evt->angleDelta().x();

    if (delta > 0)
        zoomIn();
    else if (delta < 0)
        zoomOut();

    evt->accept();
}

void TilesetPicker::updateSelectionBounds()
{
    if (selectedTiles.isEmpty())
    {
        selectedTileTL = -1;
        selectedTileBR = -1;
        return;
    }

    int minX = 21, minY = 21, maxX = -1, maxY = -1;
    for (int i : selectedTiles)
    {
        int x = i % 21;
        int y = i / 21;
        minX = qMin(minX, x);
        minY = qMin(minY, y);
        maxX = qMax(maxX, x);
        maxY = qMax(maxY, y);
    }

    selectedTileTL = minY * 21 + minX;
    selectedTileBR = maxY * 21 + maxX;
}

void TilesetPicker::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.fillRect(evt->rect(), palette().color(QPalette::Window));

    qreal scale = pickerScale();
    if (scale <= 0.0)
        return;

    painter.save();
    painter.translate(pickerOffset());
    painter.scale(scale, scale);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, scale < 1.0);

    painter.fillRect(QRect(0, 0, PickerContentPixels, PickerContentPixels), QBrush(bgColor, Qt::SolidPattern));

    for (int x = 0; x < 21; x++)
    {
        for (int y = 0; y < 21; y++)
        {
            painter.drawImage(x*21, y*21, tilesetImage, x*20 + x*4 + 2, y*20 + y*4 + 2, 20, 20, Qt::AutoColor);
        }
    }

    for (int i : selectedTiles)
        painter.fillRect(QRect(i%21*21, i/21*21, 20, 20), QBrush(QColor(160,222,255,150), Qt::SolidPattern));

    if (selectedOvTile != -1 && selectedTiles.size() == 1)
        painter.fillRect(QRect(selectedOvTile%21*21, selectedOvTile/21*21, 20, 20), QBrush(QColor(255,40,0,150), Qt::SolidPattern));

    if (drawCollision)
        paintCollisions(painter);

    painter.restore();
}

void TilesetPicker::paintCollisions(QPainter &painter)
{
    for(int row = 0; row < 21; row++)
    {
        for(int col = 0; col < 21; col++)
        {

            int tileIndex = row * 21 + col;

            if (tileIndex >= 256)
                break;

            int hitboxType = tileset->getBehaviorByte(tileIndex, 4);

            int mainBehavior = tileset->getBehaviorByte(tileIndex, 0);

            switch (mainBehavior)
            {
            case 9: // Partly Solid
            {
                if (hitboxType == 0)
                    break;

                int type = tileset->getBehaviorByte(tileIndex, 2);

                QPixmap psPixmap(ImageCache::getInstance()->get(TileCollision, QString("partly_solid.png")));
                QPixmap psOverlay = psPixmap.copy(20*type, 0, 20, 20);

                painter.drawPixmap(QRect(col*21, row*21, 20, 20), psOverlay);
                break;
            }
            case 11: // Slope
            {
                if (hitboxType == 0)
                    break;

                int slopeType = tileset->getBehaviorByte(tileIndex, 2);

                QPixmap slopesPixmap(ImageCache::getInstance()->get(TileCollision, QString("slope.png")));

                if (hitboxType == 33 || hitboxType == 34)
                    slopesPixmap = ImageCache::getInstance()->get(TileCollision, QString("solid_on_top_slope.png"));

                QPixmap slopeOverlay = slopesPixmap.copy(20*slopeType, 0, 20, 20);

                painter.drawPixmap(QRect(col*21, row*21, 20, 20), slopeOverlay);
                break;
            }
            case 12: // Upside-down Slope
            {
                if (hitboxType == 0)
                    break;

                int slopeType = tileset->getBehaviorByte(tileIndex, 2);

                QPixmap slopesPixmap(ImageCache::getInstance()->get(TileCollision, QString("slope.png")));

                if (hitboxType == 33 || hitboxType == 34)
                    slopesPixmap = ImageCache::getInstance()->get(TileCollision, QString("solid_on_top_slope.png"));

                QPixmap slopeOverlay = slopesPixmap.copy(20*slopeType, 0, 20, 20);

                // Mirror the overlay
                slopeOverlay = slopeOverlay.transformed(QTransform().scale(1, -1));

                painter.drawPixmap(QRect(col*21, row*21, 20, 20), slopeOverlay);
                break;
            }
            case 15: // Spikes
            {
                if (hitboxType == 0)
                    break;

                int slopeType = tileset->getBehaviorByte(tileIndex, 2);

                QPixmap spikesPixmap(ImageCache::getInstance()->get(TileCollision, QString("spike.png")));

                QPixmap spikeOverlay = spikesPixmap.copy(20*slopeType, 0, 20, 20);

                painter.drawPixmap(QRect(col*21, row*21, 20, 20), spikeOverlay);
                break;
            }
            default:
                switch (hitboxType)
                {
                case 0: // Beanstalk Stopper
                {
                    if (tileset->getBehaviorByte(tileIndex, 2) != 1)
                        break;

                    QPixmap bsPixmap(ImageCache::getInstance()->get(TileOverlay, QString("beanstalk_stopper.png")));
                    painter.drawPixmap(QRect(col*21, row*21, 20, 20), bsPixmap);

                    break;
                }
                case 1: // Solid
                {
                    painter.drawPixmap(QRect(col*21, row*21, 20, 20), ImageCache::getInstance()->get(TileCollision, QString("solid.png")));
                    break;
                }
                case 2: // Solid-on-Top
                {
                    painter.drawPixmap(QRect(col*21, row*21, 20, 20), ImageCache::getInstance()->get(TileCollision, QString("solid_on_top.png")));
                    break;
                }
                case 3: // Solid-on-Bottom
                {
                    painter.drawPixmap(QRect(col*21, row*21, 20, 20), ImageCache::getInstance()->get(TileCollision, QString("solid_on_bottom.png")));
                    break;
                }
                case 4: // Solid-on-Top and Bottom
                {
                    painter.drawPixmap(QRect(col*21, row*21, 20, 20), ImageCache::getInstance()->get(TileCollision, QString("solid_on_top.png")));
                    painter.drawPixmap(QRect(col*21, row*21, 20, 20), ImageCache::getInstance()->get(TileCollision, QString("solid_on_bottom.png")));
                    break;
                }
                default:
                    break;
                }
                break;
            }
        }
    }
}

void TilesetPicker::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::MiddleButton)
    {
        if (QScrollArea *area = scrollArea())
        {
            panning = true;
            panStart = evt->globalPosition().toPoint();
            panScrollStart = QPoint(area->horizontalScrollBar()->value(), area->verticalScrollBar()->value());
            setCursor(Qt::ClosedHandCursor);
            evt->accept();
        }
        return;
    }

    QPoint p = pickerPos(evt->position());
    bool onTile = p.x() >= 0 && p.y() >= 0 && p.x() < PickerContentPixels && p.y() < PickerContentPixels
                  && p.x() % 21 != 20 && p.y() % 21 != 20;

    if (evt->button() == Qt::LeftButton)
    {
        if (onTile)
        {
            int tempSelTile = p.x() / 21 + p.y() / 21 * 21;

            if (tempSelTile > 0xFF)
            {
                QToolTip::showText(evt->globalPos(), tr("You can only select the first 256 tiles due to limitations"), this, rect(), 5000);
                // TODO: Somehow this disappears after releasing the mouse button
                return;
            }

            bool ctrlHeld = evt->modifiers() & Qt::ControlModifier;

            if (ctrlHeld)
            {
                if (selectedTiles.contains(tempSelTile))
                    selectedTiles.remove(tempSelTile);
                else
                    selectedTiles.insert(tempSelTile);
            }
            else
            {
                selectedTiles.clear();
                selectedTiles.insert(tempSelTile);
            }

            dragBase = ctrlHeld ? selectedTiles : QSet<int>();
            initSelectTile = tempSelTile;
            selectDrag = true;

            updateSelectionBounds();

            emit selectedTileChanged(selectedTileTL, selectedTileBR);
            emit selectedTilesChanged(selectedTiles.values());

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
        else if (onTile)
        {
            selectedOvTile = p.x() / 21 + p.y() / 21 * 21;
            emit selectedOvTileChanged(selectedOvTile);
            update();
        }
    }
}

void TilesetPicker::mouseReleaseEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::MiddleButton)
    {
        if (panning)
        {
            panning = false;
            unsetCursor();
        }
        return;
    }

    if (evt->button() == Qt::LeftButton)
    {
        selectDrag = false;
    }
}

void TilesetPicker::mouseMoveEvent(QMouseEvent* evt)
{
    if (panning)
    {
        if (QScrollArea *area = scrollArea())
        {
            QPoint delta = evt->globalPosition().toPoint() - panStart;
            area->horizontalScrollBar()->setValue(panScrollStart.x() - delta.x());
            area->verticalScrollBar()->setValue(panScrollStart.y() - delta.y());
        }
        return;
    }

    if (!selectDrag)
        return;

    QPoint p = pickerPos(evt->position());

    if (p.x() < 0 || p.y() < 0 || p.x() >= PickerContentPixels || p.y() >= PickerContentPixels)
        return;

    if (p.x() % 21 == 20 || p.y() % 21 == 20)
        return;

    int tempSelTile = p.x() / 21 + p.y() / 21 * 21;

    int currX = tempSelTile % 21;
    int currY = tempSelTile / 21;

    int initX = initSelectTile % 21;
    int initY = initSelectTile / 21;

    int tmpSelectedTileTL = qMin(initX, currX) + qMin(initY, currY) * 21;
    int tmpSelectedTileBR = qMax(initX, currX) + qMax(initY, currY) * 21;

    if (tmpSelectedTileBR > 0xFF)
    {
        QToolTip::showText(evt->globalPos(), tr("You can only select the first 256 tiles due to limitations"), this, rect(), 2000);
        // TODO: Somehow this disappears after releasing the mouse button
        return;
    }

    selectedTiles = dragBase;
    for (int i = tmpSelectedTileTL; i <= tmpSelectedTileBR; i++)
        if (i % 21 >= tmpSelectedTileTL % 21 && i % 21 <= tmpSelectedTileBR % 21)
            selectedTiles.insert(i);

    updateSelectionBounds();

    emit selectedTileChanged(selectedTileTL, selectedTileBR);
    emit selectedTilesChanged(selectedTiles.values());

    repaint();
}

static constexpr qreal ObjectEditorFillFactor = 0.49;
static constexpr qreal ObjectEditorMinZoom = 0.25;
static constexpr qreal ObjectEditorMaxZoom = 8.0;

ObjectEditor::ObjectEditor(Tileset *tileset, QWidget *parent) : QWidget(parent)
{
    objNbr = -1;
    selX = -1;
    selY = -1;
    this->tileset = tileset;
    bgColor = Qt::white;
    showMarkers = true;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ObjectEditor::updateLayout()
{
    if (objNbr == -1)
        return;

    ObjectDef* obj = tileset->getObjectDef(objNbr);

    int natW = qMax(1, obj->width*20);
    int natH = qMax(1, obj->height*20);

    qreal fit = qMin((width() - 20) / qreal(natW), (height() - 20) / qreal(natH)) * ObjectEditorFillFactor;
    qreal scale = qMax(0.01, fit * objZoom);

    tileSize = qMax(1, qRound(20 * scale));
    currW = qRound(natW * scale);
    currH = qRound(natH * scale);

    int limitX = qMax(0, (currW - width()) / 2);
    int limitY = qMax(0, (currH - height()) / 2);
    objPan.setX(qBound(-limitX, objPan.x(), limitX));
    objPan.setY(qBound(-limitY, objPan.y(), limitY));

    currX = width()/2 - currW/2 + objPan.x();
    currY = height()/2 - currH/2 + objPan.y();
}

void ObjectEditor::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);

    painter.fillRect(evt->rect(), QBrush(Qt::gray));

    if (objNbr == -1)
        return;

    ObjectDef* obj = tileset->getObjectDef(objNbr);

    int natW = qMax(1, obj->width*20);
    int natH = qMax(1, obj->height*20);

    updateLayout();

    qreal scale = currH > 0 && obj->height > 0 ? currH / qreal(obj->height*20) : 1.0;

    if (selX >= obj->width || selY >= obj->height)
    {
        selX = -1;
        selY = -1;
        emit updateSelTileLabel(tr("None"));
    }

    QPixmap objPix(natW, natH);
    objPix.fill(bgColor);
    QPainter p(&objPix);

    TileGrid tileGrid;
    tileGrid.clear();
    tileGrid[0xFFFFFFFF] = 1;
    tileset->drawObject(p, tileGrid, objNbr, 0, 0, obj->width, obj->height, 1);

    p.end();

    painter.setRenderHint(QPainter::SmoothPixmapTransform, scale < 1.0);
    painter.drawPixmap(QRect(currX, currY, currW, currH), objPix);

    if (showMarkers)
    {
        if (obj->yRepeatStart != 255)
        {
            QPen blue(Qt::blue);
            QPen blueDashed(Qt::blue);
            blueDashed.setStyle(Qt::DashLine);

            painter.setPen(blue);
            painter.drawLine(currX, currY+obj->yRepeatStart*tileSize, currX-10, currY+obj->yRepeatStart*tileSize);
            painter.drawLine(currX, currY+obj->yRepeatEnd*tileSize, currX-10, currY+obj->yRepeatEnd*tileSize);
            painter.setPen(blueDashed);
            painter.drawLine(currX, currY+obj->yRepeatStart*tileSize, currX+currW, currY+obj->yRepeatStart*tileSize);
            painter.drawLine(currX, currY+obj->yRepeatEnd*tileSize, currX+currW, currY+obj->yRepeatEnd*tileSize);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(QRect(currX-62, currY+obj->yRepeatStart*tileSize-5,50,10), tr("Start"), Qt::AlignRight | Qt::AlignCenter);
            painter.drawText(QRect(currX-62, currY+obj->yRepeatEnd*tileSize-5,50,10), tr("End"), Qt::AlignRight | Qt::AlignCenter);
        }
        if (obj->rows[0].xRepeatStart != 255) // Render Horizontal Repeat Start/End Markers
        {
            QPen red(Qt::red);
            QPen redDashed(Qt::red);
            redDashed.setStyle(Qt::DashLine);

            painter.setPen(red);
            painter.drawLine(currX + obj->rows[0].xRepeatStart*tileSize, currY, currX + obj->rows[0].xRepeatStart*tileSize, currY-10);
            painter.drawLine(currX + obj->rows[0].xRepeatEnd*tileSize, currY, currX + obj->rows[0].xRepeatEnd*tileSize, currY-10);
            painter.setPen(redDashed);
            painter.drawLine(currX + obj->rows[0].xRepeatStart*tileSize, currY, currX + obj->rows[0].xRepeatStart*tileSize, currY+currH);
            painter.drawLine(currX + obj->rows[0].xRepeatEnd*tileSize, currY, currX + obj->rows[0].xRepeatEnd*tileSize, currY+currH);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(QRect(currX + obj->rows[0].xRepeatStart*tileSize-43, currY-20,50,10), tr("Start"), Qt::AlignRight | Qt::AlignTop);
            painter.drawText(QRect(currX + obj->rows[0].xRepeatEnd*tileSize-7, currY-20,50,10), tr("End"), Qt::AlignLeft | Qt::AlignTop);
        }
        if (obj->rows[0].slopeFlags != 0)
        {
            QPen black(Qt::black);
            QPen blackDashed(Qt::black);
            blackDashed.setStyle(Qt::DashLine);

            int y = tileSize;

            if ((obj->rows[0].slopeFlags & SlopeDirectionMask) >= SlopeDirectionUpsideDown)
                y = obj->height*tileSize - tileSize;

            painter.setPen(black);
            painter.drawLine(currX, currY+y, currX-65, currY+y);
            painter.setPen(blackDashed);
            painter.drawLine(currX, currY+y, currX+currW, currY+y);

            if ((obj->rows[0].slopeFlags & SlopeDirectionMask) < SlopeDirectionUpsideDown)
            {
                painter.drawText(QRect(currX-65, currY, 65, y), tr("Slope Top"), Qt::AlignLeft | Qt::AlignVCenter);
                painter.drawText(QRect(currX-65, currY+y, 65, currH-y), tr("Slope Base"), Qt::AlignLeft | Qt::AlignVCenter);
            }
            else
            {
                painter.drawText(QRect(currX-65, currY, 65, y), tr("Slope Base"), Qt::AlignLeft | Qt::AlignVCenter);
                painter.drawText(QRect(currX-65, currY+y, 65, currH-y), tr("Slope Bottom"), Qt::AlignLeft | Qt::AlignVCenter);
            }
        }
    }

    if (selX != -1 && selX != -1)
        painter.fillRect(currX + selX*tileSize, currY + selY*tileSize, tileSize, tileSize, QBrush(QColor(160,222,255,150)));
}

void ObjectEditor::mousePressEvent(QMouseEvent* evt)
{
    if (objNbr == -1)
        return;

    if (evt->button() == Qt::MiddleButton)
    {
        objPanning = true;
        objPanStart = evt->position().toPoint();
        objPanStartOffset = objPan;
        setCursor(Qt::ClosedHandCursor);
        evt->accept();
        return;
    }

    if (evt->x() >= currX && evt->x() < currX + currW && evt->y() >= currY && evt->y() < currY + currH)
    {
        selX = (evt->x()-currX) / tileSize;
        selY = (evt->y()-currY) / tileSize;

        if (evt->button() == Qt::RightButton)
        {
            if (evt->modifiers() == Qt::ControlModifier)
            {
                tileset->setData(objNbr, selX, selY, 1, 0x00);
                tileset->setData(objNbr, selX, selY, 2, 0x00);
            }
            else if (!paintTiles.isEmpty())
            {
                for (int i : paintTiles)
                {
                    int sX = i % 21 - paintBaseX;
                    int sY = i / 21 - paintBaseY;

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
        emit updateSelTileLabel(tr("(Repeat: 0x%1), (Tile: 0x%2), (Slot: 0x%3), (Item: 0x%4)").arg(tileset->getData(objNbr, selX, selY, 0), 1, 16).arg(tileset->getData(objNbr, selX, selY, 1), 1, 16).arg((tileset->getData(objNbr, selX, selY, 2) & 6) >> 1, 1, 16).arg((tileset->getData(objNbr, selX, selY, 2) & 120) >> 3, 1, 16));
    }
    else
    {
        selX = -1;
        selY = -1;
        emit updateSelTileLabel(tr("None"));
    }

    update();
}

void ObjectEditor::mouseMoveEvent(QMouseEvent* evt)
{
    if (!objPanning)
        return;

    objPan = objPanStartOffset + (evt->position().toPoint() - objPanStart);
    updateLayout();
    update();
}

void ObjectEditor::mouseReleaseEvent(QMouseEvent* evt)
{
    if (evt->button() == Qt::MiddleButton && objPanning)
    {
        objPanning = false;
        unsetCursor();
    }
}

void ObjectEditor::wheelEvent(QWheelEvent* evt)
{
    bool ctrlHeld = (evt->modifiers() & Qt::ControlModifier) ||
                    (QApplication::keyboardModifiers() & Qt::ControlModifier);

    if (!ctrlHeld)
    {
        evt->ignore();
        return;
    }

    int delta = evt->angleDelta().y();
    if (delta == 0)
        delta = evt->angleDelta().x();

    if (delta > 0)
        objZoom *= 1.25;
    else if (delta < 0)
        objZoom /= 1.25;

    objZoom = qBound(ObjectEditorMinZoom, objZoom, ObjectEditorMaxZoom);
    updateLayout();
    update();
    evt->accept();
}

void ObjectEditor::selectedObjectChanged(int objNbr)
{
    this->objNbr = objNbr;
    objPan = QPoint(0, 0);
    selX = -1;
    selY = -1;
    emit updateSelTileLabel(tr("None"));
    update();
}

void ObjectEditor::selectedPaintTilesChanged(QList<int> tiles)
{
    paintTiles = tiles;

    paintBaseX = 0;
    paintBaseY = 0;

    if (paintTiles.isEmpty())
        return;

    paintBaseX = paintTiles.first() % 21;
    paintBaseY = paintTiles.first() / 21;

    for (int i : paintTiles)
    {
        paintBaseX = qMin(paintBaseX, i % 21);
        paintBaseY = qMin(paintBaseY, i / 21);
    }
}
