#ifndef OBJECTSEDITIONMODE_H
#define OBJECTSEDITIONMODE_H

#include "editionmode.h"

class ObjectsEditonMode: public EditionMode
{
    Q_OBJECT
public:
    ObjectsEditonMode(Level *level);
    ~ObjectsEditonMode() {}

    void mouseDown(int x, int y, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, QRect drawrect);
    void mouseDrag(int x, int y, Qt::KeyboardModifiers modifieres, QRect drawrect);
    void mouseMove(int x, int y);
    void mouseUp(int x, int y);

    void keyPress(QKeyEvent *) {}

    void render(QPainter *painter);
    void activate() {}
    void deactivate();

    void deleteSelection();
    void copy();
    void cut();
    void paste(int currX, int currY, int currW, int currH);
    void raise();
    void lower();
    void raiseLayer();
    void lowerLayer();

    void setLayerMask(quint8 mask) { layerMask = mask; qDebug("layer mask set to %d", layerMask); }
    void toggleSprites(bool toggle) { spriteInteraction = toggle; qDebug("spriteInteration is %d", toggle); }
    void togglePaths(bool toggle) { pathInteraction = toggle; qDebug("pathInteraction is %d", toggle); }

    void select(Object* obj);
    void selectAll();

    void selectZoneContents(Zone* zone);

    void setDrawType(int drawType) { this->drawType = drawType; }
    void setObject(int selObject, int selTileset);
    void setLayer(int selLayer) { this->selLayer = selLayer; }
    void setSprite(int selSprite);

private:

    QList<Object*> selectedObjects;

    enum resizeType
    {
        ResizeNone,
        ResizeBegin,
        ResizeEnd
    };

    enum pathNodeAdd
    {
        AddNone,
        AddBefore,
        AddAfter
    };

    struct mouseAction
    {
        bool drag = false;
        pathNodeAdd pAdd = AddNone;
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

    // Only to prevent resizing when cloning
    bool clone = false;

    int drawType = -1;
    // -1: Invalid
    //  0: BGDat
    //  1: Sprite
    //  2: Entrance
    //  3: Zone
    //  4: Location
    //  etc

    int selTileset = -1;
    int selObject = -1;
    int selLayer = 0;
    int selSprite = -1;

    QList<Object*> getObjectsAtPos(int x1, int y1, int x2, int y2, bool firstOnly, QRect drawrect);
    mouseAction getActionAtPos(int x, int y);
    Qt::CursorShape getCursorAtPos(int x, int y);

    QList<Object*> cloneObjects(QList<Object*> objects);

    void updateSelectionBounds();

    // Sorts the selection, so you things end up with correct z-coordinates
    void sortSelection();

    // Last Deltas. Needed to prevent wiggling of dragging objects
    int xDeltaL;
    int yDeltaL;

    void checkEmits();

    void drawResizeKnob(int x, int y, QPainter *painter);
    void drawPlus(QPainter* painter, int x, int y);

    quint8 layerMask;
    bool pathInteraction;
    bool spriteInteraction;

};

#endif // OBJECTSEDITIONMODE_H
