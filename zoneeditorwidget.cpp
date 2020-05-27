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

ZoneEditorWidget::ZoneEditorWidget(QList<Zone*> *zones)
{
    this->zones = zones;

    multiplayerTrackings.insert(0, "Horizontal");
    multiplayerTrackings.insert(6, "Vertical");

    loadMusicIDs();

    zoneList = new QListWidget();
    zoneList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(zoneList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleZoneListIndexChange(QListWidgetItem*)));

    selectContentsBtn = new QPushButton(SettingsManager::getInstance()->getTranslation("LevelEditor", "selectZoneContents"), this);
    connect(selectContentsBtn, SIGNAL(clicked(bool)), this, SLOT(handleSelectContentsClicked()));

    screenshotBtn = new QPushButton(SettingsManager::getInstance()->getTranslation("LevelEditor", "screenshotZone"), this);
    connect(screenshotBtn, SIGNAL(clicked(bool)), this, SLOT(handleScreenshotClicked()));

    settingsTabs = new QTabWidget();
    QSizePolicy policy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    policy.setVerticalStretch(0);
    settingsTabs->setSizePolicy(policy);

    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChange(int)));

    multiplayerTracking = new QComboBox();
    multiplayerTracking->addItems(multiplayerTrackings.values());
    connect(multiplayerTracking, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleMultiPlayerTrackingChange(QString)));

    progPathId = new QSpinBox();
    progPathId->setRange(0, 255);
    connect(progPathId, SIGNAL(valueChanged(int)), this, SLOT(handleProgPathIDChange(int)));

    musicId = new QComboBox();
    for (QPair<int, QString> i : musicIds.values()) musicId->addItem(i.second);
    musicId->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    connect(musicId, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleMusicIDChange(QString)));

    unk1 = new QSpinBox();
    unk1->setRange(0, 255);
    connect(unk1, SIGNAL(valueChanged(int)), this, SLOT(handleUnk1Change(int)));

    unlimitedScrolling = new QCheckBox();
    unlimitedScrolling->setText("Unlimited");
    connect(unlimitedScrolling, SIGNAL(toggled(bool)), this, SLOT(handleUnlimitedScrollingChange(bool)));

    vertScrollingDistance = new QSpinBox();
    vertScrollingDistance->setRange(0, 65535);
    connect(vertScrollingDistance, SIGNAL(valueChanged(int)), this, SLOT(handleVertScrollingDistanceChange(int)));

    primaryUpperBound = new QSpinBox();
    primaryUpperBound->setRange(-2147483648, 2147483647);
    primaryUpperBound->setToolTip("This is the main upper bounding used by the camera it is used when the player is walking/jumping.\n"
                           "This is used to determine how close the player has to be to the top of the screen before the camera scrolls up.\n"
                           "Negative values require the player to be closer to the top of the screen, Positive values require the player to be further away.\n"
                           "A value of 0 requires the player to be >5 tiles away from the top before the camera will move.");
    connect(primaryUpperBound, SIGNAL(valueChanged(int)), this, SLOT(handlePrimaryUpperBoundChange(int)));

    primaryLowerBound = new QSpinBox();
    primaryLowerBound->setRange(-2147483648, 2147483647);
    primaryLowerBound->setToolTip("This is the main upper bounding used by the camera it is used when the player is walking/jumping.\n"
                                    "This is used to determine how close the player has to be to the bottom of the screen before the camera scrolls down.\n"
                                    "Negative values require the player to be closer to the bottom of the screen, Positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the bottom before the camera will move.");
    connect(primaryLowerBound, SIGNAL(valueChanged(int)), this, SLOT(handlePrimaryLowerBoundChange(int)));

    secondaryUpperBound = new QSpinBox();
    secondaryUpperBound->setRange(-2147483648, 2147483647);
    secondaryUpperBound->setToolTip("This is the secondary upper bounding used by the camera in certain situations, such as when climbing a vine or fence.\n"
                                    "This is used to determine how close the player has to be to the top of the screen before the camera scrolls up.\n"
                                    "Negative values require the player to be closer to the top of the screen, Positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the top before the camera will move.");
    connect(secondaryUpperBound, SIGNAL(valueChanged(int)), this, SLOT(handleSecondaryUpperBoundChange(int)));

    secondaryLowerBound = new QSpinBox();
    secondaryLowerBound->setRange(-2147483648, 2147483647);
    secondaryLowerBound->setToolTip("This is the secondary lower bounding used by the camera in certain situations, such as when climbing a vine or fence.\n"
                                    "This is used to determine how close the player has to be to the bottom of the screen before the camera scrolls down.\n"
                                    "Negative values require the player to be closer to the bottom of the screen, Positive values require the player to be further away.\n"
                                    "A value of 0 requires the player to be >5 tiles away from the bottom before the camera will move.");
    connect(secondaryLowerBound, SIGNAL(valueChanged(int)), this, SLOT(handleSecondaryLowerBoundChange(int)));

    bgXPos = new QSpinBox();
    bgXPos->setRange(-32768, 32767);
    connect(bgXPos, SIGNAL(valueChanged(int)), this, SLOT(handleBgXPosChanged(int)));

    bgYPos = new QSpinBox();
    bgYPos->setRange(-32768, 32767);
    connect(bgYPos, SIGNAL(valueChanged(int)), this, SLOT(handleBgYPosChanged(int)));

    bgParallaxMode = new QComboBox();
    bgParallaxMode->addItem("Y Offset Off, All Parallax On");
    bgParallaxMode->addItem("Y Offset On, All Parallax On");
    bgParallaxMode->addItem("Y Offset On, All Parallax Off");
    bgParallaxMode->addItem("Y Offset On, Y Parallax Off");
    bgParallaxMode->addItem("Y Offset On, X Parallax Off");
    connect(bgParallaxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(handleBgParallaxModeChange(int)));

    background = new QComboBox();
    background->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    loadBackgrounds();
    connect(background, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleBackgroundChange(QString)));

    backgroundPreview = new BgPreview();

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(zoneList);

    QHBoxLayout* btnsLayout = new QHBoxLayout();
    btnsLayout->addWidget(selectContentsBtn);
    btnsLayout->addWidget(screenshotBtn);
    layout->addLayout(btnsLayout);

    generalTab = new QWidget();
    QGridLayout* generalTabLayout = new QGridLayout();
    generalTabLayout->setMargin(5);
    id->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    generalTabLayout->addWidget(new QLabel("ID:"), 0, 0, 1, 1, Qt::AlignRight);
    generalTabLayout->addWidget(id, 0, 1);

    generalTabLayout->addWidget(new QLabel("Music:"), 1, 0, 1, 1, Qt::AlignRight);
    generalTabLayout->addWidget(musicId, 1, 1);

    generalTabLayout->addWidget(new QLabel("Multiplayer Tracking:"), 2, 0, 1, 1, Qt::AlignRight);
    generalTabLayout->addWidget(multiplayerTracking, 2, 1);

    generalTabLayout->addWidget(new QLabel("Progress Path ID:"), 3, 0, 1, 1, Qt::AlignRight);
    generalTabLayout->addWidget(progPathId, 3, 1);

    generalTabLayout->addWidget(new QLabel("Unknown 1:"), 4, 0, 1, 1, Qt::AlignRight);
    generalTabLayout->addWidget(unk1, 4, 1);

    generalTabLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 5, 0);

    generalTab->setLayout(generalTabLayout);

    boundsTab  = new QWidget();
    QGridLayout* boundsTabLayout = new QGridLayout();
    boundsTabLayout->setMargin(5);
    primaryUpperBound->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    boundsTabLayout->addWidget(new QLabel("Primary Upper Bound:"), 0, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(primaryUpperBound, 0, 1, 1, 2);

    boundsTabLayout->addWidget(new QLabel("Primary Lower Bound:"), 1, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(primaryLowerBound, 1, 1, 1, 2);

    boundsTabLayout->addWidget(new QLabel("Secondary Upper Bound:"), 2, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(secondaryUpperBound, 2, 1, 1, 2);

    boundsTabLayout->addWidget(new QLabel("Secondary Lower Bound:"), 3, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(secondaryLowerBound, 3, 1, 1, 2);

    boundsTabLayout->addWidget(new QLabel("Vertical Scrolling Distance:"), 4, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(vertScrollingDistance, 4, 1, 1, 1);

    boundsTabLayout->addWidget(unlimitedScrolling, 4, 2, 1, 1, Qt::AlignLeft);

    boundsTabLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 5, 0);

    boundsTab->setLayout(boundsTabLayout);

    backgroundTab = new QWidget();
    QGridLayout* backgroundTabLayout = new QGridLayout();
    backgroundTabLayout->setMargin(5);
    bgXPos->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    backgroundTabLayout->addWidget(new QLabel("X Offset:"), 0, 0, 1, 1, Qt::AlignRight);
    backgroundTabLayout->addWidget(bgXPos, 0, 1);

    backgroundTabLayout->addWidget(new QLabel("Y Offset:"), 1, 0, 1, 1, Qt::AlignRight);
    backgroundTabLayout->addWidget(bgYPos, 1, 1);

    backgroundTabLayout->addWidget(new QLabel("Parallax Mode:"), 2, 0, 1, 1, Qt::AlignRight);
    backgroundTabLayout->addWidget(bgParallaxMode, 2, 1);

    backgroundTabLayout->addWidget(new QLabel("Background:"), 3, 0, 1, 1, Qt::AlignRight);
    backgroundTabLayout->addWidget(background, 3, 1);

    QHBoxLayout* bgAlign = new QHBoxLayout();
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    bgAlign->addWidget(spacer);
    bgAlign->addWidget(backgroundPreview, 1);
    backgroundTabLayout->addLayout(bgAlign, 5, 0, 1, 2);

    backgroundTab->setLayout(backgroundTabLayout);

    settingsTabs->addTab(generalTab, "General");
    settingsTabs->addTab(boundsTab, "Bounds");
    settingsTabs->addTab(backgroundTab, "Background");

    layout->addWidget(settingsTabs);

    updateList();
    updateInfo();
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

void ZoneEditorWidget::updateBgPreview()
{
    QFileInfo checkFile(SettingsManager::getInstance()->dataPath("backgrounds/" + editZone->getBgName() + ".png"));
    if (checkFile.exists() && checkFile.isFile()) backgroundPreview->setBg(SettingsManager::getInstance()->dataPath("backgrounds/" + editZone->getBgName() + ".png"));
    else backgroundPreview->setBg(SettingsManager::getInstance()->dataPath("backgrounds/Invalid.png"));
}

void ZoneEditorWidget::loadMusicIDs()
{
    QFile file(SettingsManager::getInstance()->getFilePath("musicids.txt"));
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    in.setCodec("UTF-8");
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

void ZoneEditorWidget::loadBackgrounds()
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

void ZoneEditorWidget::updateList()
{
    QModelIndex index;
    if (zoneList->selectionModel()->selectedIndexes().size() != 0) index = zoneList->selectionModel()->selectedIndexes().at(0);

    zoneList->clear();
    foreach (Zone* zone, *zones)
        zoneList->addItem(QString("Zone %1: (at %2,%3) (W: %4 H: %5)").arg(zone->getid()).arg(to16(zone->getx())).arg(to16(zone->gety())).arg(to16(zone->getwidth())).arg(to16(zone->getheight())));

    zoneList->setCurrentIndex(index);
}

void ZoneEditorWidget::updateInfo()
{
    settingsTabs->setHidden(!editingAZone);
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

    primaryUpperBound->setValue(editZone->getPrimaryUpperBound());
    primaryLowerBound->setValue(editZone->getPrimaryLowerBound());
    secondaryUpperBound->setValue(editZone->getSecondaryUpperBound());
    secondaryLowerBound->setValue(editZone->getSecondaryLowerBound());
    unlimitedScrolling->setChecked(editZone->getUpScrolling() == 0xF);
    vertScrollingDistance->setValue(editZone->getUpScrolling());

    if (editZone->getUpScrolling() == 0xF)
        vertScrollingDistance->setEnabled(false);

    bgXPos->setValue(editZone->getBgXPos());
    bgYPos->setValue(editZone->getBgYPos());
    background->setCurrentText(backgrounds.value(editZone->getBgName()));
    bgParallaxMode->setCurrentIndex(editZone->getBgParallaxMode());
    updateBgPreview();

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

void ZoneEditorWidget::handleUnlimitedScrollingChange(bool val)
{
    if (!handleChanges) return;
    editZone->setUpScrolling(val? 0x0F : 0x00);

    if (val)
        vertScrollingDistance->setEnabled(false);
    else
        vertScrollingDistance->setEnabled(true);

    emit editMade();
}

void ZoneEditorWidget::handleVertScrollingDistanceChange(int val)
{
    if (!handleChanges) return;
    editZone->setUpScrolling(val);
    emit editMade();
}

void ZoneEditorWidget::handlePrimaryUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setPrimaryUpperBound(val);
    emit editMade();
}

void ZoneEditorWidget::handlePrimaryLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setPrimaryLowerBound(val);
    emit editMade();
}

void ZoneEditorWidget::handleSecondaryUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setSecondaryUpperBound(val);
    emit editMade();
}

void ZoneEditorWidget::handleSecondaryLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setSecondaryLowerBound(val);
    emit editMade();
}

void ZoneEditorWidget::handleBgParallaxModeChange(int val)
{
    if (!handleChanges) return;
    editZone->setBgParallaxMode(val);
    emit editMade();
}

void ZoneEditorWidget::handleBgXPosChanged(int val)
{
    if (!handleChanges) return;
    editZone->setBgXPos(val);
    emit editMade();
}

void ZoneEditorWidget::handleBgYPosChanged(int val)
{
    if (!handleChanges) return;
    editZone->setBgYPos(val);
    emit editMade();
}

void ZoneEditorWidget::handleBackgroundChange(QString text)
{
    if (!handleChanges) return;
    editZone->setBackgroundName(backgrounds.key(text, "Nohara_Castle"));
    updateBgPreview();
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
