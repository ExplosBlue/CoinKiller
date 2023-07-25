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

#ifndef LEVELVIEW_H
#define LEVELVIEW_H

#include <QWidget>
#include <QLayout>
#include <QList>

#include "filesystem.h"
#include "level.h"
#include "tileset.h"
#include "editmanager.h"

class LevelView : public QWidget
{
    Q_OBJECT
public:
    explicit LevelView(QWidget *parent, Level* level);
    ~LevelView();

    void setLayerMask(quint8 mask);
    void setZoom(float zoom) { this->zoom = zoom; setMinimumSize(4096*20*zoom, 4096*20*zoom); setMaximumSize(4096*20*zoom, 4096*20*zoom); update(); }
    void toggleGrid(bool toggle) { grid = toggle; update(); }
    void toggleCheckerboard(bool toggle) { checkerboard = toggle; update(); }
    void toggleRenderLiquids(bool toggle) { renderLiquids = toggle; update(); }
    void toggleRenderCameraLimits(bool toggle) { renderCameraLimits = toggle; update(); }
    void toggle3DOverlay(bool toggle) { render3DOverlay = toggle; update(); }
    void toggle2DTile(bool toggle) { render2DTile = toggle; update(); }
    void toggleSprites(bool toggle);
    void togglePaths(bool toggle);
    void toggleLocations(bool toggle);
    void toggleEntrances(bool toggle);
    qint8 saveLevel();
    void copy();
    void paste();
    void cut();
    void selectAll();
    void deleteSel();
    void raise();
    void lower();
    void raiseLayer();
    void lowerLayer();
    void setBackgroundColor(QColor bgcolor) {backgroundColor = bgcolor; update(); }

    EditManager* editManagerPtr() { return editManager; }

signals:
    void scrollTo(int x, int y);
    void updateMinimap(QRect drawrect);
    void updateMinimapBounds();
    void updateCtrlabel(QString text);

public slots:
    void selectObj(Object* obj);
    void selectZoneContents(Zone* zone);
    void screenshot(QRect rect);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

private:

    void paint(QPainter& painter, QRect rect, float zoomLvl, bool selections);

    Level* level;

    QRect drawrect;
    float zoom;

    int dragX;
    int dragY;

    EditManager* editManager;

    bool grid;
    bool checkerboard;
    bool renderLiquids;
    bool renderCameraLimits;
    bool renderSprites;
    bool renderPaths;
    bool renderLocations;
    bool render2DTile;
    bool render3DOverlay;
    bool renderEntrances;

    quint8 layerMask;

    // determines which tiles are already occupied
    TileGrid tileGrid;

    QColor backgroundColor;

};

#endif // LEVELVIEW_H
