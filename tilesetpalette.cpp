#include "tilesetpalette.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QSpacerItem>

TilesetPalette::TilesetPalette(Level* level, EditManager* editManager, Game* game)
{
    this->level = level;
    this->game = game;
    this->editManager = editManager;

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
    for (int i = 0; i < 4; i++)
    {
        QComboBox* tsNameCombobox = new QComboBox();
        QStandardItemModel* model = game->getTilesetModel(i, true);

        model->sort(0, Qt::AscendingOrder);

        for (int j = 0; j < model->rowCount(); j++)
        {
            // Tileset Name
            QModelIndex tsNameIndex = model->index(j, 0);
            QString tilesetName = tsNameIndex.data().toString();

            tsNameCombobox->addItem(tilesetName);

            // File Name
            QModelIndex fileNameIndex = model->index(j, 1);
            QString fileName = fileNameIndex.data().toString();

            tsNameCombobox->setItemData(j, fileName, Qt::UserRole);

            if (level->tilesets[i])
            {
                QString selectedTs = level->tilesets[i]->getName();

                if (selectedTs == fileName)
                    tsNameCombobox->setCurrentIndex(j);
            }
        }

        connect(tsNameCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTilesetChange(int)));

        QWidget* tabPage = new QWidget;
        QGridLayout* tabLayout = new QGridLayout;

        QLabel* tilesetLabel = new QLabel(tr("Tileset:"));
        tilesetLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        tabLayout->addWidget(tilesetLabel, 0, 0, 1, 1);
        tabLayout->addWidget(tsNameCombobox, 0, 1);

        tabPage->setLayout(tabLayout);

        objectLists[i] = new QListView();
        tabLayout->addWidget(objectLists[i], 1, 0, 1, 2);

        QString title;
        if (i == 0) title = tr("Standard");
        else if (i == 1) title = tr("Stage");
        else if (i == 2) title = tr("Background");
        else if (i == 3) title = tr("Interactive");

        tabWidget->addTab(tabPage, title);

        objectLists[i]->setFlow(QListView::LeftToRight);
        objectLists[i]->setLayoutMode(QListView::SinglePass);
        objectLists[i]->setMovement(QListView::Static);
        objectLists[i]->setResizeMode(QListView::Adjust);
        objectLists[i]->setWrapping(true);
        objectLists[i]->setIconSize(QSize(400,400));
        objectLists[i]->setVerticalScrollMode(QListView::ScrollPerPixel);
        objectLists[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);
        loadTileset(i);
    }

    connect(objectLists[0], SIGNAL(clicked(QModelIndex)), this, SLOT(on_objectsListView0_clicked(QModelIndex)));
    connect(objectLists[1], SIGNAL(clicked(QModelIndex)), this, SLOT(on_objectsListView1_clicked(QModelIndex)));
    connect(objectLists[2], SIGNAL(clicked(QModelIndex)), this, SLOT(on_objectsListView2_clicked(QModelIndex)));
    connect(objectLists[3], SIGNAL(clicked(QModelIndex)), this, SLOT(on_objectsListView3_clicked(QModelIndex)));

//    connect(objectLists[0], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView0_clicked(QModelIndex)));
//    connect(objectLists[1], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView1_clicked(QModelIndex)));
//    connect(objectLists[2], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView2_clicked(QModelIndex)));
//    connect(objectLists[3], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView3_clicked(QModelIndex)));
    connect(layer1RadioBtn, SIGNAL(toggled(bool)), SLOT(on_layerRadioButton_toggled(bool)));
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
        objectLists[tilesetNbr]->setModel(new QStandardItemModel());
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

void TilesetPalette::on_objectsListView0_clicked(const QModelIndex &index)
{
    updatePalettes(0);
    editManager->setDrawType(ObjectType::BGDATOBJECT);
    editManager->setObject(index.row(), 0);
}

void TilesetPalette::on_objectsListView1_clicked(const QModelIndex &index)
{
    updatePalettes(1);
    editManager->setDrawType(ObjectType::BGDATOBJECT);
    editManager->setObject(index.row(), 1);
}

void TilesetPalette::on_objectsListView2_clicked(const QModelIndex &index)
{
    updatePalettes(2);
    editManager->setDrawType(ObjectType::BGDATOBJECT);
    editManager->setObject(index.row(), 2);
}

void TilesetPalette::on_objectsListView3_clicked(const QModelIndex &index)
{
    updatePalettes(3);
    editManager->setDrawType(ObjectType::BGDATOBJECT);
    editManager->setObject(index.row(), 3);
}

void TilesetPalette::updatePalettes(int actualPal)
{
    for (int i = 0; i < 4; i++)
    {
        if (i == actualPal) continue;
        objectLists[i]->clearSelection();
    }
}

void TilesetPalette::on_layerRadioButton_toggled(bool checked)
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

void TilesetPalette::handleTilesetChange(int index)
{
    QComboBox* tsNameCombobox = qobject_cast<QComboBox*>(sender());

    QString tsName = tsNameCombobox->itemData(index).toString();
    int tsId = tabWidget->currentIndex();

    delete level->tilesets[tsId];

    if (tsName == "")
        level->tilesets[tsId] = NULL;
    else
        level->tilesets[tsId] = game->getTileset(tsName);

    emit updateLevelView();
    emit editMade();
    reloadTilesets();
}
