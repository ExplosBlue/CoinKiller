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
#include "ui_leveleditorwindow.h"

#include "levelview.h"
#include "game.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStandardItemModel>

LevelEditorWindow::LevelEditorWindow(QWidget *parent, Level* level) :
    QMainWindow(parent),
    ui(new Ui::LevelEditorWindow)
{
    this->level = level;
    // some extra shit here


    ui->setupUi(this);

    QString title;
    level->getName(title);
    setWindowTitle(title + " - CoinKiller");

    // Load UI Icons
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/");
    ui->actionZoom_In->setIcon(QIcon(basePath + "zoomin.png"));
    ui->actionZoom_Out->setIcon(QIcon(basePath + "zoomout.png"));
    ui->actionZoom_100->setIcon(QIcon(basePath + "zoom100.png"));
    ui->actionZoom_Maximum->setIcon(QIcon(basePath + "zoommax.png"));
    ui->actionZoom_Minimum->setIcon(QIcon(basePath + "zoommin.png"));
    ui->actionSave->setIcon((QIcon(basePath + "save.png")));
    ui->actionPaste->setIcon((QIcon(basePath + "paste.png")));
    ui->actionCut->setIcon((QIcon(basePath + "cut.png")));
    ui->actionCopy->setIcon((QIcon(basePath + "copy.png")));
    ui->actionDelete->setIcon(QIcon(basePath + "delete.png"));
    ui->actionFullscreen->setIcon(QIcon(basePath + "expand.png"));
    ui->actionGrid->setIcon(QIcon(basePath + "grid.png"));

    levelView = new LevelView(this, level);
    /*levelView->setMinimumHeight(600);
    levelView->setMaximumWidth(800);*/
    //QHBoxLayout* crappyshit = new QHBoxLayout(this);
    //this->setLayout(crappyshit);
    //this->ui->widget->setLayout(crappyshit);
    //crappyshit->addWidget(levelView);
    ui->levelViewArea->setWidget(levelView);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);


    // TEST ZONE

    propGrid = new PropertyGrid(this);
    QHBoxLayout* crappyshit = new QHBoxLayout(ui->widget_2);
    ui->widget_2->setLayout(crappyshit);
    crappyshit->addWidget(propGrid);
    //ui->widget_2->layout()->addWidget(propGrid);

    propGrid->verticalHeader()->hide();
    propGrid->horizontalHeader()->hide();

    PropertyGridModel* model = new PropertyGridModel(propGrid);
    propGrid->setModel(model);

    // TEST ZONE END



    QList<int> derpshit;
    derpshit.append(200);
    derpshit.append(999999999);
    ui->splitter->setSizes(derpshit);

    ui->sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->sidebar->setMinimumSize(200, 20);

    //this->ui->splitter->setStretchFactor(0, 0); // useless

    //this->ui->widget->layout()->addWidget(levelView);


    layerMask = 0x7;
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);
    levelView->setLayerMask(layerMask);

    zoom = 1.0;


    // Setup Object Lists
    objectLists[0] = ui->objectsListView0;
    objectLists[1] = ui->objectsListView1;
    objectLists[2] = ui->objectsListView2;
    objectLists[3] = ui->objectsListView3;

    for (int i = 0; i < 4; i++)
    {
        objectLists[i]->setFlow(QListView::LeftToRight);
        objectLists[i]->setLayoutMode(QListView::SinglePass);
        objectLists[i]->setMovement(QListView::Static);
        objectLists[i]->setResizeMode(QListView::Adjust);
        objectLists[i]->setWrapping(true);
        objectLists[i]->setIconSize(QSize(140,140));
        objectLists[i]->setVerticalScrollMode(QListView::ScrollPerPixel);
        setupObjectsModel(i);
    }
}

LevelEditorWindow::~LevelEditorWindow()
{
    delete level;
    delete ui;
}


// Functions

void LevelEditorWindow::setupObjectsModel(int tilesetNbr)
{
    if (!level->tilesets[tilesetNbr])
    {
        objectLists[tilesetNbr]->setEnabled(false);
        return;
    }
    objectLists[tilesetNbr]->setEnabled(true);

    QStandardItemModel* objectsModel = new QStandardItemModel(this);

    for (int i = 0; i < level->tilesets[tilesetNbr]->getNumObjects(); i++)
    {
        ObjectDef* obj = level->tilesets[tilesetNbr]->getObjectDef(i);
        QPixmap objPixmap(obj->width*20, obj->height*20);
        objPixmap.fill(Qt::transparent);

        QPainter p(&objPixmap);
        TileGrid tileGrid;
        tileGrid.clear();
        tileGrid[0xFFFFFFFF] = 1;
        level->tilesets[tilesetNbr]->drawObject(p, tileGrid, i, 0, 0, obj->width, obj->height, 1);
        p.end();
        QStandardItem *objItem = new QStandardItem();
        objItem->setIcon(QIcon(objPixmap));
        objItem->setToolTip(QString("Object: %1").arg(i));
        objectsModel->appendRow(objItem);
    }

    objectLists[tilesetNbr]->setModel(objectsModel);
}


// Actions

void LevelEditorWindow::on_actionToggleLayer1_toggled(bool toggle)
{
    if (toggle) layerMask |=  0x1;
    else        layerMask &= ~0x1;
    levelView->setLayerMask(layerMask);
    update();
}

void LevelEditorWindow::on_actionToggleLayer2_toggled(bool toggle)
{
    if (toggle) layerMask |=  0x2;
    else        layerMask &= ~0x2;
    levelView->setLayerMask(layerMask);
    update();
}

void LevelEditorWindow::on_actionZoom_In_triggered()
{
    if (zoom < 4)
    {
        if (zoom < 1) zoom += 0.1;
        else zoom += 0.25;
        levelView->setZoom(zoom);
        update();
    }
}

void LevelEditorWindow::on_actionZoom_Out_triggered()
{
    if (zoom > 0.3)
    {
        if (zoom <= 1.5) zoom -= 0.1;
        else zoom -= 0.25;
        levelView->setZoom(zoom);
        update();
    }
}

void LevelEditorWindow::on_actionZoom_100_triggered()
{
    zoom = 1;
    levelView->setZoom(zoom);
    update();
}

void LevelEditorWindow::on_actionZoom_Maximum_triggered()
{
    zoom = 4;
    levelView->setZoom(zoom);
    update();
}

void LevelEditorWindow::on_actionZoom_Minimum_triggered()
{
    zoom = 0.3;
    levelView->setZoom(zoom);
    update();
}

void LevelEditorWindow::on_actionSave_triggered()
{
    levelView->saveLevel();
}

void LevelEditorWindow::on_actionCopy_triggered()
{
    levelView->copy();
}

void LevelEditorWindow::on_actionPaste_triggered()
{
    levelView->paste();
}

void LevelEditorWindow::on_actionCut_triggered()
{
    levelView->cut();
}

void LevelEditorWindow::on_actionDelete_triggered()
{
    levelView->deleteSel();
}

void LevelEditorWindow::on_actionFullscreen_toggled(bool toggle)
{
    if (toggle)
    {
        showFullScreen();
        ui->actionFullscreen->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/coinkiller_data/collapse.png"));
    }
    else
    {
        showNormal();
        ui->actionFullscreen->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/coinkiller_data/expand.png"));
    }
}

void LevelEditorWindow::on_actionGrid_toggled(bool toggle)
{
    levelView->toggleGrid(toggle);
}

void LevelEditorWindow::on_objectsListView0_clicked(const QModelIndex &index)
{
    levelView->objEditionModePtr()->setDrawType(0);
    levelView->objEditionModePtr()->setObject(index.row(), 0);
}

void LevelEditorWindow::on_objectsListView1_clicked(const QModelIndex &index)
{
    levelView->objEditionModePtr()->setDrawType(0);
    levelView->objEditionModePtr()->setObject(index.row(), 1);
}

void LevelEditorWindow::on_objectsListView2_clicked(const QModelIndex &index)
{
    levelView->objEditionModePtr()->setDrawType(0);
    levelView->objEditionModePtr()->setObject(index.row(), 2);
}

void LevelEditorWindow::on_objectsListView3_clicked(const QModelIndex &index)
{
    levelView->objEditionModePtr()->setDrawType(0);
    levelView->objEditionModePtr()->setObject(index.row(), 3);
}

void LevelEditorWindow::on_radioButton_toggled(bool checked)
{
    levelView->objEditionModePtr()->setLayer(checked);
}

void LevelEditorWindow::on_actionDo_shit_I_guess_triggered()
{
    //levelView->deleteSel();
}

void LevelEditorWindow::on_paintLocation_clicked()
{
    levelView->objEditionModePtr()->setDrawType(4);
}
