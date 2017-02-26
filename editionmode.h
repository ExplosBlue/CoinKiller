#ifndef EDITIONMODE_H
#define EDITIONMODE_H

#include "level.h"

#include <QList>
#include <QKeyEvent>

enum EditMode
{
    EditMode_ObjectsMode
};

class EditionMode : public QObject
{
    Q_OBJECT
public:
    EditionMode() {}
    virtual ~EditionMode() {}

    virtual void mouseDown(int, int, Qt::MouseButtons, Qt::KeyboardModifiers, QRect) {}
    virtual void mouseDrag(int, int, Qt::KeyboardModifiers, QRect) {}
    virtual void mouseMove(int, int) {}
    virtual void mouseUp(int, int) {}

    virtual void keyPress(QKeyEvent *) {}

    virtual void activate() {}
    virtual void deactivate() {}

    virtual void render(QPainter*) {}

    virtual void deleteSelection() {}
    virtual void copy() {}
    virtual void cut() {}
    virtual void paste(int, int, int, int) {}
    virtual void raise() {}
    virtual void lower() {}
    virtual void raiseLayer() {}
    virtual void lowerLayer() {}

    virtual void select(Object*) {}

    Qt::CursorShape getActualCursor() { return actualCursor; }

protected:
    Level *level;
    Qt::CursorShape actualCursor;

signals:
    void deselected();
    void selectdObjectChanged(Object* obj);
    void updateEditors();
    void updateLevelView();
};

#endif // EDITIONMODE_H
