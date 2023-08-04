#include "spritecommands.h"

namespace Commands::SpriteCmd {

SetId::SetId(Sprite *spr, quint16 id) :
    spr(spr),
    newId(id),
    oldId(spr->getid()){
    this->setText(QObject::tr("Set Sprite Id"));
}

void SetId::undo() {
    this->spr->setid(oldId);
}

void SetId::redo() {
    this->spr->setid(newId);
}


SetLayer::SetLayer(Sprite *spr, quint8 layer) :
    spr(spr),
    newLayer(layer),
    oldLayer(spr->getLayer()) {
    this->setText(QObject::tr("Set Sprite Layer"));
}

void SetLayer::undo() {
    this->spr->setLayer(oldLayer);
}

void SetLayer::redo() {
    this->spr->setLayer(newLayer);
}


SetBits::SetBits(Sprite *spr, qint32 data, qint32 start, qint32 end) :
    spr(spr),
    newData(data),
    oldData(spr->getNybbleData(start, end)),
    start(start),
    end(end) {
    this->setText(QObject::tr("Set Sprite Bits"));
}

void SetBits::undo() {
    this->spr->setBits(oldData, start, end);
    this->spr->setRect();
}

void SetBits::redo() {
    this->spr->setBits(newData, start, end);
    this->spr->setRect();
}


SetByte::SetByte(Sprite *spr, qint32 byteIndex, quint8 value) :
    spr(spr),
    byteIndex(byteIndex),
    newValue(value),
    oldValue(spr->getByte(byteIndex)) {
    this->setText(QObject::tr("Set Byte"));
}

void SetByte::undo() {
    this->spr->setByte(byteIndex, oldValue);
    this->spr->setRect();
}

void SetByte::redo() {
    this->spr->setByte(byteIndex, newValue);
    this->spr->setRect();
}


SetNybbleData::SetNybbleData(Sprite *spr, qint32 data, qint32 start, qint32 end) :
    spr(spr),
    newData(data),
    oldData(spr->getNybbleData(start, end)),
    start(start),
    end(end) {
    this->setText(QObject::tr("Set Sprite Nybble Data"));
}

void SetNybbleData::undo() {
    this->spr->setNybbleData(oldData, start, end);
    this->spr->setRect();
}

void SetNybbleData::redo() {
    this->spr->setNybbleData(newData, start, end);
    this->spr->setRect();
}

} // namespace Commands::SpriteCmd
