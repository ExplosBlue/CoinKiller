#ifndef EDITMANAGER_H
#define EDITMANAGER_H

#include "level.h"
#include "layermask.h"

#include <QList>
#include <QKeyEvent>
#include <QUndoStack>

class EditManager : public QObject
{
    Q_OBJECT
public:
    EditManager(Level *level, QUndoStack *undoStack);
    ~EditManager() {}

    void mouseDown(int x, int y, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, QRect drawrect);
    void mouseDrag(int x, int y, Qt::KeyboardModifiers modifieres, QRect drawrect);
    void mouseMove(int x, int y);
    void mouseUp(int x, int y);

    void keyPress(QKeyEvent *) {}

    void render(QPainter *painter);

    void deleteSelection();
    void copy();
    void cut();
    void paste(int currX, int currY, int currW, int currH);
    void raise();
    void lower();
    void raiseLayer();
    void lowerLayer();

    void setLayerMask(LAYER_MASK layer, bool state);
    const quint8 getLayerMask() { return layerMask; }

    void setSpriteInteraction(bool state);
    const bool getSpriteInteraction() { return spriteInteraction; }

    void setEntranceInteraction(bool state);
    const bool getEntranceInteraction()  { return entranceInteraction; }

    void setPathInteraction(bool state);
    const bool getPathInteraction()  { return pathInteraction; }

    void toggleLocations(bool toggle) { entranceInteraction = toggle; }

    void select(Object* obj);
    void selectAll();

    void selectZoneContents(Zone* zone);

    void setDrawType(int drawType) { this->drawType = drawType; }
    void setObject(int selObject, int selTileset);
    void setLayer(int selLayer) { this->selLayer = selLayer; }
    void setSprite(int selSprite);

    void toggleSelectAfterPlacement(bool toggle) { this->selectAfterPlacement = toggle; }

    Qt::CursorShape getActualCursor() { return actualCursor; }

signals:
    void deselected();
    void selectdObjectChanged(Object* obj);
    void updateEditors();
    void updateLevelView();
    void editMade();

private:
    Level *level;
    QUndoStack *undoStack;
    Qt::CursorShape actualCursor;

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
    bool entranceInteraction;
    bool spriteInteraction;
    bool locationInteraction;

    bool selectAfterPlacement;

};

#endif // EDITMANAGER_H
