#include "zoneeditorwidget.h"
#include "unitsconvert.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTabWidget>
#include <QSpacerItem>
#include <QMessageBox>

ZoneEditorWidget::ZoneEditorWidget(QList<Zone*> *zones, QList<ZoneBackground*> *bgs, QList<ZoneBounding*> *bounds)
{
    this->zones = zones;
    this->zoneBoundings = bounds;
    this->zoneBgs = bgs;

    multiplayerTrackings.insert(0, tr("Horizontal"));
    multiplayerTrackings.insert(6, tr("Vertical"));

    loadMusicIDs();

    zoneList = new QListWidget();
    zoneList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(zoneList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleZoneListIndexChange(QListWidgetItem*)));

    selectContentsBtn = new QPushButton(tr("Select Contents"), this);
    connect(selectContentsBtn, SIGNAL(clicked(bool)), this, SLOT(handleSelectContentsClicked()));

    screenshotBtn = new QPushButton(tr("Screenshot Zone"), this);
    connect(screenshotBtn, SIGNAL(clicked(bool)), this, SLOT(handleScreenshotClicked()));

    settingsGroup = new QGroupBox();
    QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    policy.setVerticalStretch(0);
    settingsGroup->setSizePolicy(policy);

    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChange(int)));

    multiplayerTracking = new QComboBox();
    multiplayerTracking->addItems(multiplayerTrackings.values());
    connect(multiplayerTracking, SIGNAL(currentTextChanged(QString)), this, SLOT(handleMultiPlayerTrackingChange(QString)));

    progPathId = new QSpinBox();
    progPathId->setRange(0, 255);
    connect(progPathId, SIGNAL(valueChanged(int)), this, SLOT(handleProgPathIDChange(int)));

    musicId = new QComboBox();
    for (QPair<int, QString> i : musicIds.values()) musicId->addItem(i.second);
    musicId->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    connect(musicId, SIGNAL(currentTextChanged(QString)), this, SLOT(handleMusicIDChange(QString)));

    unk1 = new QSpinBox();
    unk1->setRange(0, 255);
    connect(unk1, SIGNAL(valueChanged(int)), this, SLOT(handleUnk1Change(int)));

    boundingId = new QSpinBox();
    boundingId->setRange(0, 255);
    connect(boundingId, SIGNAL(valueChanged(int)), this, SLOT(handleBoundingIDChange(int)));

    backgroundId = new QSpinBox();
    backgroundId->setRange(0, 255);
    connect(backgroundId, SIGNAL(valueChanged(int)), this, SLOT(handleBackgroundIDChange(int)));

    editBounding = new QPushButton(tr("Open Bounding Editor"), this);
    connect(editBounding, SIGNAL(clicked(bool)), this, SLOT(handleEditBoundingClicked()));

    editBackground = new QPushButton(tr("Open Background Editor"), this);
    connect(editBackground, SIGNAL(clicked(bool)), this, SLOT(handleEditBackgroundClicked()));

    boundingWidget = new ZoneBoundingWidget(bounds);
    connect(boundingWidget, SIGNAL(editMade()), this, SLOT(handleEditMade()));

    backgroundWidget = new ZoneBackgroundWidget(bgs);
    connect(backgroundWidget, SIGNAL(editMade()), this, SLOT(handleEditMade()));

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
        for (QPair<int, QString> i : musicIds.values()) musicId->addItem(i.second);
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
    zoneList->setCurrentRow(zones->indexOf(zone));
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

BgPreview::BgPreview()
{
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
    for (QPair<int, QString> i : musicIds.values())
        if (i.first == editZone->getMusicId())
            musicId->setCurrentText((i.second));

    multiplayerTracking->setCurrentText(multiplayerTrackings.value(editZone->getMultiplayerTracking()));
    progPathId->setValue(editZone->getProgPathId());
    unk1->setValue(editZone->getUnk1());
    boundingId->setValue(editZone->getBoundingId());
    backgroundId->setValue(editZone->getBackgroundId());

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

void ZoneEditorWidget::handleIDChange(int idVal)
{
    if (!handleChanges) return;
    editZone->setID(idVal);
    updateList();
    emit updateLevelView();
    emit editMade();
}

void ZoneEditorWidget::handleProgPathIDChange(int ppIDVal)
{
    if (!handleChanges) return;
    editZone->setProgPathId(ppIDVal);
    emit editMade();
}

void ZoneEditorWidget::handleMusicIDChange(QString text)
{
    if (!handleChanges) return;
    int m_id = 0;

    for (QPair<int, QString> i : musicIds.values())
        if (i.second == text) m_id = i.first;

    editZone->setMusicID(quint8(m_id));
    emit editMade();
}

void ZoneEditorWidget::handleMultiPlayerTrackingChange(QString text)
{
    if (!handleChanges) return;
    editZone->setMultiplayerTracking(multiplayerTrackings.key(text, 0));
    emit editMade();
}

void ZoneEditorWidget::handleUnk1Change(int unk1)
{
    if (!handleChanges) return;
    editZone->setUnk1(unk1);
    emit editMade();
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

    bool isValid = false;
    foreach(ZoneBounding* bounding, *zoneBoundings)
    {
        if (bounding->getId() == val)
            isValid = true;
    }

    if (isValid)
    {
        editZone->setBoundingId(val);
        emit updateLevelView();
        emit editMade();
    }
    else
    {
        QMessageBox message(QMessageBox::Information, "CoinKiller", tr("No Bounding With That ID Exists."), QMessageBox::Ok);
        message.exec();

        boundingId->setValue(editZone->getBoundingId());
    }
}

void ZoneEditorWidget::handleBackgroundIDChange(int val)
{
    if (!handleChanges) return;

    bool isValid = false;
    foreach(ZoneBackground* bg, *zoneBgs)
    {
        if (bg->getId() == val)
            isValid = true;
    }

    if (isValid)
    {
        editZone->setBackgroundId(val);
        updateList();
        emit updateLevelView();
        emit editMade();
    }
    else
    {
        QMessageBox message(QMessageBox::Information, "CoinKiller", tr("No Background With That ID Exists."), QMessageBox::Ok);
        message.exec();

        backgroundId->setValue(editZone->getBackgroundId());
    }
}

void ZoneEditorWidget::handleEditBoundingClicked()
{
    if (!handleChanges) return;
    boundingWidget->exec();
}

void ZoneEditorWidget::handleEditBackgroundClicked()
{
    if (!handleChanges) return;
    backgroundWidget->exec();
}

void ZoneEditorWidget::handleEditMade()
{
    emit editMade();
}

// Zone Background Widget

ZoneBackgroundWidget::ZoneBackgroundWidget(QList<ZoneBackground*> *backgrounds)
{
    this->zoneBgs = backgrounds;

    backgroundList = new QListWidget();
    backgroundList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(backgroundList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleBackgroundListIndexChange(QListWidgetItem*)));

    addBackgroundBtn = new QPushButton(tr("Add Background"), this);
    connect(addBackgroundBtn, SIGNAL(clicked(bool)), this, SLOT(handleAddBackgroundClicked()));

    removeBackgroundBtn = new QPushButton(tr("Remove Background"), this);
    connect(removeBackgroundBtn, SIGNAL(clicked(bool)), this, SLOT(handleRemoveBackgroundClicked()));

    backgroundId = new QSpinBox();
    backgroundId->setRange(0, 255);
    connect(backgroundId, SIGNAL(valueChanged(int)), this, SLOT(handleBackgroundIDChanged(int)));

    xPos = new QSpinBox();
    xPos->setRange(-32768, 32767);
    connect(xPos, SIGNAL(valueChanged(int)), this, SLOT(handleXPosChanged(int)));

    yPos = new QSpinBox();
    yPos->setRange(-32768, 32767);
    connect(yPos, SIGNAL(valueChanged(int)), this, SLOT(handleYPosChanged(int)));

    parallaxMode = new QComboBox();
    parallaxMode->addItem(tr("Y Offset Off, All Parallax On"));
    parallaxMode->addItem(tr("Y Offset On, All Parallax On"));
    parallaxMode->addItem(tr("Y Offset On, All Parallax Off"));
    parallaxMode->addItem(tr("Y Offset On, Y Parallax Off"));
    parallaxMode->addItem(tr("Y Offset On, X Parallax Off"));
    connect(parallaxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(handleParallaxModeChange(int)));

    background = new QComboBox();
    background->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    loadBackgrounds();
    connect(background, SIGNAL(currentTextChanged(QString)), this, SLOT(handleBackgroundChange(QString)));

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
    editBg->setParallaxMode(val);
    emit editMade();
}

void ZoneBackgroundWidget::handleXPosChanged(int val)
{
    if (!handleChanges) return;
    editBg->setXPos(val);
    emit editMade();
}

void ZoneBackgroundWidget::handleYPosChanged(int val)
{
    if (!handleChanges) return;
    editBg->setYPos(val);
    emit editMade();
}

void ZoneBackgroundWidget::handleBackgroundIDChanged(int val)
{
    if (!handleChanges) return;
    editBg->setId(val);
    updateList();
    updateInfo();
    emit editMade();
}

void ZoneBackgroundWidget::handleBackgroundChange(QString text)
{
    if (!handleChanges) return;
    editBg->setName(backgrounds.key(text, "Nohara_Castle"));
    updateBgPreview();
    emit editMade();
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

    ZoneBackground* bg = new ZoneBackground(id, 0, 0, "Nohara_Castle", 0);

    zoneBgs->append(bg);
    updateList();

    emit editMade();
}

void ZoneBackgroundWidget::handleRemoveBackgroundClicked()
{
    if (editingABg == false)
        return;

    if (!handleChanges) return;

    zoneBgs->removeOne(editBg);

    editingABg = false;
    settingsGroup->setDisabled(true);

    delete editBg;

    updateList();
    emit editMade();
}


// Zone Bounding Widget

ZoneBoundingWidget::ZoneBoundingWidget(QList<ZoneBounding*> *boundings)
{
    this->zoneBoundings = boundings;

    boundingList = new QListWidget();
    boundingList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(boundingList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleBoundingListIndexChange(QListWidgetItem*)));

    addBoundingBtn = new QPushButton(tr("Add Bounding"), this);
    connect(addBoundingBtn, SIGNAL(clicked(bool)), this, SLOT(handleAddBoundingClicked()));

    removeBoundingBtn = new QPushButton(tr("Remove Bounding"), this);
    connect(removeBoundingBtn, SIGNAL(clicked(bool)), this, SLOT(handleRemoveBoundingClicked()));

    unlimitedScrolling = new QCheckBox(tr("Unlimited"));
    connect(unlimitedScrolling, SIGNAL(toggled(bool)), this, SLOT(handleUnlimitedScrollingChange(bool)));

    boundingId = new QSpinBox();
    boundingId->setRange(0, 255);
    connect(boundingId, SIGNAL(valueChanged(int)), this, SLOT(handleBoundingIdChange(int)));

    vertScrollingDistance = new QSpinBox();
    vertScrollingDistance->setRange(0, 65535);
    connect(vertScrollingDistance, SIGNAL(valueChanged(int)), this, SLOT(handleVertScrollingDistanceChange(int)));

    primaryUpperBound = new QSpinBox();
    primaryUpperBound->setRange(-2147483648, 2147483647);
    primaryUpperBound->setToolTip(tr("This is the main upper bounding used by the camera it is used when the player is walking/jumping.\n"
                           "This is used to determine how close the player has to be to the top of the screen before the camera scrolls up.\n"
                           "Negative values require the player to be closer to the top of the screen, Positive values require the player to be further away.\n"
                           "A value of 0 requires the player to be >5 tiles away from the top before the camera will move."));
    connect(primaryUpperBound, SIGNAL(valueChanged(int)), this, SLOT(handlePrimaryUpperBoundChange(int)));

    primaryLowerBound = new QSpinBox();
    primaryLowerBound->setRange(-2147483648, 2147483647);
    primaryLowerBound->setToolTip(tr("This is the main upper bounding used by the camera it is used when the player is walking/jumping.\n"
                                    "This is used to determine how close the player has to be to the bottom of the screen before the camera scrolls down.\n"
                                    "Negative values require the player to be closer to the bottom of the screen, Positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the bottom before the camera will move."));
    connect(primaryLowerBound, SIGNAL(valueChanged(int)), this, SLOT(handlePrimaryLowerBoundChange(int)));

    secondaryUpperBound = new QSpinBox();
    secondaryUpperBound->setRange(-2147483648, 2147483647);
    secondaryUpperBound->setToolTip(tr("This is the secondary upper bounding used by the camera in certain situations, such as when climbing a vine or fence.\n"
                                    "This is used to determine how close the player has to be to the top of the screen before the camera scrolls up.\n"
                                    "Negative values require the player to be closer to the top of the screen, Positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the top before the camera will move."));
    connect(secondaryUpperBound, SIGNAL(valueChanged(int)), this, SLOT(handleSecondaryUpperBoundChange(int)));

    secondaryLowerBound = new QSpinBox();
    secondaryLowerBound->setRange(-2147483648, 2147483647);
    secondaryLowerBound->setToolTip(tr("This is the secondary lower bounding used by the camera in certain situations, such as when climbing a vine or fence.\n"
                                    "This is used to determine how close the player has to be to the bottom of the screen before the camera scrolls down.\n"
                                    "Negative values require the player to be closer to the bottom of the screen, Positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the bottom before the camera will move."));
    connect(secondaryLowerBound, SIGNAL(valueChanged(int)), this, SLOT(handleSecondaryLowerBoundChange(int)));

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

void ZoneBoundingWidget::handleBoundingListIndexChange(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editBounding = zoneBoundings->at(boundingList->row(item));
    settingsGroup->setEnabled(true);
    editingABounding = true;
    updateInfo();
}

void ZoneBoundingWidget::handleUnlimitedScrollingChange(bool val)
{
    if (!handleChanges) return;
    editBounding->setUpScrolling(val? 0x0F : 0x00);

    if (val)
        vertScrollingDistance->setEnabled(false);
    else
        vertScrollingDistance->setEnabled(true);

    emit editMade();
}

void ZoneBoundingWidget::handleVertScrollingDistanceChange(int val)
{
    if (!handleChanges) return;
    editBounding->setUpScrolling(val);
    emit editMade();
}

void ZoneBoundingWidget::handlePrimaryUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editBounding->setPrimaryUpperBound(val);
    emit editMade();
}

void ZoneBoundingWidget::handlePrimaryLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editBounding->setPrimaryLowerBound(val);
    emit editMade();
}

void ZoneBoundingWidget::handleSecondaryUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editBounding->setSecondaryUpperBound(val);
    emit editMade();
}

void ZoneBoundingWidget::handleSecondaryLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editBounding->setSecondaryLowerBound(val);
    emit editMade();
}

void ZoneBoundingWidget::handleBoundingIdChange(int val)
{
    if (!handleChanges) return;
    editBounding->setId(val);
    updateList();
    updateInfo();
    emit editMade();
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

    zoneBoundings->append(bounding);
    updateList();

    emit editMade();
}

void ZoneBoundingWidget::handleRemoveBoundingClicked()
{
    if (editingABounding == false)
        return;

    if (!handleChanges) return;

    zoneBoundings->removeOne(editBounding);

    editingABounding = false;
    settingsGroup->setDisabled(true);

    delete editBounding;

    updateList();
    emit editMade();
}
