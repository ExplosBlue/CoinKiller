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
#include "is.h"

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

    //propGrid = new PropertyGrid(this);
    //QHBoxLayout* crappyshit = new QHBoxLayout(ui->widget_2);
    //ui->widget_2->setLayout(crappyshit);
    //crappyshit->addWidget(propGrid);
    //ui->widget_2->layout()->addWidget(propGrid);

    //propGrid->verticalHeader()->hide();
    //propGrid->horizontalHeader()->hide();

    //PropertyGridModel* model = new PropertyGridModel(propGrid);
    //propGrid->setModel(model);

    // TEST ZONE END



    QList<int> derpshit;
    derpshit.append(300);
    derpshit.append(999999999);
    ui->splitter->setSizes(derpshit);
    ui->splitter->setCollapsible(0, false);

    ui->sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->sidebar->setMinimumSize(300, 20);

    //this->ui->splitter->setStretchFactor(0, 0); // useless

    //this->ui->widget->layout()->addWidget(levelView);


    layerMask = 0x7;
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);
    levelView->setLayerMask(layerMask);

    zoom = 1.0;

    // Setup Area Editor
    areaEditor = new AreaEditorWidget(level);
    ui->sidebarTabWidget->addTab(areaEditor, QIcon(basePath + "settings.png"), "");

    // Setup Tileset Picker
    tilesetPalette = new TilesetPalette(level, levelView->objEditionModePtr());
    ui->sidebarTabWidget->addTab(tilesetPalette, QIcon(basePath + "filled_box"), "");

    // Setup Sprite Picker
    spriteEditor = new SpriteEditorWidget();
    ui->sidebarTabWidget->addTab(spriteEditor, QIcon(basePath + "goomba.png"), "");
    connect(spriteEditor, SIGNAL(selectedSpriteChanged(int)), this, SLOT(setSelSprite(int)));

    // Setup Entrance Editor
    entranceEditor = new EntranceEditorWidget(&level->entrances);
    connect(entranceEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    ui->sidebarTabWidget->addTab(entranceEditor, QIcon(basePath + "entrance.png"), "");

    // Setup Zone Editor
    zoneEditor = new ZoneEditorWidget(&level->zones);
    connect(zoneEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    ui->sidebarTabWidget->addTab(zoneEditor, QIcon(basePath + "zone.png"), "");

    connect(levelView->editionModePtr(), SIGNAL(selectdObjectChanged(Object*)), this, SLOT(setObjectEdition(Object*)));
    connect(levelView->editionModePtr(), SIGNAL(deselected()), this, SLOT(deselect()));
    connect(levelView->editionModePtr(), SIGNAL(updateEditors()), this, SLOT(updateEditors()));
}

LevelEditorWindow::~LevelEditorWindow()
{
    delete level;
    delete ui;
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
        ui->actionFullscreen->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/collapse.png"));
    }
    else
    {
        showNormal();
        ui->actionFullscreen->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/expand.png"));
    }
}

void LevelEditorWindow::on_actionGrid_toggled(bool toggle)
{
    levelView->toggleGrid(toggle);
}

void LevelEditorWindow::setSelSprite(int spriteId)
{
    levelView->objEditionModePtr()->setDrawType(1);
    levelView->objEditionModePtr()->setSprite(spriteId);
}

void LevelEditorWindow::setObjectEdition(Object* obj)
{
    deselect();

    if (is<Entrance*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(3);
        entranceEditor->select(dynamic_cast<Entrance*>(obj));
    }
    else if (is<Zone*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(4);
        zoneEditor->select(dynamic_cast<Zone*>(obj));
    }
}

void LevelEditorWindow::deselect()
{
    entranceEditor->deselect();
    zoneEditor->deselect();
}

void LevelEditorWindow::updateEditors()
{
    entranceEditor->updateEditor();
    zoneEditor->updateEditor();
}

void LevelEditorWindow::on_sidebarTabWidget_currentChanged(int index)
{
    if (index == 3)
        levelView->objEditionModePtr()->setDrawType(2);
    else if (index == 4)
        levelView->objEditionModePtr()->setDrawType(3);
}
