#include "tilesetselectdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPainter>
#include <QStyle>
#include <QKeyEvent>
#include <QHeaderView>

TilesetSelectionDialog::TilesetSelectionDialog(Game* game, int slot, const QString& currentTileset, int currentObjectCount, QWidget* parent) :
    QDialog(parent),
    game(game),
    slot(slot),
    currentTileset(currentTileset),
    currentObjectCount(currentObjectCount)
{
    setWindowTitle(tr("CoinKiller - Select Tileset"));

    groups = game->getTilesetList()->groups();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    mainLayout->addWidget(splitter, 1);

    QWidget* tilesetPane = new QWidget(splitter);
    QVBoxLayout* tilesetLayout = new QVBoxLayout(tilesetPane);
    tilesetLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* filterRow = new QHBoxLayout();
    filterRow->setContentsMargins(0, 0, 0, 0);

    filterBox = new QLineEdit(tilesetPane);
    filterBox->setPlaceholderText(tr("Filter tilesets..."));
    filterBox->setClearButtonEnabled(true);
    filterRow->addWidget(filterBox, 1);

    groupCombo = new QComboBox(tilesetPane);
    groupCombo->setMinimumWidth(100);
    filterRow->addWidget(groupCombo);

    tilesetLayout->addLayout(filterRow);

    tilesetList = new QTreeWidget(tilesetPane);
    tilesetList->setHeaderLabels(QStringList{tr("Tilesets")});
    tilesetList->setRootIsDecorated(true);
    tilesetList->setUniformRowHeights(true);
    tilesetList->setSelectionMode(QAbstractItemView::SingleSelection);
    tilesetList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tilesetList->header()->setSectionsClickable(true);
    tilesetLayout->addWidget(tilesetList, 1);

    tilesetPane->setMinimumWidth(220);
    splitter->addWidget(tilesetPane);

    QWidget* previewPane = new QWidget(splitter);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewPane);
    previewLayout->setContentsMargins(0, 0, 0, 0);

    filenameLabel = new QLabel(previewPane);
    filenameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    previewLayout->addWidget(filenameLabel);

    previewList = new QListView(previewPane);
    previewList->setViewMode(QListView::IconMode);
    previewList->setFlow(QListView::LeftToRight);
    previewList->setMovement(QListView::Static);
    previewList->setResizeMode(QListView::Adjust);
    previewList->setWrapping(true);
    previewList->setIconSize(QSize(200, 200));
    previewList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    previewList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    previewList->setSelectionMode(QAbstractItemView::NoSelection);
    previewList->setFocusPolicy(Qt::NoFocus);
    previewModel = new QStandardItemModel(this);
    previewList->setModel(previewModel);
    previewLayout->addWidget(previewList, 1);

    previewPane->setMinimumWidth(200);
    splitter->addWidget(previewPane);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    splitter->setSizes({320, 540});

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    mainLayout->addLayout(bottomLayout);

    warningWidget = new QWidget(this);
    QHBoxLayout* warningLayout = new QHBoxLayout(warningWidget);
    warningLayout->setContentsMargins(0, 0, 0, 0);
    warningLayout->setSpacing(6);

    QLabel* warningIcon = new QLabel(warningWidget);
    warningIcon->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(16, 16));
    warningIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    warningLayout->addWidget(warningIcon);

    QLabel* warningText = new QLabel(tr("This tileset has fewer objects than the currently used one."), warningWidget);
    warningLayout->addWidget(warningText);

    warningWidget->setVisible(false);

    bottomLayout->addWidget(warningWidget);
    bottomLayout->addStretch();
    bottomLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(groupCombo, &QComboBox::currentIndexChanged, this, &TilesetSelectionDialog::groupChanged);
    connect(tilesetList->header(), &QHeaderView::sectionClicked, this, &TilesetSelectionDialog::sortTilesets);
    connect(tilesetList, &QTreeWidget::currentItemChanged, this, &TilesetSelectionDialog::tilesetChanged);
    connect(tilesetList, &QTreeWidget::itemDoubleClicked, this, &TilesetSelectionDialog::tilesetDoubleClicked);
    connect(filterBox, &QLineEdit::textChanged, this, &TilesetSelectionDialog::applyFilter);

    tilesetList->installEventFilter(this);

    groupCombo->addItem(tr("All"));

    for (const TilesetGroup& group : groups)
        groupCombo->addItem(group.name);

    groupCombo->setCurrentIndex(0);

    resize(860, 540);
}

TilesetSelectionDialog::~TilesetSelectionDialog()
{
    clearPreview();
}

void TilesetSelectionDialog::groupChanged()
{
    populateTilesets();
}

void TilesetSelectionDialog::populateTilesets()
{
    tilesetList->blockSignals(true);
    tilesetList->setSortingEnabled(false);
    tilesetList->clear();

    int groupIndex = groupCombo->currentIndex() - 1;

    noneItem = new QTreeWidgetItem(tilesetList);
    noneItem->setText(0, tr("<none>"));
    noneItem->setData(0, Qt::UserRole, QString());
    noneItem->setToolTip(0, tr("No tileset"));

    if (groupIndex < 0)
    {
        for (const TilesetGroup& group : groups)
            addTilesets(nullptr, matchingEntries(group.allTilesets()), group.name);
    }
    else if (groupIndex < groups.size())
    {
        addGroup(groups[groupIndex]);
    }

    QTreeWidgetItem* rowToSelect = tilesetList->topLevelItem(0);
    if (!currentTileset.isEmpty())
    {
        QTreeWidgetItem* found = findTilesetItem(currentTileset);
        if (found != nullptr)
            rowToSelect = found;
    }

    tilesetList->setCurrentItem(rowToSelect);

    tilesetList->blockSignals(false);

    tilesetChanged(tilesetList->currentItem(), nullptr);
    applyFilter();
    applySort();
}

QTreeWidgetItem* TilesetSelectionDialog::findTilesetItem(const QString& fileName) const
{
    QList<QTreeWidgetItem*> stack;
    for (int i = 0; i < tilesetList->topLevelItemCount(); i++)
        stack.append(tilesetList->topLevelItem(i));

    while (!stack.isEmpty())
    {
        QTreeWidgetItem* item = stack.takeFirst();

        if (item->data(0, Qt::UserRole).toString() == fileName)
            return item;

        for (int i = 0; i < item->childCount(); i++)
            stack.append(item->child(i));
    }

    return nullptr;
}

void TilesetSelectionDialog::sortTilesets(int column)
{
    Q_UNUSED(column);

    QHeaderView* header = tilesetList->header();

    Qt::SortOrder order = Qt::AscendingOrder;
    if (header->isSortIndicatorShown() && header->sortIndicatorOrder() == Qt::AscendingOrder)
        order = Qt::DescendingOrder;

    header->setSortIndicatorShown(true);
    header->setSortIndicator(0, order);

    applySort();
}

void TilesetSelectionDialog::applySort()
{
    QHeaderView* header = tilesetList->header();
    if (!header->isSortIndicatorShown())
        return;

    Qt::SortOrder order = header->sortIndicatorOrder();

    int noneIndex = noneItem ? tilesetList->indexOfTopLevelItem(noneItem) : -1;
    QTreeWidgetItem* none = noneIndex >= 0 ? tilesetList->takeTopLevelItem(noneIndex) : nullptr;

    for (int i = 0; i < tilesetList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tilesetList->topLevelItem(i);
        if (item->childCount() > 0)
            item->sortChildren(0, order);
    }

    tilesetList->sortItems(0, order);

    if (none != nullptr)
        tilesetList->insertTopLevelItem(0, none);
}

void TilesetSelectionDialog::addGroup(const TilesetGroup& group)
{
    for (const TilesetSubgroup& subgroup : group.subgroups)
    {
        QList<TilesetEntry> entries = matchingEntries(subgroup.tilesets);
        if (entries.isEmpty())
            continue;

        QTreeWidgetItem* subgroupItem = new QTreeWidgetItem(tilesetList);
        subgroupItem->setText(0, subgroup.name);
        subgroupItem->setFlags(subgroupItem->flags() & ~Qt::ItemIsSelectable);

        addTilesets(subgroupItem, entries);
        subgroupItem->setExpanded(true);
    }

    addTilesets(nullptr, matchingEntries(group.tilesets));
}

QList<TilesetEntry> TilesetSelectionDialog::matchingEntries(const QList<TilesetEntry>& entries) const
{
    QList<TilesetEntry> slotEntries;
    for (const TilesetEntry& entry : entries)
        if (TilesetList::matchesSlot(entry.fileName, slot))
            slotEntries.append(entry);

    return slotEntries;
}

void TilesetSelectionDialog::addTilesets(QTreeWidgetItem* parent, const QList<TilesetEntry>& entries, const QString& groupName)
{
    for (const TilesetEntry& entry : entries)
    {
        QTreeWidgetItem* item = parent ? new QTreeWidgetItem(parent) : new QTreeWidgetItem(tilesetList);
        item->setText(0, groupName.isEmpty() ? entry.name : tr("%1 - %2").arg(groupName, entry.name));
        item->setData(0, Qt::UserRole, entry.fileName);
        item->setData(0, Qt::UserRole + 1, entry.name);
        item->setToolTip(0, entry.fileName);
    }
}

void TilesetSelectionDialog::applyFilter()
{
    const QString filter = filterBox->text().trimmed();

    for (int i = 0; i < tilesetList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tilesetList->topLevelItem(i);

        QString itemName = item->data(0, Qt::UserRole + 1).toString();
        if (itemName.isEmpty())
            itemName = item->text(0);

        bool selfMatch = filter.isEmpty() || itemName.contains(filter, Qt::CaseInsensitive);

        if (item->childCount() > 0)
        {
            bool anyVisible = false;

            for (int c = 0; c < item->childCount(); c++)
            {
                QTreeWidgetItem* child = item->child(c);

                QString childName = child->data(0, Qt::UserRole + 1).toString();
                if (childName.isEmpty())
                    childName = child->text(0);

                bool childMatch = filter.isEmpty() || childName.contains(filter, Qt::CaseInsensitive);
                child->setHidden(!childMatch);
                anyVisible = anyVisible || childMatch;
            }

            item->setHidden(!anyVisible);

            if (!filter.isEmpty() && anyVisible)
                item->setExpanded(true);
        }
        else
        {
            item->setHidden(!selfMatch);
        }
    }
}

void TilesetSelectionDialog::tilesetChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);

    selectedFileName = current ? current->data(0, Qt::UserRole).toString() : QString();

    if (selectedFileName.isEmpty())
        filenameLabel->setText(QString());
    else
        filenameLabel->setText(tr("Filename: %1").arg(selectedFileName));

    loadPreview(selectedFileName);
}

bool TilesetSelectionDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress && watched == tilesetList)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (!(keyEvent->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)))
        {
            const QString text = keyEvent->text();

            if (!text.isEmpty() && text.at(0).isPrint())
            {
                filterBox->setFocus();
                filterBox->setText(filterBox->text() + text);
                return true;
            }

            if (keyEvent->key() == Qt::Key_Backspace && !filterBox->text().isEmpty())
            {
                const QString current2 = filterBox->text();
                filterBox->setFocus();
                filterBox->setText(current2.left(current2.size() - 1));
                return true;
            }
        }
    }

    return QDialog::eventFilter(watched, event);
}

void TilesetSelectionDialog::tilesetDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);

    if (item == nullptr)
        return;

    tilesetList->setCurrentItem(item);

    if (item->childCount() > 0)
    {
        item->setExpanded(!item->isExpanded());
        return;
    }

    accept();
}

void TilesetSelectionDialog::clearPreview()
{
    previewModel->clear();

    if (warningWidget != nullptr)
        warningWidget->setVisible(false);

    if (previewTileset != nullptr)
    {
        delete previewTileset;
        previewTileset = nullptr;
    }
}

void TilesetSelectionDialog::loadPreview(const QString& fileName)
{
    clearPreview();

    if (fileName.isEmpty())
        return;

    try
    {
        previewTileset = game->getTileset(fileName);
    }
    catch (...)
    {
        previewTileset = nullptr;
        return;
    }

    if (currentObjectCount >= 0 && previewTileset->getNumObjects() < currentObjectCount)
        warningWidget->setVisible(true);

    for (int i = 0; i < previewTileset->getNumObjects(); i++)
    {
        ObjectDef* obj = previewTileset->getObjectDef(i);

        QPixmap objPixmap(obj->width * 20, obj->height * 20);
        objPixmap.fill(Qt::transparent);

        QPainter p(&objPixmap);
        TileGrid tileGrid;
        tileGrid.clear();
        tileGrid[0xFFFFFFFF] = 1;
        previewTileset->Render2DTiles(true);
        previewTileset->Render3DOverlay(true);
        previewTileset->drawObject(p, tileGrid, i, 0, 0, obj->width, obj->height, 1);
        p.end();

        QStandardItem* objItem = new QStandardItem();
        objItem->setIcon(QIcon(objPixmap));
        objItem->setToolTip(tr("Object: %1").arg(i));
        previewModel->appendRow(objItem);
    }
}
