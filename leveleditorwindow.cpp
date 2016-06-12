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
#include <QInputDialog>
#include <QComboBox>
#include <QMessageBox>

LevelEditorWindow::LevelEditorWindow(QWidget *parent, Game *game, QString path) :
    QMainWindow(parent),
    ui(new Ui::LevelEditorWindow)
{
    this->game = game;
    currArea = 1;

    lvlPath = path;
    level = game->getLevel(path, currArea);

    ui->setupUi(this);

    areaSelector = new QComboBox(ui->toolBar);
    updateAreaSelector(0);
    ui->toolBar->insertWidget(ui->actionAddArea, areaSelector);
    connect(areaSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedAreaChanged(int)));

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
    ui->actionAddArea->setIcon(QIcon(basePath + "add.png"));
    ui->actionDeleteCurrentArea->setIcon(QIcon(basePath + "remove.png"));

    levelView = new LevelView(this, level);
    /*levelView->setMinimumHeight(600);
    levelView->setMaximumWidth(800);*/
    //QHBoxLayout* crappyshit = new QHBoxLayout(this);
    //this->setLayout(crappyshit);
    //this->ui->widget->setLayout(crappyshit);
    //crappyshit->addWidget(levelView);
    ui->levelViewArea->setWidget(levelView);



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
    derpshit.append(350);
    derpshit.append(999999999);
    ui->splitter->setSizes(derpshit);
    ui->splitter->setCollapsible(0, false);

    ui->sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->sidebar->setMinimumSize(350, 20);

    //this->ui->splitter->setStretchFactor(0, 0); // useless

    //this->ui->widget->layout()->addWidget(levelView);


    layerMask = 0x7;
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);


    zoom = 1.0;

    loadArea(currArea);
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

    if (is<BgdatObject*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(1);
        tilesetPalette->select(dynamic_cast<BgdatObject*>(obj));
    }
    else if (is<Sprite*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(2);
        spriteEditor->select(dynamic_cast<Sprite*>(obj));
        spriteEditor->spriteDataEditorPtr()->select(dynamic_cast<Sprite*>(obj));
    }
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
    else if (is<Location*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(5);
        locationEditor->select(dynamic_cast<Location*>(obj));
    }
}

void LevelEditorWindow::deselect()
{
    spriteEditor->spriteDataEditorPtr()->deselect();
    entranceEditor->deselect();
    zoneEditor->deselect();
    locationEditor->deselect();
}

void LevelEditorWindow::updateEditors()
{
    spriteEditor->spriteDataEditorPtr()->updateEditor();
    entranceEditor->updateEditor();
    zoneEditor->updateEditor();
    locationEditor->updateEditor();
}

void LevelEditorWindow::on_sidebarTabWidget_currentChanged(int index)
{    
    if (index == 0)
        levelView->objEditionModePtr()->setDrawType(-1);
    else
        levelView->objEditionModePtr()->setDrawType(index-1);
}

void LevelEditorWindow::updateAreaSelector(int index)
{
    int lastIndex = areaSelector->currentIndex();
    areaSelector->blockSignals(true);

    int areaCount = level->getAreaCount();
    QStringList areaStrings;
    for (int i = 0; i < areaCount; i++)
        areaStrings << QString("Area %1").arg(i+1);

    QStringListModel *model = new QStringListModel();
    model->setStringList(areaStrings);

    areaSelector->setModel(model);
    if (index == -1) areaSelector->setCurrentIndex(lastIndex);
    else areaSelector->setCurrentIndex(index-1);

    areaSelector->blockSignals(false);
}

void LevelEditorWindow::selectedAreaChanged(int area)
{
    area++;
    loadArea(area);
}

void LevelEditorWindow::loadArea(int area)
{
    if (!level->hasArea(area))
    {
        QString errorText("Area %1 does not exist!");
        QMessageBox::critical(this, "CoinKiller", errorText.arg(area), QMessageBox::Ok, 0);
        return;
    }

    level = game->getLevel(lvlPath, area);

    levelView = new LevelView(this, level);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);
    levelView->setLayerMask(layerMask);

    // Setup Area Editor
    areaEditor = new AreaEditorWidget(level, game);
    connect(areaEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));

    // Setup Tileset Picker
    tilesetPalette = new TilesetPalette(level, levelView->objEditionModePtr());
    connect(areaEditor, SIGNAL(relaodTilesetPicker()), tilesetPalette, SLOT(reloadTilesets()));

    // Setup Sprite Picker
    spriteEditor = new SpriteEditorWidget();
    connect(spriteEditor->spriteDataEditorPtr(), SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(spriteEditor, SIGNAL(selectedSpriteChanged(int)), this, SLOT(setSelSprite(int)));

    // Setup Entrance Editor
    entranceEditor = new EntranceEditorWidget(&level->entrances);
    connect(entranceEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));

    // Setup Zone Editor
    zoneEditor = new ZoneEditorWidget(&level->zones);
    connect(zoneEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));

    // Setup Location Editor
    locationEditor = new LocationEditorWidget(&level->locations);
    connect(locationEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));

    connect(levelView->editionModePtr(), SIGNAL(selectdObjectChanged(Object*)), this, SLOT(setObjectEdition(Object*)));
    connect(levelView->editionModePtr(), SIGNAL(deselected()), this, SLOT(deselect()));
    connect(levelView->editionModePtr(), SIGNAL(updateEditors()), this, SLOT(updateEditors()));

    ui->levelViewArea->setWidget(levelView);

    for(int i = ui->sidebarTabWidget->count() - 1; i >= 0; i--)
        ui->sidebarTabWidget->removeTab(i);

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/");
    ui->sidebarTabWidget->addTab(areaEditor, QIcon(basePath + "settings.png"), "");
    ui->sidebarTabWidget->addTab(tilesetPalette, QIcon(basePath + "filled_box"), "");
    ui->sidebarTabWidget->addTab(spriteEditor, QIcon(basePath + "goomba.png"), "");
    ui->sidebarTabWidget->addTab(entranceEditor, QIcon(basePath + "entrance.png"), "");
    ui->sidebarTabWidget->addTab(zoneEditor, QIcon(basePath + "zone.png"), "");
    ui->sidebarTabWidget->addTab(locationEditor, QIcon(basePath + "location.png"), "");

    levelView->toggleGrid(ui->actionGrid->isChecked());
    levelView->setZoom(zoom);

    updateAreaSelector(area);
}

void LevelEditorWindow::on_actionAddArea_triggered()
{
    if (level->getAreaCount() >= 4)
    {
        QMessageBox::information(this, "CoinKiller", "Due to limitations a Level can only contain 4 areas!", QMessageBox::Ok);
        return;
    }

    int newArea = level->addArea();

    if (newArea > 0)
        loadArea(newArea);
}

void LevelEditorWindow::on_actionDeleteCurrentArea_triggered()
{
    if (level->getAreaCount() <= 1)
    {
        QMessageBox::information(this, "CoinKiller", "You cannot delete this area since there has to be at least one area in the level!", QMessageBox::Ok);
        return;
    }
    if (!QFile(QCoreApplication::applicationDirPath()+"/coinkiller_data/blank_course.bin").exists())
    {
        QMessageBox::information(this, "CoinKiller", "The blank course file \"/coinkiller_data/blank_course.bin\" is missing!", QMessageBox::Ok);
        return;
    }

    QMessageBox warning(this);
    warning.setWindowTitle("CoinKiller");
    warning.setText("Are you sure you want to delete this area?\n\nThe level will automatically save afterwards and therefore you will not be able to undo this step!");
    warning.setIcon(QMessageBox::Warning);
    warning.setStandardButtons(QMessageBox::Yes);
    warning.addButton(QMessageBox::No);
    warning.setDefaultButton(QMessageBox::No);
    if (warning.exec() != QMessageBox::Yes)
        return;

    int area = level->removeArea(areaSelector->currentIndex()+1);

    if (area < 1)
        return;

    loadArea(area);
}
