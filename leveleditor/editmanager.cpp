#include "editmanager.h"
#include "unitsconvert.h"
#include "settingsmanager.h"
#include "is.h"

#include "commands/editorcommands.h"
#include "commands/spritecommands.h"
#include "commands/bgdatcommands.h"
#include "commands/pathcommands.h"
#include "commands/levelcommands.h"
#include "commands/objectcommands.h"
#include "commands/progresspathcommands.h"

#include <QApplication>
#include <QPainterPath>
#include <QClipboard>
#include <QDebug>

EditManager::EditManager(Level *level, QUndoStack *undoStack) :
    level(level), undoStack(undoStack)
{
    this->selectAfterPlacement = SettingsManager::getInstance()->get("SelectAfterPlacement").toBool();
}
void EditManager::mouseDown(int x, int y, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, QRect drawrect)
{
    dx = x;
    dy = y;
    lx = x;
    ly = y;

    if (buttons == Qt::RightButton)
    {
        CreateObject(x, y);
        return;
    }

    if (buttons == Qt::LeftButton)
    {
        mouseAct = getActionAtPos(x, y);

        if (mouseAct.pAdd != AddNone)
        {
            Object* oldNode = selectedObjects[0];
            selectedObjects.clear();

            if (is<PathNode*>(oldNode))
            {
                PathNode* oldPNode = dynamic_cast<PathNode*>(oldNode);
                Path* parentPath = oldPNode->getParentPath();
                PathNode* newNode = new PathNode(toNext10(x), toNext10(y), parentPath);
                quint32 index = parentPath->getIndexOfNode(oldPNode);

                if (mouseAct.pAdd == AddAfter) {
                    index += 1;
                }

                undoStack->push(new Commands::PathCmd::InsertNode(parentPath, newNode, index));
                selectedObjects.append(newNode);
            }
            else
            {
                ProgressPathNode* oldPNode = dynamic_cast<ProgressPathNode*>(oldNode);
                ProgressPath* parentPath = oldPNode->getParentPath();
                ProgressPathNode* newNode = new ProgressPathNode(toNext10(x), toNext10(y), parentPath);
                quint32 index = parentPath->getIndexOfNode(oldPNode);

                if (mouseAct.pAdd == AddAfter) {
                    index += 1;
                }

                undoStack->push(new Commands::ProgressPathCmd::InsertNode(parentPath, newNode, index));
                selectedObjects.append(newNode);
            }

            if (selectedObjects.size() == 1) {
                emit selectdObjectChanged(selectedObjects[0]);
            }
        }

        else if (!mouseAct.drag || (mouseAct.hor == ResizeNone && mouseAct.vert == ResizeNone))
        {
            QList<Object*> tempSelObjects = selectedObjects;
            bool shift = false;

            if (modifiers != Qt::ShiftModifier) {
                selectedObjects = getObjectsAtPos(dx, dy, dx, dy, true, drawrect);
            }
            else if (modifiers != Qt::ControlModifier)
            {
                shift = true;
                QList<Object*> newObjList = getObjectsAtPos(dx, dy, dx, dy, true, drawrect);
                if (newObjList.size() == 1)
                {
                    if(!selectedObjects.contains(newObjList[0]))
                        selectedObjects.append(newObjList[0]);
                    else
                        selectedObjects.removeOne(newObjList[0]);
                }
            }

            if (!shift && selectedObjects.size() != 0 && tempSelObjects.contains(selectedObjects[0]))
            {
                selectedObjects = tempSelObjects;
            }

            if (selectedObjects.size() == 0)
            {
                interactMode = InteractionMode::Selection;
            }
            else if (modifiers == Qt::ControlModifier)
            {
                sortSelection();
                cloneObjects(selectedObjects);
                emit updateEditors();
                clone = true;
            }

            checkEmits();
        }

        mouseAct = getActionAtPos(x, y);
        if (!clone) actualCursor = getCursorAtPos(x, y);
        else actualCursor = Qt::SizeAllCursor;
        updateSelectionBounds();
    }
}

void EditManager::mouseDrag(int x, int y, Qt::KeyboardModifiers modifiers, QRect drawrect)
{
    if (interactMode == InteractionMode::Creation)
    {
        lx = x;
        ly = y;

        if (qAbs((dx-lx) + (dy-ly)) < 20)
        {
            return;
        }

        if (!resizingObjects) {
            undoStack->beginMacro(tr("Resized Selection"));
            resizingObjects = true;
        }

        int xpos = typeRound(dx, newObject->getType());
        int ypos = typeRound(dy, newObject->getType());

        int mX = typeRound(qMax(0, lx), newObject->getType());
        int mY = typeRound(qMax(0, ly), newObject->getType());

        int w = qAbs(mX-xpos);
        int h = qAbs(mY-ypos);

        if (newObject->getType() == ObjectType::BGDATOBJECT)
        {
            if (mX < 0) mX = 0;
            else w += 20;
            if (mY < 0) mY = 0;
            else h += 20;
        }

        undoStack->push(new Commands::ObjectCmd::Move(newObject, qMin(xpos, mX), qMin(ypos, mY)));
        undoStack->push(new Commands::ObjectCmd::Resize(newObject, w, h));

        emit updateEditors();

        return;
    }

    if (interactMode == InteractionMode::Selection)
    {
        lx = x;
        ly = y;
        selectedObjects.clear();
        selectedObjects = getObjectsAtPos(lx, ly, dx, dy, false, drawrect);
    }
    else
    {
        updateSelectionBounds();

        // Drag
        if ((mouseAct.hor == ResizeNone && mouseAct.vert == ResizeNone) || clone)
        {
            int xDelta = x-lx;
            int yDelta = y-ly;

            // Quick and dirty fix to prevent the dragging objects from wiggling sometimes
            if (xDelta == -xDeltaL) xDelta = 0;
            else xDeltaL = xDelta;
            if (yDelta == -yDeltaL) yDelta = 0;
            else yDeltaL = yDelta;

            if (xDelta < -minBoundX) xDelta = -minBoundX;
            if (yDelta < -minBoundY) yDelta = -minBoundY;
            if (xDelta == 0 && yDelta == 0) return;

            int snap = 0;
            if (selectionHasBGDats)
            {
                xDelta = toNext20(xDelta);
                yDelta = toNext20(yDelta);
                snap = 20;
            }
            else if (modifiers == Qt::AltModifier)
            {
                xDelta = toNext16Compatible(xDelta);
                xDelta = toNext16Compatible(xDelta);
            }
            else
            {
                xDelta = toNext10(xDelta);
                yDelta = toNext10(yDelta);
                snap = 10;
            }

            if (!movingObjects) {
                undoStack->beginMacro(tr("Moved Selection"));
                movingObjects = true;
            }

            foreach (Object* obj, selectedObjects)
            {
                undoStack->push(new Commands::ObjectCmd::IncreasePosition(obj, xDelta, yDelta, snap));
            }

            minBoundX += xDelta;
            minBoundY += yDelta;
            lx += xDelta;
            ly += yDelta;

            emit updateEditors();
        }
        // Resize
        else
        {
            int xDelta = x-lx;
            int yDelta = y-ly;
            int minSize = 0;

            int snap = 0;
            if (selectionHasBGDats)
            {
                xDelta = toNext20(xDelta);
                yDelta = toNext20(yDelta);
                minSize = 20;
                snap = 0;
            }
            else if (modifiers == Qt::AltModifier)
            {
                xDelta = toNext16Compatible(xDelta);
                xDelta = toNext16Compatible(xDelta);
            }
            else
            {
                xDelta = toNext10(xDelta);
                yDelta = toNext10(yDelta);
                snap = 10;
            }

            int xMoveDelta = 0;
            int xResizeDelta = 0;
            int yMoveDelta = 0;
            int yResizeDelta = 0;

            if (xDelta == 0 && yDelta == 0) return;

            if (mouseAct.hor == ResizeBegin)
            {
                if (-xDelta <= -minSizeX) xDelta = -(-minSizeX) - minSize;
                if (xDelta < -minBoundX) xDelta = -minBoundX;
                xMoveDelta = xDelta;
                xResizeDelta = -xDelta;
            }
            if (mouseAct.vert == ResizeBegin)
            {
                if (-yDelta <= -minSizeY) yDelta = -(-minSizeY) - minSize;
                if (yDelta < -minBoundY) yDelta = -minBoundY;
                yMoveDelta = yDelta;
                yResizeDelta = -yDelta;
            }
            if (mouseAct.hor == ResizeEnd)
            {
                if (xDelta <= -minSizeX) xDelta = -minSizeX + minSize;
                xResizeDelta = xDelta;
            }
            if (mouseAct.vert == ResizeEnd)
            {
                if (yDelta <= -minSizeY) yDelta = -minSizeY + minSize;
                yResizeDelta = yDelta;
            }

            if (xMoveDelta == 0 && yMoveDelta == 0 && xResizeDelta == 0 && yResizeDelta == 0) return;

            minBoundX += xMoveDelta;
            minBoundY += yMoveDelta;
            minSizeX += xResizeDelta;
            minSizeY += yResizeDelta;

            if (!resizingObjects) {
                undoStack->beginMacro(tr("Resized Selection"));
                resizingObjects = true;
            }

            foreach (Object* obj, selectedObjects)
            {
                undoStack->push(new Commands::ObjectCmd::IncreasePosition(obj, xMoveDelta, yMoveDelta));

                if (obj->isResizable()) {
                    undoStack->push(new Commands::ObjectCmd::IncreaseSize(obj, xResizeDelta, yResizeDelta, snap));
                }
            }

            lx += xDelta;
            ly += yDelta;

            emit updateEditors();
        }
    }
}

void EditManager::mouseMove(int x, int y)
{
    actualCursor = getCursorAtPos(x, y);
}

void EditManager::mouseUp(int x, int y)
{
    mouseAct = getActionAtPos(x, y);
    actualCursor = getCursorAtPos(x, y);

    checkEmits();

    if (movingObjects) {
        undoStack->endMacro();
        movingObjects = false;
    }

    if (resizingObjects) {
        undoStack->endMacro();
        resizingObjects = false;
    }

    interactMode = InteractionMode::None;
    clone = false;
}

void EditManager::CreateObject(quint32 x, quint32 y)
{
    selectedObjects.clear();

    if (drawType == ObjectType::INVALID) {
        return;
    }

    if (drawType == ObjectType::BGDATOBJECT)
    {
        if (selTileset == -1 || selObject == -1) {
            return;
        }

        int id = selTileset << 12;
        id = (id & 0xF000) | selObject;
        ObjectDef *obj = level->tilesets[selTileset]->getObjectDef(selObject);
        BgdatObject *bgdatObj = new BgdatObject(toNext20(x-10), toNext20(y-10), obj->width*20, obj->height*20, id, selLayer);

        undoStack->push(new Commands::LevelCmd::InsertBgdatObj(level, bgdatObj));

        if (selectAfterPlacement) {
            newObject = bgdatObj;
            interactMode = InteractionMode::Creation;
            selectedObjects.append(bgdatObj);
        }
    }
    else if (drawType == ObjectType::SPRITE)
    {
        if (selSprite == -1) {
            return;
        }

        Sprite *spr = new Sprite(0, 0, selSprite);
        spr->setRect();
        int xpos = qMax(0, toNext10(dx-spr->getwidth()/2-spr->getOffsetX()));
        int ypos = qMax(0, toNext10(dy-spr->getheight()/2-spr->getOffsetY()));

        spr->setPosition(xpos, ypos);

        undoStack->push(new Commands::LevelCmd::InsertSprite(level, spr));

        if (selectAfterPlacement) {
            selectedObjects.append(spr);
        }
    }
    else if (drawType == ObjectType::ENTRANCE)
    {
        Entrance *entr = level->newEntrance(x-10, y-10);
        entr->setRect();

        undoStack->push(new Commands::LevelCmd::InsertEntrance(level, entr));

        if (selectAfterPlacement) {
            selectedObjects.append(entr);
        }
    }
    else if (drawType == ObjectType::ZONE)
    {
        Zone* zone = level->newZone(x-200, y-120);

        undoStack->beginMacro(tr("Inserted Zone at X: %1 Y: %2").arg(zone->getx()).arg(zone->gety()));
        undoStack->push(new Commands::LevelCmd::InsertZone(level, zone));

        if (level->backgrounds.isEmpty()) {
            undoStack->push(new Commands::LevelCmd::InsertZoneBackground(level, new ZoneBackground()));
        }

        if (level->boundings.isEmpty()) {
            undoStack->push(new Commands::LevelCmd::InsertZoneBounding(level, new ZoneBounding()));
        }

        undoStack->endMacro();

        if (selectAfterPlacement) {
            selectedObjects.append(zone);
        }
    }
    else if (drawType == ObjectType::LOCATION)
    {
        Location* loc = level->newLocation(toNext20(x-10), toNext20(y-10));

        undoStack->push(new Commands::LevelCmd::InsertLocation(level, loc));

        if (selectAfterPlacement) {
            newObject = loc;
            interactMode = InteractionMode::Creation;
            selectedObjects.append(loc);
        }
    }
    else if (drawType == ObjectType::PATHNODE)
    {
        Path* path = level->newPath();
        PathNode* node = new PathNode(qMax(toNext10(x), 0), qMax(toNext10(y), 0), path);

        undoStack->beginMacro(tr("Inserted Path %1 at X: %2 Y: %3").arg(path->getid()).arg(node->getx()).arg(node->gety()));
        undoStack->push(new Commands::LevelCmd::InsertPath(level, path));
        undoStack->push(new Commands::PathCmd::InsertNode(path, node));
        undoStack->endMacro();

        if (selectAfterPlacement) {
            selectedObjects.append(node);
        }
    }
    else if (drawType == ObjectType::PROGRESSPATHNODE)
    {
        ProgressPath* path = level->newProgressPath();
        ProgressPathNode* node = new ProgressPathNode(qMax(toNext10(x), 0), qMax(toNext10(y), 0), path);

        undoStack->beginMacro(tr("Inserted Progress Path %1 at X: %2 Y: %3").arg(path->getid()).arg(node->getx()).arg(node->gety()));
        undoStack->push(new Commands::LevelCmd::InsertProgressPath(level, path));
        undoStack->push(new Commands::ProgressPathCmd::InsertNode(path, node));
        undoStack->endMacro();

        if (selectAfterPlacement) {
            selectedObjects.append(node);
        }
    }

    checkEmits();
    emit updateEditors();
    if (selectedObjects.size() == 1) {
        emit selectdObjectChanged(selectedObjects[0]);
    }
}

void EditManager::render(QPainter *painter)
{
    foreach (Object* obj, selectedObjects)
    {
        if (is<Sprite*>(obj) && !static_cast<Sprite*>(obj)->getSelectionRects()->empty())
        {
            Sprite* spr = static_cast<Sprite*>(obj);

            foreach (QRect rect, *spr->getSelectionRects())
            {
                QRect objrect(obj->getx() + rect.x(), obj->gety() + rect.y(), rect.width(), rect.height());

                painter->setPen(QPen(QColor(255,255,255,200), 1, Qt::DotLine));
                painter->drawRect(objrect);
                painter->fillRect(objrect, QColor(255,255,255,75));
            }
        }
        else
        {
            QRect objrect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());

            painter->setPen(QPen(QColor(255,255,255,200), 1, Qt::DotLine));
            painter->drawRect(objrect);
            painter->fillRect(objrect, QColor(255,255,255,75));
        }

        if (obj->isResizable())
        {
            drawResizeKnob(obj->getx(), obj->gety(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth(), obj->gety(), painter);
            drawResizeKnob(obj->getx(), obj->gety() + obj->getheight(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth(), obj->gety() + obj->getheight(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth()/2, obj->gety(), painter);
            drawResizeKnob(obj->getx(), obj->gety() + obj->getheight()/2, painter);
            drawResizeKnob(obj->getx() + obj->getwidth()/2, obj->gety() + obj->getheight(), painter);
            drawResizeKnob(obj->getx() + obj->getwidth(), obj->gety() + obj->getheight()/2, painter);
        }
    }

    if (selectedObjects.count() == 1)
    {
        Object* node = selectedObjects[0];

        if (is<PathNode*>(node) || is<ProgressPathNode*>(node))
        {
            painter->setPen(QPen(Qt::black));

            QPainterPath path1;
            path1.moveTo(node->getx()-10, node->gety()-10);
            path1.quadTo(QPoint(node->getx()-24, node->gety()-10), QPoint(node->getx()-30, node->gety()));
            path1.quadTo(QPoint(node->getx()-24, node->gety()+10), QPoint(node->getx()-10, node->gety()+10));
            painter->fillPath(path1, QBrush(QColor(100, 100, 100, 175)));
            painter->drawPath(path1);

            QPainterPath path2;
            path2.moveTo(node->getx()+10, node->gety()-10);
            path2.quadTo(QPoint(node->getx()+24, node->gety()-10), QPoint(node->getx()+30, node->gety()));
            path2.quadTo(QPoint(node->getx()+24, node->gety()+10), QPoint(node->getx()+10, node->gety()+10));
            painter->fillPath(path2, QBrush(QColor(100, 100, 100, 175)));
            painter->drawPath(path2);

            drawPlus(painter, node->getx()-28, node->gety()-10);
            drawPlus(painter, node->getx()+8, node->gety()-10);
        }
    }

    if (interactMode == InteractionMode::Selection)
    {
        QRect selArea(qMin(dx, lx), qMin(dy, ly), qAbs(lx-dx), qAbs(ly-dy));
        painter->setPen(QPen(QColor(0,80,180), 0.5));
        painter->fillRect(selArea, QColor(160,222,255,50));
        painter->drawRect(selArea);
    }
}

void EditManager::drawPlus(QPainter *painter, int x, int y)
{
    QPainterPath path1;
    path1.addRoundedRect(x+9, y+4, 2, 12 , 2.0, 2.0);
    painter->fillPath(path1, Qt::white);
    QPainterPath path2;
    path2.addRoundedRect(x+4, y+9, 12, 2 , 2.0, 2.0);
    painter->fillPath(path2, Qt::white);
}

void EditManager::drawResizeKnob(int x, int y, QPainter *painter)
{
    painter->fillRect(x-2, y-2, 4, 4, QBrush(Qt::white));
}

QList<Object*> EditManager::getObjectsAtPos(int x1, int y1, int x2, int y2, bool firstOnly, QRect drawrect)
{
    QList<Object*> objects;

    if (x1>x2)
    {
        int tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1>y2)
    {
        int tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    QRect area = QRect(QPoint(x1, y1), QPoint(x2, y2));

    for (int l = 1; l >= 0; l--)
    {
        if (!(layerMask & (1 << l)))
            continue;
        foreach (BgdatObject* bgdat, level->objects[l]) if (bgdat->clickDetection(area)) objects.append(bgdat);
    }
    if (locationInteraction) foreach (Location* loc, level->locations) if (loc->clickDetection(area)) objects.append(loc);
    if (spriteInteraction) foreach (Sprite* spr, level->sprites) if (spr->clickDetection(area)) objects.append(spr);
    if (entranceInteraction) foreach (Entrance* entr, level->entrances) if (entr->clickDetection(area)) objects.append(entr);
    if (pathInteraction) foreach (Path* path, level->paths) foreach (PathNode* node, path->getNodes()) if (node->clickDetection(area)) objects.append(node);
    if (pathInteraction) foreach (ProgressPath* path, level->progressPaths) foreach (ProgressPathNode* node, path->getNodes()) if (node->clickDetection(area)) objects.append(node);

    foreach (Zone* zone, level->zones)
    {
        int adjustX = 0;
        int adjustY = 0;
        if (zone->getx() < drawrect.x())
            adjustX += drawrect.x()-zone->getx();
        if (zone->gety() < drawrect.y())
            adjustY += drawrect.y()-zone->gety();
        if (zone->clickDetection(area.adjusted(-adjustX,-adjustY,0,0)))
            objects.append(zone);
    }

    if (firstOnly && objects.size() > 1)
    {
        Object* object = objects[objects.size()-1];
        objects.clear();
        objects.append(object);
    }


    return objects;
}

void EditManager::cloneObjects(QList<Object *> objects)
{
    selectedObjects.clear();
    undoStack->beginMacro("Cloned Objects");

    foreach (Object* o, objects)
    {
        if (is<BgdatObject*>(o)) {
            BgdatObject *obj = new BgdatObject(dynamic_cast<BgdatObject*>(o));
            undoStack->push(new Commands::LevelCmd::InsertBgdatObj(level, obj));
            selectedObjects.append(obj);
        }
        if (is<Sprite*>(o)) {
            Sprite *spr = new Sprite(dynamic_cast<Sprite*>(o));
            undoStack->push(new Commands::LevelCmd::InsertSprite(level, spr));
            selectedObjects.append(spr);
        }
        if (is<Entrance*>(o)) {
            Entrance *entr = new Entrance(dynamic_cast<Entrance*>(o));
            undoStack->push(new Commands::LevelCmd::InsertEntrance(level, entr));
            selectedObjects.append(entr);
        }
        if (is<Location*>(o)) {
            Location *loc = new Location(dynamic_cast<Location*>(o));
            undoStack->push(new Commands::LevelCmd::InsertLocation(level, loc));
            selectedObjects.append(loc);
        }
        if (is<Zone*>(o)) {
            Zone *zone = new Zone(dynamic_cast<Zone*>(o));
            undoStack->push(new Commands::LevelCmd::InsertZone(level, zone));
            selectedObjects.append(zone);
        }
    }

    undoStack->endMacro();
}

EditManager::mouseAction EditManager::getActionAtPos(int x, int y)
{
    mouseAction act;

    if (selectedObjects.size() == 1)
    {
        Object* node = selectedObjects[0];

        if (is<PathNode*>(node))
        {
            if (QRect(node->getx()-30, node->gety()-10, 20, 20).contains(x, y))
            {
                PathNode* n = dynamic_cast<PathNode*>(node);

                int nodeID = n->getParentPath()->getIndexOfNode(n);

                if (nodeID == 0 && n->getParentPath()->getNumberOfNodes() > nodeID+1)
                {
                    if (n->getx() >= n->getParentPath()->getNode(nodeID+1)->getx())
                        act.pAdd = AddAfter;
                    else
                        act.pAdd = AddBefore;
                }
                else if (nodeID == 0)
                    act.pAdd = AddBefore;
                else if (n->getx() >= n->getParentPath()->getNode(nodeID-1)->getx())
                    act.pAdd = AddBefore;
                else
                    act.pAdd = AddAfter;
            }
            else if (QRect(node->getx()+10, node->gety()-10, 20, 20).contains(x, y))
            {
                PathNode* n = dynamic_cast<PathNode*>(node);
                int nodeID = n->getParentPath()->getIndexOfNode(n);

                if (nodeID == 0 && n->getParentPath()->getNumberOfNodes() > nodeID+1)
                {
                    if (n->getx() >= n->getParentPath()->getNode(nodeID+1)->getx())
                        act.pAdd = AddBefore;
                    else
                        act.pAdd = AddAfter;
                }
                else if (nodeID == 0)
                    act.pAdd = AddAfter;
                else if (n->getx() >= n->getParentPath()->getNode(nodeID-1)->getx())
                    act.pAdd = AddAfter;
                else
                    act.pAdd = AddBefore;
            }

            if (act.pAdd != AddNone)
                return act;
        }
        else if (is<ProgressPathNode*>(node))
        {
            Object* node = selectedObjects[0];

            if (is<ProgressPathNode*>(node))
            {
                if (QRect(node->getx()-30, node->gety()-10, 20, 20).contains(x, y))
                {
                    ProgressPathNode* n = dynamic_cast<ProgressPathNode*>(node);

                    int nodeID = n->getParentPath()->getIndexOfNode(n);

                    if (nodeID == 0 && n->getParentPath()->getNumberOfNodes() > nodeID+1)
                    {
                        if (n->getx() >= n->getParentPath()->getNode(nodeID+1)->getx())
                            act.pAdd = AddAfter;
                        else
                            act.pAdd = AddBefore;
                    }
                    else if (nodeID == 0)
                        act.pAdd = AddBefore;
                    else if (n->getx() >= n->getParentPath()->getNode(nodeID-1)->getx())
                        act.pAdd = AddBefore;
                    else
                        act.pAdd = AddAfter;
                }
                else if (QRect(node->getx()+10, node->gety()-10, 20, 20).contains(x, y))
                {
                    ProgressPathNode* n = dynamic_cast<ProgressPathNode*>(node);
                    int nodeID = n->getParentPath()->getIndexOfNode(n);

                    if (nodeID == 0 && n->getParentPath()->getNumberOfNodes() > nodeID+1)
                    {
                        if (n->getx() >= n->getParentPath()->getNode(nodeID+1)->getx())
                            act.pAdd = AddBefore;
                        else
                            act.pAdd = AddAfter;
                    }
                    else if (nodeID == 0)
                        act.pAdd = AddAfter;
                    else if (n->getx() >= n->getParentPath()->getNode(nodeID-1)->getx())
                        act.pAdd = AddAfter;
                    else
                        act.pAdd = AddBefore;
                }

                if (act.pAdd != AddNone)
                    return act;
            }
        }
    }

    for (int i = selectedObjects.size()-1; i >= 0; i--)
    {
        Object* o = selectedObjects[i];

        if (o->isResizable())
        {
            act.drag = true;

            if (x >= o->getx() - 8 && x <= o->getx() + 4)
                act.hor = ResizeBegin;
            else if (x >= o->getx() + o->getwidth() - 4 && x <= o->getx() + o->getwidth() + 8)
                act.hor = ResizeEnd;
            else if (x >= o->getx() && x <= o->getx() + o->getwidth())
                act.hor = ResizeNone;
            else act.drag = false;

            if (y >= o->gety() - 8 && y <= o->gety() + 4)
                act.vert = ResizeBegin;
            else if (y >= o->gety() + o->getheight() - 4 && y <= o->gety() + o->getheight() + 8)
                act.vert = ResizeEnd;
            else if (y >= o->gety() && y <= o->gety() + o->getheight())
                act.vert = ResizeNone;
            else act.drag = false;

            if (act.hor == ResizeNone && act.vert == ResizeNone && is<Zone*>(o) && !dynamic_cast<Zone*>(o)->clickDetection(x, y))
                act.drag = false;
        }
        else
        {
            act.hor = ResizeNone;
            act.vert = ResizeNone;

            act.drag = false;
            if (x >= o->getx() + o->getOffsetX() && x < o->getx() + o->getOffsetX() + o->getwidth() && y >= o->gety() + o->getOffsetY() && y < o->gety() + o->getOffsetY() + o->getheight())
                act.drag = true;
        }

        if (act.drag) return act;
    }

    return act;
}

Qt::CursorShape EditManager::getCursorAtPos(int x, int y)
{
    mouseAction act = getActionAtPos(x, y);

    if (!act.drag) return Qt::ArrowCursor;

    if (act.vert == ResizeBegin && act.hor == ResizeBegin) return Qt::SizeFDiagCursor;
    if (act.vert == ResizeEnd && act.hor == ResizeEnd) return Qt::SizeFDiagCursor;
    if (act.vert == ResizeBegin && act.hor == ResizeEnd) return Qt::SizeBDiagCursor;
    if (act.vert == ResizeEnd && act.hor == ResizeBegin) return Qt::SizeBDiagCursor;
    if (act.vert == ResizeNone && act.hor == ResizeNone) return Qt::SizeAllCursor;
    if (act.vert == ResizeNone) return Qt::SizeHorCursor;
    if (act.hor == ResizeNone) return Qt::SizeVerCursor;

    return Qt::ArrowCursor;
}

void EditManager::updateSelectionBounds()
{
    minBoundX = 2147483647;
    minBoundY = 2147483647;
    maxBoundX = 0;
    maxBoundY = 0;
    minSizeX = 2147483647;
    minSizeY = 2147483647;
    selectionHasBGDats = false;

    foreach (Object* o, selectedObjects)
    {
        if (o->getx() < minBoundX) minBoundX = o->getx();
        if (o->gety() < minBoundY) minBoundY = o->gety();
        if (o->getx() + o->getwidth() > maxBoundX) maxBoundX = o->getx() + o->getwidth();
        if (o->gety() + o->getheight() > maxBoundY) maxBoundY = o->gety() + o->getheight();

        if (o->isResizable())
        {
            if (is<Zone*>(o))
            {
                if (o->getwidth() - 400 < minSizeX) minSizeX = o->getwidth() - 400;
                if (o->getheight() - 240 < minSizeY) minSizeY = o->getheight() - 240;
            }
            else
            {
                if (o->getwidth() < minSizeX) minSizeX = o->getwidth();
                if (o->getheight() < minSizeY) minSizeY = o->getheight();
            }
        }

        if (is<BgdatObject*>(o))
            selectionHasBGDats = true;
    }
}

void EditManager::sortSelection()
{
    QList<Object*> sortedObjects;

    for (int i=0; i<2; i++) foreach (BgdatObject* bgdat, level->objects[i]) if (selectedObjects.contains(bgdat)) sortedObjects.append(bgdat);
    foreach (Sprite* spr, level->sprites) if (selectedObjects.contains(spr)) sortedObjects.append(spr);
    foreach (Entrance* entr, level->entrances) if (selectedObjects.contains(entr)) sortedObjects.append(entr);
    foreach (Location* loc, level->locations) if (selectedObjects.contains(loc)) sortedObjects.append(loc);
    foreach (Zone* zone, level->zones) if (selectedObjects.contains(zone)) sortedObjects.append(zone);

    selectedObjects = sortedObjects;
}

void EditManager::select(Object *obj)
{
    selectedObjects.clear();
    selectedObjects.append(obj);
}

void EditManager::selectAll()
{
    selectedObjects.clear();

    for (int i=0; i<2; i++) foreach (BgdatObject* bgdat, level->objects[i]) selectedObjects.append(bgdat);
    foreach (Sprite* spr, level->sprites) selectedObjects.append(spr);
    foreach (Entrance* entr, level->entrances) selectedObjects.append(entr);
    foreach (Location* loc, level->locations) selectedObjects.append(loc);
    foreach (Zone* zone, level->zones) selectedObjects.append(zone);
    foreach (Path* path, level->paths) foreach (PathNode* node, path->getNodes()) selectedObjects.append(node);
    foreach (ProgressPath* path, level->progressPaths) foreach (ProgressPathNode* node, path->getNodes()) selectedObjects.append(node);
}

void EditManager::deselect()
{
    selectedObjects.clear();
}

void EditManager::selectZoneContents(Zone* zone)
{
    selectAll();

    QRect zoneRect(zone->getx(), zone->gety(), zone->getwidth(),zone->getheight());

    for (int i = selectedObjects.size() - 1; i >= 0; i--)
    {
        Object* obj = selectedObjects[i];

        if (!zoneRect.intersects(QRect(obj->getx() + obj->getOffsetX(), obj->gety() + obj->getOffsetY(), obj->getwidth(), obj->getheight())))
            selectedObjects.removeAt(i);
    }
}

void EditManager::deleteSelection()
{
    if (selectedObjects.isEmpty()) {
        return;
    }

    undoStack->beginMacro(tr("Deleted %1 Object(s)").arg(selectedObjects.count()));
    foreach (Object* obj, selectedObjects)
    {
        if (is<BgdatObject*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeleteBgdatObject(level, dynamic_cast<BgdatObject*>(obj)));
        }
        else if (is<Sprite*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeleteSprite(level, dynamic_cast<Sprite*>(obj)));
        }
        else if (is<Entrance*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeleteEntrance(level, dynamic_cast<Entrance*>(obj)));
        }
        else if (is<Zone*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeleteZone(level, dynamic_cast<Zone*>(obj)));
        }
        else if (is<Location*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeleteLocation(level, dynamic_cast<Location*>(obj)));
        }
        else if (is<PathNode*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeletePathNode(level, dynamic_cast<PathNode*>(obj)));
        }
        else if (is<ProgressPathNode*>(obj))
        {
            undoStack->push(new Commands::LevelCmd::DeleteProgressPathNode(level, dynamic_cast<ProgressPathNode*>(obj)));
        }
    }
    undoStack->endMacro();

    selectedObjects.clear();
    actualCursor = Qt::ArrowCursor;
    mouseAction act;
    mouseAct = act;
    checkEmits();
    emit updateEditors();
}

void EditManager::copy()
{
    if (selectedObjects.size() == 0)
        return;

    int minX = 0x7FFFFFFF;
    int maxX = 0;
    int minY = 0x7FFFFFFF;
    int maxY = 0;

    bool hasBgDats = false;

    foreach (Object* obj, selectedObjects)
    {
        if (obj->getType() == ObjectType::BGDATOBJECT)
            hasBgDats = true;

        if (obj->getx()+obj->getOffsetX() < minX) minX = obj->getx()+obj->getOffsetX();
        if (obj->gety()+obj->getOffsetY() < minY) minY = obj->gety()+obj->getOffsetY();
        if (obj->getx()+obj->getOffsetX()+obj->getwidth() > maxX) maxX = obj->getx()+obj->getOffsetX()+obj->getwidth();
        if (obj->gety()+obj->getOffsetY()+obj->getheight() > maxY) maxY = obj->gety()+obj->getOffsetY()+obj->getheight();
    }

    QString clipboardText = QString("CoinKillerClip|%1:%2").arg(maxX-minX).arg(maxY-minY);

    if (hasBgDats)
    {
        minX = typeRound(minX, ObjectType::BGDATOBJECT);
        minY = typeRound(minY, ObjectType::BGDATOBJECT);
    }

    QList<Path*> paths;
    QList<ProgressPath*> progPaths;

    bool copyObject = false;

    QString nextText = "";

    foreach (Object* obj, selectedObjects)
    {
        if (is<PathNode*>(obj))
        {
            PathNode* pathNode = dynamic_cast<PathNode*>(obj);
            Path* parentPath = pathNode->getParentPath();

            if (paths.empty())
            {
                paths.append(parentPath);
                copyObject = true;
            }
            else
            {
                for (int i = 0; i < paths.count(); i++)
                {
                    if (parentPath == paths[i])
                    {
                        copyObject = false;
                        break;
                    }
                    else
                    {
                        if (i == paths.count()-1)
                        {
                            paths.append(parentPath);
                            copyObject = true;
                            break;
                        }
                    }
                }
            }
        }
        else if (is<ProgressPathNode*>(obj))
        {
            ProgressPathNode* progPathNode = dynamic_cast<ProgressPathNode*>(obj);
            ProgressPath* parentPath = progPathNode->getParentPath();

            if (progPaths.empty())
            {
                progPaths.append(parentPath);
                copyObject = true;
            }
            else
            {
                for (int i = 0; i < progPaths.count(); i++)
                {
                    if (parentPath == progPaths[i])
                    {
                        copyObject = false;
                        break;
                    }
                    else
                    {
                        if (i == progPaths.count()-1)
                        {
                            progPaths.append(parentPath);
                            copyObject = true;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            copyObject = true;
        }

        if (copyObject)
        {
            nextText = obj->toString(-minX, -minY);

            if (nextText != "")
                clipboardText.append("|" + nextText);

            copyObject = false;
        }
    }

    QApplication::clipboard()->setText(clipboardText);
}

void EditManager::cut()
{
    copy();
    deleteSelection();
}

void EditManager::paste(int currX, int currY, int currW, int currH)
{
    QStringList sections = QApplication::clipboard()->text().split('|');

    if (sections.size() < 2)
        return;

    if (sections[0] != "CoinKillerClip")
        return;

    QStringList pasteSizes = sections[1].split(':');
    int pOffsetX = qMax(toNext20(currX + currW/2 - pasteSizes[0].toInt()/2), 0);
    int pOffsetY = qMax(toNext20(currY + currH/2 - pasteSizes[1].toInt()/2), 0);

    selectedObjects.clear();

    undoStack->beginMacro(tr("Pasted"));

    for (int i = 2; i < sections.size(); i++)
    {
        QStringList params = sections[i].split(':');

        switch (params[0].toInt())
        {
        case 0: // BG dat
        {
            BgdatObject* newObj = new BgdatObject(params[3].toInt()+pOffsetX, params[4].toInt()+pOffsetY, params[5].toInt(), params[6].toInt(), params[1].toInt(), params[2].toInt());
            undoStack->push(new Commands::LevelCmd::InsertBgdatObj(level, newObj));
            selectedObjects.append(newObj);
            break;
        }
        case 1: // Sprite
        {
            Sprite* newSpr = new Sprite(params[2].toInt()+pOffsetX, params[3].toInt()+pOffsetY, params[1].toInt());
            newSpr->setLayer(params[4].toInt());
            for (int i = 0; i < 12; i++) {
                newSpr->setByte(i, params[i+5].toUInt());
            }
            newSpr->setRect();
            undoStack->push(new Commands::LevelCmd::InsertSprite(level, newSpr));
            selectedObjects.append(newSpr);
            break;
        }
        case 2: // Entrance
        {
            Entrance* newEntr = new Entrance(params[3].toInt()+pOffsetX, params[4].toInt()+pOffsetY, params[7].toInt(), params[8].toInt(), params[1].toInt(), params[5].toInt(), params[6].toInt(), params[2].toInt(), params[9].toInt(), params[10].toInt(), params[11].toInt());
            newEntr->setRect();
            undoStack->push(new Commands::LevelCmd::InsertEntrance(level, newEntr));
            selectedObjects.append(newEntr);
            break;
        }
        case 3: // Zone
        {
            Zone* newZone = new Zone(params[1].toInt()+pOffsetX, params[2].toInt()+pOffsetY, params[3].toInt(), params[4].toInt(), params[5].toInt(), params[6].toInt(), params[7].toInt(), params[8].toInt(), params[9].toInt(), params[10].toInt(), params[11].toInt(), params[12].toInt());
            undoStack->push(new Commands::LevelCmd::InsertZone(level, newZone));
            selectedObjects.append(newZone);
            break;
        }
        case 4: // Location
        {
            Location* newLoc = new Location(params[2].toInt()+pOffsetX, params[3].toInt()+pOffsetY, params[4].toInt(), params[5].toInt(), params[1].toInt());
            undoStack->push(new Commands::LevelCmd::InsertLocation(level, newLoc));
            selectedObjects.append(newLoc);
            break;
        }
        case 5: // Path
        {
            Path* newPath = level->newPath();
            newPath->setId(params[1].toInt());
            newPath->setLoop(params[2].toInt());
            undoStack->push(new Commands::LevelCmd::InsertPath(level, newPath));

            QStringList pathNodes = params[3].split(';');
            for (int i = 0; i < pathNodes.size(); i++)
            {
                QStringList nodeData = pathNodes[i].split(',');
                PathNode* newNode = new PathNode(nodeData[1].toInt()+pOffsetX, nodeData[2].toInt()+pOffsetY, nodeData[3].toFloat(), nodeData[4].toFloat(), nodeData[5].toInt(), nodeData[6].toInt(), nodeData[7].toInt(), nodeData[8].toInt(), newPath);
                undoStack->push(new Commands::PathCmd::InsertNode(newPath, newNode, nodeData[0].toInt()));
                selectedObjects.append(newPath->getNode(i));
            }
            break;
        }
        case 6: // Progress Path
        {
            ProgressPath* newProgPath = level->newProgressPath();
            newProgPath->setId(params[1].toInt());
            newProgPath->setAlternatePathFlag(params[2].toInt());
            undoStack->push(new Commands::LevelCmd::InsertProgressPath(level, newProgPath));

            QStringList progPathNodes = params[3].split(';');
            for (int i = 0; i < progPathNodes.size(); i++)
            {
                QStringList nodeData = progPathNodes[i].split(',');
                ProgressPathNode* newProgPathNode = new ProgressPathNode(nodeData[1].toInt()+pOffsetX, nodeData[2].toInt()+pOffsetY, newProgPath);
                undoStack->push(new Commands::ProgressPathCmd::InsertNode(newProgPath, newProgPathNode, nodeData[0].toInt()));
                selectedObjects.append(newProgPath->getNode(i));
            }
            break;
        }
        default: { break; }
        }
    }
    undoStack->endMacro();

    emit updateEditors();
    checkEmits();
}

void EditManager::raise()
{
    if (selectedObjects.isEmpty()) {
        return;
    }

    sortSelection();
    undoStack->beginMacro(tr("Raised %1 Object(s)").arg(selectedObjects.count()));
    foreach (Object* obj, selectedObjects)
    {
        undoStack->push(new Commands::LevelCmd::RaiseObject(level, obj));
    }
    undoStack->endMacro();
}

void EditManager::lower()
{
    if (selectedObjects.isEmpty()) {
        return;
    }

    sortSelection();
    undoStack->beginMacro(tr("Lowered %1 Object(s)").arg(selectedObjects.count()));
    foreach (Object* obj, selectedObjects)
    {
        undoStack->push(new Commands::LevelCmd::LowerObject(level, obj));
    }
    undoStack->endMacro();
}

void EditManager::raiseLayer()
{
    if (selectedObjects.isEmpty()) {
        return;
    }

    sortSelection();
    // TODO: Count should show number of tiles raised NOT number of objects in selection
    undoStack->beginMacro(tr("Raised Layer of %1 Object(s)").arg(selectedObjects.count()));
    foreach (Object* obj, selectedObjects)
    {
        if (!is<BgdatObject*>(obj)) {
            continue;
        }

        undoStack->push(new Commands::LevelCmd::RaiseBgdatLayer(level, dynamic_cast<BgdatObject*>(obj)));
    }
    undoStack->endMacro();
}

void EditManager::lowerLayer()
{
    if (selectedObjects.isEmpty()) {
        return;
    }

    sortSelection();
    // TODO: Count should show number of tiles lowered NOT number of objects in selection
    undoStack->beginMacro(tr("Lowered Layer of %1 Object(s)").arg(selectedObjects.count()));
    foreach (Object* obj, selectedObjects)
    {
        if (!is<BgdatObject*>(obj)) {
            continue;
        }

        undoStack->push(new Commands::LevelCmd::LowerBgdatLayer(level, dynamic_cast<BgdatObject*>(obj)));
    }
    undoStack->endMacro();
}

void EditManager::checkEmits()
{
    if (selectedObjects.size() != 1)
        emit deselected();
    else
        emit selectdObjectChanged(selectedObjects[0]);
}

void EditManager::setObject(int selObject, int selTileset)
{
    if (this->selObject == selObject && this->selTileset == selTileset) {
        return;
    }

    this->selObject = selObject;
    this->selTileset = selTileset;

    swapSelectedBgdatObjects();

    emit updateLevelView();
}

void EditManager::swapSelectedBgdatObjects()
{
    QList<BgdatObject*> swappableObjects;

    foreach (Object* o, selectedObjects) {
        if(is<BgdatObject*>(o)) {
            BgdatObject* obj = dynamic_cast<BgdatObject*>(o);
            if (obj->getObjID() != selObject || obj->getTsID() != selTileset) {
                swappableObjects.append(obj);
            }
        }
    }

    if (!swappableObjects.empty()) {
        undoStack->beginMacro(tr("Swapped Object(s)"));
        foreach (BgdatObject* obj, swappableObjects) {
            undoStack->push(new Commands::BgdatCmd::ChangeObject(obj, selObject, selTileset));
        }
        undoStack->endMacro();
    }
}

void EditManager::setSprite(int selSprite)
{
    if (this->selSprite == selSprite) {
        return;
    }

    this->selSprite = selSprite;

    swapSelectedSprites();

    emit updateLevelView();
}

void EditManager::swapSelectedSprites()
{
    QList<Sprite*> swappableSprites;

    foreach (Object* o, selectedObjects) {
        if(is<Sprite*>(o)) {
            Sprite* spr = dynamic_cast<Sprite*>(o);
            if (spr->getid() != selSprite) {
                swappableSprites.append(spr);
            }
        }
    }

    if (!swappableSprites.isEmpty()) {

        if (swappableSprites.size() == 1) {
            undoStack->push(new Commands::SpriteCmd::SetId(swappableSprites[0], selSprite));
            return;
        }

        undoStack->beginMacro(tr("Swapped Sprites(s)"));
        foreach (Sprite* spr, swappableSprites) {
            undoStack->push(new Commands::SpriteCmd::SetId(spr, selSprite));
        }
        undoStack->endMacro();
    }
}

void EditManager::setLayerMask(LAYER_MASK layer, bool state)
{
    QUndoCommand *layerMaskCmd = new Commands::EditorCmd::SetLayerMask(&layerMask, layer, state);
    undoStack->push(layerMaskCmd);
}

void EditManager::enableSpriteInteraction(bool state)
{
    QUndoCommand *spriteInteractionCmd = new Commands::EditorCmd::SetSpriteInteraction(&spriteInteraction, state);
    undoStack->push(spriteInteractionCmd);
}

void EditManager::enablePathInteraction(bool state)
{
    QUndoCommand *pathInteractionCmd = new Commands::EditorCmd::SetPathInteraction(&pathInteraction, state);
    undoStack->push(pathInteractionCmd);
}

void EditManager::enableEntranceInteraction(bool state)
{
    QUndoCommand *entranceInteractionCmd = new Commands::EditorCmd::SetEntranceInteraction(&entranceInteraction, state);
    undoStack->push(entranceInteractionCmd);
}

void EditManager::enableLocationInteraction(bool state)
{
    QUndoCommand *locationInteractionCmd = new Commands::EditorCmd::SetLocationInteraction(&locationInteraction, state);
    undoStack->push(locationInteractionCmd);
}
