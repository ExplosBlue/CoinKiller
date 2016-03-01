#include "zoneeditorwidget.h"
#include "unitsconvert.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QResizeEvent>

ZoneEditorWidget::ZoneEditorWidget(QList<Zone*> *zones)
{
    this->zones = zones;

    multiplayerTrackings.insert(0, "Horizontal");
    multiplayerTrackings.insert(6, "Vertical");

    loadMusicIDs();

    zoneList = new QListWidget();
    connect(zoneList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleZoneListIndexChange(QListWidgetItem*)));

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
    connect(musicId, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleMusicIDChange(QString)));

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

    background = new QComboBox();
    loadBackgrounds();
    connect(background, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleBackgroundChange(QString)));

    backgroundPreview = new BgPreview();

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(zoneList);

    edits = new QWidget();
    QGridLayout* subLayout = new QGridLayout();
    subLayout->setMargin(0);
    edits->setLayout(subLayout);
    layout->addWidget(edits);

    subLayout->addWidget(new QLabel("ID:"), 0, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(id, 0, 1);

    subLayout->addWidget(new QLabel("Music:"), 1, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(musicId, 1, 1);

    subLayout->addWidget(new QLabel("Multiplayer Tracking:"), 2, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(multiplayerTracking, 2, 1);

    subLayout->addWidget(new QLabel("Progress Path ID:"), 3, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(progPathId, 3, 1);

    subLayout->addWidget(new HorLine(), 4, 0, 1, 2);

    subLayout->addWidget(new QLabel("Upper Bound:"), 5, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(upperBound, 5, 1);

    subLayout->addWidget(new QLabel("Lower Bound:"), 6, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(lowerBound, 6, 1);

    subLayout->addWidget(new QLabel("Unknown Upper Bound:"), 7, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(unkUpperBound, 7, 1);

    subLayout->addWidget(new QLabel("Unknown Lower Bound:"), 8, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(unkLowerBound, 8, 1);

    subLayout->addWidget(new HorLine(), 9, 0, 1, 2);

    subLayout->addWidget(new QLabel("Background:"), 10, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(background, 10, 1);

    QHBoxLayout* bgAlign = new QHBoxLayout();
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    bgAlign->addWidget(spacer);
    bgAlign->addWidget(backgroundPreview, 1);
    subLayout->addLayout(bgAlign, 11, 0, 1, 2);

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
    QFile file(QCoreApplication::applicationDirPath() + "/coinkiller_data/musicids.txt");
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

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
        zoneList->addItem(QString("%1 (at %2,%3)").arg(zone->getid()).arg(to16(zone->getx())).arg(to16(zone->gety())));

    zoneList->setCurrentIndex(index);
}

void ZoneEditorWidget::updateInfo()
{
    if (editingAZone == false)
    {
        edits->setHidden(true);
        return;
    }
    edits->setHidden(false);

    handleChanges = false;
    id->setValue(editZone->getid());
    multiplayerTracking->setCurrentText(multiplayerTrackings.value(editZone->getMultiplayerTracking()));
    musicId->setCurrentText(musicIds.value(editZone->getMusicId()));
    progPathId->setValue(editZone->getProgPathId());
    upperBound->setValue(editZone->getUpperBound());
    lowerBound->setValue(editZone->getLowerBound());
    unkUpperBound->setValue(editZone->getUnkUpperBound());
    unkLowerBound->setValue(editZone->getUnkLowerBound());
    background->setCurrentText(backgrounds.value(editZone->getBgName()));
    updateBgPreview();
    handleChanges = true;
}

void ZoneEditorWidget::handleZoneListIndexChange(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editZone = zones->at(zoneList->row(item));
    editingAZone = true;
    updateInfo();
}

void ZoneEditorWidget::handleIDChange(int idVal)
{
    if (!handleChanges) return;
    editZone->setID(idVal);
    updateList();
    emit updateLevelView();
}

void ZoneEditorWidget::handleProgPathIDChange(int ppIDVal)
{
    if (!handleChanges) return;
    editZone->setProgPathId(ppIDVal);
}

void ZoneEditorWidget::handleMusicIDChange(QString text)
{
    if (!handleChanges) return;
    editZone->setMusicID(musicIds.key(text, 0));
}

void ZoneEditorWidget::handleMultiPlayerTrackingChange(QString text)
{
    if (!handleChanges) return;
    editZone->setMultiplayerTracking(multiplayerTrackings.key(text, 0));
}

void ZoneEditorWidget::handleUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setUpperBound(val);
}

void ZoneEditorWidget::handleLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setLowerBound(val);
}

void ZoneEditorWidget::handleUnkUpperBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setUnkUpperBound(val);
}

void ZoneEditorWidget::handleUnkLowerBoundChange(int val)
{
    if (!handleChanges) return;
    editZone->setUnkLowerBound(val);
}

void ZoneEditorWidget::handleBackgroundChange(QString text)
{
    if (!handleChanges) return;
    editZone->setBackgroundName(backgrounds.key(text, "Nohara_Castle"));
    updateBgPreview();
}
