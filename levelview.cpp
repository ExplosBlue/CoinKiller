/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

#include "leveleditorwindow.h"
#include "levelview.h"
#include "unitsconvert.h"

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QRectF>
#include <QPaintEvent>


LevelView::LevelView(QWidget *parent, Level* level) : QWidget(parent)
{
    this->level = level;

    layerMask = 0x7; // failsafe
    selType = 0;

    zoom = 1;
}


void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.scale(zoom,zoom);

    QRect drawrect(evt->rect().x()/zoom, evt->rect().y()/zoom, evt->rect().width()/zoom, evt->rect().height()/zoom);


    //qDebug("draw %d,%d %d,%d", drawrect.x(), drawrect.y(), drawrect.width(), drawrect.height());

    painter.fillRect(drawrect, QColor(119,136,153));
    tileGrid.clear();

    // Render Tiles
    for (int l = 1; l >= 0; l--)
    {
        if (!(layerMask & (1<<l)))
            continue;

        tileGrid[0xFFFFFFFF] = l+1;

        for (int i = level->objects[l].size()-1; i >= 0; i--)
        {
            const BgdatObject& obj = level->objects[l].at(i);

            // don't draw shit that is outside of the view
            // (TODO: also eliminate individual out-of-view tiles)
            if (!drawrect.intersects(QRect(obj.getx(), obj.gety(), obj.getwidth(), obj.getheight())))
                continue;

            quint16 tsid = (obj.getid() >> 12) & 0x3;
            if (level->tilesets[tsid])
            {
                level->tilesets[tsid]->drawObject(painter, tileGrid, obj.getid()&0x0FFF, obj.getx()/20, obj.gety()/20, obj.getwidth()/20, obj.getheight()/20, 1);
            }
            else
            {
                // TODO fallback
                qDebug("attempt to draw obj %04X with non-existing tileset", obj.getid());
            }
        }
    }

    painter.setRenderHint(QPainter::Antialiasing);

    // Render Locations
    for (int i = 0; i < level->locations.size(); i++)
    {
        const Location& loc = level->locations.at(i);

        QRect locrect(loc.getx(), loc.gety(), loc.getwidth(), loc.getheight());

        if (!drawrect.intersects(locrect))
            continue;

        painter.fillRect(locrect, QBrush(QColor(255,255,0,100)));

        painter.setPen(QColor(0,0,0));
        painter.drawRect(locrect);

        QString locText = QString("%1").arg(loc.getid());
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.setPen(QColor(255,255,255));
        painter.drawText(locrect.adjusted(5,5,0,0), locText);
    }

    // Render Sprites
    for (int i = 0; i < level->sprites.size(); i++)
    {
        const Sprite& spr = level->sprites.at(i);

        QRect sprRect(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight());

        if (!drawrect.intersects(sprRect))
            continue;

        QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

        switch (spr.getid()) {
        case 8: // Swoop
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "swoop.png"));
            break;
        case 22: // Special Exit Controller
            painter.fillRect(sprRect, QBrush(QColor(50,255,0,100)));
            painter.setPen(QColor(0,0,0));
            painter.drawRect(sprRect);
            painter.setFont(QFont("Arial", 8, QFont::Bold));
            painter.drawText(sprRect.adjusted(3,3,0,0), "Warp");
            break;
        case 27: // POW Block
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "pow_block.png"));
            break;
        case 29: // Bob-omb
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "bob-omb.png"));
            break;
        case 35: // Lava Bubble
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "lava_bubble.png"));
            break;
        case 52: // Checkpoint Flag
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "checkpoint_flag.png"));
            break;
        case 55: // Coin
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "coin.png"));
            break;
        case 66: // Pipe Cannon
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "pipe_cannon.png"));
            break;
        case 69: // Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door.png"));
            break;
        case 70: // Castle Boss Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door_castle.png"));
            break;
        case 71: // Closed Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door_closed.png"));
            break;
        case 73: // Ghost House Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door_ghosthouse.png"));
            break;
        case 75: // Tower Boss Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door_tower.png"));
            break;
        case 77: // Thwomp
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "thwomp.png"));
            break;
        case 97: // End of Level Flag
            if (spr.getNybble(10) == 0)
            {
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "flagpole.png"));
                painter.drawPixmap(spr.getx()+200, spr.gety()+80, 120, 120,QPixmap(basePath + "castle.png"));
            }
            else
            {
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "flagpole_secret.png"));
                painter.drawPixmap(spr.getx()+200, spr.gety()+80, 120, 120,QPixmap(basePath + "castle_secret.png"));
            }
            break;
        case 95: // Blooper
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "blooper.png"));
            break;
        case 99: // Wiggler
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "wiggler.png"));
            break;
        case 106: // Trampoline
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "trampoline.png"));
            break;
        case 109: // Signboard
            switch (spr.getNybble(5)) {
            case 0:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_0.png"));
                break;
            case 1:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_1.png"));
                break;
            case 2:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_2.png"));
                break;
            case 3:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_3.png"));
                break;
            case 4:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_4.png"));
                break;
            case 5:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_5.png"));
                break;
            case 6:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_6.png"));
                break;
            case 8:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_8.png"));
                break;
            case 9:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_9.png"));
                break;
            default:
                painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "sign_7.png"));
                break;
            }
            break;

        case 110: // Dry Bones
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "dry_bones.png"));
            break;
        case 111: // Giant Dry Bones
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "giant_dry_bones.png"));
            break;
        case 135: // Goomba
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "goomba.png"));
            break;
        case 136: // Bone Goomba
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "bone_goomba.png"));
            break;
        case 137: // Micro Goomba
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "micro_goomba.png"));
            break;
        case 138: // Paragoomba
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "paragoomba.png"));
            break;
        case 139: // Goomba Tower
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+1, spr.getwidth(), 21, QPixmap(basePath + "goomba_tower_bottom.png"));
            for (int i = 0; i < (spr.getNybble(5)-2); i++) painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()-21*(i+1)+1, spr.getwidth(), 21, QPixmap(basePath + "goomba_tower_middle.png"));
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), 25, QPixmap(basePath + "goomba_tower_top.png"));
            break;
        case 158: // Buzzy Beetle
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "buzzy_beetle.png"));
            break;
        case 165: // Koopa Troopa
            if (spr.getNybble(5) == 0) painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "koopa_troopa_green.png"));
            else painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "koopa_troopa_red.png")); // Needs new image
            break;
        case 185: // Koopa Troopa
            if (spr.getNybble(5) == 0) painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "koopa_paratroopa_green.png"));
            else painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "koopa_paratroopa_red.png"));
            break;
        case 175: // Grounded Piranha Plant
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "piranha_plant.png"));
            break;
        case 184: // Parabomb
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "parabomb.png"));
            break;
        case 219: // Star Coin
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "star_coin.png"));
            break;
        case 221: // ! Switch
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "exclamation_switch.png"));
            break;
        case 223: // ? Switch
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "question_switch.png"));
            break;
        case 225: // P Switch
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "p_switch.png"));
            break;
        case 234: // Spiked Ball
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "spiked_ball.png"));
            break;
        case 267: // Long Question Block
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "long_question_block.png"));
            break;
        case 273: // Coin Roulette Block
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "coin_roulette_block.png"));
            break;
        case 274: // Flying Gold Block Spawn Point
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "flying_gold_block.png"));
            break;
        case 278: // Assist Block
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "assist_block.png"));
            break;
        case 279: // Lemmy Ball
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "lemmy_ball.png"));
            break;
        case 287: // Toad House Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door_toadhouse.png"));
            break;
        case 294: // Warp Cannon
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "warp_cannon.png"));
            break;
        case 296: // Toad
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "toad.png"));
            break;
        default:
            QRect sprrect(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight());

            painter.setPen(QColor(0,0,0));

            QPainterPath path;
            path.addRoundedRect(sprrect, 2.0, 2.0);
            QColor color(0,90,150,200);
            painter.fillPath(path, color);
            painter.drawPath(path);

            QString spriteText = QString("%1").arg(spr.getid());
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(sprrect, spriteText, Qt::AlignHCenter | Qt::AlignVCenter);
            break;
        }
    }

    // Render Entrences
    for (int i = 0; i < level->entrances.size(); i++)
    {
        const Entrance& entr = level->entrances.at(i);

        QRect entrrect(entr.getx(), entr.gety(), 20, 20);

        if (!drawrect.intersects(entrrect))
            continue;

        painter.setPen(QColor(0,0,0));

        QPainterPath path;
        path.addRoundedRect(entrrect, 2.0, 2.0);
        QColor color(182,3,3,200);
        painter.fillPath(path, color);
        painter.drawPath(path);

        QString entrText = QString("%1").arg(entr.getid());
        painter.setFont(QFont("Arial", 7, QFont::Normal));
        painter.drawText(entrrect, entrText, Qt::AlignHCenter | Qt::AlignVCenter);
    }

    // Render Paths
    for (int i = 0; i < level->paths.size(); i++)
    {
        const Path& path = level->paths.at(i);
        QList<PathNode> nodes  = path.getNodes();

        for (int j = 0; j < nodes.size() - 1; j++)
        {
            QLine pathLine(QPoint(nodes[j].getx()+10, nodes[j].gety()+10), QPoint(nodes[j+1].getx()+10, nodes[j+1].gety()+10));

            if (!drawrect.intersects(QRect(pathLine.x1(), pathLine.y1(), pathLine.x2()-pathLine.x1(), pathLine.y2()-pathLine.y1())))
                continue;

            QPen pen(QColor(0,255,20));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(pathLine);
        }

        for (int j = 0; j < nodes.size(); j++)
        {
            QRect pathrect(nodes[j].getx(), nodes[j].gety(), 20, 20);

            if (!drawrect.intersects(pathrect))
                continue;

            painter.setPen(QColor(0,0,0));

            QPainterPath painterPath;
            painterPath.addRoundedRect(pathrect, 2.0, 2.0);
            QColor color(0,255,20,200);
            painter.fillPath(painterPath, color);
            painter.drawPath(painterPath);

            QString pathText = QString("%1-%2").arg(path.getid()).arg(j+1);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(pathrect, pathText, Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // Render Progress Paths
    for (int i = 0; i < level->progressPaths.size(); i++)
    {
        const ProgressPath& pPath = level->progressPaths.at(i);
        QList<ProgressPathNode> nodes  = pPath.getNodes();

        for (int j = 0; j < nodes.size() - 1; j++)
        {
            QLine ppathLine(QPoint(nodes[j].getx()+10, nodes[j].gety()+10), QPoint(nodes[j+1].getx()+10, nodes[j+1].gety()+10));

            if (!drawrect.intersects(QRect(ppathLine.x1(), ppathLine.y1(), ppathLine.x2()-ppathLine.x1(), ppathLine.y2()-ppathLine.y1())))
                continue;

            QPen pen(QColor(220,255,0));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(ppathLine);
        }

        for (int j = 0; j < nodes.size(); j++)
        {
            QRect ppathrect(nodes[j].getx(), nodes[j].gety(), 20, 20);

            if (!drawrect.intersects(ppathrect))
                continue;

            painter.setPen(QColor(0,0,0));

            QPainterPath path;
            path.addRoundedRect(ppathrect, 2.0, 2.0);
            QColor color(220,255,0,200);
            painter.fillPath(path, color);
            painter.drawPath(path);

            QString pPathText = QString("%1-%2").arg(pPath.getid()).arg(j+1);
            painter.setFont(QFont("Arial", 7, QFont::Normal));
            painter.drawText(ppathrect, pPathText, Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }

    // Render Zones
    for (int i = 0; i < level->zones.size(); i++)
    {
        const Zone& zone = level->zones.at(i);

        QRect zonerect(zone.getx(), zone.gety(), zone.getwidth(), zone.getheight());

        if (!drawrect.intersects(zonerect))
            continue;

        painter.setPen(QColor(255,255,255));

        painter.drawRect(zonerect);

        QString zoneText = QString("Zone %1").arg(zone.getid());
        painter.setFont(QFont("Arial", 10, QFont::Normal));
        painter.drawText(zonerect.adjusted(5,5,0,0), zoneText);
    }

    // Render Selection
    for (int i = 0; i < selObjects.size(); i++)
    {
        QRect objrect(selObjects[i]->getx()+selObjects[i]->getOffsetX(), selObjects[i]->gety()+selObjects[i]->getOffsetY(), selObjects[i]->getwidth(), selObjects[i]->getheight());

        painter.setPen(QPen(QColor(255,255,255,200), 1, Qt::DotLine));
        painter.drawRect(objrect);
        painter.fillRect(objrect, QColor(255,255,255,75));
    }
}


void LevelView::mousePressEvent(QMouseEvent* evt)
{
    int x = evt->x()/zoom;
    int y = evt->y()/zoom;

    if (evt->button() != Qt::LeftButton)
        return;

    bool hitSelction = false;
    for (int i = 0; i < selObjects.size(); i++)
    {
        if (selObjects[i]->clickDetection(x,y,0,0))
        {
            hitSelction = true;
            break;
        }
    }

    if (evt->modifiers() != Qt::ShiftModifier && !hitSelction) selObjects.clear();

    selObjects.append(selObjectsCheck(x, y, 0, 0, false));

    // Remove doubled entrys
    for (int i = 0; i < selObjects.size(); i++)
    {
        for (int j = 0; j < selObjects.size(); j++) if (i != j && selObjects[i] == selObjects[j]) selObjects.removeAt(j);
    }

    dragX = x;
    dragY = y;
    for (int i = 0; i < selObjects.size(); i++)
    {
        selObjects[i]->setDrag(selObjects[i]->getx(), selObjects[i]->gety());
    }

    update();
}


void LevelView::mouseMoveEvent(QMouseEvent* evt)
{    
    int x = evt->x()/zoom;
    int y = evt->y()/zoom;

    if (evt->buttons() != Qt::LeftButton) // checkme?
        return;

    bool roundToFullTile = false;
    for (int i = 0; i < selObjects.size(); i++)
    {
        if (selObjects[i]->getType() == 1)
        {
            roundToFullTile = true;
            break;
        }
    }

    for (int i = 0; i < selObjects.size(); i++)
    {
        int finalX, finalY;

        // Rounded to next Tile
        if (roundToFullTile)
        {
            finalX = selObjects[i]->getDragX() + toNext20(x - dragX);
            finalY = selObjects[i]->getDragY() + toNext20(y - dragY);
        }

        // For Based on 16
        else
        {
            // Drag stuff freely
            if (evt->modifiers() == Qt::AltModifier)
            {
                finalX = toNext16Compatible(x - selObjects[i]->getDragX());
                finalY = toNext16Compatible(y - selObjects[i]->getDragY());
            }
            // Rounded to next half Tile
            else
            {
                finalX = toNext10(x - selObjects[i]->getDragX());
                finalY = toNext10(y - selObjects[i]->getDragY());
            }
        }

        // clamp coords
        if (finalX < 0) finalX = 0;
        else if (finalX > 0xFFFF*20) finalX = 0xFFFF*20;
        if (finalY < 0) finalY = 0;
        else if (finalY > 0xFFFF*20) finalY = 0xFFFF*20;

        selObjects[i]->setPosition(finalX, finalY);
    }

    update();
}

void LevelView::moveEvent(QMoveEvent *)
{
    update();
}

void LevelView::saveLevel()
{
    level->save();
}

QList<Object*> LevelView::selObjectsCheck(int x, int y, int w, int h, bool multiSelect)
{
    QList<Object*> objects;

    bool stopChecking = false;

    // Check for Progress Path Nodes
    if (!stopChecking && !multiSelect)
    {
        for (int p = level->progressPaths.size()-1; p >= 0; p--)
        {
            for (int i = level->progressPaths[p].getNodes().size()-1; i >= 0; i--)
            {
                ProgressPathNode& node = level->progressPaths[p].getNodeReference(i);

                if (node.clickDetection(x,y,w,h))
                {
                    objects.append(&node);

                    stopChecking = true;
                    break;
                }
            }

            if (stopChecking && !multiSelect) break;
        }
    }

    // Check for Path Nodes
    if (!stopChecking && !multiSelect)
    {
        for (int p = level->paths.size()-1; p >= 0; p--)
        {
            for (int i = level->paths[p].getNodes().size()-1; i >= 0; i--)
            {
                PathNode& node = level->paths[p].getNodeReference(i);

                if (node.clickDetection(x,y,w,h))
                {
                    objects.append(&node);

                    stopChecking = true;
                    break;
                }
            }

            if (stopChecking && !multiSelect) break;
        }
    }

    // Check for Entrances
    if (!stopChecking && !multiSelect)
    {
        for (int i = level->entrances.size()-1; i >= 0; i--)
        {
            Entrance& entr = level->entrances[i];
            if (entr.clickDetection(x,y,w,h))
            {
                objects.append(&entr);

                stopChecking = true;
                break;
            }
        }
    }

    // Check for Sprites
    if (!stopChecking && !multiSelect)
    {
        for (int i = level->sprites.size()-1; i >= 0; i--)
        {
            Sprite& spr = level->sprites[i];
            if (spr.clickDetection(x,y,w,h))
            {
                objects.append(&spr);

                stopChecking = true;
                break;
            }
        }
    }

    // Check for Tiles
    if (!stopChecking && !multiSelect)
    {
        for (int l = 0; l < 2; l++)
        {
            if (!(layerMask & (1<<l)))
                continue;

            for (int i = level->objects[l].size()-1; i >= 0; i--)
            {
                BgdatObject& obj = level->objects[l][i];
                if (obj.clickDetection(x,y,w,h))
                {
                    objects.append(&obj);

                    stopChecking = true;
                    break;
                }
            }

            if (stopChecking && !multiSelect) break;
        }
    }

    return objects;
}
