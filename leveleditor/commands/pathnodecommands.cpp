#include "pathnodecommands.h"

namespace Commands::PathNodeCmd {

SetSpeed::SetSpeed(PathNode *node, float speed) :
    node(node),
    newSpeed(speed),
    oldSpeed(node->getSpeed()) {
    this->setText(QObject::tr("Changed Path Node Speed"));
}

void SetSpeed::undo() {
    node->setSpeed(oldSpeed);
}

void SetSpeed::redo() {
    node->setSpeed(newSpeed);
}


SetAccel::SetAccel(PathNode *node, float accel) :
    node(node),
    newAccel(accel),
    oldAccel(node->getAccel()) {
    this->setText(QObject::tr("Changed Path Node Acceleration"));
}

void SetAccel::undo() {
    node->setAccel(oldAccel);
}

void SetAccel::redo() {
    node->setAccel(newAccel);
}


SetDelay::SetDelay(PathNode *node, quint16 delay) :
    node(node),
    newDelay(delay),
    oldDelay(node->getDelay()) {
    this->setText(QObject::tr("Changed Path Node Delay"));
}

void SetDelay::undo() {
    node->setDelay(oldDelay);
}

void SetDelay::redo() {
    node->setDelay(newDelay);
}


SetRotation::SetRotation(PathNode *node, qint16 rotation) :
    node(node),
    newRotation(rotation),
    oldRotation(node->getRotation()) {
    this->setText(QObject::tr("Changed Path Node Rotation"));
}

void SetRotation::undo() {
    node->setRotation(oldRotation);
}

void SetRotation::redo() {
    node->setRotation(newRotation);
}


SetVariableField::SetVariableField(PathNode *node, quint8 value) :
    node(node),
    newValue(value),
    oldValue(node->getVariableField()) {
    this->setText(QObject::tr("Changed Path Node Variable Field"));
}

void SetVariableField::undo() {
    node->setVariableField(oldValue);
}

void SetVariableField::redo() {
    node->setVariableField(newValue);
}


SetNextPathID::SetNextPathID(PathNode *node, quint8 id) :
    node(node),
    newId(id),
    oldId(node->getNextPathID()) {
    this->setText(QObject::tr("Changed Path Node Next Path ID"));
}

void SetNextPathID::undo() {
    node->setNextPathID(oldId);
}

void SetNextPathID::redo() {
    node->setNextPathID(newId);
}

} // namespace Commands::PathNodeCmd
