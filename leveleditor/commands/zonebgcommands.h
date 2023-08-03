#ifndef COMMANDS_ZONEBGCMD_H
#define COMMANDS_ZONEBGCMD_H

#include <QUndoCommand>

#include "objects.h"

namespace Commands::ZoneBgCmd {

class SetParallaxMode : public QUndoCommand
{
public:
    SetParallaxMode(ZoneBackground *background, quint8 parallax);

    void undo() override;
    void redo() override;

private:
    ZoneBackground *const background;
    const quint8 oldParallax;
    const quint8 newParallax;
};


class SetXPos : public QUndoCommand
{
public:
    SetXPos(ZoneBackground *background, qint16 xPos);

    void undo() override;
    void redo() override;

private:
    ZoneBackground *const background;
    const qint16 oldXPos;
    const qint16 newXPos;
};


class SetYPos : public QUndoCommand
{
public:
    SetYPos(ZoneBackground *background, qint16 yPos);

    void undo() override;
    void redo() override;

private:
    ZoneBackground *const background;
    const qint16 oldYPos;
    const qint16 newYPos;
};


class SetId : public QUndoCommand
{
public:
    SetId(ZoneBackground *background, quint16 id);

    void undo() override;
    void redo() override;

private:
    ZoneBackground *const background;
    const quint16 oldId;
    const quint16 newId;
};


class SetName : public QUndoCommand
{
public:
    SetName(ZoneBackground *background, QString id);

    void undo() override;
    void redo() override;

private:
    ZoneBackground *const background;
    const QString oldName;
    const QString newName;
};

} // namespace Commands::ZoneBgCmd

#endif // COMMANDS_ZONEBGCMD_H
