#include "zoneeditorwidget.h"
#include "unitsconvert.h"
#include "settingsmanager.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTabWidget>
#include <QSpacerItem>
#include <QMessageBox>

#include "commands/zonecommands.h"
#include "commands/zonebgcommands.h"
#include "commands/zoneboundingcommands.h"
#include "commands/levelcommands.h"

ZoneEditorWidget::ZoneEditorWidget(QList<Zone*> *zones, QList<ZoneBackground*> *bgs, QList<ZoneBounding*> *bounds, QUndoStack *undoStack, Level *level, QWidget *parent) :
    QWidget(parent),
    zones(zones),
    zoneBoundings(bounds),
    zoneBgs(bgs),
    undoStack(undoStack),
    level(level) {

    multiplayerTrackings.insert(0, tr("Right and Down"));
    multiplayerTrackings.insert(1, tr("Right and Up"));
    multiplayerTrackings.insert(2, tr("Left and Down"));
    multiplayerTrackings.insert(3, tr("Left and Up"));
    multiplayerTrackings.insert(4, tr("Down and Right"));
    multiplayerTrackings.insert(5, tr("Down and Left"));
    multiplayerTrackings.insert(6, tr("Up and Right"));
    multiplayerTrackings.insert(7, tr("Up and Left"));
    multiplayerTrackings.insert(8, tr("Right and Right"));
    loadMusicIDs();

    zoneList = new QListWidget();
    zoneList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(zoneList, &QListWidget::itemClicked, this, &ZoneEditorWidget::handleZoneListIndexChange);

    selectContentsBtn = new QPushButton(tr("Select Contents"), this);
    connect(selectContentsBtn, &QAbstractButton::clicked, this, &ZoneEditorWidget::handleSelectContentsClicked);

    screenshotBtn = new QPushButton(tr("Screenshot Zone"), this);
    connect(screenshotBtn, &QAbstractButton::clicked, this, &ZoneEditorWidget::handleScreenshotClicked);

    settingsGroup = new QGroupBox();
    QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    policy.setVerticalStretch(0);
    settingsGroup->setSizePolicy(policy);

    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, &QSpinBox::valueChanged, this, &ZoneEditorWidget::handleIDChange);

    multiplayerTracking = new QComboBox();
    multiplayerTracking->addItems(multiplayerTrackings.values());
    connect(multiplayerTracking, &QComboBox::currentTextChanged, this, &ZoneEditorWidget::handleMultiPlayerTrackingChange);

    progPathId = new QSpinBox();
    progPathId->setRange(0, 255);
    connect(progPathId, &QSpinBox::valueChanged, this, &ZoneEditorWidget::handleProgPathIDChange);

    musicId = new QComboBox();
    for (auto iter = musicIds.begin(), end = musicIds.end(); iter != end; iter++) {
        musicId->addItem(iter->second);
    }

    musicId->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    connect(musicId, &QComboBox::currentTextChanged, this, &ZoneEditorWidget::handleMusicIDChange);

    unk1 = new QSpinBox();
    unk1->setRange(0, 255);
    connect(unk1, &QSpinBox::valueChanged, this, &ZoneEditorWidget::handleUnk1Change);

    boundingId = new QSpinBox();
    boundingId->setRange(0, 255);
    boundingId->setReadOnly(true);
    boundingId->setEnabled(false);

    backgroundId = new QSpinBox();
    backgroundId->setRange(0, 255);
    backgroundId->setReadOnly(true);
    backgroundId->setEnabled(false);

    cameraFlagGroup = new QGroupBox();
    cameraFlagGroup->setSizePolicy(policy);

    camFlag1 = new QCheckBox(tr("Don't zoom out on enter"));
    camFlag1->setToolTip(tr("If checked, the camera will start at the normal size when the zone is entered. Does not apply to entrances that do not zoom out."));
    camFlag1->setProperty("flag", 1);
    connect(camFlag1, &QCheckBox::toggled, this, &ZoneEditorWidget::handleCameraFlagsChange);

    camFlag2 = new QCheckBox(tr("Move X position to zone center on enter"));
    camFlag2->setToolTip(tr("If checked, the camera will start at the horizontal center of the zone when it is entered. Does not apply to non-starting entrances."));
    camFlag2->setProperty("flag", 2);
    connect(camFlag2, &QCheckBox::toggled, this, &ZoneEditorWidget::handleCameraFlagsChange);

    camFlag3 = new QCheckBox(tr("Snap camera Y position to ground state"));
    camFlag3->setToolTip(tr("If checked, the camera will rapidly move up or down to where the player has last landed."));
    camFlag3->setProperty("flag", 4);
    connect(camFlag3, &QCheckBox::toggled, this, &ZoneEditorWidget::handleCameraFlagsChange);

    camFlag4 = new QCheckBox(tr("Freeze X position at the end of the zone"));
    camFlag4->setToolTip(tr("If checked, the camera will freeze in place horizontally once it reaches the end of the zone."));
    camFlag4->setProperty("flag", 8);
    connect(camFlag4, &QCheckBox::toggled, this, &ZoneEditorWidget::handleCameraFlagsChange);

    editBounding = new QPushButton(tr("Open Bounding Editor"), this);
    connect(editBounding, &QPushButton::clicked, this, &ZoneEditorWidget::handleEditBoundingClicked);

    editBackground = new QPushButton(tr("Open Background Editor"), this);
    connect(editBackground, &QAbstractButton::clicked, this, &ZoneEditorWidget::handleEditBackgroundClicked);

    boundingWidget = new ZoneBoundingWidget(bounds, undoStack, level);
    connect(boundingWidget, &ZoneBoundingWidget::selectedBoundingChanged, this, &ZoneEditorWidget::handleBoundingIDChange);

    backgroundWidget = new ZoneBackgroundWidget(bgs, undoStack, level);
    connect(backgroundWidget, &ZoneBackgroundWidget::selectedBackgroundChanged, this, &ZoneEditorWidget::handleBackgroundIDChange);

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(zoneList);

    QHBoxLayout* btnsLayout = new QHBoxLayout();
    btnsLayout->addWidget(selectContentsBtn);
    btnsLayout->addWidget(screenshotBtn);
    layout->addLayout(btnsLayout);

    settingsGroup = new QGroupBox();
    QGridLayout* settingsLayout = new QGridLayout();
    settingsLayout->setContentsMargins(5, 5, 5, 5);
    id->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    settingsLayout->addWidget(new QLabel(tr("ID:")), 0, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(id, 0, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Music:")), 1, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(musicId, 1, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Multiplayer Tracking:")), 2, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(multiplayerTracking, 2, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Progress Path ID:")), 3, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(progPathId, 3, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Unknown 1:")), 4, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(unk1, 4, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Bounding ID:")), 5, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(boundingId, 5, 1);
    settingsLayout->addWidget(editBounding, 5, 2);

    settingsLayout->addWidget(new QLabel(tr("Background ID:")), 6, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(backgroundId, 6, 1);
    settingsLayout->addWidget(editBackground, 6, 2);

    settingsLayout->addWidget(new QLabel(tr("Camera Flags:")), 7, 0, 1, 1, Qt::AlignCenter);

    QGridLayout* cameraFlagsLayout = new QGridLayout();
    cameraFlagsLayout->setContentsMargins(5, 5, 5, 5);

    cameraFlagsLayout->addWidget(camFlag1, 0, 0);
    cameraFlagsLayout->addWidget(camFlag2, 1, 0);
    cameraFlagsLayout->addWidget(camFlag3, 2, 0);
    cameraFlagsLayout->addWidget(camFlag4, 3, 0);

    cameraFlagGroup->setLayout(cameraFlagsLayout);
    settingsLayout->addWidget(cameraFlagGroup, 7, 1, 1, 2);

    settingsGroup->setLayout(settingsLayout);

    layout->addWidget(settingsGroup);

    updateList();
    updateInfo();
}

ZoneEditorWidget::~ZoneEditorWidget()
{
    if (backgroundWidget)
        delete backgroundWidget;

    if (boundingWidget)
        delete boundingWidget;
}

void ZoneEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        loadMusicIDs();
        for (auto iter = musicIds.begin(), end = musicIds.end(); iter != end; iter++) {
            musicId->addItem(iter->second);
        }
        updateEditor();
    }

    QWidget::changeEvent(event);
}

void ZoneEditorWidget::deselect()
{
    editingAZone = false;
    updateInfo();
    zoneList->clearSelection();
}

void ZoneEditorWidget::select(Zone *zone)
{
    editZone = zone;
    editingAZone = true;
    updateInfo();
    zoneList->setCurrentRow(static_cast<qint32>(zones->indexOf(zone)));
}

void ZoneEditorWidget::updateEditor()
{
    updateList();
    updateInfo();
}

void ZoneEditorWidget::loadMusicIDs()
{
    QFile file(SettingsManager::getInstance()->getFilePath("musicids.txt"));
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    int i = 0;

    musicIds.clear();
    while(!in.atEnd())
    {
        QStringList parts = in.readLine().split(":");
        if (parts.count() == 2)
            musicIds.insert(i, qMakePair(parts[0].toInt(), parts[1]));

        i++;
    }

    file.close();
}

BgPreview::BgPreview(QWidget *parent) :
    QLabel(parent) {
    bg = QPixmap(400,240);
    bg.fill(Qt::black);
    setMinimumWidth(100);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMaximumWidth(300);
}

void BgPreview::setBg(QPixmap bg)
{
    this->bg = bg;
    setMinimumHeight(width() * 240 / 400);
    setMaximumHeight(minimumHeight());
    setPixmap(bg.scaled(width(), height(), Qt::KeepAspectRatio));
}

void BgPreview::resizeEvent(QResizeEvent *)
{
    setMinimumHeight(width() * 240 / 400);
    setMaximumHeight(minimumHeight());
    setPixmap(bg.scaled(width(), height(), Qt::KeepAspectRatio));
}

void ZoneEditorWidget::updateList()
{
    QModelIndex index;
    if (zoneList->selectionModel()->selectedIndexes().size() != 0) index = zoneList->selectionModel()->selectedIndexes().at(0);

    zoneList->clear();
    foreach (Zone* zone, *zones)
        zoneList->addItem(tr("Zone %1: (X: %2, Y: %3) (W: %4 H: %5)").arg(zone->getid()).arg(to16(zone->getx())).arg(to16(zone->gety())).arg(to16(zone->getwidth())).arg(to16(zone->getheight())));

    zoneList->setCurrentIndex(index);
}

void ZoneEditorWidget::updateInfo()
{
    settingsGroup->setHidden(!editingAZone);
    selectContentsBtn->setHidden(!editingAZone);
    screenshotBtn->setHidden(!editingAZone);

    if (editingAZone == false)
        return;

    handleChanges = false;

    id->setValue(editZone->getid());
    for (auto iter = musicIds.begin(), end = musicIds.end(); iter != end; iter++) {
        if (iter->first == editZone->getMusicId()) {
            musicId->setCurrentText((iter->second));
        }
    }

    multiplayerTracking->setCurrentText(multiplayerTrackings.value(editZone->getMultiplayerTracking()));
    progPathId->setValue(editZone->getProgPathId());
    unk1->setValue(editZone->getUnk1());
    boundingId->setDisabled(true);
    boundingId->setStyleSheet("color: gray");
    boundingId->setValue(editZone->getBoundingId());
    backgroundId->setDisabled(true);
    backgroundId->setStyleSheet("color: gray");
    backgroundId->setValue(editZone->getBackgroundId());
    camFlag1->setChecked(editZone->getCameraFlags() & 1);
    camFlag2->setChecked(editZone->getCameraFlags() & 2);
    camFlag3->setChecked(editZone->getCameraFlags() & 4);
    camFlag4->setChecked(editZone->getCameraFlags() & 8);

    handleChanges = true;
}

void ZoneEditorWidget::handleZoneListIndexChange(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editZone = zones->at(zoneList->row(item));
    editingAZone = true;
    updateInfo();
    emit selectedZoneChanged(editZone);
}

void ZoneEditorWidget::handleIDChange(int id)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneCmd::SetId(editZone, id));
    updateList();
}

void ZoneEditorWidget::handleProgPathIDChange(int progPathId)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneCmd::SetProgPathId(editZone, progPathId));
}

void ZoneEditorWidget::handleMusicIDChange(QString text)
{
    if (!handleChanges) return;

    int musicId = 0;
    for (auto iter = musicIds.begin(), end = musicIds.end(); iter != end; iter++) {
        if (iter->second == text) {
            musicId = iter->first;
            break;
        }
    }

    undoStack->push(new Commands::ZoneCmd::SetMusicId(editZone, musicId));
}

void ZoneEditorWidget::handleMultiPlayerTrackingChange(QString text)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneCmd::SetMultiplayerTracking(editZone, multiplayerTrackings.key(text, 0)));
}

void ZoneEditorWidget::handleUnk1Change(int unk1)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneCmd::SetUnk1(editZone, unk1));
}

void ZoneEditorWidget::handleCameraFlagsChange(bool checked)
{
    if (!handleChanges) return;

    QObject* obj = sender();
    if (!obj) return;

    int flag = obj->property("flag").toInt();
    int cameraFlags = editZone->getCameraFlags();

    if (checked)
        cameraFlags |= flag;
    else
        cameraFlags &= ~flag;

    undoStack->push(new Commands::ZoneCmd::SetCameraFlags(editZone, cameraFlags));
}

void ZoneEditorWidget::handleSelectContentsClicked()
{
    if (!handleChanges) return;
    emit selectZoneContents(editZone);
}

void ZoneEditorWidget::handleScreenshotClicked()
{
    if (!handleChanges) return;
    emit screenshot(QRect(editZone->getx(), editZone->gety(), editZone->getwidth(), editZone->getheight()));
}

void ZoneEditorWidget::handleBoundingIDChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneCmd::SetBoundingId(editZone, val));
}

void ZoneEditorWidget::handleBackgroundIDChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneCmd::SetBackgroundId(editZone, val));
    updateList();
}

void ZoneEditorWidget::handleEditBoundingClicked()
{
    if (!handleChanges) return;
    boundingWidget->updateWidget();
    boundingWidget->setSelectedIndex(editZone->getBoundingId());
    boundingWidget->exec();
}

void ZoneEditorWidget::handleEditBackgroundClicked()
{
    if (!handleChanges) return;
    backgroundWidget->updateWidget();
    backgroundWidget->setSelectedIndex(editZone->getBackgroundId());
    backgroundWidget->exec();
}


// Zone Background Widget
ZoneBackgroundWidget::ZoneBackgroundWidget(QList<ZoneBackground*> *backgrounds, QUndoStack *undoStack, Level *level, QWidget *parent) :
    QDialog(parent),
    zoneBgs(backgrounds),
    undoStack(undoStack),
    level(level) {

    backgroundList = new QListWidget();
    backgroundList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(backgroundList, &QListWidget::itemClicked, this, &ZoneBackgroundWidget::handleBackgroundListIndexChange);
    connect(backgroundList, &QListWidget::itemDoubleClicked, this, &ZoneBackgroundWidget::handleBackgroundListDoubleClick);

    addBackgroundBtn = new QPushButton(tr("Add Background"), this);
    connect(addBackgroundBtn, &QAbstractButton::clicked, this, &ZoneBackgroundWidget::handleAddBackgroundClicked);

    removeBackgroundBtn = new QPushButton(tr("Remove Background"), this);
    connect(removeBackgroundBtn, &QAbstractButton::clicked, this, &ZoneBackgroundWidget::handleRemoveBackgroundClicked);

    backgroundId = new QSpinBox();
    backgroundId->setRange(0, 255);
    connect(backgroundId, &QSpinBox::valueChanged, this, &ZoneBackgroundWidget::handleBackgroundIDChanged);

    xPos = new QSpinBox();
    xPos->setRange(-32768, 32767);
    connect(xPos, &QSpinBox::valueChanged, this, &ZoneBackgroundWidget::handleXPosChanged);

    yPos = new QSpinBox();
    yPos->setRange(-32768, 32767);
    connect(yPos, &QSpinBox::valueChanged, this, &ZoneBackgroundWidget::handleYPosChanged);

    parallaxMode = new QComboBox();
    parallaxMode->addItem(tr("Y Offset Off, All Parallax On"));
    parallaxMode->addItem(tr("Y Offset On, All Parallax On"));
    parallaxMode->addItem(tr("Y Offset On, All Parallax Off"));
    parallaxMode->addItem(tr("Y Offset On, Y Parallax Off"));
    parallaxMode->addItem(tr("Y Offset On, X Parallax Off"));
    connect(parallaxMode, &QComboBox::currentIndexChanged, this, &ZoneBackgroundWidget::handleParallaxModeChange);

    background = new QComboBox();
    background->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    loadBackgrounds();
    connect(background, &QComboBox::currentTextChanged, this, &ZoneBackgroundWidget::handleBackgroundChange);

    backgroundPreview = new BgPreview();

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(backgroundList);

    QHBoxLayout* btnsLayout = new QHBoxLayout();
    btnsLayout->addWidget(addBackgroundBtn);
    btnsLayout->addWidget(removeBackgroundBtn);
    layout->addLayout(btnsLayout);

    settingsGroup = new QGroupBox();
    settingsGroup->setTitle(tr("Background: None"));
    settingsGroup->setDisabled(true);

    QGridLayout* settingsLayout = new QGridLayout();
    settingsLayout->setContentsMargins(5, 5, 5, 5);
    backgroundId->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    settingsLayout->addWidget(new QLabel(tr("ID:")), 0, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(backgroundId, 0, 1);

    settingsLayout->addWidget(new QLabel(tr("X Offset:")), 1, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(xPos, 1, 1);

    settingsLayout->addWidget(new QLabel(tr("Y Offset:")), 2, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(yPos, 2, 1);

    settingsLayout->addWidget(new QLabel(tr("Parallax Mode:")), 3, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(parallaxMode, 3, 1);

    settingsLayout->addWidget(new QLabel(tr("Background:")), 4, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(background, 4, 1);

    QHBoxLayout* bgAlign = new QHBoxLayout();
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    bgAlign->addWidget(spacer);
    bgAlign->addWidget(backgroundPreview, 1);
    settingsLayout->addLayout(bgAlign, 5, 0, 1, 2);

    settingsGroup->setLayout(settingsLayout);

    layout->addWidget(settingsGroup);

    updateList();
    updateInfo();
}

void ZoneBackgroundWidget::updateList()
{
    QModelIndex index;
    if (backgroundList->selectionModel()->selectedIndexes().size() != 0) index = backgroundList->selectionModel()->selectedIndexes().at(0);

    backgroundList->clear();
    foreach (ZoneBackground* bg, *zoneBgs)
        backgroundList->addItem(tr("Background %1").arg(bg->getId()));

    backgroundList->setCurrentIndex(index);
}

void ZoneBackgroundWidget::updateInfo()
{
    if (editingABg == false)
        return;

    handleChanges = false;

    backgroundId->setValue(editBg->getId());
    settingsGroup->setTitle(tr("Background: %1").arg(editBg->getId()));
    xPos->setValue(editBg->getXPos());
    yPos->setValue(editBg->getYPos());
    background->setCurrentText(backgrounds.value(editBg->getName()));
    parallaxMode->setCurrentIndex(editBg->getParallaxMode());
    updateBgPreview();

    handleChanges = true;
}

void ZoneBackgroundWidget::updateWidget()
{
    updateList();
    updateInfo();
}

void ZoneBackgroundWidget::handleBackgroundListIndexChange(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editBg = zoneBgs->at(backgroundList->row(item));
    settingsGroup->setEnabled(true);
    editingABg = true;
    updateInfo();
}

void ZoneBackgroundWidget::loadBackgrounds()
{
    QFile file(SettingsManager::getInstance()->dataPath("backgrounds/backgroundnames.txt"));
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

    while(!in.atEnd())
    {
        QStringList nameStrings = in.readLine().split(":");
        background->addItem(nameStrings[1]);
        backgrounds.insert(nameStrings[0], nameStrings[1]);
    }

    file.close();
}

void ZoneBackgroundWidget::updateBgPreview()
{
    QFileInfo checkFile(SettingsManager::getInstance()->dataPath("backgrounds/" + editBg->getName() + ".png"));
    if (checkFile.exists() && checkFile.isFile()) backgroundPreview->setBg(SettingsManager::getInstance()->dataPath("backgrounds/" + editBg->getName() + ".png"));
    else backgroundPreview->setBg(SettingsManager::getInstance()->dataPath("backgrounds/Invalid.png"));
}

void ZoneBackgroundWidget::handleParallaxModeChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBgCmd::SetParallaxMode(editBg, val));
}

void ZoneBackgroundWidget::handleXPosChanged(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBgCmd::SetXPos(editBg, static_cast<qint16>(val)));
}

void ZoneBackgroundWidget::handleYPosChanged(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBgCmd::SetYPos(editBg, static_cast<qint16>(val)));
}

void ZoneBackgroundWidget::handleBackgroundIDChanged(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBgCmd::SetId(editBg, val));
    updateList();
    updateInfo();
}

void ZoneBackgroundWidget::handleBackgroundChange(QString text)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBgCmd::SetName(editBg, backgrounds.key(text, "Nohara")));
    updateBgPreview();
}

void ZoneBackgroundWidget::handleAddBackgroundClicked()
{
    if (!handleChanges) return;

    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (ZoneBackground* bg, *zoneBgs)
            if (bg->getId() == id) check = true;
        if (!check) break;
        else if (id == 255)
            return;
    }

    ZoneBackground* bg = new ZoneBackground(id, 0, 0, "Nohara", 0);
    undoStack->push(new Commands::LevelCmd::InsertZoneBackground(level, bg));

    updateList();
}

void ZoneBackgroundWidget::handleRemoveBackgroundClicked()
{
    if (editingABg == false)
        return;

    if (!handleChanges) return;

    undoStack->push(new Commands::LevelCmd::DeleteZoneBackground(level, editBg));

    editingABg = false;
    settingsGroup->setDisabled(true);

    updateList();
}

void ZoneBackgroundWidget::handleBackgroundListDoubleClick(QListWidgetItem *item)
{
    if (!handleChanges) return;
    emit selectedBackgroundChanged(zoneBgs->at(backgroundList->row(item))->getId());

    this->close();
}

void ZoneBackgroundWidget::setSelectedIndex(int index)
{
    QList items = backgroundList->findItems(tr("Background %1").arg(index), Qt::MatchExactly);

    if (items.isEmpty())
        return;

    backgroundList->setCurrentIndex(backgroundList->indexFromItem(items.at(0)));
    backgroundList->setFocus();
    editBg = zoneBgs->at(backgroundList->row(items.at(0)));
    editingABg = true;
    updateInfo();
    settingsGroup->setEnabled(true);
}


// Zone Bounding Widget
ZoneBoundingWidget::ZoneBoundingWidget(QList<ZoneBounding*> *boundings, QUndoStack *undoStack, Level *level, QWidget *parent) :
    QDialog(parent),
    zoneBoundings(boundings),
    undoStack(undoStack),
    level(level) {

    boundingList = new QListWidget();
    boundingList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(boundingList, &QListWidget::itemClicked, this, &ZoneBoundingWidget::handleBoundingListIndexChange);
    connect(boundingList, &QListWidget::itemDoubleClicked, this, &ZoneBoundingWidget::handleBoundingListDoubleClick);

    addBoundingBtn = new QPushButton(tr("Add Bounding"), this);
    connect(addBoundingBtn, &QAbstractButton::clicked, this, &ZoneBoundingWidget::handleAddBoundingClicked);

    removeBoundingBtn = new QPushButton(tr("Remove Bounding"), this);
    connect(removeBoundingBtn, &QAbstractButton::clicked, this, &ZoneBoundingWidget::handleRemoveBoundingClicked);

    unlimitedScrolling = new QCheckBox(tr("Unlimited"));
    connect(unlimitedScrolling, &QAbstractButton::toggled, this, &ZoneBoundingWidget::handleUnlimitedScrollingChange);

    boundingId = new QSpinBox();
    boundingId->setRange(0, 255);
    connect(boundingId, &QSpinBox::valueChanged, this, &ZoneBoundingWidget::handleBoundingIdChange);

    vertScrollingDistance = new QSpinBox();
    vertScrollingDistance->setRange(0, 65535);
    connect(vertScrollingDistance, &QSpinBox::valueChanged, this, &ZoneBoundingWidget::handleVertScrollingDistanceChange);

    primaryUpperBound = new QSpinBox();
    primaryUpperBound->setRange(-2147483648, 2147483647);
    primaryUpperBound->setToolTip(tr("This is the main upper bounding used by the camera it is used when the player is walking/jumping.\n"
                           "This is used to determine how close the player has to be to the top of the screen before the camera scrolls up.\n"
                           "Negative values require the player to be closer to the top of the screen, positive values require the player to be further away.\n"
                           "A value of 0 requires the player to be >5 tiles away from the top before the camera will move."));
    connect(primaryUpperBound, &QSpinBox::valueChanged, this, &ZoneBoundingWidget::handlePrimaryUpperBoundChange);

    primaryLowerBound = new QSpinBox();
    primaryLowerBound->setRange(-2147483648, 2147483647);
    primaryLowerBound->setToolTip(tr("This is the main upper bounding used by the camera it is used when the player is walking/jumping.\n"
                                    "This is used to determine how close the player has to be to the bottom of the screen before the camera scrolls down.\n"
                                    "Positive values require the player to be closer to the bottom of the screen, negative values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the bottom before the camera will move."));
    connect(primaryLowerBound, &QSpinBox::valueChanged, this, &ZoneBoundingWidget::handlePrimaryLowerBoundChange);

    secondaryUpperBound = new QSpinBox();
    secondaryUpperBound->setRange(-2147483648, 2147483647);
    secondaryUpperBound->setToolTip(tr("This is the secondary upper bounding used by the camera in certain situations, such as when climbing a vine or fence.\n"
                                    "This is used to determine how close the player has to be to the top of the screen before the camera scrolls up.\n"
                                    "Negative values require the player to be closer to the top of the screen, positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the top before the camera will move."));
    connect(secondaryUpperBound, &QSpinBox::valueChanged, this, &ZoneBoundingWidget::handleSecondaryUpperBoundChange);

    secondaryLowerBound = new QSpinBox();
    secondaryLowerBound->setRange(-2147483648, 2147483647);
    secondaryLowerBound->setToolTip(tr("This is the secondary lower bounding used by the camera in certain situations, such as when climbing a vine or fence.\n"
                                    "This is used to determine how close the player has to be to the bottom of the screen before the camera scrolls down.\n"
                                    "Positive values require the player to be closer to the bottom of the screen, negative values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the bottom before the camera will move."));
    connect(secondaryLowerBound, &QSpinBox::valueChanged, this, &ZoneBoundingWidget::handleSecondaryLowerBoundChange);

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(boundingList);

    QHBoxLayout* btnsLayout = new QHBoxLayout();
    btnsLayout->addWidget(addBoundingBtn);
    btnsLayout->addWidget(removeBoundingBtn);
    layout->addLayout(btnsLayout);

    settingsGroup = new QGroupBox();
    settingsGroup->setTitle(tr("Bounding: None"));
    settingsGroup->setDisabled(true);

    QGridLayout* settingsLayout = new QGridLayout();
    settingsLayout->setContentsMargins(5, 5, 5, 5);
    boundingId->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    settingsLayout->addWidget(new QLabel(tr("ID:")), 0, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(boundingId, 0, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Primary Upper Bound:")), 1, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(primaryUpperBound, 1, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Primary Lower Bound:")), 2, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(primaryLowerBound, 2, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Secondary Upper Bound:")), 3, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(secondaryUpperBound, 3, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Secondary Lower Bound:")), 4, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(secondaryLowerBound, 4, 1, 1, 2);

    settingsLayout->addWidget(new QLabel(tr("Vertical Scrolling Distance:")), 5, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(vertScrollingDistance, 5, 1, 1, 1);
    settingsLayout->addWidget(unlimitedScrolling, 5, 2, 1, 1, Qt::AlignLeft);

    settingsGroup->setLayout(settingsLayout);

    layout->addWidget(settingsGroup);

    updateList();
    updateInfo();
}

void ZoneBoundingWidget::updateList()
{
    QModelIndex index;
    if (boundingList->selectionModel()->selectedIndexes().size() != 0) index = boundingList->selectionModel()->selectedIndexes().at(0);

    boundingList->clear();

    foreach (ZoneBounding* bounding, *zoneBoundings)
        boundingList->addItem(tr("Bounding %1").arg(bounding->getId()));

    boundingList->setCurrentIndex(index);
}

void ZoneBoundingWidget::updateInfo()
{
    if (editingABounding == false)
        return;

    handleChanges = false;

    boundingId->setValue(editBounding->getId());
    settingsGroup->setTitle(tr("Bounding: %1").arg(editBounding->getId()));
    primaryUpperBound->setValue(editBounding->getPrimaryUpperBound());
    primaryLowerBound->setValue(editBounding->getPrimaryLowerBound());
    secondaryUpperBound->setValue(editBounding->getSecondaryUpperBound());
    secondaryLowerBound->setValue(editBounding->getSecondaryLowerBound());
    unlimitedScrolling->setChecked(editBounding->getUpScrolling() == 0xF);
    vertScrollingDistance->setValue(editBounding->getUpScrolling());

    if (editBounding->getUpScrolling() == 0xF)
        vertScrollingDistance->setEnabled(false);
    else
        vertScrollingDistance->setEnabled(true);

    handleChanges = true;
}

void ZoneBoundingWidget::updateWidget()
{
    updateList();
    updateInfo();
}

void ZoneBoundingWidget::handleBoundingListIndexChange(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editBounding = zoneBoundings->at(boundingList->row(item));
    settingsGroup->setEnabled(true);
    editingABounding = true;
    updateInfo();
}

void ZoneBoundingWidget::handleBoundingListDoubleClick(QListWidgetItem *item)
{
    if (!handleChanges) return;
    emit selectedBoundingChanged(zoneBoundings->at(boundingList->row(item))->getId());

    this->close();
}

void ZoneBoundingWidget::handleUnlimitedScrollingChange(bool val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetUpScrolling(editBounding, val? 0x0F : 0x00));

    if (val)
        vertScrollingDistance->setEnabled(false);
    else
        vertScrollingDistance->setEnabled(true);
}

void ZoneBoundingWidget::handleVertScrollingDistanceChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetUpScrolling(editBounding, val));
}

void ZoneBoundingWidget::handlePrimaryUpperBoundChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetPrimaryUpperBound(editBounding, val));

}

void ZoneBoundingWidget::handlePrimaryLowerBoundChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetPrimaryLowerBound(editBounding, val));
}

void ZoneBoundingWidget::handleSecondaryUpperBoundChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetSecondaryUpperBound(editBounding, val));
}

void ZoneBoundingWidget::handleSecondaryLowerBoundChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetSecondaryLowerBound(editBounding, val));
}

void ZoneBoundingWidget::handleBoundingIdChange(int val)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ZoneBoundingCmd::SetId(editBounding, val));
    updateList();
    updateInfo();
}

void ZoneBoundingWidget::handleAddBoundingClicked()
{
    if (!handleChanges) return;

    quint8 id = 0;
    for(;; id++)
    {
        bool check = false;
        foreach (ZoneBounding* bounding, *zoneBoundings)
            if (bounding->getId() == id) check = true;
        if (!check) break;
        else if (id == 255)
            return;
    }

    ZoneBounding* bounding = new ZoneBounding(id, 0, 0, 0, 0, 0);
    undoStack->push(new Commands::LevelCmd::InsertZoneBounding(level, bounding));
    updateList();
}

void ZoneBoundingWidget::handleRemoveBoundingClicked()
{
    if (editingABounding == false)
        return;

    if (!handleChanges) return;

    undoStack->push(new Commands::LevelCmd::DeleteZoneBounding(level, editBounding));

    editingABounding = false;
    settingsGroup->setDisabled(true);

    updateList();
}

void ZoneBoundingWidget::setSelectedIndex(int index)
{
    QList items = boundingList->findItems(tr("Bounding %1").arg(index), Qt::MatchExactly);

    if (items.isEmpty())
        return;
    else
    {
        boundingList->setCurrentIndex(boundingList->indexFromItem(items.at(0)));
        boundingList->setFocus();
        editBounding = zoneBoundings->at(boundingList->row(items.at(0)));
        editingABounding = true;
        updateInfo();
        settingsGroup->setEnabled(true);
    }
}
