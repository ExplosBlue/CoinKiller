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

#include "settingsmanager.h"

#include <QHBoxLayout>
#include <QSizePolicy>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QScrollBar>
#include <QColorDialog>
#include <QStatusBar>

#ifdef USE_KDE_BLUR
    #include <KF5/KWindowSystem/KWindowEffects>
#endif

LevelEditorWindow::LevelEditorWindow(LevelManager* lvlMgr, int initialArea) :
    WindowBase(lvlMgr->getParent()),
    ui(new Ui::LevelEditorWindow)
{
    this->lvlMgr = lvlMgr;
    this->settings = SettingsManager::getInstance();

    if (settings->get("maximised", false).toBool())
        this->setWindowState(Qt::WindowMaximized);

    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    areaSelector = new QComboBox(this);
    ui->toolBar->insertWidget(ui->actionAddArea, areaSelector);
    connect(areaSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAreaIndexChange(int)));

    editStatus = new QLabel(this);
    ui->statusbar->addWidget(editStatus);
    //connect(editStatus, SIGNAL(updateLevelLabel(QString)), editStatus, SLOT(setText(QString)));

    // Load UI Icons
    QString basePath(settings->dataPath("icons/"));
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
    ui->actionToggleLayer1->setIcon(QIcon(basePath + "layer1.png"));
    ui->actionToggleLayer2->setIcon(QIcon(basePath + "layer2.png"));
    ui->actionToggleSprites->setIcon(QIcon(basePath + "sprite.png"));
    ui->actionTogglePaths->setIcon(QIcon(basePath + "path.png"));
    ui->actionToggleLocations->setIcon(QIcon(basePath + "location.png"));
    ui->actionToggle3DOverlay->setIcon(QIcon(basePath + "3D.png"));
    ui->actionToggle2DTile->setIcon(QIcon(basePath + "2D.png"));

    toolboxDock = new QDockWidget(this);
    toolboxDock->setObjectName("toolboxDock");
    toolboxDock->setWindowTitle(tr("Toolbox"));
    toolboxTabs = new QTabWidget(this);
    connect(toolboxTabs, SIGNAL(currentChanged(int)), this, SLOT(toolboxTabsCurrentChanged(int)));
    toolboxDock->setWidget(toolboxTabs);

    minimapDock = new QDockWidget(this);
    minimapDock->setObjectName("minimapDock");
    minimapDock->setWindowTitle(tr("Minimap"));

    addDockWidget(Qt::LeftDockWidgetArea, toolboxDock);
    addDockWidget(Qt::LeftDockWidgetArea, minimapDock);

    restoreState(settings->get("lvleditorState").toByteArray());
    updateDockedWidgetCheckboxes();
    connect(toolboxDock, SIGNAL(visibilityChanged(bool)), this, SLOT(updateDockedWidgetCheckboxes()));
    connect(minimapDock, SIGNAL(visibilityChanged(bool)), this, SLOT(updateDockedWidgetCheckboxes()));

    loadArea(initialArea, false, true);

    updateAreaSelector(initialArea);
    ui->actionGrid->setChecked(settings->get("grid", false).toBool());
    ui->actionCheckerboard->setChecked(settings->get("checkerboard", false).toBool());
    ui->actionRenderLiquids->setChecked(settings->get("renderLiquids", true).toBool());
    ui->actionRenderCameraLimits->setChecked(settings->get("renderCameraLimits", true).toBool());
    ui->actionHideStatusbar->setChecked(settings->get("lvleditorHideStatusBar", false).toBool());

    editStatus->setText(tr("Ready!"));

#ifdef USE_KDE_BLUR
    if (KWindowEffects::isEffectAvailable(KWindowEffects::BlurBehind))
    {
        setAttribute(Qt::WA_TranslucentBackground);
        KWindowEffects::enableBlurBehind(winId(), true);
        setBlurStylesheet();
    }

    ui->levelViewArea->setFrameShape(QScrollArea::NoFrame);
    ui->levelViewArea->setLineWidth(0);
    ui->levelViewArea->setFrameShadow(QScrollArea::Plain);
#endif

}

LevelEditorWindow::~LevelEditorWindow()
{
    if (closeLvlOnClose)
        lvlMgr->closeArea(level);
    if (lvlMgr->getOpenedAreaCount() == 0)
        delete lvlMgr;
    delete ui;
}

void LevelEditorWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QMainWindow::changeEvent(event);
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

void LevelEditorWindow::on_actionToggleLocations_toggled(bool toggle)
{
    levelView->toggleLocations(toggle);
    update();
}

void LevelEditorWindow::on_actionToggle3DOverlay_toggled(bool toggle)
{
    levelView->toggle3DOverlay(toggle);
    update();
}

void LevelEditorWindow::on_actionToggle2DTile_toggled(bool toggle)
{
    levelView->toggle2DTile(toggle);
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
    editStatus->setText(tr("Unsaved Changes"));
    unsavedChanges = true;
}

void LevelEditorWindow::on_actionSave_triggered()
{
    qint8 res = levelView->saveLevel();
    if (res != 0)
        editStatus->setText(tr("Save Failed"));
    else
    {
        editStatus->setText(tr("Changes Saved"));
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
    spriteEditor->spriteIdsPtr()->deselect();
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
        ui->actionFullscreen->setIcon(QIcon(SettingsManager::getInstance()->dataPath("icons/collapse.png")));
    }
    else
    {
        showNormal();
        ui->actionFullscreen->setIcon(QIcon(SettingsManager::getInstance()->dataPath("icons/expand.png")));
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

void LevelEditorWindow::on_actionSelectAfterPlacement_toggled(bool toggle)
{
    levelView->objEditionModePtr()->toggleSelectAfterPlacement(toggle);
    settings->set("SelectAfterPlacement", toggle);
}

void LevelEditorWindow::setObjectEdition(Object* obj)
{
    deselect();

    if (is<BgdatObject*>(obj))
    {
        toolboxTabs->setCurrentIndex(1);
        tilesetPalette->select(dynamic_cast<BgdatObject*>(obj));
    }
    else if (is<Sprite*>(obj))
    {
        toolboxTabs->setCurrentIndex(2);
        spriteEditor->select(dynamic_cast<Sprite*>(obj));
        spriteEditor->spriteDataEditorPtr()->select(dynamic_cast<Sprite*>(obj));
    }
    if (is<Entrance*>(obj))
    {
        toolboxTabs->setCurrentIndex(3);
        entranceEditor->select(dynamic_cast<Entrance*>(obj));
    }
    else if (is<Zone*>(obj))
    {
        toolboxTabs->setCurrentIndex(4);
        zoneEditor->select(dynamic_cast<Zone*>(obj));
    }
    else if (is<Location*>(obj))
    {
        toolboxTabs->setCurrentIndex(5);
        locationEditor->select(dynamic_cast<Location*>(obj));
    }
    else if (is<PathNode*>(obj))
    {
        toolboxTabs->setCurrentIndex(6);
        pathEditor->select(dynamic_cast<PathNode*>(obj));
    }
    else if (is<ProgressPathNode*>(obj))
    {
        toolboxTabs->setCurrentIndex(7);
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
    spriteEditor->spriteIdsPtr()->deselect();
}

void LevelEditorWindow::updateEditors()
{
    spriteEditor->spriteDataEditorPtr()->updateEditor();
    entranceEditor->updateEditor();
    zoneEditor->updateEditor();
    locationEditor->updateEditor();
    pathEditor->updateEditor();
    progPathEditor->updateEditor();
    spriteEditor->spriteIdsPtr()->updateEditor();
}

void LevelEditorWindow::toolboxTabsCurrentChanged(int index)
{
    if (index == 0)
        levelView->objEditionModePtr()->setDrawType(-1);
    else
    {
        levelView->objEditionModePtr()->setDrawType(index-1);
    }
}

void LevelEditorWindow::on_actionAddArea_triggered()
{
    bool ignore = false;
    if (unsavedChanges)
    {
        QMessageBox message(this);
        message.setWindowTitle(tr("Unsaved Changes"));
        message.setText(tr("Do you want to save your changes?"));
        message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)message.layout();
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

        switch (message.exec())
        {
        case QMessageBox::Save:
            levelView->saveLevel();
            unsavedChanges = false;
            editStatus->setText(tr("Changes Saved"));
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
        if (lvlMgr->getAreaCount() >= 4)
        {
            QMessageBox::information(this, "CoinKiller", tr("Due to limitations there can only be a maximum of 4 areas in a level."), QMessageBox::Ok);
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
        QMessageBox::information(this, "CoinKiller", tr("This area cannot be deleted because there has to be at least one area in a level."), QMessageBox::Ok);
        return;
    }

    QMessageBox warning;
    warning.setWindowTitle("CoinKiller");
    warning.setText(tr("Are you sure you want to delete this area?\n\nThe level will automatically save afterwards and therefore the deletion of this area cannot be undone afterwards."));
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
        QMessageBox::information(this, "CoinKiller", tr("Area %1 does not exist.").arg(id));
        return;
    }

    toolboxTabs->blockSignals(true);

    if (closeLevel)
        lvlMgr->closeArea(level);

    if (!init)
    {
        for(int i = toolboxTabs->count() - 1; i >= 0; i--)
        {
            QWidget* deleteWidget = toolboxTabs->widget(i);
            toolboxTabs->removeTab(i);
            delete deleteWidget;
        }

        delete levelView;
        delete miniMap;
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
    ui->actionToggleLocations->setChecked(true);
    ui->actionToggle2DTile->setChecked(true);
    ui->actionToggle3DOverlay->setChecked(true);
    ui->actionSelectAfterPlacement->setChecked(settings->get("SelectAfterPlacement").toBool());
    levelView->setLayerMask(layerMask);
    levelView->toggleGrid(ui->actionGrid->isChecked());
    levelView->toggleCheckerboard(ui->actionCheckerboard->isChecked());
    levelView->toggleRenderLiquids(ui->actionRenderLiquids->isChecked());
    levelView->toggleRenderCameraLimits(ui->actionRenderCameraLimits->isChecked());
    levelView->setBackgroundColor(settings->getColor("lewColor", QColor(119,136,153)));

#ifdef USE_KDE_BLUR
    setBlurStylesheet();
#endif

    levelView->toggleSprites(ui->actionToggleSprites->isChecked());
    levelView->togglePaths(ui->actionTogglePaths->isChecked());
    levelView->toggleLocations(ui->actionToggleLocations->isChecked());

    zoom = 1.0;

    QString basePath(settings->dataPath("icons/"));

    // Setup Area Editor
    areaEditor = new AreaEditorWidget(level, lvlMgr->getGame());
    connect(areaEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(areaEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Tileset Picker
    tilesetPalette = new TilesetPalette(level, levelView->objEditionModePtr());
    connect(areaEditor, SIGNAL(relaodTilesetPicker()), tilesetPalette, SLOT(reloadTilesets()));

    // Setup Sprite Picker
    spriteEditor = new SpriteEditorWidget(&level->sprites);
    connect(spriteEditor->spriteDataEditorPtr(), SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(spriteEditor, SIGNAL(currentSpriteChanged(int)), this, SLOT(setSelSprite(int)));
    connect(spriteEditor, SIGNAL(selectedSpriteChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(spriteEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(spriteEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Entrance Editor
    entranceEditor = new EntranceEditorWidget(&level->entrances);
    connect(entranceEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(entranceEditor, SIGNAL(selectedEntrChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(entranceEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Zone Editor
    zoneEditor = new ZoneEditorWidget(&level->zones, &level->backgrounds, &level->boundings);
    connect(zoneEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(zoneEditor, SIGNAL(selectedZoneChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(zoneEditor, SIGNAL(selectZoneContents(Zone*)), levelView, SLOT(selectZoneContents(Zone*)));
    connect(zoneEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));
    connect(zoneEditor, SIGNAL(screenshot(QRect)), levelView, SLOT(screenshot(QRect)));

    // Setup Location Editor
    locationEditor = new LocationEditorWidget(&level->locations);
    connect(locationEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(locationEditor, SIGNAL(selectedLocChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(locationEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Path Editor
    pathEditor = new PathEditorWidget(&level->paths);
    connect(pathEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(pathEditor, SIGNAL(selectedPathChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(pathEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Progress Path Editor
    progPathEditor = new ProgressPathEditorWidget(&level->progressPaths);
    connect(progPathEditor, SIGNAL(updateLevelView()), levelView, SLOT(update()));
    connect(progPathEditor, SIGNAL(selectedProgPathChanged(Object*)), levelView, SLOT(selectObj(Object*)));
    connect(progPathEditor, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    // Setup Event Editor
    eventEditor = new EventEditorWidget(level);

    connect(levelView, SIGNAL(scrollTo(int,int)), this, SLOT(scrollTo(int,int)));
    connect(levelView->editionModePtr(), SIGNAL(selectdObjectChanged(Object*)), this, SLOT(setObjectEdition(Object*)));
    connect(levelView->editionModePtr(), SIGNAL(deselected()), this, SLOT(deselect()));
    connect(levelView->editionModePtr(), SIGNAL(updateEditors()), this, SLOT(updateEditors()));
    connect(levelView->editionModePtr(), SIGNAL(editMade()), this, SLOT(handleEditMade()));

    toolboxTabs->setUsesScrollButtons(true);
    toolboxTabs->addTab(areaEditor, QIcon(basePath + "settings.png"), "");
    toolboxTabs->setTabToolTip(0, "Area Settings");
    toolboxTabs->addTab(tilesetPalette, QIcon(basePath + "block.png"), "");
    toolboxTabs->setTabToolTip(1, "Tileset Palette");
    toolboxTabs->addTab(spriteEditor, QIcon(basePath + "sprite.png"), "");
    toolboxTabs->setTabToolTip(2, "Sprites");
    toolboxTabs->addTab(entranceEditor, QIcon(basePath + "entrance.png"), "");
    toolboxTabs->setTabToolTip(3, "Entrances");
    toolboxTabs->addTab(zoneEditor, QIcon(basePath + "zone.png"), "");
    toolboxTabs->setTabToolTip(4, "Zones");
    toolboxTabs->addTab(locationEditor, QIcon(basePath + "location.png"), "");
    toolboxTabs->setTabToolTip(5, "Locations");
    toolboxTabs->addTab(pathEditor, QIcon(basePath + "path.png"), "");
    toolboxTabs->setTabToolTip(6, "Paths");
    toolboxTabs->addTab(progPathEditor, QIcon(basePath + "progress_path.png"), "");
    toolboxTabs->setTabToolTip(7, "Progress Paths");
    toolboxTabs->addTab(eventEditor, QIcon(basePath + "event.png"), "");
    toolboxTabs->setTabToolTip(8, "Area Event States");

    miniMap = new LevelMiniMap(this, level);
    connect(levelView, SIGNAL(updateMinimap(QRect)), miniMap, SLOT(update_(QRect)));
    connect(levelView, SIGNAL(updateMinimapBounds()), miniMap, SLOT(updateBounds()));
    connect(miniMap, SIGNAL(scrollTo(int,int)), this, SLOT(scrollToWithZoom(int,int)));
    //ui->miniMap->setWidget(miniMap);
    minimapDock->setWidget(miniMap);

    update();

    toolboxTabs->blockSignals(false);
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
       areaStrings << tr("Area %2").arg(i+1);

    QStringListModel *model = new QStringListModel();
        model->setStringList(areaStrings);

    areaSelector->setModel(model);
    if (index == -1) areaSelector->setCurrentIndex(lastIndex);
    else areaSelector->setCurrentIndex(index-1);

    areaSelector->blockSignals(false);

    QString title;
    level->getName(title);
    setWindowTitle(tr("CoinKiller - Editing: %1").arg(title));
}

void LevelEditorWindow::handleAreaIndexChange(int index)
{
    bool ignore = false;

    if (unsavedChanges)
    {
        QMessageBox message(this);
        message.setWindowTitle(tr("Unsaved Changes"));
        message.setText(tr("Do you want to save your changes?"));
        message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)message.layout();
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

        switch (message.exec())
        {
        case QMessageBox::Save:
            levelView->saveLevel();
            unsavedChanges = false;
            editStatus->setText(tr("Changes Saved"));
            break;
        case QMessageBox::Discard:
            unsavedChanges = false;
            editStatus->setText(tr("Ready!"));
            break;
        case QMessageBox::Cancel:
            ignore = true;
            break;
        }
    }

    if (!ignore)
    {
        index++;

        if (lvlMgr->areaIsOpen(index))
        {
            updateAreaSelector(level->getAreaID());
            QMessageBox::information(this, "CoinKiller", tr("Area %1 cannot be opened because it is already opened in another editor window.").arg(index), QMessageBox::Ok);
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

void LevelEditorWindow::scrollToWithZoom(int x, int y)
{
    qMax(0.0f, x*zoom);
    qMax(0.0f, y*zoom);
    ui->levelViewArea->horizontalScrollBar()->setValue(x*zoom);
    ui->levelViewArea->verticalScrollBar()->setValue(y*zoom);
}

void LevelEditorWindow::on_actionSetBackgroundColor_triggered()
{
    QColorDialog::ColorDialogOptions options = QColorDialog::DontUseNativeDialog;
#ifdef USE_KDE_BLUR
    options |= QColorDialog::ShowAlphaChannel;
#endif

    QColor bgColor = QColorDialog::getColor(settings->getColor("lewColor", QColor(119,136,153)), this, tr("Select Background Color"), options);
    if(bgColor.isValid())
    {
        levelView->setBackgroundColor(bgColor);
        settings->setColor("lewColor", bgColor);
    }
}

void LevelEditorWindow::on_actionResetBackgroundColor_triggered()
{
    QMessageBox::StandardButton reset;
    reset = QMessageBox::question(this, "CoinKiller", tr("Are you sure you wish to reset the background color?"), QMessageBox::Cancel|QMessageBox::Ok);
    if(reset == QMessageBox::Ok)
    {
        levelView->setBackgroundColor(QColor(119,136,153));
        settings->setColor("lewColor", QColor(119,136,153));
    }
}

void LevelEditorWindow::on_actionHideStatusbar_toggled(bool hide)
{
    settings->set("lvleditorHideStatusBar", hide);
    ui->statusbar->setHidden(hide);
}

void LevelEditorWindow::closeEvent(QCloseEvent *event)
{
    if (unsavedChanges)
    {
        QMessageBox message(this);
        message.setWindowTitle(tr("Unsaved Changes"));
        message.setText(tr("Do you want to save your changes?"));
        message.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);

        QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)message.layout();
        layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

        switch (message.exec())
        {
        case QMessageBox::Save:
            levelView->saveLevel();
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        }
    }
    else event->accept();

    if (event->isAccepted())
    {
        settings->set("lvleditorState", saveState());
    }
}

void LevelEditorWindow::on_actionShowToolbox_toggled(bool checked)
{
    toolboxDock->setHidden(!checked);
}

void LevelEditorWindow::on_actionShowMinimap_toggled(bool checked)
{
    minimapDock->setHidden(!checked);
}

void LevelEditorWindow::updateDockedWidgetCheckboxes()
{
    ui->actionShowToolbox->setChecked(!toolboxDock->isHidden());
    ui->actionShowMinimap->setChecked(!minimapDock->isHidden());
}

#ifdef USE_KDE_BLUR
void LevelEditorWindow::setBlurStylesheet()
{
    setStyleSheet("#centralwidget { background-color: rgba(0,0,0,0); }");
}

void LevelEditorWindow::paintEvent(QPaintEvent* evt)
{
    QColor bgColor = QWidget::palette().color(QWidget::backgroundRole());

    QPoint origin = ui->levelViewArea->mapToGlobal(ui->levelViewArea->pos()) - ui->menubar->mapToGlobal(ui->menubar->pos());
    QRect clearRect(origin.x(), origin.y(), ui->levelViewArea->width(), ui->levelViewArea->height());

    int barWidth = ui->levelViewArea->verticalScrollBar()->width();
    int barHeight = ui->levelViewArea->horizontalScrollBar()->height();
    QPoint scrollBoxPos = origin + QPoint(ui->levelViewArea->width() - barWidth, ui->levelViewArea->height() - barHeight);
    QRect scrollBoxFix(scrollBoxPos.x(), scrollBoxPos.y(), barWidth, barHeight);

    QPainter painter;
    painter.begin(this);

    QPainterPath path;
    path.setFillRule(Qt::OddEvenFill);
    path.addRect(evt->rect());
    path.addRect(clearRect);
    path.addRect(scrollBoxFix);
    painter.setBrush(bgColor);
    painter.setPen(bgColor);
    painter.drawPath(path);

    painter.end();

    //ui->levelViewArea->horizontalScrollBar()->height()
}
#endif
