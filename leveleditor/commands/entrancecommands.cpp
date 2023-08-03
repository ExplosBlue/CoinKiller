#include "entrancecommands.h"

namespace Commands::EntranceCmd {

SetType::SetType(Entrance *entr, quint8 type) :
    entr(entr),
    newType(type),
    oldType(entr->getEntrType()) {
    this->setText(QObject::tr("Set Entrance Type"));
}

void SetType::undo() {
    this->entr->setEntrType(oldType);
    this->entr->setRect();
}

void SetType::redo() {
    this->entr->setEntrType(newType);
    this->entr->setRect();
}


SetId::SetId(Entrance *entr, quint8 id) :
    entr(entr),
    newId(id),
    oldId(entr->getid()) {
    this->setText(QObject::tr("Set Entrance ID"));
}

void SetId::undo() {
    this->entr->setId(oldId);
}

void SetId::redo() {
    this->entr->setId(newId);
}


SetDestEntr::SetDestEntr(Entrance *entr, quint8 destEntrId) :
    entr(entr),
    newDest(destEntrId),
    oldDest(entr->getDestEntr()) {
    this->setText(QObject::tr("Set Entrance Dest Entrance"));
}

void SetDestEntr::undo() {
    this->entr->setDestEntr(oldDest);
}

void SetDestEntr::redo() {
    this->entr->setDestEntr(newDest);
}


SetDestArea::SetDestArea(Entrance *entr, quint8 destAreaId) :
    entr(entr),
    newDest(destAreaId),
    oldDest(entr->getDestArea()) {
    this->setText(QObject::tr("Set Entrance Dest Area"));
}

void SetDestArea::undo() {
    this->entr->setDestArea(oldDest);
}

void SetDestArea::redo() {
    this->entr->setDestArea(newDest);
}


SetCamXOffset::SetCamXOffset(Entrance *entr, qint16 xOffset) :
    entr(entr),
    newXOffset(xOffset),
    oldXOffset(entr->getCameraX()) {
    this->setText(QObject::tr("Set Entrance Camera X Offset"));
}

void SetCamXOffset::undo() {
    this->entr->setCameraX(oldXOffset);
}

void SetCamXOffset::redo() {
    this->entr->setCameraX(newXOffset);
}


SetCamYOffset::SetCamYOffset(Entrance *entr, qint16 yOffset) :
    entr(entr),
    newYOffset(yOffset),
    oldYOffset(entr->getCameraY()) {
    this->setText(QObject::tr("Set Entrance Camera Y Offset"));
}

void SetCamYOffset::undo() {
    this->entr->setCameraY(oldYOffset);
}

void SetCamYOffset::redo() {
    this->entr->setCameraY(newYOffset);
}


SetUnk1::SetUnk1(Entrance *entr, quint8 unk1) :
    entr(entr),
    newUnk1(unk1),
    oldUnk1(entr->getUnk1()) {
    this->setText(QObject::tr("Set Entrance Unk1"));
}

void SetUnk1::undo() {
    this->entr->setUnk1(oldUnk1);
}

void SetUnk1::redo() {
    this->entr->setUnk1(newUnk1);
}


SetUnk2::SetUnk2(Entrance *entr, quint8 unk2) :
    entr(entr),
    newUnk2(unk2),
    oldUnk2(entr->getUnk2()) {
    this->setText(QObject::tr("Set Entrance Unk2"));
}

void SetUnk2::undo() {
    this->entr->setUnk2(oldUnk2);
}

void SetUnk2::redo() {
    this->entr->setUnk2(newUnk2);
}


SetSettingsBit::SetSettingsBit(Entrance *entr, bool value, qint32 bit) :
    entr(entr),
    bitValue(value),
    bitIndex(bit) {
    this->setText(QObject::tr("Set Entrance Setting Bit"));
}

void SetSettingsBit::undo() {
    this->entr->setSettingsBit(!bitValue, bitIndex);
}

void SetSettingsBit::redo() {
    this->entr->setSettingsBit(bitValue, bitIndex);
}

} // namespace Commands::EntranceCmd
