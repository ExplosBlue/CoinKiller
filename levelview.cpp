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
}


void LevelView::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect drawrect = evt->rect();

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

    // Render Locations
    for (int i = 0; i < level->locations.size(); i++)
    {
        const Location& loc = level->locations.at(i);

        QRect locrect(loc.getx(), loc.gety(), loc.getwidth(), loc.getheight());

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

        QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

        QRect sprRect(spr.getx(), spr.gety(), spr.getwidth(), spr.getheight());

        switch (spr.getid()) {
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
        case 135: // Goomba
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "goomba.png"));
            break;
        case 139: // Goomba Tower
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+1, spr.getwidth(), 21, QPixmap(basePath + "goomba_tower_bottom.png"));
            for (int i = 0; i < (spr.getNybble(5)-2); i++) painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()-21*(i+1)+1, spr.getwidth(), 21, QPixmap(basePath + "goomba_tower_middle.png"));
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), 25, QPixmap(basePath + "goomba_tower_top.png"));
            break;
        case 165:
            if (spr.getNybble(5) == 0) painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "koopa_green.png"));
            else painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "koopa_red.png"));
            break;
        case 219: // Star Coin
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "star_coin.png"));
            break;
        case 225: // P Switch
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "p_switch.png"));
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
        case 287: // Toad House Door
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "door_toadhouse.png"));
            break;
        case 294: // Warp Cannon
            painter.drawPixmap(spr.getx()+spr.getOffsetX(), spr.gety()+spr.getOffsetY(), spr.getwidth(), spr.getheight(), QPixmap(basePath + "warp_cannon.png"));
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
            QPen pen(QColor(0,255,20));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(QPoint(nodes[j].getx()+10, nodes[j].gety()+10), QPoint(nodes[j+1].getx()+10, nodes[j+1].gety()+10));
        }

        for (int j = 0; j < nodes.size(); j++)
        {
            QRect pathrect(nodes[j].getx(), nodes[j].gety(), 20, 20);

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
            QPen pen(QColor(220,255,0));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(QPoint(nodes[j].getx()+10, nodes[j].gety()+10), QPoint(nodes[j+1].getx()+10, nodes[j+1].gety()+10));
        }

        for (int j = 0; j < nodes.size(); j++)
        {
            QRect ppathrect(nodes[j].getx(), nodes[j].gety(), 20, 20);

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

        painter.setPen(QColor(255,255,255));

        painter.drawRect(zonerect);

        QString zoneText = QString("Zone %1").arg(zone.getid());
        painter.setFont(QFont("Arial", 10, QFont::Normal));
        painter.drawText(zonerect.adjusted(5,5,0,0), zoneText);
    }

    // Render Selection
    if (selType != 0)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);

        QRect objrect(selObject->getx()+selObject->getOffsetX(), selObject->gety()+selObject->getOffsetY(), selObject->getwidth(), selObject->getheight());

        objrect.adjust(-1, -1, 0, 0);
        painter.setPen(QColor(0,0,0));
        painter.drawRect(objrect);

        objrect.adjust(1, 1, -1, -1);
        painter.setPen(QColor(255,255,255));
        painter.drawRect(objrect);
    }
}


void LevelView::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    selType = 0;

    // Check for Progress Path Nodes
    if (selType == 0)
    {
        for (int p = level->progressPaths.size()-1; p >= 0; p--)
        {
            for (int i = level->progressPaths[p].getNodes().size()-1; i >= 0; i--)
            {
                ProgressPathNode& node = level->progressPaths[p].getNodeReference(i);

                if (node.clickDetection(evt->x(), evt->y()))
                {
                    selType = 7;
                    selObject = &node;

                    dragX = evt->x() - node.getx();
                    dragY = evt->y() - node.gety();

                    break;
                }
            }

            if (selType) break;
        }
    }

    // Check for Path Nodes
    if (selType == 0)
    {
        for (int p = level->paths.size()-1; p >= 0; p--)
        {
            for (int i = level->paths[p].getNodes().size()-1; i >= 0; i--)
            {
                PathNode& node = level->paths[p].getNodeReference(i);

                if (node.clickDetection(evt->x(), evt->y()))
                {
                    selType = 6;
                    selObject = &node;

                    dragX = evt->x() - node.getx();
                    dragY = evt->y() - node.gety();

                    break;
                }
            }

            if (selType) break;
        }
    }

    // Check for Entrances
    if (selType == 0)
    {
        for (int i = level->entrances.size()-1; i >= 0; i--)
        {
            Entrance& entr = level->entrances[i];
            if (entr.clickDetection(evt->x(), evt->y()))
            {
                selType = 3;
                selObject = &entr;

                dragX = evt->x() - entr.getx();
                dragY = evt->y() - entr.gety();

                break;
            }
        }
    }

    // Check for Sprites
    if (selType == 0)
    {
        for (int i = level->sprites.size()-1; i >= 0; i--)
        {
            Sprite& spr = level->sprites[i];
            if (spr.clickDetection(evt->x(), evt->y()))
            {
                selType = 2;
                selObject = &spr;

                dragX = evt->x() - spr.getx();
                dragY = evt->y() - spr.gety();

                break;
            }
        }
    }

    // Check for Tiles
    if (selType == 0)
    {
        for (int l = 0; l < 2; l++)
        {
            if (!(layerMask & (1<<l)))
                continue;

            for (int i = level->objects[l].size()-1; i >= 0; i--)
            {
                BgdatObject& obj = level->objects[l][i];
                if (obj.clickDetection(evt->x(), evt->y()))
                {
                    // hit!
                    selType = 1;
                    selObject = &obj;

                    dragX = evt->x() - obj.getx();
                    dragY = evt->y() - obj.gety();

                    qDebug("OBJ %04X", obj.getid());

                    break;
                }
            }

            if (selType) break;
        }
    }

    update();
}


void LevelView::mouseMoveEvent(QMouseEvent* evt)
{    
    if (evt->buttons() != Qt::LeftButton) // checkme?
        return;

    if (selType != 0)
    {
        int finalX, finalY;

        // Rounded to next Tile
        if (selType == 1)
        {
            finalX = toNext20(evt->x() - dragX);
            finalY = toNext20(evt->y() - dragY);
        }

        // For Based on 16
        else
        {
            // Drag stuff freely
            if (evt->modifiers() & Qt::AltModifier)
            {
                finalX = toNext16Compatible(evt->x() - dragX);
                finalY = toNext16Compatible(evt->y() - dragY);
            }
            // Rounded to next half Tile
            else
            {
                finalX = toNext10(evt->x() - dragX);
                finalY = toNext10(evt->y() - dragY);
            }
        }

        // clamp coords
        if (finalX < 0) finalX = 0;
        else if (finalX > 0xFFFF*20) finalX = 0xFFFF*20;
        if (finalY < 0) finalY = 0;
        else if (finalY > 0xFFFF*20) finalY = 0xFFFF*20;

        selObject->setPosition(finalX, finalY);
    }

    update();
}

