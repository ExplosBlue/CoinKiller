#include "zonecommands.h"

namespace Commands::ZoneCmd {

SetId::SetId(Zone *zone, quint8 id) :
    zone(zone),
    newId(id),
    oldId(zone->getid()) {
    this->setText(QObject::tr("Set Zone ID"));
}

void SetId::undo() {
    zone->setID(oldId);
}

void SetId::redo() {
    zone->setID(newId);
}


SetProgPathId::SetProgPathId(Zone *zone, quint8 id) :
    zone(zone),
    newId(id),
    oldId(zone->getProgPathId()) {
    this->setText(QObject::tr("Set Zone Progress Path ID"));
}

void SetProgPathId::undo() {
    zone->setProgPathId(oldId);
}

void SetProgPathId::redo() {
    zone->setProgPathId(newId);
}


SetMusicId::SetMusicId(Zone *zone, quint8 id) :
    zone(zone),
    newId(id),
    oldId(zone->getMusicId()) {
    this->setText(QObject::tr("Set Zone Music ID"));
}

void SetMusicId::undo() {
    zone->setMusicID(oldId);
}

void SetMusicId::redo() {
    zone->setMusicID(newId);
}


SetMultiplayerTracking::SetMultiplayerTracking(Zone *zone, quint8 tracking) :
    zone(zone),
    newTracking(tracking),
    oldTracking(zone->getMultiplayerTracking()) {
    this->setText(QObject::tr("Set Zone Multiplayer Tracking"));
}

void SetMultiplayerTracking::undo() {
    zone->setMultiplayerTracking(oldTracking);
}

void SetMultiplayerTracking::redo() {
    zone->setMultiplayerTracking(newTracking);
}


SetUnk1::SetUnk1(Zone *zone, quint32 value) :
    zone(zone),
    newValue(value),
    oldValue(zone->getUnk1()) {
    this->setText(QObject::tr("Set Zone Unk1"));
}

void SetUnk1::undo() {
    zone->setUnk1(oldValue);
}

void SetUnk1::redo() {
    zone->setUnk1(newValue);
}


SetBoundingId::SetBoundingId(Zone *zone, quint8 id) :
    zone(zone),
    newId(id),
    oldId(zone->getBoundingId()) {
    this->setText(QObject::tr("Set Zone Bounding ID"));
}

void SetBoundingId::undo() {
    zone->setBoundingId(oldId);
}

void SetBoundingId::redo() {
    zone->setBoundingId(newId);
}


SetBackgroundId::SetBackgroundId(Zone *zone, quint8 id) :
    zone(zone),
    newId(id),
    oldId(zone->getBackgroundId()) {
    this->setText(QObject::tr("Set Zone Background ID"));
}

void SetBackgroundId::undo() {
    zone->setBackgroundId(oldId);
}

void SetBackgroundId::redo() {
    zone->setBackgroundId(newId);
}


SetCameraFlags::SetCameraFlags(Zone *zone, quint8 cameraFlags) :
    zone(zone),
    newValue(cameraFlags),
    oldValue(zone->getCameraFlags()) {
    this->setText(QObject::tr("Set Zone Camera Flags"));
}

void SetCameraFlags::undo() {
    zone->setCameraFlags(oldValue);
}

void SetCameraFlags::redo() {
    zone->setCameraFlags(newValue);
}

} // namespace Commands::ZoneCmd
