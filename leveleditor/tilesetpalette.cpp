#include "tilesetpalette.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QStandardItemModel>
#include <QSpacerItem>
#include <QStylePainter>
#include <QStyleOptionComboBox>

#include <algorithm>

#include "tilesetselectdialog.h"
#include "commands/levelcommands.h"

namespace {

class TilesetComboBox : public QComboBox
{
public:
    using QComboBox::QComboBox;

    QString displaySuffix;

protected:
    void paintEvent(QPaintEvent*) override
    {
        QStylePainter painter(this);
        QStyleOptionComboBox opt;
        initStyleOption(&opt);
        painter.drawComplexControl(QStyle::CC_ComboBox, opt);
        opt.currentText += displaySuffix;
        painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
    }
};

}

TilesetPalette::TilesetPalette(Level* level, EditManager* editManager, Game* game, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent),
    level(level),
    editManager(editManager),
    game(game),
    undoStack(undoStack) {

    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);

    QHBoxLayout* topLayout = new QHBoxLayout();
    layout->addLayout(topLayout);

    QLabel* label = new QLabel(tr("Paint on:"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    topLayout->addWidget(label);

    QRadioButton* layer1RadioBtn = new QRadioButton(tr("Layer 1"));
    layer1RadioBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layer1RadioBtn->setChecked(true);
    topLayout->addWidget(layer1RadioBtn);

    QRadioButton* layer2RadioBtn = new QRadioButton(tr("Layer 2"));
    layer2RadioBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    topLayout->addWidget(layer2RadioBtn);

    topLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);

    // Setup Tileset Selector
    for (int tsSlot = 0; tsSlot < 4; tsSlot++) {
        QWidget* tabPage = new QWidget;
        QGridLayout* tabLayout = new QGridLayout;

        QLabel* tilesetLabel = new QLabel(tr("Tileset:"));
        tilesetLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        QComboBox* tsPicker = new TilesetComboBox();
        tsPicker->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        tilesetPickers.insert(tsSlot, tsPicker);
        connect(tsPicker, &QComboBox::currentIndexChanged, this, &TilesetPalette::tilesetComboChosen);

        QPushButton* changeButton = new QPushButton(tr("Change..."));
        changeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        tilesetButtons.insert(tsSlot, changeButton);
        connect(changeButton, &QPushButton::clicked, this, &TilesetPalette::changeTileset);

        tabLayout->addWidget(tilesetLabel, 0, 0, 1, 1);
        tabLayout->addWidget(tsPicker, 0, 1);
        tabLayout->addWidget(changeButton, 0, 2);

        tabPage->setLayout(tabLayout);

        objectLists.insert(tsSlot, new QListView());
        tabLayout->addWidget(objectLists[tsSlot], 1, 0, 1, 3);

        QString title;
        if (tsSlot == 0) title = tr("Standard");
        else if (tsSlot == 1) title = tr("Stage");
        else if (tsSlot == 2) title = tr("Background");
        else if (tsSlot == 3) title = tr("Interactive");

        tabWidget->addTab(tabPage, title);

        objectLists[tsSlot]->setFlow(QListView::LeftToRight);
        objectLists[tsSlot]->setLayoutMode(QListView::SinglePass);
        objectLists[tsSlot]->setMovement(QListView::Static);
        objectLists[tsSlot]->setResizeMode(QListView::Adjust);
        objectLists[tsSlot]->setWrapping(true);
        objectLists[tsSlot]->setIconSize(QSize(400, 400));
        objectLists[tsSlot]->setVerticalScrollMode(QListView::ScrollPerPixel);
        objectLists[tsSlot]->setEditTriggers(QAbstractItemView::NoEditTriggers);
        loadTileset(tsSlot);

        connect(objectLists[tsSlot], &QListView::clicked, this, &TilesetPalette::objectsListViewClicked);
        connect(objectLists[tsSlot], &QListView::entered, this, &TilesetPalette::objectsListViewClicked);
    }

    connect(layer1RadioBtn, &QRadioButton::toggled, this, &TilesetPalette::layerToggled);

    updateTilesetPickers();
}

void TilesetPalette::updateEditor()
{
    reloadTilesets();
    updateTilesetPickers();
}

void TilesetPalette::updateTilesetPickers()
{
    for (int tsSlot = 0; tsSlot < 4; tsSlot++)
        populateTilesetCombo(tsSlot);
}

void TilesetPalette::populateTilesetCombo(int tsSlot)
{
    QComboBox* tsPicker = tilesetPickers[tsSlot];
    tsPicker->blockSignals(true);
    tsPicker->clear();

    QString current = level->tilesets[tsSlot] ? level->tilesets[tsSlot]->getName() : QString();
    QString group = current.isEmpty() ? QString("Retail") : game->getTilesetList()->groupName(current);

    if (group.isEmpty())
        group = "Retail";

    QList<TilesetEntry> entries;
    for (const TilesetGroup& tsGroup : game->getTilesetList()->groups())
    {
        if (tsGroup.name != group)
            continue;

        for (const TilesetEntry& entry : tsGroup.tilesets)
            if (TilesetList::matchesSlot(entry.fileName, tsSlot))
                entries.append(entry);
    }

    std::sort(entries.begin(), entries.end(), [](const TilesetEntry& a, const TilesetEntry& b) {
        return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
    });

    tsPicker->addItem(tr("<none>"), QString());
    for (const TilesetEntry& entry : entries)
        tsPicker->addItem(entry.name, entry.fileName);

    int index = 0;
    if (!current.isEmpty())
    {
        for (int i = 1; i < tsPicker->count(); i++)
        {
            if (tsPicker->itemData(i).toString() == current)
            {
                index = i;
                break;
            }
        }
    }

    tsPicker->setCurrentIndex(index);

    TilesetComboBox* tilesetCombo = static_cast<TilesetComboBox*>(tsPicker);
    tilesetCombo->displaySuffix = (index > 0) ? QString(" (%1)").arg(group) : QString();

    tsPicker->blockSignals(false);
    tsPicker->update();
}

void TilesetPalette::reloadTilesets()
{
    for (int i = 0; i < 4; i++) loadTileset(i);
}

void TilesetPalette::loadTileset(int tilesetNbr)
{
    if (!level->tilesets[tilesetNbr])
    {
        objectLists[tilesetNbr]->setEnabled(false);

        QStandardItemModel* objectsModel = dynamic_cast<QStandardItemModel*>(objectLists[tilesetNbr]->model());
        if (objectsModel == nullptr)
        {
           objectsModel = new QStandardItemModel(this);
        }
        objectsModel->clear();
        objectLists[tilesetNbr]->setModel(objectsModel);
        return;
    }

    objectLists[tilesetNbr]->setEnabled(true);

    QStandardItemModel* objectsModel = dynamic_cast<QStandardItemModel*>(objectLists[tilesetNbr]->model());
    if (objectsModel == nullptr)
    {
        objectsModel = new QStandardItemModel(this);
    }
    objectsModel->clear();

    for (int i = 0; i < level->tilesets[tilesetNbr]->getNumObjects(); i++)
    {
        ObjectDef* obj = level->tilesets[tilesetNbr]->getObjectDef(i);
        QPixmap objPixmap(obj->width*20, obj->height*20);
        objPixmap.fill(Qt::transparent);

        QPainter p(&objPixmap);
        TileGrid tileGrid;
        tileGrid.clear();
        tileGrid[0xFFFFFFFF] = 1;
        level->tilesets[tilesetNbr]->Render2DTiles(true);
        level->tilesets[tilesetNbr]->Render3DOverlay(true);
        level->tilesets[tilesetNbr]->drawObject(p, tileGrid, i, 0, 0, obj->width, obj->height, 1);
        p.end();
        QStandardItem *objItem = new QStandardItem();
        objItem->setIcon(QIcon(objPixmap));
        objItem->setToolTip(tr("Object: %1").arg(i));
        objectsModel->appendRow(objItem);
    }

    objectLists[tilesetNbr]->setModel(objectsModel);
}

void TilesetPalette::objectsListViewClicked(const QModelIndex &index)
{
    QListView *listView  = qobject_cast<QListView*>(QObject::sender());
    int tilsetId = objectLists.indexOf(listView);

    updatePalettes(tilsetId);
    editManager->setDrawType(ObjectType::BGDATOBJECT);
    editManager->setObject(index.row(), tilsetId);
}

void TilesetPalette::updatePalettes(int actualPal)
{
    for (int i = 0; i < 4; i++)
    {
        if (i == actualPal) continue;
        objectLists[i]->clearSelection();
    }
}

void TilesetPalette::layerToggled(bool checked)
{
    editManager->setLayer(!checked);
}

void TilesetPalette::select(BgdatObject *obj)
{
    int tsid = obj->getTsID();
    tabWidget->setCurrentIndex(tsid);
    objectLists[tsid]->setCurrentIndex(objectLists[tsid]->model()->index(obj->getObjID(), 0));

    updatePalettes(tsid);
    editManager->setDrawType(ObjectType::BGDATOBJECT);
    editManager->setObject(obj->getObjID(), obj->getTsID());
}

void TilesetPalette::changeTileset()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    int tsId = tilesetButtons.indexOf(button);

    if (tsId < 0)
        return;

    QString current = level->tilesets[tsId] ? level->tilesets[tsId]->getName() : QString();
    int currentObjectCount = level->tilesets[tsId] ? level->tilesets[tsId]->getNumObjects() : -1;

    TilesetSelectionDialog dialog(game, tsId, current, currentObjectCount, this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString tsName = dialog.selectedTileset();

    if (tsName == current)
        return;

    Tileset* newTs = nullptr;

    if (!tsName.isEmpty())
    {
        try
        {
            newTs = game->getTileset(tsName);
        }
        catch (...)
        {
            return;
        }
    }

    undoStack->push(new Commands::LevelCmd::SetTileset(level, tsId, newTs));

    reloadTilesets();
    updateTilesetPickers();
}

void TilesetPalette::tilesetComboChosen(int index)
{
    QComboBox* tsPicker = qobject_cast<QComboBox*>(sender());
    int tsId = tilesetPickers.indexOf(tsPicker);

    if (tsId < 0 || index < 0)
        return;

    QString tsName = tsPicker->itemData(index).toString();
    QString current = level->tilesets[tsId] ? level->tilesets[tsId]->getName() : QString();

    if (tsName == current)
        return;

    Tileset* newTs = nullptr;

    if (!tsName.isEmpty())
    {
        try
        {
            newTs = game->getTileset(tsName);
        }
        catch (...)
        {
            updateTilesetPickers();
            return;
        }
    }

    undoStack->push(new Commands::LevelCmd::SetTileset(level, tsId, newTs));

    reloadTilesets();
    updateTilesetPickers();
}
