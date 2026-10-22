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
#include <QUndoStack>
#include <QWheelEvent>
#include <QApplication>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QNativeGestureEvent>
#include <QPoint>
#include <QPointF>
#include <QElapsedTimer>

#include "filesystem/filesystem.h"
#include "level.h"
#include "tileset.h"
#include "editmanager.h"

class QTimer;

class LevelView : public QWidget
{
    Q_OBJECT
public:
    explicit LevelView(QWidget *parent, Level *level, QUndoStack *undoStack);
    ~LevelView();

    static constexpr float MinZoom = 0.3f;
    static constexpr float MaxZoom = 4.0f;

    float getZoom() const { return zoom; }
    void setZoom(float zoom);

    void zoomTo(float newZoom);
    void zoomBy(int steps);
    static float steppedZoom(float from, int steps);

    void toggleGrid(bool toggle) { grid = toggle; update(); }
    void toggleCheckerboard(bool toggle) { checkerboard = toggle; update(); }
    void toggleRenderLiquids(bool toggle) { renderLiquids = toggle; update(); }
    void toggleRenderControllers(bool toggle) { renderControllers = toggle; update(); }
    void toggleRenderCameraLimits(bool toggle) { renderCameraLimits = toggle; update(); }
    void toggle3DOverlay(bool toggle) { render3DOverlay = toggle; invalidateRenderCaches(); update(); }
    void toggle2DTile(bool toggle) { render2DTile = toggle; invalidateRenderCaches(); update(); }

    qint8 saveLevel();
    void copy();
    void paste();
    void cut();
    void selectAll();
    void deselect();
    void deleteSel();
    void raise();
    void lower();
    void raiseLayer();
    void lowerLayer();
    void setBackgroundColor(QColor bgcolor) {backgroundColor = bgcolor; update(); }

    EditManager* editManagerPtr() { return editManager; }

signals:
    void scrollTo(float x, float y);
    void updateMinimap(QRect drawrect);
    void updateMinimapBounds();
    void updateCtrlabel(QString text);

public slots:
    void selectObj(Object* obj);
    void selectZoneContents(Zone* zone);
    void screenshot(QRect rect);

    void requestUpdate();
    void onLevelScrolled();

protected:
    bool event(QEvent *) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;

private:

    void paint(QPainter& painter, QRect rect, QRect viewport, float zoomLvl, bool selections);
    void handleWheelZoom(QWheelEvent* evt);
    void handleWheelScroll(QWheelEvent* evt);
    void handlePinchZoom(QPinchGesture* gesture);
    void handleNativeZoom(QNativeGestureEvent* evt);
    void invalidateRenderCaches();
    QRect visibleRect() const;
    QList<QRect> pinnedZoneLabelRects() const;
    QPointF viewportTopLeft() const;
    void scrollToPos(const QPointF& pos);
    void startAnimationTimer(QTimer* timer);
    void stopScrollAnimations();
    QPointF clampScroll(const QPointF& pos) const;
    void startSmoothScroll(float dx, float dy);
    void smoothScrollStep();
    void startMomentum();
    void startInertia(const QPointF& velocity, qreal decel, qreal damping);
    void inertiaStep();
    void startSmoothZoom(float targetZoom, const QPointF& focalPoint);
    void finishSmoothZoom();
    void smoothZoomStep();

    void applyZoom(float newZoom, const QPointF& focalPoint);
    QPointF viewportCenter() const;

    Level* level;

    QUndoStack *undoStack;

    QRect drawrect;
    QList<QRect> lastPinnedLabelRects;
    float zoom;

    QPointF panLastGlobal;

    EditManager* editManager;

    bool grid;
    bool checkerboard;
    bool renderLiquids;
    bool renderControllers;
    bool renderCameraLimits;
    bool render2DTile;
    bool render3DOverlay;

    // determines which tiles are already occupied
    TileGrid tileGrid;

    QColor backgroundColor;

    bool updatePending = false;

    QPointF panTarget;

    QTimer* smoothScrollTimer;
    QPointF smoothScrollTarget;
    QPointF lastSmoothScrollPos;

    QTimer* inertiaTimer;
    QPointF inertiaVelocity;
    QPointF inertiaPos;
    qreal inertiaDecel;
    qreal inertiaDamping;
    QElapsedTimer inertiaClock;

    QPointF panFastVelocity;
    QPointF panSlowVelocity;
    QElapsedTimer panClock;

    QTimer* scrollMomentumTimer;
    QPointF scrollVelocity;
    bool scrollFromTouchpad = false;
    QElapsedTimer scrollEventClock;

    QTimer* smoothZoomTimer;
    float smoothZoomTarget;
    QPointF smoothZoomFocalOffset;

    int scrollAccumulator;
};

#endif // LEVELVIEW_H
