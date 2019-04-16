#include "zoneeditorwidget.h"
#include "unitsconvert.h"

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
    musicId->addItems(musicIds.values());
    musicId->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    connect(musicId, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleMusicIDChange(QString)));

    unk1 = new QSpinBox();
    unk1->setRange(0, 65535);
    connect(unk1, SIGNAL(valueChanged(int)), this, SLOT(handleUnk1Change(int)));

    upScrolling = new QCheckBox();
    upScrolling->setText("Scroll Vertically");
    connect(upScrolling, SIGNAL(toggled(bool)), this, SLOT(handleUpScrollingChange(bool)));

    upperBound = new QSpinBox();
    upperBound->setRange(-2147483648, 2147483647);
    connect(upperBound, SIGNAL(valueChanged(int)), this, SLOT(handleUpperBoundChange(int)));

    lowerBound = new QSpinBox();
    lowerBound->setRange(-2147483648, 2147483647);
    connect(lowerBound, SIGNAL(valueChanged(int)), this, SLOT(handleLowerBoundChange(int)));

    unkUpperBound = new QSpinBox();
    unkUpperBound->setRange(-2147483648, 2147483647);
    connect(unkUpperBound, SIGNAL(valueChanged(int)), this, SLOT(handleUnkUpperBoundChange(int)));

    unkLowerBound = new QSpinBox();
    unkLowerBound->setRange(-2147483648, 2147483647);
    connect(unkLowerBound, SIGNAL(valueChanged(int)), this, SLOT(handleUnkLowerBoundChange(int)));

    bgXPos = new QSpinBox();
    bgXPos->setRange(-32768, 32767);
    connect(bgXPos, SIGNAL(valueChanged(int)), this, SLOT(handleBgXPosChanged(int)));

    bgYPos = new QSpinBox();
    bgYPos->setRange(-32768, 32767);
    connect(bgYPos, SIGNAL(valueChanged(int)), this, SLOT(handleBgYPosChanged(int)));

    bgParallaxMode = new QComboBox();
    bgParallaxMode->addItem("Y Offset Off, All Parallx On");
    bgParallaxMode->addItem("Y Offset On, All Parallx On");
    bgParallaxMode->addItem("Y Offset On, All Parallx Off");
    bgParallaxMode->addItem("Y Offset On, Y Parallx Off");
    bgParallaxMode->addItem("Y Offset On, X Parallx Off");
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
    upperBound->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width

    boundsTabLayout->addWidget(new QLabel("Upper Bound:"), 0, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(upperBound, 0, 1);

    boundsTabLayout->addWidget(new QLabel("Lower Bound:"), 1, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(lowerBound, 1, 1);

    boundsTabLayout->addWidget(new QLabel("Unknown Upper Bound:"), 2, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(unkUpperBound, 2, 1);

    boundsTabLayout->addWidget(new QLabel("Unknown Lower Bound:"), 3, 0, 1, 1, Qt::AlignRight);
    boundsTabLayout->addWidget(unkLowerBound, 3, 1);

    boundsTabLayout->addWidget(upScrolling, 4, 1, 1, 2, Qt::AlignLeft);

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

    backgroundTabLayout->addWidget(new QLabel("Parallax Mode:"), 3, 0, 1, 1, Qt::AlignRight);
    backgroundTabLayout->addWidget(bgParallaxMode, 3, 1);

    backgroundTabLayout->addWidget(new QLabel("Background:"), 4, 0, 1, 1, Qt::AlignRight);
    backgroundTabLayout->addWidget(background, 4, 1);

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
    QFileInfo checkFile(QCoreApplication::applicationDirPath() + "/coinkiller_data/backgrounds/" + editZone->getBgName() + ".png");
    if (checkFile.exists() && checkFile.isFile()) backgroundPreview->setBg(QCoreApplication::applicationDirPath() + "/coinkiller_data/backgrounds/" + editZone->getBgName() + ".png");
    else backgroundPreview->setBg(QCoreApplication::applicationDirPath() + "/coinkiller_data/backgrounds/Invalid.png");
}

void ZoneEditorWidget::loadMusicIDs()
{
    QFile file(SettingsManager::getInstance()->getFilePath("musicids.txt"));
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    in.setCodec("UTF-8");

    musicIds.clear();
    while(!in.atEnd())
    {
        QStringList idStrings = in.readLine().split(":");
        musicIds.insert(idStrings[0].toInt(), idStrings[1]);
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
    QFile file(QCoreApplication::applicationDirPath() + "/coinkiller_data/backgrounds/backgroundnames.txt");
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
    multiplayerTracking->setCurrentText(multiplayerTrackings.value(editZone->getMultiplayerTracking()));
    musicId->setCurrentText(musicIds.value(editZone->getMusicId()));
    unk1->setValue(editZone->getUnk1());
    progPathId->setValue(editZone->getProgPathId());
    upScrolling->setChecked(editZone->getUpScrolling() != 0);
    upperBound->setValue(editZone->getUpperBound());
    lowerBound->setValue(editZone->getLowerBound());
    unkUpperBound->setValue(editZone->getUnkUpperBound());
    unkLowerBound->setValue(editZone->getUnkLowerBound());
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
    editZone->setMusicID(musicIds.key(text, 0));
    emit editMade();
}

void ZoneEditorWidget::handleMultiPlayerTrackingChange(QString text)
{
    if (!handleChanges) return;
    editZone->setMultiplayerTracking(multiplayerTrackings.key(text, 0));
    emit editMade();
}

void ZoneEditorWidget::handleUnk1Change(int val)
{
    if (!handleChanges) return;
    editZone->setUnk1(val);
    emit editMade();
}

void ZoneEditorWidget::handleUpScrollingChange(bool val)
{
    if (!handleChanges) return;
    editZone->setUpScrolling(val? 0x0F : 0x00);
    emit editMade();
}

void ZoneEditorWidget::handleUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setUpperBound(val);
    emit editMade();
}

void ZoneEditorWidget::handleLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setLowerBound(val);
    emit editMade();
}

void ZoneEditorWidget::handleUnkUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setUnkUpperBound(val);
    emit editMade();
}

void ZoneEditorWidget::handleUnkLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setUnkLowerBound(val);
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
