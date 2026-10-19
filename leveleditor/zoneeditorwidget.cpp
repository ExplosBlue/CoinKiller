#include "zoneeditorwidget.h"
#include "backgroundlist.h"
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
#include <QScrollBar>
#include <QTimer>
#include <QSplitter>
#include <algorithm>
#include <cmath>

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

    zoneList = new SortableTreeWidget();
    zoneList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    zoneList->setColumnCount(2);
    zoneList->setHeaderLabels(QStringList() << tr("Zone ID") << tr("Position"));
    zoneList->setSortableColumns({0, 1});
    zoneList->setFlexibleColumn(0);
    connect(zoneList, &QTreeWidget::itemClicked, this, &ZoneEditorWidget::handleZoneListIndexChange);

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

    boundingId = new QComboBox();
    boundingId->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(boundingId, &QComboBox::currentIndexChanged, this, &ZoneEditorWidget::handleBoundingIdComboChange);

    backgroundId = new QComboBox();
    backgroundId->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(backgroundId, &QComboBox::currentIndexChanged, this, &ZoneEditorWidget::handleBackgroundIdComboChange);

    foreach (const BackgroundEntry& entry, BackgroundList().entries())
        backgroundNames.insert(entry.fileName, entry.name);

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

    editBounding = new QPushButton(tr("Edit Boundings"), this);
    editBounding->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(editBounding, &QPushButton::clicked, this, &ZoneEditorWidget::handleEditBoundingClicked);

    editBackground = new QPushButton(tr("Edit Backgrounds"), this);
    editBackground->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(editBackground, &QAbstractButton::clicked, this, &ZoneEditorWidget::handleEditBackgroundClicked);

    const int editorButtonWidth = qMax(editBounding->sizeHint().width(), editBackground->sizeHint().width()) + 14;
    editBounding->setFixedWidth(editorButtonWidth);
    editBackground->setFixedWidth(editorButtonWidth);

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
        zoneList->setHeaderLabels(QStringList() << tr("Zone ID") << tr("Position"));

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

    for (int i = 0; i < zoneList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = zoneList->topLevelItem(i);
        if (static_cast<Zone*>(item->data(0, Qt::UserRole).value<void*>()) == zone)
        {
            zoneList->setCurrentItem(item);
            break;
        }
    }
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
    setAlignment(Qt::AlignCenter);
}

QSize BgPreview::sizeHint() const
{
    return QSize(maximumWidth(), maximumWidth() * 240 / 400);
}

QSize BgPreview::minimumSizeHint() const
{
    return QSize(minimumWidth(), minimumWidth() * 240 / 400);
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
    Zone* selected = nullptr;
    if (!zoneList->selectedItems().isEmpty())
        selected = static_cast<Zone*>(zoneList->selectedItems().first()->data(0, Qt::UserRole).value<void*>());

    zoneList->clear();
    foreach (Zone* zone, *zones)
    {
        SortableTreeItem* item = new SortableTreeItem();
        item->setData(0, Qt::DisplayRole, zone->getid());
        item->setSortKey(0, static_cast<double>(zone->getid()));
        item->setText(1, tr("(X: %1, Y: %2) (W: %3, H: %4)").arg(to16(zone->getx())).arg(to16(zone->gety())).arg(to16(zone->getwidth())).arg(to16(zone->getheight())));
        item->setSortKey(1, std::hypot(static_cast<double>(to16(zone->getx())), static_cast<double>(to16(zone->gety()))));
        item->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(zone)));
        zoneList->addTopLevelItem(item);
    }

    zoneList->resort();

    if (selected == nullptr)
        return;

    for (int i = 0; i < zoneList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = zoneList->topLevelItem(i);
        if (static_cast<Zone*>(item->data(0, Qt::UserRole).value<void*>()) == selected)
        {
            zoneList->setCurrentItem(item);
            break;
        }
    }
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
    populateBoundingIds();
    populateBackgroundIds();
    camFlag1->setChecked(editZone->getCameraFlags() & 1);
    camFlag2->setChecked(editZone->getCameraFlags() & 2);
    camFlag3->setChecked(editZone->getCameraFlags() & 4);
    camFlag4->setChecked(editZone->getCameraFlags() & 8);

    handleChanges = true;
}

void ZoneEditorWidget::populateBoundingIds()
{
    QList<int> ids;
    QMap<int, ZoneBounding*> bounds;

    foreach (ZoneBounding* bound, *zoneBoundings)
    {
        ids.append(bound->getId());
        bounds.insert(bound->getId(), bound);
    }

    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());

    const int current = editZone ? editZone->getBoundingId() : 0;

    if (!ids.isEmpty() && !ids.contains(current))
        ids.append(current);

    boundingId->blockSignals(true);
    boundingId->clear();

    foreach (int id, ids)
    {
        ZoneBounding* bounding = bounds.value(id, nullptr);

        if (bounding)
            boundingId->addItem(tr("%1: %2").arg(id).arg(tr("(%1, %2)").arg(bounding->getPrimaryUpperBound()).arg(bounding->getPrimaryLowerBound())), id);
        else
            boundingId->addItem(QString::number(id), id);
    }

    if (ids.isEmpty())
    {
        boundingId->addItem(tr("None"));
        boundingId->setEnabled(false);
    }
    else
    {
        boundingId->setEnabled(true);
        boundingId->setCurrentIndex(ids.indexOf(current));
    }

    boundingId->blockSignals(false);
}

void ZoneEditorWidget::populateBackgroundIds()
{
    QList<int> ids;
    QMap<int, QString> names;

    foreach (ZoneBackground* bg, *zoneBgs)
    {
        ids.append(bg->getId());
        names.insert(bg->getId(), backgroundNames.value(bg->getName(), bg->getName()));
    }

    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());

    const int current = editZone ? editZone->getBackgroundId() : 0;

    if (!ids.isEmpty() && !ids.contains(current))
        ids.append(current);

    backgroundId->blockSignals(true);
    backgroundId->clear();

    foreach (int id, ids)
    {
        const QString name = names.value(id);

        if (name.isEmpty())
            backgroundId->addItem(QString::number(id), id);
        else
            backgroundId->addItem(tr("%1: %2").arg(id).arg(name), id);
    }

    if (ids.isEmpty())
    {
        backgroundId->addItem(tr("None"));
        backgroundId->setEnabled(false);
    }
    else
    {
        backgroundId->setEnabled(true);
        backgroundId->setCurrentIndex(ids.indexOf(current));
    }

    backgroundId->blockSignals(false);
}

void ZoneEditorWidget::handleZoneListIndexChange(QTreeWidgetItem *item)
{
    if (!handleChanges) return;
    editZone = static_cast<Zone*>(item->data(0, Qt::UserRole).value<void*>());
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

void ZoneEditorWidget::handleBoundingIdComboChange()
{
    if (!handleChanges) return;
    if (editZone == nullptr) return;

    const int val = boundingId->currentData().toInt();

    if (val == editZone->getBoundingId())
        return;

    handleBoundingIDChange(val);
}

void ZoneEditorWidget::handleBackgroundIdComboChange()
{
    if (!handleChanges) return;
    if (editZone == nullptr) return;

    const int val = backgroundId->currentData().toInt();

    if (val == editZone->getBackgroundId())
        return;

    handleBackgroundIDChange(val);
}

void ZoneEditorWidget::handleEditBoundingClicked()
{
    if (!handleChanges) return;
    boundingWidget->updateWidget();
    boundingWidget->setSelectedIndex(editZone->getBoundingId());
    boundingWidget->exec();
    updateInfo();
}

void ZoneEditorWidget::handleEditBackgroundClicked()
{
    if (!handleChanges) return;
    backgroundWidget->updateWidget();
    backgroundWidget->setSelectedIndex(editZone->getBackgroundId());
    backgroundWidget->exec();
    updateInfo();
}


// Zone Background Widget
namespace {

bool g_backgroundThumbnailView = true;

}

BackgroundThumbnailDelegate::BackgroundThumbnailDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void BackgroundThumbnailDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
    {
        const bool enabled = option.state & QStyle::State_Enabled;
        const QColor highlight = option.palette.color(enabled ? QPalette::Active : QPalette::Disabled, QPalette::Highlight);

        painter->fillRect(option.rect, highlight);
    }

    QStyledItemDelegate::paint(painter, option, index);
}

BackgroundThumbnailList::BackgroundThumbnailList(QWidget *parent) :
    QListWidget(parent)
{
}

void BackgroundThumbnailList::relayoutItems()
{
    doItemsLayout();
    scheduleDelayedItemsLayout();
}

void BackgroundThumbnailList::setContentMargins(int left, int right)
{
    setViewportMargins(left, 0, right, 0);
}

QMargins BackgroundThumbnailList::contentMargins() const
{
    return viewportMargins();
}

void BackgroundThumbnailList::resizeEvent(QResizeEvent *event)
{
    QListWidget::resizeEvent(event);

    doItemsLayout();
}

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

    backgroundPreview = new BgPreview();

    groupCombo = new QComboBox();
    connect(groupCombo, &QComboBox::currentIndexChanged, this, &ZoneBackgroundWidget::handleGroupChange);

    thumbnailToggleBtn = new QToolButton(this);
    thumbnailToggleBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    thumbnailToggleBtn->setToolTip(tr("Toggle list and thumbnail view"));
    connect(thumbnailToggleBtn, &QToolButton::clicked, this, &ZoneBackgroundWidget::handleViewToggle);

    selectFilter = new QLineEdit();
    selectFilter->setPlaceholderText(tr("Filter backgrounds..."));
    selectFilter->setClearButtonEnabled(true);
    connect(selectFilter, &QLineEdit::textChanged, this, &ZoneBackgroundWidget::handleSelectFilterChange);

    selectList = new QTreeWidget();
    selectList->setHeaderLabels(QStringList{tr("Backgrounds")});
    selectList->setRootIsDecorated(false);
    selectList->setUniformRowHeights(true);
    selectList->setSelectionMode(QAbstractItemView::SingleSelection);
    selectList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(selectList, &QTreeWidget::itemClicked, this, &ZoneBackgroundWidget::handleSelectListClicked);

    thumbnailList = new BackgroundThumbnailList();
    thumbnailList->setViewMode(QListView::IconMode);
    thumbnailList->setResizeMode(QListView::Adjust);
    thumbnailList->setMovement(QListView::Static);
    thumbnailList->setFlow(QListView::LeftToRight);
    thumbnailList->setWrapping(true);
    thumbnailList->setIconSize(QSize(120, 72));
    thumbnailList->setSpacing(0);
    thumbnailList->setItemAlignment(Qt::AlignCenter);
    thumbnailList->setWordWrap(true);
    thumbnailList->setSelectionMode(QAbstractItemView::SingleSelection);
    thumbnailList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    thumbnailList->viewport()->installEventFilter(this);
    thumbnailList->setItemDelegate(new BackgroundThumbnailDelegate(this));
    connect(thumbnailList, &QListWidget::itemClicked, this, &ZoneBackgroundWidget::handleThumbnailClicked);

    QPalette listPalette = thumbnailList->palette();
    listPalette.setColor(QPalette::Window, listPalette.color(QPalette::Base));
    thumbnailList->setPalette(listPalette);
    thumbnailList->setAutoFillBackground(true);

    selectStack = new QStackedWidget();
    selectStack->addWidget(selectList);
    selectStack->addWidget(thumbnailList);

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

    settingsLayout->addWidget(backgroundPreview, 4, 0, 1, 2);

    settingsGroup->setLayout(settingsLayout);

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setChildrenCollapsible(false);
    layout->addWidget(mainSplitter);

    QWidget* editorPane = new QWidget(mainSplitter);
    QVBoxLayout* editorLayout = new QVBoxLayout(editorPane);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->addWidget(backgroundList, 1);

    QHBoxLayout* btnsLayout = new QHBoxLayout();
    btnsLayout->addWidget(addBackgroundBtn);
    btnsLayout->addWidget(removeBackgroundBtn);
    editorLayout->addLayout(btnsLayout);

    editorLayout->addWidget(settingsGroup);
    mainSplitter->addWidget(editorPane);

    QWidget* selectPane = new QWidget(mainSplitter);
    selectPane->setMinimumWidth(180);
    QVBoxLayout* selectLayout = new QVBoxLayout(selectPane);
    selectLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* filterRow = new QHBoxLayout();
    filterRow->addWidget(selectFilter, 1);
    filterRow->addWidget(thumbnailToggleBtn);
    filterRow->addWidget(groupCombo);
    selectLayout->addLayout(filterRow);
    selectLayout->addWidget(selectStack, 1);
    mainSplitter->addWidget(selectPane);

    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({314, 421});

    loadBackgrounds();
    setSelectionEnabled(false);

    setThumbnailView(g_backgroundThumbnailView);

    updateList();
    updateInfo();

    setWindowTitle(tr("CoinKiller - Select Background"));

    resize(765, 540);
}

void ZoneBackgroundWidget::updateList()
{
    const int currentRow = backgroundList->currentRow();

    backgroundList->clear();
    foreach (ZoneBackground* bg, *zoneBgs)
    {
        const QString name = backgroundNames.value(bg->getName(), bg->getName());

        backgroundList->addItem(tr("Background %1 - %2").arg(bg->getId()).arg(name));
        backgroundList->item(backgroundList->count() - 1)->setData(Qt::UserRole, bg->getId());
    }

    if (currentRow >= 0 && currentRow < backgroundList->count())
    {
        backgroundList->setCurrentRow(currentRow);
        backgroundList->item(currentRow)->setSelected(true);
    }
}

QListWidgetItem* ZoneBackgroundWidget::backgroundItemById(int id) const
{
    for (int i = 0; i < backgroundList->count(); i++)
    {
        QListWidgetItem* item = backgroundList->item(i);

        if (item->data(Qt::UserRole).toInt() == id)
            return item;
    }

    return nullptr;
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
    parallaxMode->setCurrentIndex(editBg->getParallaxMode());

    updateBgPreview();

    populateChoices();

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
    setSelectionEnabled(true);
    editingABg = true;
    updateInfo();
}

void ZoneBackgroundWidget::loadBackgrounds()
{
    backgroundGroups = BackgroundList().groups();

    for (const BackgroundEntry& entry : BackgroundList().entries())
        backgroundNames.insert(entry.fileName, entry.name);

    populateGroups();
    populateChoices();
}

void ZoneBackgroundWidget::populateGroups()
{
    groupCombo->blockSignals(true);
    groupCombo->clear();
    groupCombo->addItem(tr("All"));

    for (const BackgroundGroup& group : backgroundGroups)
        groupCombo->addItem(group.name);

    groupCombo->blockSignals(false);
    groupCombo->setCurrentIndex(0);
}

QList<BackgroundEntry> ZoneBackgroundWidget::currentGroupEntries() const
{
    QList<BackgroundEntry> entries;

    int groupIndex = groupCombo->currentIndex() - 1;

    if (groupIndex < 0)
    {
        for (const BackgroundGroup& group : backgroundGroups)
            entries.append(group.entries);
    }
    else if (groupIndex < backgroundGroups.size())
    {
        entries = backgroundGroups[groupIndex].entries;
    }

    return entries;
}

void ZoneBackgroundWidget::populateChoices()
{
    const QList<BackgroundEntry> entries = currentGroupEntries();
    const QString current = editBg ? editBg->getName() : QString();

    selectList->blockSignals(true);
    selectList->setSortingEnabled(false);
    selectList->clear();

    for (const BackgroundEntry& entry : entries)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(selectList);
        item->setText(0, entry.name);
        item->setData(0, Qt::UserRole, entry.fileName);
        item->setToolTip(0, entry.fileName);
    }

    selectList->setSortingEnabled(true);
    selectList->sortItems(0, Qt::AscendingOrder);

    QTreeWidgetItem* rowToSelect = nullptr;

    for (int i = 0; i < selectList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = selectList->topLevelItem(i);

        if (item->data(0, Qt::UserRole).toString() == current)
        {
            rowToSelect = item;
            break;
        }
    }

    selectList->setCurrentItem(rowToSelect);
    selectList->blockSignals(false);

    populateThumbnails();

    applyFilter();
}

QSize ZoneBackgroundWidget::thumbnailTileSize() const
{
    const QSize iconSize = thumbnailList->iconSize();
    const QFontMetrics fm(font());

    return QSize(iconSize.width() + 10, iconSize.height() + fm.height() * 2 + 10);
}

void ZoneBackgroundWidget::updateThumbnailGrid()
{
    const QSize tileSize = thumbnailTileSize();
    const bool gridChanged = tileSize != thumbnailList->gridSize();

    if (gridChanged)
        thumbnailList->setGridSize(tileSize);

    for (int i = 0; i < thumbnailList->count(); i++)
        thumbnailList->item(i)->setSizeHint(tileSize);

    const int frame = thumbnailList->frameWidth();
    const int scrollbar = style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, thumbnailList);
    const int available = thumbnailList->width() - frame * 2 - scrollbar;
    const int columns = qMax(1, available / tileSize.width());
    const int used = columns * tileSize.width();

    const int content = used + 4;
    const int margin = qMax(0, (available - content) / 2);
    const int right = qMax(0, available - content - margin);

    const bool marginsChanged = thumbnailList->contentMargins().left() != margin || thumbnailList->contentMargins().right() != right;

    if (marginsChanged)
        thumbnailList->setContentMargins(margin, right);

    thumbnailList->relayoutItems();

    if (gridChanged || marginsChanged)
        QTimer::singleShot(0, this, &ZoneBackgroundWidget::updateThumbnailGrid);
}

bool ZoneBackgroundWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == thumbnailList->viewport() && event->type() == QEvent::Resize)
        updateThumbnailGrid();

    return QDialog::eventFilter(watched, event);
}

void ZoneBackgroundWidget::populateThumbnails()
{
    const QList<BackgroundEntry> entries = currentGroupEntries();
    const QString current = editBg ? editBg->getName() : QString();

    const QSize iconSize = thumbnailList->iconSize();

    thumbnailList->blockSignals(true);
    thumbnailList->clear();

    QListWidgetItem* itemToSelect = nullptr;

    for (const BackgroundEntry& entry : entries)
    {
        QPixmap preview(BackgroundList::previewPath(entry.fileName));

        if (!preview.isNull())
            preview = preview.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QListWidgetItem* item = new QListWidgetItem(thumbnailList);
        item->setIcon(QIcon(preview));
        item->setText(entry.name);
        item->setTextAlignment(Qt::AlignCenter);
        item->setSizeHint(thumbnailList->gridSize());
        item->setData(Qt::UserRole, entry.fileName);
        item->setToolTip(entry.fileName);

        if (entry.fileName == current)
            itemToSelect = item;
    }

    thumbnailList->sortItems(Qt::AscendingOrder);
    thumbnailList->setCurrentItem(itemToSelect);
    thumbnailList->blockSignals(false);

    updateThumbnailGrid();
}

void ZoneBackgroundWidget::applyFilter()
{
    const QString filter = selectFilter->text().trimmed();

    for (int i = 0; i < selectList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = selectList->topLevelItem(i);
        item->setHidden(!filter.isEmpty() && !item->text(0).contains(filter, Qt::CaseInsensitive));
    }

    for (int i = 0; i < thumbnailList->count(); i++)
    {
        QListWidgetItem* item = thumbnailList->item(i);
        item->setHidden(!filter.isEmpty() && !item->text().contains(filter, Qt::CaseInsensitive));
    }

    updateThumbnailGrid();
}

void ZoneBackgroundWidget::updateBgPreview()
{
    if (editBg == nullptr)
        return;

    backgroundPreview->setBg(QPixmap(BackgroundList::previewPath(editBg->getName())));
}

void ZoneBackgroundWidget::handleGroupChange()
{
    populateChoices();
}

void ZoneBackgroundWidget::handleSelectListClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if (!handleChanges) return;
    if (editingABg == false) return;
    if (item == nullptr) return;

    QString name = item->data(0, Qt::UserRole).toString();

    if (name.isEmpty() || name == editBg->getName())
        return;

    undoStack->push(new Commands::ZoneBgCmd::SetName(editBg, name));
    updateList();
    updateInfo();
}

void ZoneBackgroundWidget::handleThumbnailClicked(QListWidgetItem* item)
{
    if (!handleChanges) return;
    if (editingABg == false) return;
    if (item == nullptr) return;

    const QString name = item->data(Qt::UserRole).toString();

    if (name.isEmpty() || name == editBg->getName())
        return;

    undoStack->push(new Commands::ZoneBgCmd::SetName(editBg, name));
    updateList();
    updateInfo();
}

void ZoneBackgroundWidget::handleViewToggle()
{
    setThumbnailView(!g_backgroundThumbnailView);
}

void ZoneBackgroundWidget::setThumbnailView(bool enabled)
{
    g_backgroundThumbnailView = enabled;

    selectStack->setCurrentIndex(enabled ? 1 : 0);
}

void ZoneBackgroundWidget::handleSelectFilterChange(const QString& text)
{
    Q_UNUSED(text);

    applyFilter();
}

void ZoneBackgroundWidget::setSelectionEnabled(bool enabled)
{
    groupCombo->setEnabled(enabled);
    selectList->setEnabled(enabled);
    selectFilter->setEnabled(enabled);
    thumbnailList->setEnabled(enabled);
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

    QListWidgetItem* item = backgroundItemById(id);

    if (item == nullptr)
        return;

    backgroundList->setCurrentItem(item);

    editBg = zoneBgs->at(backgroundList->row(item));
    settingsGroup->setEnabled(true);
    setSelectionEnabled(true);
    editingABg = true;

    updateInfo();
}

void ZoneBackgroundWidget::handleRemoveBackgroundClicked()
{
    if (editingABg == false)
        return;

    if (!handleChanges) return;

    undoStack->push(new Commands::LevelCmd::DeleteZoneBackground(level, editBg));

    editingABg = false;
    editBg = nullptr;
    settingsGroup->setDisabled(true);
    setSelectionEnabled(false);

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
    QListWidgetItem* item = backgroundItemById(index);

    if (item == nullptr)
        return;

    backgroundList->setCurrentItem(item);
    backgroundList->setFocus();
    editBg = zoneBgs->at(backgroundList->row(item));
    editingABg = true;
    updateInfo();
    settingsGroup->setEnabled(true);
    setSelectionEnabled(true);
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

    setWindowTitle(tr("CoinKiller - Edit Boundings"));
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
