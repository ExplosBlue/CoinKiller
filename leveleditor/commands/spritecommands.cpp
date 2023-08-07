#include "spritecommands.h"

namespace Commands::SpriteCmd {

SetId::SetId(Sprite *spr, quint16 id) :
    spr(spr),
    newId(id),
    oldId(spr->getid()) {
    updateText();

    if (newId == oldId) {
        this->setObsolete(true);
    }
}

void SetId::undo() {
    this->spr->setid(oldId);
}

void SetId::redo() {
    this->spr->setid(newId);
}

bool SetId::mergeWith(const QUndoCommand *cmd) {
    if (cmd->id() != this->id()) {
        return false;
    }

    const SetId *other = static_cast<const SetId*>(cmd);
    if (other->spr != this->spr) {
        return false;
    }

    newId = other->newId;
    updateText();

    if (newId == oldId) {
        this->setObsolete(true);
    }

    return true;
}

void SetId::updateText() {
    this->setText(QObject::tr("Sprite | Changed ID from %1 to %2").arg(oldId).arg(newId));
}


SetLayer::SetLayer(Sprite *spr, quint8 layer) :
    spr(spr),
    newLayer(layer),
    oldLayer(spr->getLayer()) {
    updateText();

    if (newLayer == oldLayer) {
        this->setObsolete(true);
    }
}

void SetLayer::undo() {
    this->spr->setLayer(oldLayer);
}

void SetLayer::redo() {
    this->spr->setLayer(newLayer);
}

bool SetLayer::mergeWith(const QUndoCommand *cmd) {
    if (cmd->id() != this->id()) {
        return false;
    }

    const SetLayer *other = static_cast<const SetLayer*>(cmd);
    if (other->spr != this->spr) {
        return false;
    }

    newLayer = other->newLayer;
    updateText();

    if (newLayer == oldLayer) {
        this->setObsolete(true);
    }

    return true;
}

void SetLayer::updateText() {

    QString oldLayerStr = (oldLayer) ? "Layer 2" : "Layer 1";
    QString newLayerStr = (newLayer) ? "Layer 2" : "Layer 1";

    this->setText(QObject::tr("Sprite | Changed layer from %1 to %2").arg(oldLayerStr, newLayerStr));
}


SetBits::SetBits(Sprite *spr, qint32 data, qint32 start, qint32 end) :
    spr(spr),
    newData(data),
    oldData(spr->getBits(start, end)),
    start(start),
    end(end) {
    updateText();

    if (newData == oldData) {
        this->setObsolete(true);
    }
}

void SetBits::undo() {
    this->spr->setBits(oldData, start, end);
    this->spr->setRect();
}

void SetBits::redo() {
    this->spr->setBits(newData, start, end);
    this->spr->setRect();
}

bool SetBits::mergeWith(const QUndoCommand *cmd) {
    if (cmd->id() != this->id()) {
        return false;
    }

    const SetBits *other = static_cast<const SetBits*>(cmd);
    if ((other->spr != this->spr) || (other->start != this->start) || (other->end != this->end)) {
        return false;
    }

    newData = other->newData;
    updateText();

    if (newData == oldData) {
        this->setObsolete(true);
    }

    return true;
}

void SetBits::updateText() {
    if (start == end) {
        this->setText(QObject::tr("Sprite | Changed bit %1 from %2 to %3").arg(start).arg(oldData, 0, 16).arg(newData, 0, 16));
    } else {
        this->setText(QObject::tr("Sprite | Changed bits %1-%2 from %3 to %4").arg(start).arg(end).arg(oldData, 0, 16).arg(newData, 0, 16));
    }
}


SetByte::SetByte(Sprite *spr, qint32 byteIndex, quint8 value) :
    spr(spr),
    byteIndex(byteIndex),
    newValue(value),
    oldValue(spr->getByte(byteIndex)) {
    updateText();

    if (newValue == oldValue) {
        this->setObsolete(true);
    }
}

void SetByte::undo() {
    this->spr->setByte(byteIndex, oldValue);
    this->spr->setRect();
}

void SetByte::redo() {
    this->spr->setByte(byteIndex, newValue);
    this->spr->setRect();
}

bool SetByte::mergeWith(const QUndoCommand *cmd) {
    if (cmd->id() != this->id()) {
        return false;
    }

    const SetByte *other = static_cast<const SetByte*>(cmd);
    if ((other->spr != this->spr) || (other->byteIndex != this->byteIndex)) {
        return false;
    }

    newValue = other->newValue;
    updateText();

    if (newValue == oldValue) {
        this->setObsolete(true);
    }

    return true;
}

void SetByte::updateText() {
    this->setText(QObject::tr("Sprite | Changed byte %1 from %2 to %3").arg(byteIndex).arg(oldValue, 0, 16).arg(newValue, 0, 16));
}


SetNybbleData::SetNybbleData(Sprite *spr, qint32 data, qint32 start, qint32 end) :
    spr(spr),
    newData(data),
    oldData(spr->getNybbleData(start, end)),
    start(start),
    end(end) {
    updateText();

    if (newData == oldData) {
        this->setObsolete(true);
    }
}

void SetNybbleData::undo() {
    this->spr->setNybbleData(oldData, start, end);
    this->spr->setRect();
}

void SetNybbleData::redo() {
    this->spr->setNybbleData(newData, start, end);
    this->spr->setRect();
}

bool SetNybbleData::mergeWith(const QUndoCommand *cmd) {
    if (cmd->id() != this->id()) {
        return false;
    }

    const SetNybbleData *other = static_cast<const SetNybbleData*>(cmd);
    if ((other->spr != this->spr) || (other->start != this->start) || (other->end != this->end)) {
        return false;
    }

    newData = other->newData;
    updateText();

    if (newData == oldData) {
        this->setObsolete(true);
    }

    return true;
}

void SetNybbleData::updateText() {
    if (start == end) {
        this->setText(QObject::tr("Sprite | Changed nybble %1 from %2 to %3").arg(start).arg(oldData, 0, 16).arg(newData, 0, 16));
    } else {
        this->setText(QObject::tr("Sprite | Changed nybbles %1-%2 from %3 to %4").arg(start).arg(end).arg(oldData, 0, 16).arg(newData, 0, 16));
    }
}

} // namespace Commands::SpriteCmd
