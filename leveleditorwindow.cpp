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

#include "is.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QScrollBar>
#include <QColorDialog>

LevelEditorWindow::LevelEditorWindow(LevelManager* lvlMgr, int initialArea) :
    QMainWindow(lvlMgr->getParent()),
    ui(new Ui::LevelEditorWindow)
{
    this->lvlMgr = lvlMgr;
    this->settings = SettingsManager::getInstance();

    if (settings->get("maximised", false).toBool())
        this->setWindowState(Qt::WindowMaximized);

    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    loadTranslations();

    areaSelector = new QComboBox(this);
    ui->toolBar->insertWidget(ui->actionAddArea, areaSelector);
    connect(areaSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAreaIndexChange(int)));

    ui->statusbar->addWidget(editStatus);
    //connect(editStatus, SIGNAL(updateLevelLabel(QString)), editStatus, SLOT(setText(QString)));

    // Prevent level view background from being white on first load.
    if (settings->get("initialLoad") != "no")
    {
        settings->set("initialLoad", "no");
        settings->setColor("lewColor", QColor(119,136,153));
    }

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
    ui->actionSelectAll->setIcon(QIcon(basePath + "select_all.png"));
    ui->actionRaise->setIcon(QIcon(basePath + "raise.png"));
    ui->actionLower->setIcon(QIcon(basePath + "lower.png"));
    ui->actionRaiseLayer->setIcon(QIcon(basePath + "layer_up.png"));
    ui->actionLowerLayer->setIcon(QIcon(basePath + "layer_down.png"));
    ui->actionFullscreen->setIcon(QIcon(basePath + "expand.png"));
    ui->actionGrid->setIcon(QIcon(basePath + "grid.png"));
    ui->actionCheckerboard->setIcon(QIcon(basePath + "checkerboard.png"));
    ui->actionRenderLiquids->setIcon(QIcon(basePath + "render_liquids.png"));
    ui->actionRenderCameraLimits->setIcon(QIcon(basePath + "render_camera_limits.png"));
    ui->actionAddArea->setIcon(QIcon(basePath + "add.png"));
    ui->actionDeleteCurrentArea->setIcon(QIcon(basePath + "remove.png"));
    ui->actionSetBackgroundColor->setIcon(QIcon(basePath + "colors.png"));
    ui->actionResetBackgroundColor->setIcon(QIcon(basePath + "delete_colors.png"));
    ui->actionResetUiLayout->setIcon(QIcon(basePath + "reset_ui.png"));

    QList<int> splitterSizes;
    splitterSizes.append(350);
    splitterSizes.append(999999999);
    ui->splitter->setSizes(splitterSizes);
    ui->splitter->setCollapsible(0, false);

    loadArea(initialArea, false, true);

    updateAreaSelector(initialArea);
    ui->actionGrid->setChecked(settings->get("grid", false).toBool());
    ui->actionCheckerboard->setChecked(settings->get("checkerboard", false).toBool());
    ui->actionRenderLiquids->setChecked(settings->get("renderLiquids", true).toBool());
    ui->actionRenderCameraLimits->setChecked(settings->get("renderCameraLimits", true).toBool());

    editStatus->setText("Ready!"); // todo: do translation shit
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
    ui->menuSettings->setTitle(settings->getTranslation("General", "settings"));

    ui->actionSave->setText(settings->getTranslation("General", "save"));
    ui->actionSave->setToolTip(settings->getTranslation("General", "save"));

    ui->actionPaste->setText(settings->getTranslation("General", "paste"));
    ui->actionPaste->setToolTip(settings->getTranslation("General", "paste"));

    ui->actionCut->setText(settings->getTranslation("General", "cut"));
    ui->actionCut->setToolTip(settings->getTranslation("General", "cut"));

    ui->actionSelectAll->setText(settings->getTranslation("General", "selectAll"));
    ui->actionSelectAll->setToolTip(settings->getTranslation("General", "selectAll"));

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

    ui->actionCheckerboard->setText(settings->getTranslation("LevelEditor", "checkerboard"));
    ui->actionCheckerboard->setToolTip(settings->getTranslation("LevelEditor", "checkerboard"));

    ui->actionRenderLiquids->setText(settings->getTranslation("LevelEditor", "renderLiquids"));
    ui->actionRenderLiquids->setToolTip(settings->getTranslation("LevelEditor", "renderLiquids"));

    ui->actionRenderCameraLimits->setText(settings->getTranslation("LevelEditor", "renderCameraLimits"));
    ui->actionRenderCameraLimits->setToolTip(settings->getTranslation("LevelEditor", "renderCameraLimits"));

    ui->actionAddArea->setText(settings->getTranslation("LevelEditor", "addArea"));
    ui->actionAddArea->setToolTip(settings->getTranslation("LevelEditor", ""));

    ui->actionDeleteCurrentArea->setText(settings->getTranslation("LevelEditor", "removeArea"));
    ui->actionDeleteCurrentArea->setToolTip(settings->getTranslation("LevelEditor", "removeArea"));

    ui->actionToggleLayer1->setText(settings->getTranslation("LevelEditor", "layer") + " 1");
    ui->actionToggleLayer1->setToolTip(settings->getTranslation("LevelEditor", "layer") + " 1");

    ui->actionToggleLayer2->setText(settings->getTranslation("LevelEditor", "layer") + " 2");
    ui->actionToggleLayer2->setToolTip(settings->getTranslation("LevelEditor", "layer") + " 2");

    ui->actionToggleSprites->setText(settings->getTranslation("LevelEditor", "sprites"));
    ui->actionToggleSprites->setToolTip(settings->getTranslation("LevelEditor", "sprites"));

    ui->actionTogglePaths->setText(settings->getTranslation("LevelEditor", "paths"));
    ui->actionTogglePaths->setToolTip(settings->getTranslation("LevelEditor", "paths"));

    ui->actionSetBackgroundColor->setText(settings->getTranslation("LevelEditor", "setBgColor"));
    ui->actionSetBackgroundColor->setToolTip(settings->getTranslation("LevelEditor", "setBgColor"));

    ui->actionResetBackgroundColor->setText(settings->getTranslation("LevelEditor", "resetBgColor"));
    ui->actionResetBackgroundColor->setToolTip(settings->getTranslation("LevelEditor", "resetBgColor"));

    ui->actionResetUiLayout->setText(settings->getTranslation("LevelEditor", "resetUi"));
    ui->actionResetUiLayout->setToolTip(settings->getTranslation("LevelEditor", "resetUi"));
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

void LevelEditorWindow::on_actionToggleSprites_toggled(bool toggle)
{
    levelView->toggleSprites(toggle);
    update();
}

void LevelEditorWindow::on_actionTogglePaths_toggled(bool toggle)
{
    levelView->togglePaths(toggle);
    update();
}

void LevelEditorWindow::on_actionZoom_In_triggered()
{
    if (zoom < 4)
    {
        if (zoom < 1) zoom += 0.1f;
        else zoom += 0.25;
        levelView->setZoom(zoom);

        update();
    }
}

void LevelEditorWindow::on_actionZoom_Out_triggered()
{
    if (zoom > 0.3)
    {
        if (zoom <= 1.5) zoom -= 0.1f;
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
    zoom = 0.3f;
    levelView->setZoom(zoom);
    update();
}

void LevelEditorWindow::handleEditMade()
{
    editStatus->setText(settings->getTranslation("General", "unsavedChanges"));
    unsavedChanges = true;
}

void LevelEditorWindow::on_actionSave_triggered()
{
    qint8 res = levelView->saveLevel();
    if (res != 0)
        editStatus->setText("Save Failed.");
    else
    {
        editStatus->setText(settings->getTranslation("General", "changesSaved"));
        unsavedChanges = false;
    }
}

void LevelEditorWindow::on_actionCopy_triggered()
{
    levelView->copy();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionPaste_triggered()
{
    spriteIds->deselect();
    levelView->paste();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionCut_triggered()
{
    levelView->cut();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionDelete_triggered()
{
    levelView->deleteSel();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionSelectAll_triggered()
{
    levelView->selectAll();
}

void LevelEditorWindow::on_actionRaise_triggered()
{
    levelView->raise();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionLower_triggered()
{
    levelView->lower();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionRaiseLayer_triggered()
{
    levelView->raiseLayer();
    emit handleEditMade();
}

void LevelEditorWindow::on_actionLowerLayer_triggered()
{
    levelView->lowerLayer();
    emit handleEditMade();
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
    settings->set("grid", toggle);
}

void LevelEditorWindow::on_actionCheckerboard_toggled(bool toggle)
{
    levelView->toggleCheckerboard(toggle);
    settings->set("checkerboard", toggle);
}

void LevelEditorWindow::on_actionRenderLiquids_toggled(bool toggle)
{
    levelView->toggleRenderLiquids(toggle);
    settings->set("renderLiquids", toggle);
}

void LevelEditorWindow::on_actionRenderCameraLimits_toggled(bool toggle)
{
    levelView->toggleRenderCameraLimits(toggle);
    settings->set("renderCameraLimits", toggle);
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
        tilesetPaletteDock->show();
        tilesetPaletteDock->raise();
        tilesetPalette->select(dynamic_cast<BgdatObject*>(obj));
    }
    else if (is<Sprite*>(obj))
    {
        spriteEditorDock->show();
        spriteEditorDock->raise();
        spriteEditor->select(dynamic_cast<Sprite*>(obj));
        spriteEditor->spriteDataEditorPtr()->select(dynamic_cast<Sprite*>(obj));
    }
    if (is<Entrance*>(obj))
    {
        entranceEditorDock->show();
        entranceEditorDock->raise();
        entranceEditor->select(dynamic_cast<Entrance*>(obj));
    }
    else if (is<Zone*>(obj))
    {
        zoneEditorDock->show();
        zoneEditorDock->raise();
        zoneEditor->select(dynamic_cast<Zone*>(obj));
    }
    else if (is<Location*>(obj))
    {
        locationEditorDock->show();
        locationEditorDock->raise();
        locationEditor->select(dynamic_cast<Location*>(obj));
    }
    else if (is<PathNode*>(obj))
    {
        pathEditorDock->show();
        pathEditorDock->raise();
        pathEditor->select(dynamic_cast<PathNode*>(obj));
    }
    else if (is<ProgressPathNode*>(obj))
    {
        progPathEditorDock->show();
        progPathEditorDock->raise();
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
    spriteIds->deselect();
}

void LevelEditorWindow::updateEditors()
{
    spriteEditor->spriteDataEditorPtr()->updateEditor();
    entranceEditor->updateEditor();
    zoneEditor->updateEditor();
    locationEditor->updateEditor();
    pathEditor->updateEditor();
    progPathEditor->updateEditor();
    spriteIds->updateEditor();
}

void LevelEditorWindow::on_actionAddArea_triggered()
{
    bool ignore = false;
    if (unsavedChanges)
    {
        QMessageBox message(this);
        message.setWindowTitle(settings->getTranslation("General", "unsavedChanges"));
        message.setText(settings->getTranslation("General", "wantToSave"));
        message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)message.layout();
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

        switch (message.exec())
        {
        case QMessageBox::Save:
            levelView->saveLevel();
            unsavedChanges = false;
            editStatus->setText(settings->getTranslation("General", "changesSaved"));
            break;
        case QMessageBox::Discard:
            unsavedChanges = false;
            editStatus->setText("");
            break;
        case QMessageBox::Cancel:
            ignore = true;
            break;
        }
    }

    if (!ignore)
    {
        settings->set("dockPositions",this->saveState(1));

        if (lvlMgr->getAreaCount() >= 4)
        {
            QMessageBox::information(this, "CoinKiller", "Due to limitations there can only be a maximum of 4 areas in a level.", QMessageBox::Ok);
            return;
        }

    int seekArea = lvlMgr->addArea(lvlMgr->getAreaCount());
    loadArea(seekArea);
    updateAreaSelector(seekArea);
    }
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
    }

    level = lvlMgr->openArea(id);

    levelView = new LevelView(this, level);
    ui->levelViewArea->setWidget(levelView);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);

    layerMask = 0x7;
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);
    ui->actionToggleSprites->setChecked(true);
    ui->actionTogglePaths->setChecked(true);
    levelView->setLayerMask(layerMask);
    levelView->toggleGrid(ui->actionGrid->isChecked());
    levelView->toggleCheckerboard(ui->actionCheckerboard->isChecked());
    levelView->toggleRenderLiquids(ui->actionRenderLiquids->isChecked());
    levelView->toggleRenderCameraLimits(ui->actionRenderCameraLimits->isChecked());
    levelView->setBackgroundColor(settings->getColor("lewColor"));

    zoom = 1.0;

    setupWidgets(false, init);

    // Avoid issue with objects being un-selectable when opening an area
    levelView->toggleSprites(false);
    levelView->toggleSprites(true);
    levelView->togglePaths(false);
    levelView->togglePaths(true);
    update();
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
    bool ignore = false;

    if (unsavedChanges)
    {
        QMessageBox message(this);
        message.setWindowTitle(settings->getTranslation("General", "unsavedChanges"));
        message.setText(settings->getTranslation("General", "wantToSave"));
        message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)message.layout();
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

        switch (message.exec())
        {
        case QMessageBox::Save:
            levelView->saveLevel();
            unsavedChanges = false;
            editStatus->setText(settings->getTranslation("General", "changesSaved"));
            break;
        case QMessageBox::Discard:
            unsavedChanges = false;
            editStatus->setText("Ready!");
            break;
        case QMessageBox::Cancel:
            ignore = true;
            break;
        }
    }

    if (!ignore)
    {
        settings->set("dockPositions",this->saveState(1));

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
    else
    {
        areaSelector->blockSignals(true);
        areaSelector->setCurrentIndex(level->getAreaID()-1);
        areaSelector->blockSignals(false);
    }
}

void LevelEditorWindow::handleMgrUpdate()
{
    updateAreaSelector(level->getAreaID());
}

void LevelEditorWindow::scrollTo(int x, int y)
{
    qMax(0, x);
    qMax(0, y);
    ui->levelViewArea->horizontalScrollBar()->setValue(x);
    ui->levelViewArea->verticalScrollBar()->setValue(y);
}

void LevelEditorWindow::setupWidgets(bool resetLayout, bool init)
{
    if (!init)
    {
        delete areaEditor;
        delete tilesetPalette;
        delete spriteEditor;
        delete entranceEditor;
        delete zoneEditor;
        delete locationEditor;
        delete pathEditor;
        delete progPathEditor;
        delete spriteIds;
        delete miniMap;

        delete areaEditorDock;
        delete tilesetPaletteDock;
        delete spriteEditorDock;
        delete entranceEditorDock;
        delete zoneEditorDock;
        delete locationEditorDock;
        delete pathEditorDock;
        delete progPathEditorDock;
        delete spriteIdsDock;
        delete miniMapDock;
    }

    // Setup Area Editor
    areaEditor = new AreaEditorWidget(level, lvlMgr->getGame());
    connect(areaEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(areaEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    areaEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "areaEditor"), this);
    areaEditorDock->setWidget(areaEditor);
    areaEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    areaEditorDock->setObjectName("areaEditor");

    // Setup Tileset Picker
    tilesetPalette = new TilesetPalette(level, levelView->objEditionModePtr());
    connect(areaEditor, SIGNAL(relaodTilesetPicker()), tilesetPalette, SLOT(reloadTilesets()));

    tilesetPaletteDock = new QDockWidget(settings->getTranslation("LevelEditor", "tilesetEditor"), this);
    tilesetPaletteDock->setWidget(tilesetPalette);
    tilesetPaletteDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    tilesetPaletteDock->setObjectName("tilesetPallete");

    // Setup Sprite Picker
    spriteEditor = new SpriteEditorWidget();
    connect(spriteEditor->spriteDataEditorPtr(), SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(spriteEditor, SIGNAL(selectedSpriteChanged(int)), this, SLOT(setSelSprite(int)));
    connect(spriteEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    spriteEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "spriteEditor"), this);
    spriteEditorDock->setWidget(spriteEditor);
    spriteEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    spriteEditorDock->setObjectName("spriteEditor");

    // Setup Entrance Editor
    entranceEditor = new EntranceEditorWidget(&level->entrances);
    connect(entranceEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(entranceEditor, SIGNAL(selectedEntrChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(entranceEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    entranceEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "entranceEditor"), this);
    entranceEditorDock->setWidget(entranceEditor);
    entranceEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    entranceEditorDock->setObjectName("entranceEditor");

    // Setup Zone Editor
    zoneEditor = new ZoneEditorWidget(&level->zones);
    connect(zoneEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(zoneEditor, SIGNAL(selectedZoneChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(zoneEditor, SIGNAL(selectZoneContents(Zone*)), levelView, SLOT(selectZoneContents(Zone*)));
    connect(zoneEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    zoneEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "zoneEditor"), this);
    zoneEditorDock->setWidget(zoneEditor);
    zoneEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    zoneEditorDock->setObjectName("zoneEditor");

    // Setup Location Editor
    locationEditor = new LocationEditorWidget(&level->locations);
    connect(locationEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(locationEditor, SIGNAL(selectedLocChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(locationEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    locationEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "locationEditor"), this);
    locationEditorDock->setWidget(locationEditor);
    locationEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    locationEditorDock->setObjectName("locationEditor");

    // Setup Path Editor
    pathEditor = new PathEditorWidget(&level->paths);
    connect(pathEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(pathEditor, SIGNAL(selectedPathChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(pathEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    pathEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "pathEditor"), this);
    pathEditorDock->setWidget(pathEditor);
    pathEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    pathEditorDock->setObjectName("pathEditor");

    // Setup Progress Path Editor
    progPathEditor = new ProgressPathEditorWidget(&level->progressPaths);
    connect(progPathEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(progPathEditor, SIGNAL(selectedProgPathChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(progPathEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    progPathEditorDock = new QDockWidget(settings->getTranslation("LevelEditor", "progPathEditor"), this);
    progPathEditorDock->setWidget(progPathEditor);
    progPathEditorDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    progPathEditorDock->setObjectName("progPathEditor");

    // Setup Sprite Ids Widget
    spriteIds = new SpriteIdWidget(&level->sprites);
    connect(spriteIds, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(spriteIds, SIGNAL(selectedSpriteChanged(Object*)), levelView, SLOT(selectObj(Object*)));

    spriteIdsDock = new QDockWidget(settings->getTranslation("LevelEditor", "eventIds"), this);
    spriteIdsDock->setWidget(spriteIds);
    spriteIdsDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    spriteIdsDock->setObjectName("spriteIds");

    // Setup Minimap
    QScrollArea* miniMapArea = new QScrollArea(this);
    miniMapArea->setWidgetResizable(true);
    miniMapArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    miniMapArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    miniMap = new LevelMiniMap(this, level);
    connect(levelView, SIGNAL(updateMinimap(QRect)), miniMap, SLOT(update_(QRect)));
    connect(levelView, SIGNAL(updateMinimapBounds()), miniMap, SLOT(updateBounds()));
    connect(miniMap, SIGNAL(scrollTo(int,int)), this, SLOT(scrollTo(int,int)));
    miniMapArea->setWidget(miniMap);

    miniMapDock = new QDockWidget(settings->getTranslation("LevelEditor", "miniMap"), this);
    miniMapDock->setWidget(miniMapArea);
    miniMapDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    miniMapDock->setObjectName("miniMap");

    connect(levelView, SIGNAL(scrollTo(int,int)), this, SLOT(scrollTo(int,int)));
    connect(levelView->editionModePtr(), SIGNAL(selectdObjectChanged(Object*)), this, SLOT(setObjectEdition(Object*)));
    connect(levelView->editionModePtr(), SIGNAL(deselected()), this, SLOT(deselect()));
    connect(levelView->editionModePtr(), SIGNAL(updateEditors()), this, SLOT(updateEditors()));
    connect(levelView->editionModePtr(), SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Ui Layout
    if ((this->restoreState(settings->get("dockPositions").toByteArray(), 1)) && (!resetLayout))
        this->restoreState(settings->get("dockPositions").toByteArray(), 1);
    else
    {
        addDockWidget(Qt::LeftDockWidgetArea, areaEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, tilesetPaletteDock);
        addDockWidget(Qt::LeftDockWidgetArea, spriteEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, spriteIdsDock);
        addDockWidget(Qt::LeftDockWidgetArea, entranceEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, zoneEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, locationEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, pathEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, progPathEditorDock);
        addDockWidget(Qt::LeftDockWidgetArea, miniMapDock);

        tabifyDockWidget(areaEditorDock, tilesetPaletteDock);
        tabifyDockWidget(tilesetPaletteDock, spriteEditorDock);
        tabifyDockWidget(spriteEditorDock, spriteIdsDock);
        tabifyDockWidget(spriteIdsDock, entranceEditorDock);
        tabifyDockWidget(entranceEditorDock, zoneEditorDock);
        tabifyDockWidget(zoneEditorDock, locationEditorDock);
        tabifyDockWidget(locationEditorDock, pathEditorDock);
        tabifyDockWidget(pathEditorDock, progPathEditorDock);
    }

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    areaEditorDock->show();
    areaEditorDock->raise();
}

void LevelEditorWindow::on_actionResetUiLayout_triggered()
{
    setupWidgets(true, false);
}

void LevelEditorWindow::on_actionSetBackgroundColor_triggered()
{
    QColor bgColor = QColorDialog::getColor(settings->getColor("lewColor", QColor(119,136,153)), this, "Select Background Color",  QColorDialog::DontUseNativeDialog);
    if(bgColor.isValid())
    {
        levelView->setBackgroundColor(bgColor);
        settings->setColor("lewColor", bgColor);
    }
}

void LevelEditorWindow::on_actionResetBackgroundColor_triggered()
{
    QMessageBox::StandardButton reset;
    reset = QMessageBox::question(this, "CoinKiller", QString("Are you sure you wish to reset the background color?"), QMessageBox::Cancel|QMessageBox::Ok);
    if(reset == QMessageBox::Ok)
    {
        levelView->setBackgroundColor(QColor(119,136,153));
        settings->setColor("lewColor", QColor(119,136,153));
    }
}

void LevelEditorWindow::closeEvent(QCloseEvent *event)
{
    if (unsavedChanges)
    {
        QMessageBox message(this);
        message.setWindowTitle(settings->getTranslation("General", "unsavedChanges"));
        message.setText(settings->getTranslation("General", "wantToSave"));
        message.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);

        QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)message.layout();
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

        switch (message.exec())
        {
        case QMessageBox::Save:
            settings->set("dockPositions",this->saveState(1));
            levelView->saveLevel();
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        case QMessageBox::Discard:
            settings->set("dockPositions",this->saveState(1));
            event->accept();
            break;
        }
    }
    else
    {
        settings->set("dockPositions",this->saveState(1));
        event->accept();
    }
}
