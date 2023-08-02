#include "zonebgcommands.h"

#include <utility>

namespace Commands::ZoneBgCmd {

SetParallaxMode::SetParallaxMode(ZoneBackground *background, quint8 parallax) :
    background(background),
    oldParallax(background->getParallaxMode()),
    newParallax(parallax) {
    this->setText(QObject::tr("Set Zone Background Parallax Mode"));
}

void SetParallaxMode::undo() {
    background->setParallaxMode(oldParallax);
}

void SetParallaxMode::redo() {
    background->setParallaxMode(newParallax);
}


SetXPos::SetXPos(ZoneBackground *background, qint16 xPos) :
    background(background),
    oldXPos(background->getXPos()),
    newXPos(xPos) {
    this->setText(QObject::tr("Set Zone Background X Pos"));
}

void SetXPos::undo() {
    background->setXPos(oldXPos);
}

void SetXPos::redo() {
    background->setXPos(newXPos);
}


SetYPos::SetYPos(ZoneBackground *background, qint16 xPos) :
    background(background),
    oldYPos(background->getYPos()),
    newYPos(xPos) {
    this->setText(QObject::tr("Set Zone Background Y Pos"));
}

void SetYPos::undo() {
    background->setYPos(oldYPos);
}

void SetYPos::redo() {
    background->setYPos(newYPos);
}


SetId::SetId(ZoneBackground *background, quint16 id) :
    background(background),
    oldId(background->getId()),
    newId(id) {
    this->setText(QObject::tr("Set Zone Background ID"));
}

void SetId::undo() {
    background->setId(oldId);
}

void SetId::redo() {
    background->setId(newId);
}


SetName::SetName(ZoneBackground *background, QString name) :
    background(background),
    oldName(background->getName()),
    newName(std::move(name)) {
    this->setText(QObject::tr("Set Zone Background Name"));
}

void SetName::undo() {
    background->setName(oldName);
}

void SetName::redo() {
    background->setName(newName);
}

} // namespace Commands::ZoneBgCmd
