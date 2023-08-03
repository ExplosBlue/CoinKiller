#include "objectcommands.h"

namespace Commands::ObjectCmd {

IncreasePosition::IncreasePosition(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap) :
    obj(obj),
    deltaX(deltaX),
    deltaY(deltaY),
    snap(snap),
    oldX(obj->getx()),
    oldY(obj->gety()) {
    this->setText(QObject::tr("Increased object position"));
}

void IncreasePosition::undo() {
    obj->setPosition(oldX, oldY);
}

void IncreasePosition::redo() {
    obj->increasePosition(deltaX, deltaY, snap);
}

bool IncreasePosition::mergeWith(const QUndoCommand *otherCommand) {
    if (otherCommand->id() != this->id())
        return false;

    const IncreasePosition* otherIncrease = static_cast<const IncreasePosition*>(otherCommand);

    if (otherIncrease->obj != this->obj) {
        return false;
    }

    deltaX += otherIncrease->deltaX;
    deltaY += otherIncrease->deltaY;
    return true;
}


Move::Move(Object *obj, quint32 x, quint32 y) :
    obj(obj),
    newX(x),
    newY(y),
    oldX(obj->getx()),
    oldY(obj->gety()) {
    this->setText(QObject::tr("Object Moved to X: %1 Y: %2").arg(newX).arg(newY));
}

void Move::undo() {
    obj->setPosition(oldX, oldY);
}

void Move::redo() {
    obj->setPosition(newX, newY);
}

bool Move::mergeWith(const QUndoCommand *otherCommand) {
    if (otherCommand->id() != this->id())
        return false;

    const Move* otherMove = static_cast<const Move*>(otherCommand);

    if (otherMove->obj != this->obj) {
        return false;
    }

    newX = otherMove->newX;
    newY = otherMove->newY;
    return true;
}


Resize::Resize(Object *obj, quint32 w, quint32 h) :
    obj(obj),
    oldW(obj->getwidth()),
    oldH(obj->getheight()),
    newW(w),
    newH(h) {
    this->setText(QObject::tr("Object Resized to W: %1 H: %2").arg(newW).arg(newH));
}

void Resize::undo() {
    obj->resize(oldW, oldH);
}

void Resize::redo() {
    obj->resize(newW, newH);
}

bool Resize::mergeWith(const QUndoCommand *otherCommand) {
    if (otherCommand->id() != this->id())
        return false;

    const Resize* otherResize = static_cast<const Resize*>(otherCommand);

    if (otherResize->obj != this->obj) {
        return false;
    }

    newW = otherResize->newW;
    newH = otherResize->newH;
    return true;
}


IncreaseSize::IncreaseSize(Object *obj, quint32 deltaX, quint32 deltaY, quint32 snap) :
    obj(obj),
    deltaX(deltaX),
    deltaY(deltaY),
    snap(snap),
    oldW(obj->getwidth()),
    oldH(obj->getheight()) {
    this->setText(QObject::tr("Increased object size"));
}

void IncreaseSize::undo() {
    obj->resize(oldW, oldH);
}

void IncreaseSize::redo() {
    obj->increaseSize(deltaX, deltaY, snap);
}

} // namespace Commands::ObjectCmd
