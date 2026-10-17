#include "tilesetselectdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPainter>
#include <QStyle>
#include <QKeyEvent>

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

    QWidget* groupPane = new QWidget(splitter);
    QVBoxLayout* groupLayout = new QVBoxLayout(groupPane);
    groupLayout->setContentsMargins(0, 0, 0, 0);
    groupLayout->addWidget(new QLabel(tr("Groups"), groupPane));

    groupList = new QListWidget(groupPane);
    groupList->setSelectionMode(QAbstractItemView::SingleSelection);
    groupLayout->addWidget(groupList, 1);

    groupPane->setMinimumWidth(110);
    splitter->addWidget(groupPane);

    QWidget* tilesetPane = new QWidget(splitter);
    QVBoxLayout* tilesetLayout = new QVBoxLayout(tilesetPane);
    tilesetLayout->setContentsMargins(0, 0, 0, 0);

    filterBox = new QLineEdit(tilesetPane);
    filterBox->setPlaceholderText(tr("Filter tilesets..."));
    filterBox->setClearButtonEnabled(true);
    tilesetLayout->addWidget(filterBox);

    tilesetList = new QTreeWidget(tilesetPane);
    tilesetList->setHeaderLabels(QStringList{tr("Tilesets")});
    tilesetList->setRootIsDecorated(false);
    tilesetList->setUniformRowHeights(true);
    tilesetList->setSelectionMode(QAbstractItemView::SingleSelection);
    tilesetList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tilesetLayout->addWidget(tilesetList, 1);

    tilesetPane->setMinimumWidth(160);
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

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 3);
    splitter->setSizes({160, 280, 440});

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

    connect(groupList, &QListWidget::currentRowChanged, this, &TilesetSelectionDialog::groupChanged);
    connect(tilesetList, &QTreeWidget::currentItemChanged, this, &TilesetSelectionDialog::tilesetChanged);
    connect(tilesetList, &QTreeWidget::itemDoubleClicked, this, &TilesetSelectionDialog::tilesetDoubleClicked);
    connect(filterBox, &QLineEdit::textChanged, this, &TilesetSelectionDialog::applyFilter);

    groupList->installEventFilter(this);
    tilesetList->installEventFilter(this);

    int selectedGroup = 0;
    if (!currentTileset.isEmpty())
    {
        for (int g = 0; g < groups.size(); g++)
        {
            bool found = false;
            for (const TilesetEntry& entry : groups[g].tilesets)
            {
                if (entry.fileName == currentTileset)
                {
                    selectedGroup = g;
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }
    }

    if (groups.isEmpty())
    {
        QListWidgetItem* noGroups = new QListWidgetItem(tr("No Groups"));
        noGroups->setFlags(Qt::NoItemFlags);
        groupList->addItem(noGroups);
        populateTilesets();
    }
    else
    {
        for (const TilesetGroup& group : groups)
            groupList->addItem(group.name);

        groupList->setCurrentRow(selectedGroup);
    }

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

    int groupIndex = groupList->currentRow();
    if (groupIndex >= 0 && groupIndex < groups.size())
    {
        QTreeWidgetItem* noneItem = new QTreeWidgetItem(tilesetList);
        noneItem->setText(0, tr("<none>"));
        noneItem->setData(0, Qt::UserRole, QString());
        noneItem->setToolTip(0, tr("No tileset"));

        for (const TilesetEntry& entry : groups[groupIndex].tilesets)
        {
            if (!TilesetList::matchesSlot(entry.fileName, slot))
                continue;

            QTreeWidgetItem* item = new QTreeWidgetItem(tilesetList);
            item->setText(0, entry.name);
            item->setData(0, Qt::UserRole, entry.fileName);
            item->setToolTip(0, entry.fileName);
        }

        tilesetList->setSortingEnabled(true);
        tilesetList->sortItems(0, Qt::AscendingOrder);

        QTreeWidgetItem* rowToSelect = tilesetList->topLevelItem(0);
        if (!currentTileset.isEmpty())
        {
            for (int i = 0; i < tilesetList->topLevelItemCount(); i++)
            {
                QTreeWidgetItem* item = tilesetList->topLevelItem(i);
                if (item->data(0, Qt::UserRole).toString() == currentTileset)
                {
                    rowToSelect = item;
                    break;
                }
            }
        }

        tilesetList->setCurrentItem(rowToSelect);
    }

    tilesetList->blockSignals(false);

    tilesetChanged(tilesetList->currentItem(), nullptr);
    applyFilter();
}

void TilesetSelectionDialog::applyFilter()
{
    const QString filter = filterBox->text().trimmed();

    for (int i = 0; i < tilesetList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tilesetList->topLevelItem(i);
        bool match = filter.isEmpty() || item->text(0).contains(filter, Qt::CaseInsensitive);
        item->setHidden(!match);
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
    if (event->type() == QEvent::KeyPress && (watched == groupList || watched == tilesetList))
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
