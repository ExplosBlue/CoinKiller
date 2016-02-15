#ifndef EDITIONMODE_H
#define EDITIONMODE_H

#include "level.h"
#include "unitsconvert.h"

#include <QMouseEvent>

class EditionMode
{
public:
    EditionMode() {}
    virtual ~EditionMode() {}
    virtual void mousePressEvent(QMouseEvent *) {}
    virtual void mouseReleaseEvent(QMouseEvent *) {}
    virtual void mouseMoveEvent(QMouseEvent *) {}
    virtual void deleteAction() {}

protected:
    Level* level;

};

class ObjectsEditonMode: public EditionMode
{
public:
    ObjectsEditonMode() {}
    ObjectsEditonMode(Level* level);
    ~ObjectsEditonMode() {}
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
    void deleteAction();
    void render(QPainter *painter);

    void setZoom(float zoom) { this->zoom = zoom; }

    void setDrawType(int drawType) { this->drawType = drawType; }
    void setObject(int selObject, int selTileset) { this->selObject = selObject; this->selTileset = selTileset; }
    void setLayer(int selLayer) { this->selLayer = selLayer; }
    void setSprite(int selSprite) { this->selSprite = selSprite; }

private:
    int dx, dy;     // Last Click Position
    int lx, ly;     // Last Mouse Position

    QList<Object*> selectedObjects;
    Object* newObject;
    bool paintingNewObject;
    bool selectionMode;
    bool selectionHasBGDats;
    bool dragMode;
    int minSelX, minSelY, maxSelX, maxSelY;
    float zoom;

    void findSelectedObjects(int x1, int y1, int x2, int y2, bool firstOnly, bool clearSelection);
    void setDrags();

    int drawType;
    int selLayer;
    int selObject;
    int selTileset;
    int selSprite;
    // TODO: etc
};

#endif // EDITIONMODE_H
