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

class LevelView : public QWidget
{
    Q_OBJECT
public:
    explicit LevelView(QWidget *parent, Level* level);

    void setLayerMask(quint8 mask) { layerMask = mask; update(); }
    void setZoom(float zoom) { this->zoom = zoom; update(); }

    void saveLevel();
    void copy();
    void paste();
    void cut();
    void deleteSel();

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent *) Q_DECL_OVERRIDE;

private:
    QList<Object*> selObjectsCheck(int x, int y, int w, int h, bool multiSelect);

    Level* level;

    float zoom;

    QList<Object*> selObjects;

    int editMode;
    // 0: Nothing
    // 1: Dragging
    // 2: Area Selection

    int dragX, dragY;

    QRect selArea;

    quint8 layerMask;

    // determines which tiles are already occupied
    TileGrid tileGrid;
};

#endif // LEVELVIEW_H
