#ifndef EDITIONMODE_H
#define EDITIONMODE_H

#include "level.h"

#include <QList>

class EditionMode : public QObject
{
    Q_OBJECT
public:
    EditionMode() {}
    virtual ~EditionMode() {}

    virtual void mouseDown(int, int, Qt::MouseButtons, Qt::KeyboardModifiers) {}
    virtual void mouseDrag(int, int, Qt::KeyboardModifiers) {}
    virtual void mouseMove(int, int) {}
    virtual void mouseUp(int, int) {}

    virtual void render(QPainter*) {}

    virtual void deleteSelection() {}

    Qt::CursorShape getActualCursor() { return actualCursor; }

protected:
    Level *level;
    QList<Object*> selectedObjects;
    Qt::CursorShape actualCursor;

signals:
    void deselected();
    void selectdObjectChanged(Object* obj);
    void updateEditors();
};


class ObjectsEditonMode: public EditionMode
{
    Q_OBJECT
public:
    ObjectsEditonMode(Level *level);
    ~ObjectsEditonMode() {}

    void mouseDown(int x, int y, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers);
    void mouseDrag(int x, int y, Qt::KeyboardModifiers modifieres);
    void mouseMove(int x, int y);
    void mouseUp(int x, int y);

    void render(QPainter *painter);

    void deleteSelection();

    void setDrawType(int drawType) { this->drawType = drawType; }
    void setObject(int selObject, int selTileset) { this->selObject = selObject; this->selTileset = selTileset; }
    void setLayer(int selLayer) { this->selLayer = selLayer; }
    void setSprite(int selSprite) { this->selSprite = selSprite; }

private:

    enum resizeType
    {
        ResizeNone,
        ResizeBegin,
        ResizeEnd
    };

    struct mouseAction
    {
        bool drag = false;
        resizeType vert = ResizeNone;
        resizeType hor = ResizeNone;
    };

    int dx, dy;
    int lx, ly;
    mouseAction mouseAct;

    bool selectionMode;
    bool selectionHasBGDats;

    int minBoundX, minBoundY;
    int maxBoundX, maxBoundY;
    int minSizeX, minSizeY;

    bool creatNewObject = false;
    Object* newObject;

    int drawType = -1;
    // -1: Invalid
    //  0: BGDat
    //  1: Sprite
    //  2: Entrance
    //  3: Zone
    //  4: Location
    //  etc

    int selTileset = 0;
    int selObject = 0;
    int selLayer = 0;
    int selSprite = 0;

    QList<Object*> getObjectsAtPos(int x1, int y1, int x2, int y2, bool firstOnly);
    mouseAction getActionAtPos(int x, int y);
    Qt::CursorShape getCursorAtPos(int x, int y);

    void updateSelectionBounds();

    void drawResizeKnob(int x, int y, QPainter *painter);

    // Last Deltas. Needed to prevent wiggling of dragging objects
    int xDeltaL;
    int yDeltaL;

    void checkEmits();
};

#endif // EDITIONMODE_H
