#ifndef TILESETEDITORWIDGETS_H
#define TILESETEDITORWIDGETS_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGridLayout>
#include <QSpinBox>
#include <QSet>

#include "tileset.h"
#include "settingsmanager.h"

class QScrollArea;

// Tileset Picker
class TilesetPicker : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetPicker(Tileset *tileset, QWidget *parent);
    void setTilesetImage(QImage& image);
    void setBGColor(QColor bgColor) { this->bgColor = bgColor; update(); }
    void setOvTile(int ovTile) { this->selectedOvTile = ovTile; update(); }
    void toggleCollisionView(bool toggle) { drawCollision = toggle; }
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void attachViewport(QWidget *viewport);
    void setZoom(qreal zoom);
    qreal zoomLevel() const { return zoom; }
    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void selectedTileChanged(int tileTL, int tileBR);
    void selectedTilesChanged(QList<int> tiles);
    void selectedOvTileChanged(int ovTile);
    void zoomChanged(qreal zoom);

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

private:
    void paintCollisions(QPainter& painter);
    void fitToViewport();
    void updateSelectionBounds();

    QScrollArea *scrollArea() const;

    qreal pickerScale() const;
    QPointF pickerOffset() const;
    QPoint pickerPos(const QPointF& pos) const;
    int contentPixels() const;

    Tileset *tileset;

    QImage tilesetImage;
    QColor bgColor;
    int selectedTileTL;
    int selectedTileBR;
    int selectedOvTile;

    QSet<int> selectedTiles;
    QSet<int> dragBase;

    bool selectDrag;
    int initSelectTile;

    bool drawCollision = false;

    qreal zoom = 1.0;
    bool autoFit = true;
    QWidget *viewportWidget = nullptr;

    bool panning = false;
    QPoint panStart;
    QPoint panScrollStart;
};


// Object Editor
class ObjectEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectEditor(Tileset *tileset, QWidget *parent);
    void setBGColor(QColor bgColor) { this->bgColor = bgColor; update(); }
    void setMarkers(bool value) { this->showMarkers = value; update(); }

signals:
    void updateSelTileLabel(QString text);
    void tilesetChanged();

public slots:
    void selectedObjectChanged(int objNbr);
    void selectedPaintTilesChanged(QList<int> tiles);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;

private:
    void updateLayout();

    Tileset *tileset;

    int objNbr;
    QColor bgColor;
    bool showMarkers;

    int currX, currY, currW, currH;
    int tileSize = 20;
    int selX, selY;
    QList<int> paintTiles;
    int paintBaseX = 0;
    int paintBaseY = 0;

    qreal objZoom = 1.0;
    QPoint objPan;
    bool objPanning = false;
    QPoint objPanStart;
    QPoint objPanStartOffset;
};

#endif // TILESETEDITORWIDGETS_H
