#include "tilesetpalette.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QSpacerItem>

TilesetPalette::TilesetPalette(Level* level, ObjectsEditonMode* objEditionMode)
{
    this->level = level;
    this->objEditionMode = objEditionMode;

    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);

    QHBoxLayout* topLayout = new QHBoxLayout();
    layout->addLayout(topLayout);

    QLabel* label = new QLabel("Paint on:");
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    topLayout->addWidget(label);

    QRadioButton* layer1RadioBtn = new QRadioButton("Layer 1");
    layer1RadioBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layer1RadioBtn->setChecked(true);
    topLayout->addWidget(layer1RadioBtn);

    QRadioButton* layer2RadioBtn = new QRadioButton("Layer 2");
    layer2RadioBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    topLayout->addWidget(layer2RadioBtn);

    topLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Preferred));

    tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);

    for (int i = 0; i < 4; i++)
    {
        objectLists[i] = new QListView();
        tabWidget->addTab(objectLists[i], QString("%1").arg(i));
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
    connect(objectLists[0], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView0_clicked(QModelIndex)));
    connect(objectLists[1], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView1_clicked(QModelIndex)));
    connect(objectLists[2], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView2_clicked(QModelIndex)));
    connect(objectLists[3], SIGNAL(entered(QModelIndex)), this, SLOT(on_objectsListView3_clicked(QModelIndex)));
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
        level->tilesets[tilesetNbr]->drawObject(p, tileGrid, i, 0, 0, obj->width, obj->height, 1);
        p.end();
        QStandardItem *objItem = new QStandardItem();
        objItem->setIcon(QIcon(objPixmap));
        objItem->setToolTip(QString("Object: %1").arg(i));
        objectsModel->appendRow(objItem);
    }

    objectLists[tilesetNbr]->setModel(objectsModel);
}

void TilesetPalette::on_objectsListView0_clicked(const QModelIndex &index)
{
    updatePalettes(0);
    objEditionMode->setDrawType(0);
    objEditionMode->setObject(index.row(), 0);
}

void TilesetPalette::on_objectsListView1_clicked(const QModelIndex &index)
{
    updatePalettes(1);
    objEditionMode->setDrawType(0);
    objEditionMode->setObject(index.row(), 1);
}

void TilesetPalette::on_objectsListView2_clicked(const QModelIndex &index)
{
    updatePalettes(2);
    objEditionMode->setDrawType(0);
    objEditionMode->setObject(index.row(), 2);
}

void TilesetPalette::on_objectsListView3_clicked(const QModelIndex &index)
{
    updatePalettes(3);
    objEditionMode->setDrawType(0);
    objEditionMode->setObject(index.row(), 3);
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
    objEditionMode->setLayer(!checked);
}

void TilesetPalette::select(BgdatObject *obj)
{
    int tsid = (obj->getid() >> 12) & 0x3;
    tabWidget->setCurrentIndex(tsid);
    objectLists[tsid]->setCurrentIndex(objectLists[tsid]->model()->index(obj->getid()&0x0FFF, 0));

    updatePalettes(tsid);
    objEditionMode->setDrawType(0);
    objEditionMode->setObject(obj->getid()&0x0FFF, (obj->getid() >> 12) & 0x3);
}
