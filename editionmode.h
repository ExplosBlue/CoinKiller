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

    void setDrawType(int drawType) { this->drawType = drawType; }
    void setObject(int selObject, int selTileset) { this->selObject = selObject; this->selTileset = selTileset; }
    void setLayer(int selLayer) { this->selLayer = selLayer; }

private:
    int dx, dy;     // Last Click Position
    int lx, ly;     // Last Mouse Position

    QList<Object*> selectedObjects;
    Object* newObject;
    bool paintingNewObject;

    int drawType;
    int selLayer;
    int selObject;
    int selTileset;
    int selSprite;
    // TODO: etc
};

#endif // EDITIONMODE_H
