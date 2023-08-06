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
#include "settingsdialog.h"

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
    WindowBase(lvlMgr->getParent()), ui(new Ui::LevelEditorWindow)
{
    this->lvlMgr = lvlMgr;
    this->settings = SettingsManager::getInstance();

    if (settings->get("maximised", false).toBool())
        this->setWindowState(Qt::WindowMaximized);

    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    areaSelector = new QComboBox(this);
    ui->toolBar->insertWidget(ui->actionAddArea, areaSelector);
    connect(areaSelector, &QComboBox::currentIndexChanged, this, &LevelEditorWindow::handleAreaIndexChange);

    statusLabel = new QLabel(this);
    ui->statusbar->addWidget(statusLabel);

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

    connect(ui->actionShowPreferences, &QAction::triggered, this, &LevelEditorWindow::showPreferencesDialog);

    ui->actionToggleLayer1->setIcon(QIcon(basePath + "layer1.png"));
    connect(ui->actionToggleLayer1, &QAction::toggled, this, &LevelEditorWindow::toggleLayer);

    ui->actionToggleLayer2->setIcon(QIcon(basePath + "layer2.png"));
    connect(ui->actionToggleLayer2, &QAction::toggled, this, &LevelEditorWindow::toggleLayer);

    ui->actionToggleSprites->setIcon(QIcon(basePath + "sprite.png"));
    connect(ui->actionToggleSprites, &QAction::toggled, this, &LevelEditorWindow::toggleSprites);

    ui->actionToggleEntrances->setIcon(QIcon(basePath + "entrance.png"));
    connect(ui->actionToggleEntrances, &QAction::toggled, this, &LevelEditorWindow::toggleEntrances);

    ui->actionTogglePaths->setIcon(QIcon(basePath + "path.png"));
    connect(ui->actionTogglePaths, &QAction::toggled, this, &LevelEditorWindow::togglePaths);

    ui->actionToggleLocations->setIcon(QIcon(basePath + "location.png"));
    connect(ui->actionToggleLocations, &QAction::toggled, this, &LevelEditorWindow::toggleLocations);

    ui->actionToggle3DOverlay->setIcon(QIcon(basePath + "3D.png"));
    ui->actionToggle2DTile->setIcon(QIcon(basePath + "2D.png"));

    // Set Shortcuts
    QList<QKeySequence> deleteShortcuts;
    deleteShortcuts << QKeySequence("del") << QKeySequence("backspace");
    ui->actionDelete->setShortcuts(deleteShortcuts);

    toolboxDock = new QDockWidget(this);
    toolboxDock->setObjectName("toolboxDock");
    toolboxDock->setWindowTitle(tr("Toolbox"));
    toolboxTabs = new QTabWidget(this);
    connect(toolboxTabs, &QTabWidget::currentChanged, this, &LevelEditorWindow::toolboxTabsCurrentChanged);
    toolboxDock->setWidget(toolboxTabs);

    minimapDock = new QDockWidget(this);
    minimapDock->setObjectName("minimapDock");
    minimapDock->setWindowTitle(tr("Minimap"));

    addDockWidget(Qt::LeftDockWidgetArea, toolboxDock);
    addDockWidget(Qt::LeftDockWidgetArea, minimapDock);

    // Undo/Redo
    undoStack = new QUndoStack(this);
    connect(undoStack, &QUndoStack::indexChanged, this, &LevelEditorWindow::historyStateChanged);

    actionUndo = undoStack->createUndoAction(this, tr("&Undo"));
    actionUndo->setIcon(QIcon(basePath + "undo.png"));
    actionUndo->setShortcuts(QKeySequence::Undo);

    actionRedo = undoStack->createRedoAction(this, tr("&Redo"));
    actionRedo->setIcon(QIcon(basePath + "redo.png"));
    actionRedo->setShortcuts(QKeySequence::Redo);

    // Add actions to ui
    ui->toolBar->addAction(actionUndo);
    ui->toolBar->addAction(actionRedo);

    // Add actions to ui
    ui->menuEdit->addAction(actionUndo);
    ui->menuEdit->addAction(actionRedo);

    // Create undo view
    undoView = new QUndoView(undoStack);
    undoView->setEmptyLabel(tr("<Empty>"));

    historyDock = new QDockWidget(this);
    historyDock->setObjectName("historyDock");
    historyDock->setWindowTitle(tr("History"));
    historyDock->setWidget(undoView);
    historyDock->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, historyDock);

    restoreState(settings->get("lvleditorState").toByteArray());
    updateDockedWidgetCheckboxes();
    connect(toolboxDock, &QDockWidget::visibilityChanged, this, &LevelEditorWindow::updateDockedWidgetCheckboxes);
    connect(minimapDock, &QDockWidget::visibilityChanged, this, &LevelEditorWindow::updateDockedWidgetCheckboxes);
    connect(minimapDock, &QDockWidget::visibilityChanged, this, &LevelEditorWindow::updateDockedWidgetCheckboxes);

    loadArea(initialArea, false, true);

    updateAreaSelector(initialArea);
    ui->actionGrid->setChecked(settings->get("grid", false).toBool());
    ui->actionCheckerboard->setChecked(settings->get("checkerboard", false).toBool());
    ui->actionRenderLiquids->setChecked(settings->get("renderLiquids", true).toBool());
    ui->actionRenderCameraLimits->setChecked(settings->get("renderCameraLimits", true).toBool());

    loadSettings();
    setStatus(Ready);

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

void LevelEditorWindow::showPreferencesDialog(bool show) {
    SettingsDialog settingsDialog(this);
    connect(&settingsDialog, &SettingsDialog::changesApplied, this, &LevelEditorWindow::loadSettings);
    settingsDialog.exec();
}

void LevelEditorWindow::loadSettings() {
    levelView->setBackgroundColor(settings->getLEWindowColor());
    undoStack->setUndoLimit(static_cast<int>(settings->getLEUndoLimit()));
    levelView->editManagerPtr()->toggleSelectAfterPlacement(settings->getLESelectOnPlace());
    ui->statusbar->setHidden(!settings->getLEShowStatusbar());
}

void LevelEditorWindow::historyStateChanged(int index)
{
    Q_UNUSED(index);

    setStatus(Unsaved);

    if (levelView == nullptr) {
        return;
    }

    levelView->update();
    updateEditors();

    if (undoStack->index() == undoStack->count()) {
        return;
    }

    // Update button states

    ui->actionToggleLayer1->blockSignals(true);
    ui->actionToggleLayer1->setChecked(levelView->editManagerPtr()->getLayerMask() & LAYER_MASK::LAYER_ONE);
    ui->actionToggleLayer1->blockSignals(false);

    ui->actionToggleLayer2->blockSignals(true);
    ui->actionToggleLayer2->setChecked(levelView->editManagerPtr()->getLayerMask() & LAYER_MASK::LAYER_TWO);
    ui->actionToggleLayer2->blockSignals(false);

    ui->actionToggleSprites->blockSignals(true);
    ui->actionToggleSprites->setChecked(levelView->editManagerPtr()->spriteInteractionEnabled());
    ui->actionToggleSprites->blockSignals(false);

    ui->actionToggleEntrances->blockSignals(true);
    ui->actionToggleEntrances->setChecked(levelView->editManagerPtr()->entranceInteractionEnabled());
    ui->actionToggleEntrances->blockSignals(false);

    ui->actionTogglePaths->blockSignals(true);
    ui->actionTogglePaths->setChecked(levelView->editManagerPtr()->pathInteractionEnabled());
    ui->actionTogglePaths->blockSignals(false);

    ui->actionToggleLocations->blockSignals(true);
    ui->actionToggleLocations->setChecked(levelView->editManagerPtr()->locationInteractionEnabled());
    ui->actionToggleLocations->blockSignals(false);

    levelView->editManagerPtr()->clearSelection();
    deselect();
    levelView->update();
}

// Actions

void LevelEditorWindow::toggleLayer(bool toggle)
{
    LAYER_MASK mask = LAYER_MASK::NONE;

    QAction *action = qobject_cast<QAction*>(sender());
    if (action == ui->actionToggleLayer1)
    {
        mask = LAYER_MASK::LAYER_ONE;
    }
    else if (action == ui->actionToggleLayer2)
    {
        mask = LAYER_MASK::LAYER_TWO;
    }
    else
    {
        qDebug() << "toggleLayer: Unknown Sender";
        return;
    }

    levelView->editManagerPtr()->setLayerMask(mask, toggle);
}

void LevelEditorWindow::toggleSprites(bool toggle)
{
    levelView->editManagerPtr()->enableSpriteInteraction(toggle);
}

void LevelEditorWindow::toggleEntrances(bool toggle)
{
    levelView->editManagerPtr()->enableEntranceInteraction(toggle);
}

void LevelEditorWindow::togglePaths(bool toggle)
{
    levelView->editManagerPtr()->enablePathInteraction(toggle);
}

void LevelEditorWindow::toggleLocations(bool toggle)
{
    levelView->editManagerPtr()->enableLocationInteraction(toggle);
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

void LevelEditorWindow::setStatus(EditorStatus newStatus)
{
    switch (newStatus) {
    case EditorStatus::Ready:
        status = Ready;
        statusLabel->setText(tr("Ready!"));
        break;
    case EditorStatus::Unsaved:
        status = Unsaved;
        statusLabel->setText(tr("Unsaved Changes"));
        break;
    case EditorStatus::SaveFailed:
        status = SaveFailed;
        statusLabel->setText(tr("Save Failed"));
        break;
    case EditorStatus::ChangesSaved:
        status = ChangesSaved;
        statusLabel->setText(tr("Changes Saved"));
    default:
        break;
    }
}

void LevelEditorWindow::on_actionSave_triggered()
{
    qint8 res = levelView->saveLevel();
    if (res != 0) {
        setStatus(SaveFailed);
    }
    else {
        setStatus(ChangesSaved);
    }
}

void LevelEditorWindow::on_actionCopy_triggered()
{
    levelView->copy();
}

void LevelEditorWindow::on_actionPaste_triggered()
{
    spriteEditor->spriteIdsPtr()->deselect();
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

void LevelEditorWindow::on_actionSelectAll_triggered()
{
    levelView->selectAll();
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
    levelView->editManagerPtr()->setDrawType(ObjectType::SPRITE);
    levelView->editManagerPtr()->setSprite(spriteId);
}

void LevelEditorWindow::handleSelectionChanged(Object* obj)
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
    if (!editorsLoaded()) {
        return;
    }

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
    if (!editorsLoaded()) {
        return;
    }

    tilesetPalette->updateEditor();
    areaEditor->updateEditor();
    entranceEditor->updateEditor();
    zoneEditor->updateEditor();
    locationEditor->updateEditor();
    pathEditor->updateEditor();
    progPathEditor->updateEditor();
    spriteEditor->spriteIdsPtr()->updateEditor();
//    spriteEditor->spriteDataEditorPtr()->updateEditor();
}

bool LevelEditorWindow::editorsLoaded()
{
    return (entranceEditor != nullptr &&
            zoneEditor != nullptr &&
            locationEditor != nullptr &&
            pathEditor != nullptr &&
            progPathEditor != nullptr &&
            spriteEditor != nullptr);
}

void LevelEditorWindow::toolboxTabsCurrentChanged(int index)
{
    switch (index) {
    case 0:
        levelView->editManagerPtr()->setDrawType(ObjectType::INVALID);
        break;
    case 1:
        levelView->editManagerPtr()->setDrawType(ObjectType::BGDATOBJECT);
        break;
    case 2:
        levelView->editManagerPtr()->setDrawType(ObjectType::SPRITE);
        break;
    case 3:
        levelView->editManagerPtr()->setDrawType(ObjectType::ENTRANCE);
        break;
    case 4:
        levelView->editManagerPtr()->setDrawType(ObjectType::ZONE);
        break;
    case 5:
        levelView->editManagerPtr()->setDrawType(ObjectType::LOCATION);
        break;
    case 6:
        levelView->editManagerPtr()->setDrawType(ObjectType::PATHNODE);
        break;
    case 7:
        levelView->editManagerPtr()->setDrawType(ObjectType::PROGRESSPATHNODE);
        break;
    default:
        break;
    }
}

const int LevelEditorWindow::showSaveDialog()
{
    QMessageBox message(this);
    message.setWindowTitle(tr("Unsaved Changes"));
    message.setText(tr("Do you want to save your changes?"));
    message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    QSpacerItem* spacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = qobject_cast<QGridLayout*>(message.layout());
    layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

    int exitCode = message.exec();

    switch (exitCode)
    {
    case QMessageBox::Save:
        levelView->saveLevel();
        setStatus(ChangesSaved);
        break;
    case QMessageBox::Discard:
        setStatus(Ready);
        break;
    }

    return exitCode;
}

void LevelEditorWindow::on_actionAddArea_triggered() {

    if (status == EditorStatus::Unsaved || status == EditorStatus::SaveFailed) {
        int exitCode = showSaveDialog();

        if (exitCode == QMessageBox::Cancel) {
            return;
        }
    }

    if (lvlMgr->getAreaCount() >= 4) {
        QMessageBox::information(this, "CoinKiller", tr("Due to limitations there can only be a maximum of 4 areas in a level."), QMessageBox::Ok);
        return;
    }

    int seekArea = lvlMgr->addArea(lvlMgr->getAreaCount());
    loadArea(seekArea);
    updateAreaSelector(seekArea);
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
    warning.setText(tr("Are you sure you want to delete this area?\n\n"
                       "The level will automatically save afterwards and therefore the deletion of this area cannot be undone afterwards."));
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

        undoStack->blockSignals(true);
    }

    level = lvlMgr->openArea(id);

    levelView = new LevelView(this, level, undoStack);
    ui->levelViewArea->setWidget(levelView);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);

    levelView->editManagerPtr()->setLayerMask(LAYER_MASK::ALL, true);
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);
    ui->actionToggleSprites->setChecked(true);
    ui->actionTogglePaths->setChecked(true);
    ui->actionToggleLocations->setChecked(true);
    ui->actionToggle2DTile->setChecked(true);
    ui->actionToggle3DOverlay->setChecked(true);
    ui->actionToggleEntrances->setChecked(true);

    levelView->toggleGrid(ui->actionGrid->isChecked());
    levelView->toggleCheckerboard(ui->actionCheckerboard->isChecked());
    levelView->toggleRenderLiquids(ui->actionRenderLiquids->isChecked());
    levelView->toggleRenderCameraLimits(ui->actionRenderCameraLimits->isChecked());

#ifdef USE_KDE_BLUR
    setBlurStylesheet();
#endif

    levelView->editManagerPtr()->enableSpriteInteraction(ui->actionToggleSprites->isChecked());
    levelView->editManagerPtr()->enableEntranceInteraction(ui->actionToggleEntrances->isChecked());
    levelView->editManagerPtr()->enablePathInteraction(ui->actionTogglePaths->isChecked());
    levelView->editManagerPtr()->enableLocationInteraction(ui->actionToggleLocations->isChecked());

    zoom = 1.0;

    QString basePath(settings->dataPath("icons/"));

    // Setup Area Editor
    areaEditor = new AreaEditorWidget(level, lvlMgr->getGame(), undoStack);

    // Setup Tileset Picker
    tilesetPalette = new TilesetPalette(level, levelView->editManagerPtr(), lvlMgr->getGame(), undoStack);

    // Setup Sprite Picker
    spriteEditor = new SpriteEditorWidget(&level->sprites, undoStack);
    connect(spriteEditor, &SpriteEditorWidget::currentSpriteChanged, this, &LevelEditorWindow::setSelSprite);
    connect(spriteEditor, &SpriteEditorWidget::selectedSpriteChanged, levelView, &LevelView::selectObj);

    // Setup Entrance Editor
    entranceEditor = new EntranceEditorWidget(&level->entrances, undoStack);
    connect(entranceEditor, &EntranceEditorWidget::selectedEntrChanged, levelView, &LevelView::selectObj);

    // Setup Zone Editor
    zoneEditor = new ZoneEditorWidget(&level->zones, &level->backgrounds, &level->boundings, undoStack, level);
    connect(zoneEditor, &ZoneEditorWidget::selectedZoneChanged, levelView, &LevelView::selectObj);
    connect(zoneEditor, &ZoneEditorWidget::selectZoneContents, levelView, &LevelView::selectZoneContents);
    connect(zoneEditor, &ZoneEditorWidget::screenshot, levelView, &LevelView::screenshot);

    // Setup Location Editor
    locationEditor = new LocationEditorWidget(&level->locations, undoStack);
    connect(locationEditor, &LocationEditorWidget::selectedLocChanged, levelView, &LevelView::selectObj);

    // Setup Path Editor
    pathEditor = new PathEditorWidget(&level->paths, undoStack);
    connect(pathEditor, &PathEditorWidget::selectedPathChanged, levelView, &LevelView::selectObj);

    // Setup Progress Path Editor
    progPathEditor = new ProgressPathEditorWidget(&level->progressPaths, undoStack);
    connect(progPathEditor, &ProgressPathEditorWidget::selectedProgPathChanged, levelView, &LevelView::selectObj);

    // Setup Level View
    connect(levelView, &LevelView::scrollTo, this, &LevelEditorWindow::scrollTo);
    connect(levelView->editManagerPtr(), &EditManager::selectdObjectChanged, this, &LevelEditorWindow::handleSelectionChanged);
    connect(levelView->editManagerPtr(), &EditManager::deselected, this, &LevelEditorWindow::deselect);
    connect(levelView->editManagerPtr(), &EditManager::updateEditors, this, &LevelEditorWindow::updateEditors);

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

    miniMap = new LevelMiniMap(this, level);
    connect(levelView, &LevelView::updateMinimap, miniMap, &LevelMiniMap::update_);
    connect(levelView, &LevelView::updateMinimapBounds, miniMap, &LevelMiniMap::updateBounds);
    connect(miniMap, &LevelMiniMap::scrollTo, this, &LevelEditorWindow::scrollTo);
    minimapDock->setWidget(miniMap);

    update();

    toolboxTabs->blockSignals(false);

    undoStack->clear();
    undoStack->blockSignals(false);

    loadSettings();
    setStatus(Ready);
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

    QStringListModel *model = new QStringListModel(this);
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
    if (status == EditorStatus::Unsaved || status == EditorStatus::SaveFailed) {
       int exitCode = showSaveDialog();

       if (exitCode == QMessageBox::Cancel) {
            areaSelector->blockSignals(true);
            areaSelector->setCurrentIndex(level->getAreaID()-1);
            areaSelector->blockSignals(false);
            return;
       }
    }

   index++;

   if (lvlMgr->areaIsOpen(index)) {
        updateAreaSelector(level->getAreaID());
        QMessageBox::information(this, "CoinKiller", tr("Area %1 cannot be opened because it is already opened in another editor window.").arg(index), QMessageBox::Ok);
        return;
   }

   if (QApplication::queryKeyboardModifiers() & Qt::ControlModifier) {
        updateAreaSelector(level->getAreaID());
        lvlMgr->openAreaEditor(index);
   }
   else {
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
    qMax(0.0f, x*zoom);
    qMax(0.0f, y*zoom);
    ui->levelViewArea->horizontalScrollBar()->setValue(x*zoom);
    ui->levelViewArea->verticalScrollBar()->setValue(y*zoom);
}

void LevelEditorWindow::closeEvent(QCloseEvent *event)
{
    if (status == EditorStatus::Unsaved || status == EditorStatus::SaveFailed) {
        int exitCode = showSaveDialog();

        if (exitCode == QMessageBox::Cancel) {
            event->ignore();
        } else {
            event->accept();
        }
    }

    if (event->isAccepted()) {
        undoStack->clear();
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

void LevelEditorWindow::on_actionShowHistory_toggled(bool checked)
{
    historyDock->setHidden(!checked);
}

void LevelEditorWindow::updateDockedWidgetCheckboxes()
{
    ui->actionShowToolbox->setChecked(!toolboxDock->isHidden());
    ui->actionShowMinimap->setChecked(!minimapDock->isHidden());
    ui->actionShowHistory->setChecked(!historyDock->isHidden());
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
