#include "levelcommands.h"

namespace Commands::LevelCmd {

RaiseObject::RaiseObject(Level *level, Object *obj) :
    level(level),
    obj(obj) {
    this->setText(QObject::tr("Raised Object"));
}

void RaiseObject::undo() {
    level->lower(obj);
}

void RaiseObject::redo() {
    level->raise(obj);
}


LowerObject::LowerObject(Level *level, Object *obj) :
    level(level),
    obj(obj) {
    this->setText(QObject::tr("Lowered Object"));
}

void LowerObject::undo() {
    level->raise(obj);
}

void LowerObject::redo() {
    level->lower(obj);
}


SetTileset::SetTileset(Level *level, quint8 tsSlot, Tileset* tileset) :
    level(level),
    tsSlot(tsSlot),
    newTs(tileset),
    oldTs(level->tilesets[tsSlot]) {
    this->setText(QObject::tr("Changed Tileset"));
}

SetTileset::~SetTileset() {
    if (newTsDeletable) {
        delete newTs;
    } else {
        delete oldTs;
    }
}

void SetTileset::undo() {
    level->tilesets[tsSlot] = oldTs;
    newTsDeletable = true;
}

void SetTileset::redo() {
    level->tilesets[tsSlot] = newTs;
    newTsDeletable = false;
}


InsertBgdatObj::InsertBgdatObj(Level *level, BgdatObject *obj) :
    level(level),
    obj(obj) {
    this->setText(QObject::tr("Inserted Object at X: %1 Y: %2").arg(obj->getx()).arg(obj->gety()));
}

InsertBgdatObj::~InsertBgdatObj() {
    if (deletable) {
        delete obj;
    }
}

void InsertBgdatObj::undo() {
    level->objects[obj->getLayer()].removeOne(obj);
    deletable = true;
}

void InsertBgdatObj::redo() {
    level->objects[obj->getLayer()].append(obj);
    deletable = false;
}


DeleteBgdatObject::DeleteBgdatObject(Level *level, BgdatObject *obj) :
    level(level),
    obj(obj),
    oldIndex(level->objects[obj->getLayer()].indexOf(obj)) {
    this->setText(QObject::tr("Deleted BgdatObject"));
}

DeleteBgdatObject::~DeleteBgdatObject() {
    if (deletable) {
        delete obj;
    }
}

void DeleteBgdatObject::undo() {
    level->objects[obj->getLayer()].insert(oldIndex, obj);
    deletable = false;
}

void DeleteBgdatObject::redo() {
    level->objects[obj->getLayer()].removeOne(obj);
    deletable = true;
}


RaiseBgdatLayer::RaiseBgdatLayer(Level *level, BgdatObject *obj) :
    level(level),
    obj(obj),
    prevLayer(obj->getLayer()),
    prevIndex(level->objects[prevLayer].indexOf(obj)) {
    this->setText(QObject::tr("Raised Layer of Object"));
}

void RaiseBgdatLayer::undo() {
    level->objects[newLayer].removeOne(obj);
    level->objects[prevLayer].insert(prevIndex, obj);
    obj->setLayer(prevLayer);
}

void RaiseBgdatLayer::redo() {
    level->raiseLayer(obj);
    newLayer = obj->getLayer();
}


LowerBgdatLayer::LowerBgdatLayer(Level *level, BgdatObject *obj) :
    level(level),
    obj(obj),
    prevLayer(obj->getLayer()),
    prevIndex(level->objects[prevLayer].indexOf(obj)) {
    this->setText(QObject::tr("Lowered Layer of Object"));
}

void LowerBgdatLayer::undo() {
    level->objects[newLayer].removeOne(obj);
    level->objects[prevLayer].insert(prevIndex, obj);
    obj->setLayer(prevLayer);
}

void LowerBgdatLayer::redo() {
    level->lowerLayer(obj);
    newLayer = obj->getLayer();
}


InsertSprite::InsertSprite(Level *level, Sprite *spr) :
    level(level),
    spr(spr) {
    this->setText(QObject::tr("Inserted Sprite %1 at X: %2 Y: %3").arg(spr->getid()).arg(spr->getx()).arg(spr->gety()));
}

InsertSprite::~InsertSprite() {
    if (deletable) {
        delete spr;
    }
}

void InsertSprite::undo() {
    level->sprites.removeOne(spr);

    if (level->isCameraLimit(spr)) {
        level->removeCameraLimit(spr);
    }

    deletable = true;
}

void InsertSprite::redo() {
    level->sprites.append(spr);

    if (level->isCameraLimit(spr)) {
        level->insertCameraLimit(spr);
    }

    deletable = false;
}


DeleteSprite::DeleteSprite(Level *level, Sprite *spr) :
    level(level),
    spr(spr),
    oldIndex(level->sprites.indexOf(spr)) {
    this->setText(QObject::tr("Deleted Sprite"));
}

DeleteSprite::~DeleteSprite() {
    if (deletable) {
        delete spr;
    }
}

void DeleteSprite::undo() {
    level->sprites.insert(oldIndex, spr);

    if (level->isCameraLimit(spr)) {
        level->insertCameraLimit(spr);
    }

    deletable = false;
}

void DeleteSprite::redo() {
    level->sprites.removeOne(spr);

    if (level->isCameraLimit(spr)) {
        level->removeCameraLimit(spr);
    }

    deletable = true;
}


InsertEntrance::InsertEntrance(Level *level, Entrance *entr) :
    level(level),
    entr(entr) {
    this->setText(QObject::tr("Inserted Entrance at X: %1 Y: %2").arg(entr->getx()).arg(entr->gety()));
}

InsertEntrance::~InsertEntrance() {
    if (deletable) {
        delete entr;
    }
}

void InsertEntrance::undo() {
    level->entrances.removeOne(entr);
    deletable = true;
}

void InsertEntrance::redo() {
    level->entrances.append(entr);
    deletable = false;
}


DeleteEntrance::DeleteEntrance(Level *level, Entrance *entr) :
    level(level),
    entr(entr),
    oldIndex(level->entrances.indexOf(entr)) {
    this->setText(QObject::tr("Deleted Entrance"));
}

DeleteEntrance::~DeleteEntrance() {
    if (deletable) {
        delete entr;
    }
}

void DeleteEntrance::undo() {
    level->entrances.insert(oldIndex, entr);
    deletable = false;
}

void DeleteEntrance::redo() {
    level->entrances.removeOne(entr);
    deletable = true;
}


InsertZone::InsertZone(Level *level, Zone *zone) :
    level(level),
    zone(zone) {
    this->setText(QObject::tr("Inserted Zone at X: %1 Y: %2").arg(zone->getx()).arg(zone->gety()));
}

InsertZone::~InsertZone() {
    if (deletable) {
        delete zone;
    }
}

void InsertZone::undo() {
    level->zones.removeOne(zone);
    deletable = true;
}

void InsertZone::redo() {
    level->zones.append(zone);
    deletable = false;
}


DeleteZone::DeleteZone(Level *level, Zone *zone) :
    level(level),
    zone(zone),
    oldIndex(level->zones.indexOf(zone)) {
    this->setText(QObject::tr("Deleted Zone"));
}

DeleteZone::~DeleteZone() {
    if (deletable) {
        delete zone;
    }
}

void DeleteZone::undo() {
    level->zones.insert(oldIndex, zone);
    deletable = false;
}

void DeleteZone::redo() {
    level->zones.removeOne(zone);
    deletable = true;
}


InsertZoneBackground::InsertZoneBackground(Level *level, ZoneBackground *background) :
    level(level),
    background(background) {
    this->setText(QObject::tr("Inserted Zone Background"));
}

InsertZoneBackground::~InsertZoneBackground() {
    if (deletable) {
        delete background;
    }
}

void InsertZoneBackground::undo() {
    level->backgrounds.removeOne(background);
    deletable = true;
}

void InsertZoneBackground::redo() {
    level->backgrounds.append(background);
    deletable = false;
}


DeleteZoneBackground::DeleteZoneBackground(Level *level, ZoneBackground *background) :
    level(level),
    background(background) {
    this->setText(QObject::tr("Deleted Zone Background"));
}

DeleteZoneBackground::~DeleteZoneBackground() {
    if (deletable) {
        delete background;
    }
}

void DeleteZoneBackground::undo() {
    level->backgrounds.append(background);
    deletable = false;
}

void DeleteZoneBackground::redo() {
    level->backgrounds.removeOne(background);
    deletable = true;
}


InsertZoneBounding::InsertZoneBounding(Level *level, ZoneBounding *bounding) :
    level(level),
    bounding(bounding) {
    this->setText(QObject::tr("Inserted Zone Bounding"));
}

InsertZoneBounding::~InsertZoneBounding() {
    if (deletable) {
        delete bounding;
    }
}

void InsertZoneBounding::undo() {
    level->boundings.removeOne(bounding);
    deletable = true;
}

void InsertZoneBounding::redo() {
    level->boundings.append(bounding);
    deletable = false;
}


DeleteZoneBounding::DeleteZoneBounding(Level *level, ZoneBounding *bounding) :
    level(level),
    bounding(bounding) {
    this->setText(QObject::tr("Deleted Zone Bounding"));
}

DeleteZoneBounding::~DeleteZoneBounding() {
    if (deletable) {
        delete bounding;
    }
}

void DeleteZoneBounding::undo() {
    level->boundings.append(bounding);
    deletable = false;
}

void DeleteZoneBounding::redo() {
    level->boundings.removeOne(bounding);
    deletable = true;
}


InsertLocation::InsertLocation(Level *level, Location *location) :
    level(level),
    location(location) {
    this->setText(QObject::tr("Inserted Location at X: %1 Y: %2").arg(location->getx()).arg(location->gety()));
}

InsertLocation::~InsertLocation() {
    if (deletable) {
        delete location;
    }
}

void InsertLocation::undo() {
    level->locations.removeOne(location);
    deletable = true;
}

void InsertLocation::redo() {
    level->locations.append(location);
    deletable = false;
}


DeleteLocation::DeleteLocation(Level *level, Location *loc) :
    level(level),
    loc(loc),
    oldIndex(level->locations.indexOf(loc)) {
    this->setText(QObject::tr("Deleted Location"));
}

DeleteLocation::~DeleteLocation() {
    if (deletable) {
        delete loc;
    }
}

void DeleteLocation::undo() {
    level->locations.insert(oldIndex, loc);
    deletable = false;
}

void DeleteLocation::redo() {
    level->locations.removeOne(loc);
    deletable = true;
}


InsertPath::InsertPath(Level *level, Path *path) :
    level(level),
    path(path) {
    this->setText(QObject::tr("Inserted Path %1").arg(path->getid()));
}

InsertPath::~InsertPath() {
    if (deletable) {
        delete path;
    }
}

void InsertPath::undo() {
    level->paths.removeOne(path);
    deletable = true;
}

void InsertPath::redo() {
    level->paths.append(path);
    deletable = false;
}


DeletePathNode::DeletePathNode(Level *level, PathNode *node) :
    level(level),
    node(node),
    path(node->getParentPath()),
    oldNodeIndex(path->getIndexOfNode(node)) {

    if (path->getNumberOfNodes() <= 1) {
        wasLastNode = true;
        oldPathIndex = level->paths.indexOf(path);
    }

    this->setText(QObject::tr("Deleted PathNode"));
}

DeletePathNode::~DeletePathNode() {
    if (deletable) {
        delete node;

        if (wasLastNode) {
            delete path;
        }
    }
}

void DeletePathNode::undo() {
    if (wasLastNode) {
        level->paths.insert(oldPathIndex, path);
    }

    path->insertNode(node, oldNodeIndex);
    deletable = false;
}

void DeletePathNode::redo() {
    path->removeNode(node);

    if (wasLastNode) {
        level->paths.removeOne(path);
    }

    deletable = true;
}


InsertProgressPath::InsertProgressPath(Level *level, ProgressPath *path) :
    level(level),
    path(path) {
    this->setText(QObject::tr("Inserted Progress Path %1").arg(path->getid()));
}

InsertProgressPath::~InsertProgressPath() {
    if (deletable) {
        delete path;
    }
}

void InsertProgressPath::undo() {
    level->progressPaths.removeOne(path);
    deletable = true;
}

void InsertProgressPath::redo() {
    level->progressPaths.append(path);
    deletable = false;
}


DeleteProgressPathNode::DeleteProgressPathNode(Level *level, ProgressPathNode *node) :
    level(level),
    node(node),
    path(node->getParentPath()),
    oldNodeIndex(path->getIndexOfNode(node)) {

    if (path->getNumberOfNodes() <= 1) {
        wasLastNode = true;
        oldPathIndex = level->progressPaths.indexOf(path);
    }

    this->setText(QObject::tr("Deleted ProgressPathNode"));
}

DeleteProgressPathNode::~DeleteProgressPathNode() {
    if (deletable) {
        delete node;

        if (wasLastNode) {
            delete path;
        }
    }
}

void DeleteProgressPathNode::undo() {
    if (wasLastNode) {
        level->progressPaths.insert(oldPathIndex, path);
    }

    path->insertNode(node, oldNodeIndex);
    deletable = false;
}

void DeleteProgressPathNode::redo() {
    path->removeNode(node);

    if (wasLastNode) {
        level->progressPaths.removeOne(path);
    }

    deletable = true;
}

} // namespace Commands::LevelCmd
