#ifndef TILESETEDITORWIDGETS_H
#define TILESETEDITORWIDGETS_H

#include <QWidget>
#include <QMouseEvent>
#include <QGridLayout>
#include <QSpinBox>

#include "tileset.h"
#include "settingsmanager.h"

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

signals:
    void selectedTileChanged(int tileTL, int tileBR);
    void selectedOvTileChanged(int ovTile);

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    void paintCollisions(QPainter& painter);

    Tileset *tileset;

    QImage tilesetImage;
    QColor bgColor;
    int selectedTileTL;
    int selectedTileBR;
    int selectedOvTile;

    bool selectDrag;
    int initSelectTile;

    bool drawCollision = false;
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
    void selectedPaintTileChanged(int tileTL, int tileBR);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    Tileset *tileset;

    int objNbr;
    QColor bgColor;
    bool showMarkers;

    int currX, currY, currW, currH;
    int selX, selY;
    int paintTileNbrTL;
    int paintTileNbrBR;
};

#endif // TILESETEDITORWIDGETS_H
