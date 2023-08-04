#ifndef COMMANDS_LEVELCMD_H
#define COMMANDS_LEVELCMD_H

#include <QUndoCommand>

#include "level.h"

namespace Commands::LevelCmd {

class RaiseObject : public QUndoCommand
{
public:
    RaiseObject(Level *level, Object *obj);

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Object *const obj;
};


class LowerObject : public QUndoCommand
{
public:
    LowerObject(Level *level, Object *obj);

    void undo() override;
    void redo() override;

private:
    Level *level;
    Object *obj;
};


class SetTileset : public QUndoCommand
{
public:
    SetTileset(Level *level, quint8 tsSlot, Tileset* tileset);
    ~SetTileset() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    const quint8 tsSlot;
    Tileset *newTs;
    Tileset *oldTs;
    bool newTsDeletable = false;
};


class InsertBgdatObj : public QUndoCommand
{
public:
    InsertBgdatObj(Level *level, BgdatObject *obj);
    ~InsertBgdatObj() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    BgdatObject *obj;
    bool deletable = false;
};


class DeleteBgdatObject : public QUndoCommand
{
public:
    DeleteBgdatObject(Level *level, BgdatObject *obj);
    ~DeleteBgdatObject() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    BgdatObject *obj;
    quint32 oldIndex;
    bool deletable = false;
};


class RaiseBgdatLayer : public QUndoCommand
{
public:
    RaiseBgdatLayer(Level *level, BgdatObject *obj);

    void undo() override;
    void redo() override;

private:
    Level *level;
    BgdatObject *obj;
    quint32 prevLayer;
    quint32 newLayer;
    quint32 prevIndex;
};


class LowerBgdatLayer : public QUndoCommand
{
public:
    LowerBgdatLayer(Level *level, BgdatObject *obj);

    void undo() override;
    void redo() override;

private:
    Level *level;
    BgdatObject *obj;
    quint32 prevLayer;
    quint32 newLayer;
    quint32 prevIndex;
};


class InsertSprite : public QUndoCommand
{
public:
    InsertSprite(Level *level, Sprite *spr);
    ~InsertSprite() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Sprite *spr;
    bool deletable = false;
};


class DeleteSprite : public QUndoCommand
{
public:
    DeleteSprite(Level *level, Sprite *spr);
    ~DeleteSprite() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Sprite *spr;
    quint32 oldIndex;
    bool deletable;
};


class InsertEntrance : public QUndoCommand
{
public:
    InsertEntrance(Level *level, Entrance *entr);
    ~InsertEntrance() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Entrance *entr;
    bool deletable = false;
};


class DeleteEntrance : public QUndoCommand
{
public:
    DeleteEntrance(Level *level, Entrance *entr);
    ~DeleteEntrance() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Entrance *entr;
    quint32 oldIndex;
    bool deletable = false;
};


class InsertZone : public QUndoCommand
{
public:
    InsertZone(Level *level, Zone *zone);
    ~InsertZone() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Zone *zone;
    bool deletable = false;
};


class DeleteZone : public QUndoCommand
{
public:
    DeleteZone(Level *level, Zone *zone);
    ~DeleteZone() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Zone *zone;
    quint32 oldIndex;
    bool deletable = false;
};


class InsertZoneBackground : public QUndoCommand
{
public:
    InsertZoneBackground(Level *level, ZoneBackground *background);
    ~InsertZoneBackground() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ZoneBackground *background;
    bool deletable = false;
};


class DeleteZoneBackground : public QUndoCommand
{
public:
    DeleteZoneBackground(Level *level, ZoneBackground *background);
    ~DeleteZoneBackground() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ZoneBackground *background;
    bool deletable = false;
};


class InsertZoneBounding : public QUndoCommand
{
public:
    InsertZoneBounding(Level *level, ZoneBounding *bounding);
    ~InsertZoneBounding() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ZoneBounding *bounding;
    bool deletable = false;
};


class DeleteZoneBounding : public QUndoCommand
{
public:
    DeleteZoneBounding(Level *level, ZoneBounding *bounding);
    ~DeleteZoneBounding() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ZoneBounding *bounding;
    bool deletable = false;
};


class InsertLocation : public QUndoCommand
{
public:
    InsertLocation(Level *level, Location *location);
    ~InsertLocation() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Location *location;
    bool deletable = false;
};


class DeleteLocation : public QUndoCommand
{
public:
    DeleteLocation(Level *level, Location *loc);
    ~DeleteLocation() override;

    void undo() override;
    void redo() override;

private:
    Level *level;
    Location *loc;
    quint32 oldIndex;
    bool deletable = false;
};


class InsertPath : public QUndoCommand
{
public:
    InsertPath(Level *level, Path *path);
    ~InsertPath() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    Path *path;
    bool deletable = false;
};


class DeletePathNode : public QUndoCommand
{
public:
    DeletePathNode(Level *level, PathNode *node);
    ~DeletePathNode() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    PathNode *node;
    Path *path;
    quint32 oldNodeIndex;
    quint32 oldPathIndex;
    bool wasLastNode = false;
    bool deletable = false;
};


class InsertProgressPath : public QUndoCommand
{
public:
    InsertProgressPath(Level *level, ProgressPath *path);
    ~InsertProgressPath() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ProgressPath *path;
    bool deletable = false;
};


class DeleteProgressPathNode : public QUndoCommand
{
public:
    DeleteProgressPathNode(Level *level, ProgressPathNode *node);
    ~DeleteProgressPathNode() override;

    void undo() override;
    void redo() override;

private:
    Level *const level;
    ProgressPathNode *node;
    ProgressPath *path;
    quint32 oldNodeIndex;
    quint32 oldPathIndex;
    bool wasLastNode = false;
    bool deletable = false;
};

} // namespace Commands::LevelCmd

#endif // EDITORCOMMAND_LEVELCMD_H
