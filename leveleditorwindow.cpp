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
#include "is.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QScrollBar>

LevelEditorWindow::LevelEditorWindow(LevelManager* lvlMgr, int initialArea, SettingsManager *settings) :
    QMainWindow(lvlMgr->getParent()),
    ui(new Ui::LevelEditorWindow)
{
    this->lvlMgr = lvlMgr;
    this->settings = settings;

    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    loadTranslations();

    areaSelector = new QComboBox(this);
    ui->toolBar->insertWidget(ui->actionAddArea, areaSelector);
    connect(areaSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAreaIndexChange(int)));

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
    ui->actionRaise->setIcon(QIcon(basePath + "raise.png"));
    ui->actionLower->setIcon(QIcon(basePath + "lower.png"));
    ui->actionRaiseLayer->setIcon(QIcon(basePath + "layer_up.png"));
    ui->actionLowerLayer->setIcon(QIcon(basePath + "layer_down.png"));
    ui->actionFullscreen->setIcon(QIcon(basePath + "expand.png"));
    ui->actionGrid->setIcon(QIcon(basePath + "grid.png"));
    ui->actionAddArea->setIcon(QIcon(basePath + "add.png"));
    ui->actionDeleteCurrentArea->setIcon(QIcon(basePath + "remove.png"));

    QList<int> derpshit;
    derpshit.append(350);
    derpshit.append(999999999);
    ui->splitter->setSizes(derpshit);
    ui->splitter->setCollapsible(0, false);

    ui->sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->sidebar->setMinimumSize(350, 20);

    loadArea(initialArea, false, true);
    updateAreaSelector(initialArea);
}

LevelEditorWindow::~LevelEditorWindow()
{
    if (closeLvlOnClose)
        lvlMgr->closeArea(level);
    if (lvlMgr->getOpenedAreaCount() == 0)
        delete lvlMgr;
    delete ui;
}

void LevelEditorWindow::loadTranslations()
{
    ui->menuFile->setTitle(settings->getTranslation("General", "file"));
    ui->menuEdit->setTitle(settings->getTranslation("General", "edit"));
    ui->menuView->setTitle(settings->getTranslation("LevelEditor", "view"));

    ui->actionSave->setText(settings->getTranslation("General", "save"));
    ui->actionSave->setToolTip(settings->getTranslation("General", "save"));

    ui->actionPaste->setText(settings->getTranslation("General", "paste"));
    ui->actionPaste->setToolTip(settings->getTranslation("General", "paste"));

    ui->actionCut->setText(settings->getTranslation("General", "cut"));
    ui->actionCut->setToolTip(settings->getTranslation("General", "cut"));

    ui->actionCopy->setText(settings->getTranslation("General", "copy"));
    ui->actionCopy->setToolTip(settings->getTranslation("General", "copy"));

    ui->actionDelete->setText(settings->getTranslation("General", "delete"));
    ui->actionDelete->setToolTip(settings->getTranslation("General", "delete"));

    ui->actionRaise->setText(settings->getTranslation("LevelEditor", "raiseToTop"));
    ui->actionRaise->setToolTip(settings->getTranslation("LevelEditor", "raiseToTop"));

    ui->actionLower->setText(settings->getTranslation("LevelEditor", "lowerToBottom"));
    ui->actionLower->setToolTip(settings->getTranslation("LevelEditor", "lowerToBottom"));

    ui->actionRaiseLayer->setText(settings->getTranslation("LevelEditor", "raiseLayer"));
    ui->actionRaiseLayer->setToolTip(settings->getTranslation("LevelEditor", "raiseLayer"));

    ui->actionLowerLayer->setText(settings->getTranslation("LevelEditor", "lowerLayer"));
    ui->actionLowerLayer->setToolTip(settings->getTranslation("LevelEditor", "lowerLayer"));

    ui->actionZoom_In->setText(settings->getTranslation("LevelEditor", "zoomIn"));
    ui->actionZoom_In->setToolTip(settings->getTranslation("LevelEditor", "zoomIn"));

    ui->actionZoom_Out->setText(settings->getTranslation("LevelEditor", "zoomOut"));
    ui->actionZoom_Out->setToolTip(settings->getTranslation("LevelEditor", "zoomOut"));

    ui->actionZoom_Maximum->setText(settings->getTranslation("LevelEditor", "zoomMax"));
    ui->actionZoom_Maximum->setToolTip(settings->getTranslation("LevelEditor", "zoomMax"));

    ui->actionZoom_Minimum->setText(settings->getTranslation("LevelEditor", "zoomMin"));
    ui->actionZoom_Minimum->setToolTip(settings->getTranslation("LevelEditor", "zoomMin"));

    ui->actionZoom_100->setText(settings->getTranslation("LevelEditor", "zoom100"));
    ui->actionZoom_100->setToolTip(settings->getTranslation("LevelEditor", "zoom100"));

    ui->actionFullscreen->setText(settings->getTranslation("LevelEditor", "fullscreen"));
    ui->actionFullscreen->setToolTip(settings->getTranslation("LevelEditor", "fullscreen"));

    ui->actionGrid->setText(settings->getTranslation("LevelEditor", "grid"));
    ui->actionGrid->setToolTip(settings->getTranslation("LevelEditor", "grid"));

    ui->actionAddArea->setText(settings->getTranslation("LevelEditor", "addArea"));
    ui->actionAddArea->setToolTip(settings->getTranslation("LevelEditor", "addArea"));

    ui->actionDeleteCurrentArea->setText(settings->getTranslation("LevelEditor", "removeArea"));
    ui->actionDeleteCurrentArea->setToolTip(settings->getTranslation("LevelEditor", "removeArea"));

    ui->actionToggleLayer1->setText(settings->getTranslation("LevelEditor", "layer") + " 1");
    ui->actionToggleLayer1->setToolTip(settings->getTranslation("LevelEditor", "layer") + " 1");

    ui->actionToggleLayer2->setText(settings->getTranslation("LevelEditor", "layer") + " 2");
    ui->actionToggleLayer2->setToolTip(settings->getTranslation("LevelEditor", "layer") + " 2");
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

void LevelEditorWindow::on_actionRaise_triggered()
{
    levelView->raise();
}

void LevelEditorWindow::on_actionLower_triggered()
{
    levelView->lower();
}

void LevelEditorWindow::on_actionRaiseLayer_triggered()
{
    levelView->raiseLayer();
}

void LevelEditorWindow::on_actionLowerLayer_triggered()
{
    levelView->lowerLayer();
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
    else if (is<PathNode*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(6);
        pathEditor->select(dynamic_cast<PathNode*>(obj));
    }
    else if (is<ProgressPathNode*>(obj))
    {
        ui->sidebarTabWidget->setCurrentIndex(7);
        progPathEditor->select(dynamic_cast<ProgressPathNode*>(obj));
    }
}

void LevelEditorWindow::deselect()
{
    spriteEditor->spriteDataEditorPtr()->deselect();
    entranceEditor->deselect();
    zoneEditor->deselect();
    locationEditor->deselect();
    pathEditor->deselect();
    progPathEditor->deselect();
}

void LevelEditorWindow::updateEditors()
{
    spriteEditor->spriteDataEditorPtr()->updateEditor();
    entranceEditor->updateEditor();
    zoneEditor->updateEditor();
    locationEditor->updateEditor();
    pathEditor->updateEditor();
    progPathEditor->updateEditor();
}

void LevelEditorWindow::on_sidebarTabWidget_currentChanged(int index)
{    
    if (index == 0)
        levelView->objEditionModePtr()->setDrawType(-1);
    else
        levelView->objEditionModePtr()->setDrawType(index-1);
}

void LevelEditorWindow::on_actionAddArea_triggered()
{
    if (lvlMgr->getAreaCount() >= 4)
    {
        QMessageBox::information(this, "CoinKiller", "Due to limitations there can only be a maximum of 4 areas in a level.", QMessageBox::Ok);
        return;
    }

    int seekArea = lvlMgr->addArea(level->getAreaID());
    loadArea(seekArea);
    updateAreaSelector(seekArea);
}

void LevelEditorWindow::on_actionDeleteCurrentArea_triggered()
{
    if (lvlMgr->getAreaCount() <= 1)
    {
        QMessageBox::information(this, "CoinKiller", "This area cannot be deleted because there has to be at least one area in a level.", QMessageBox::Ok);
        return;
    }

    QMessageBox warning;
    warning.setWindowTitle("CoinKiller");
    warning.setText("Are you sure you want to delete this area?\n\nThe level will automatically save afterwards and therefore the deletion of this area cannot be undone afterwards.");
    warning.setStandardButtons(QMessageBox::Yes);
    warning.addButton(QMessageBox::No);
    warning.setDefaultButton(QMessageBox::No);
    warning.setIcon(QMessageBox::Warning);

    if (warning.exec() == QMessageBox::No)
        return;

    int seekArea = lvlMgr->removeArea(level);

    if (!lvlMgr->hasArea(seekArea))
    {
        closeLvlOnClose = false;
        close();
        return;
    }

    loadArea(seekArea, false);
    updateAreaSelector(seekArea);
}

void LevelEditorWindow::loadArea(int id, bool closeLevel, bool init)
{
    if (!lvlMgr->hasArea(id))
    {
        QMessageBox::information(this, "CoinKiller", QString("The Area %1 is not existing.").arg(id));
        return;
    }

    if (closeLevel)
        lvlMgr->closeArea(level);

    if (!init)
    {
        delete levelView;

        for(int i = ui->sidebarTabWidget->count() - 1; i >= 0; i--)
        {
            QWidget* deleteWidget = ui->sidebarTabWidget->widget(i);
            ui->sidebarTabWidget->removeTab(i);
            delete deleteWidget;
        }
    }

    level = lvlMgr->openArea(id);

    levelView = new LevelView(this, level);
    ui->levelViewArea->setWidget(levelView);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);

    layerMask = 0x7;
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);
    levelView->setLayerMask(layerMask);
    levelView->toggleGrid(ui->actionGrid->isChecked());

    zoom = 1.0;

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/");

    // Setup Area Editor
    areaEditor = new AreaEditorWidget(level, lvlMgr->getGame());
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
    connect(entranceEditor, SIGNAL(selectedEntrChanged(Object*)), levelView, SLOT(selectObj(Object*)));

    // Setup Zone Editor
    zoneEditor = new ZoneEditorWidget(&level->zones);
    connect(zoneEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(zoneEditor, SIGNAL(selectedZoneChanged(Object*)), levelView, SLOT(selectObj(Object*)));

    // Setup Location Editor
    locationEditor = new LocationEditorWidget(&level->locations);
    connect(locationEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(locationEditor, SIGNAL(selectedLocChanged(Object*)), levelView, SLOT(selectObj(Object*)));

    // Setup Path Editor
    pathEditor = new PathEditorWidget(&level->paths);
    connect(pathEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(pathEditor, SIGNAL(selectedPathChanged(Object*)), levelView, SLOT(selectObj(Object*)));

    // Setup Progress Path Editor
    progPathEditor = new ProgressPathEditorWidget(&level->progressPaths);
    connect(progPathEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(progPathEditor, SIGNAL(selectedProgPathChanged(Object*)), levelView, SLOT(selectObj(Object*)));

    connect(levelView, SIGNAL(scrollTo(int,int)), this, SLOT(scrollTo(int,int)));
    connect(levelView->editionModePtr(), SIGNAL(selectdObjectChanged(Object*)), this, SLOT(setObjectEdition(Object*)));
    connect(levelView->editionModePtr(), SIGNAL(deselected()), this, SLOT(deselect()));
    connect(levelView->editionModePtr(), SIGNAL(updateEditors()), this, SLOT(updateEditors()));


    ui->sidebarTabWidget->addTab(areaEditor, QIcon(basePath + "settings.png"), "");
    ui->sidebarTabWidget->addTab(tilesetPalette, QIcon(basePath + "filled_box"), "");
    ui->sidebarTabWidget->addTab(spriteEditor, QIcon(basePath + "goomba.png"), "");
    ui->sidebarTabWidget->addTab(entranceEditor, QIcon(basePath + "entrance.png"), "");
    ui->sidebarTabWidget->addTab(zoneEditor, QIcon(basePath + "zone.png"), "");
    ui->sidebarTabWidget->addTab(locationEditor, QIcon(basePath + "location.png"), "");
    ui->sidebarTabWidget->addTab(pathEditor, QIcon(basePath + "path.png"), "");
    ui->sidebarTabWidget->addTab(progPathEditor, QIcon(basePath + "progress_path.png"), "");
}

void LevelEditorWindow::updateAreaSelector(int index)
{
    ui->actionAddArea->setEnabled(lvlMgr->getAreaCount() < 4);
    ui->actionDeleteCurrentArea->setEnabled(lvlMgr->getAreaCount() > 1);

    int lastIndex = areaSelector->currentIndex();

    areaSelector->blockSignals(true);

    int areaCount = lvlMgr->getAreaCount();
    QStringList areaStrings;
    for (int i = 0; i < areaCount; i++)
       areaStrings << QString("%1 %2").arg(settings->getTranslation("LevelEditor", "area")).arg(i+1);

    QStringListModel *model = new QStringListModel();
        model->setStringList(areaStrings);

    areaSelector->setModel(model);
    if (index == -1) areaSelector->setCurrentIndex(lastIndex);
    else areaSelector->setCurrentIndex(index-1);

    areaSelector->blockSignals(false);

    QString title;
    level->getName(title);
    setWindowTitle("CoinKiller - Editing: " + title);
}

void LevelEditorWindow::handleAreaIndexChange(int index)
{
    index++;

    if (lvlMgr->areaIsOpen(index))
    {
        updateAreaSelector(level->getAreaID());
        QMessageBox::information(this, "CoinKiller", QString("Area %1 cannot be opened because it is already opened in anoter editor window.").arg(index), QMessageBox::Ok);
        return;
    }

    if (QApplication::queryKeyboardModifiers() &= Qt::ControlModifier)
    {
        updateAreaSelector(level->getAreaID());
        lvlMgr->openAreaEditor(index);
    }
    else
    {
        loadArea(index);
        updateAreaSelector();
    }
}

void LevelEditorWindow::handleMgrUpdate()
{
    updateAreaSelector(level->getAreaID());
}

void LevelEditorWindow::scrollTo(int x, int y)
{
    ui->levelViewArea->horizontalScrollBar()->setValue(x);
    ui->levelViewArea->verticalScrollBar()->setValue(y);
}
