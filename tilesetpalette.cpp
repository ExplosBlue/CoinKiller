#include "tilesetpalette.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QSpacerItem>

#include "EditorCommands/settileset.h"

TilesetPalette::TilesetPalette(Level* level, EditManager* editManager, Game* game, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent), level(level), editManager(editManager), game(game), undoStack(undoStack) {

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
        QComboBox* tsPicker = new QComboBox();
        tilesetPickers.insert(tsSlot, tsPicker);
        QStandardItemModel* model = game->getTilesetModel(tsSlot, true);
        model->setParent(this);

        model->sort(0, Qt::AscendingOrder);

        for (int pickerIndex = 0; pickerIndex < model->rowCount(); pickerIndex++) {
            // Tileset Name
            QModelIndex tsNameIndex = model->index(pickerIndex, 0);
            QString tilesetName = tsNameIndex.data().toString();

            tsPicker->addItem(tilesetName);

            // File Name
            QModelIndex fileNameIndex = model->index(pickerIndex, 1);
            QString fileName = fileNameIndex.data().toString();

            tsPicker->setItemData(pickerIndex, fileName, Qt::UserRole);

            if (level->tilesets[tsSlot]) {
                QString selectedTs = level->tilesets[tsSlot]->getName();

                if (selectedTs == fileName) {
                    tsPicker->setCurrentIndex(pickerIndex);
                }
            }
        }

        connect(tsPicker, &QComboBox::currentIndexChanged, this, &TilesetPalette::tilesetPickerChosen);

        QWidget* tabPage = new QWidget;
        QGridLayout* tabLayout = new QGridLayout;

        QLabel* tilesetLabel = new QLabel(tr("Tileset:"));
        tilesetLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        tabLayout->addWidget(tilesetLabel, 0, 0, 1, 1);
        tabLayout->addWidget(tsPicker, 0, 1);

        tabPage->setLayout(tabLayout);

        objectLists.insert(tsSlot, new QListView());
        tabLayout->addWidget(objectLists[tsSlot], 1, 0, 1, 2);

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

}

void TilesetPalette::updateEditor()
{
    reloadTilesets();
    updateTilesetPickerIndex();
}

void TilesetPalette::updateTilesetPickerIndex()
{
    for (int tsSlot = 0; tsSlot < 4; tsSlot++) {

        QComboBox *tsPicker = tilesetPickers[tsSlot];
        tsPicker->blockSignals(true);

        if (!level->tilesets[tsSlot]) {
            tsPicker->setCurrentIndex(0);
            tsPicker->blockSignals(false);
            continue;
        }

        QString selectedTs = level->tilesets[tsSlot]->getName();

        for (int pickerIndex = 0; pickerIndex < tsPicker->model()->rowCount(); pickerIndex++) {
            QString fileName = tsPicker->itemData(pickerIndex).toString();

            if (selectedTs == fileName) {
                tsPicker->setCurrentIndex(pickerIndex);
                break;
            }
        }
        tsPicker->blockSignals(false);
    }
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
        objectLists[tilesetNbr]->setModel(new QStandardItemModel(this));
        return;
    }
    objectLists[tilesetNbr]->setEnabled(true);

    QStandardItemModel* objectsModel = new QStandardItemModel(this);

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

void TilesetPalette::tilesetPickerChosen(int index)
{
    QComboBox* tsNameCombobox = qobject_cast<QComboBox*>(sender());

    QString tsName = tsNameCombobox->itemData(index).toString();
    int tsId = tabWidget->currentIndex();

    undoStack->push(new EditorCommand::SetTileset(level->tilesets[tsId], game, tsName));

    emit updateLevelView();
    emit editMade();
    reloadTilesets();
}
