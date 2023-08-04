#include "zoneboundingcommands.h"

namespace Commands::ZoneBoundingCmd {

SetUpScrolling::SetUpScrolling(ZoneBounding *bounding, quint16 value) :
    bounding(bounding),
    newValue(value),
    oldValue(bounding->getUpScrolling()) {
    this->setText(QObject::tr("Set Zone Bounding Upscrolling"));
}

void SetUpScrolling::undo() {
    bounding->setUpScrolling(oldValue);
}

void SetUpScrolling::redo() {
    bounding->setUpScrolling(newValue);
}


SetPrimaryUpperBound::SetPrimaryUpperBound(ZoneBounding *bounding, qint32 value) :
    bounding(bounding),
    newValue(value),
    oldValue(bounding->getPrimaryUpperBound()) {
    this->setText(QObject::tr("Set Zone Bounding Primary Upper Bound"));
}

void SetPrimaryUpperBound::undo() {
    bounding->setPrimaryUpperBound(oldValue);
}

void SetPrimaryUpperBound::redo() {
    bounding->setPrimaryUpperBound(newValue);
}


SetPrimaryLowerBound::SetPrimaryLowerBound(ZoneBounding *bounding, qint32 value) :
    bounding(bounding),
    newValue(value),
    oldValue(bounding->getPrimaryLowerBound()) {
    this->setText(QObject::tr("Set Zone Bounding Primary Lower Bound"));
}

void SetPrimaryLowerBound::undo() {
    bounding->setPrimaryLowerBound(oldValue);
}

void SetPrimaryLowerBound::redo() {
    bounding->setPrimaryLowerBound(newValue);
}


SetSecondaryUpperBound::SetSecondaryUpperBound(ZoneBounding *bounding, qint32 value) :
    bounding(bounding),
    newValue(value),
    oldValue(bounding->getSecondaryUpperBound()) {
    this->setText(QObject::tr("Set Zone Bounding Secondary Upper Bound"));
}

void SetSecondaryUpperBound::undo() {
    bounding->setSecondaryUpperBound(oldValue);
}

void SetSecondaryUpperBound::redo() {
    bounding->setSecondaryUpperBound(newValue);
}


SetSecondaryLowerBound::SetSecondaryLowerBound(ZoneBounding *bounding, qint32 value) :
    bounding(bounding),
    newValue(value),
    oldValue(bounding->getSecondaryLowerBound()) {
    this->setText(QObject::tr("Set Zone Bounding Secondary Lower Bound"));
}

void SetSecondaryLowerBound::undo() {
    bounding->setSecondaryLowerBound(oldValue);
}

void SetSecondaryLowerBound::redo() {
    bounding->setSecondaryLowerBound(newValue);
}


SetId::SetId(ZoneBounding *bounding, quint16 id) :
    bounding(bounding),
    newId(id),
    oldId(bounding->getId()) {
    this->setText(QObject::tr("Set Zone Bounding ID"));
}

void SetId::undo() {
    bounding->setId(oldId);
}

void SetId::redo() {
    bounding->setId(newId);
}

} // namespace Commands::ZoneBoundingCmd

