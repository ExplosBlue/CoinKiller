#include "areaeditorwidget.h"
#include "game.h"

#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QTabWidget>
#include <QMessageBox>

AreaEditorWidget::AreaEditorWidget(Level* level, Game *game)
{
    this->level = level;
    this->game = game;

    specialLevelFlags1.insert(0, "Normal");
    specialLevelFlags1.insert(2, "Powerup / Star Toad House");
    specialLevelFlags1.insert(3, "1UP Toad House");
    specialLevelFlags1.insert(4, "Rainbow Course");
    specialLevelFlags1.insert(5, "Credits");
    specialLevelFlags1.insert(6, "Cannon Stage");

    specialLevelFlags2.insert(0, "Normal");
    specialLevelFlags2.insert(1, "Ghost House");
    specialLevelFlags2.insert(7, "Reznor Battle");

    QGridLayout* layout = new QGridLayout();
    setLayout(layout);

    layout->addWidget(new QLabel("Time Limit:"), 0, 0, 1, 1, Qt::AlignRight);
    timeLimit = new QSpinBox();
    timeLimit->setRange(0, 999);
    connect(timeLimit, SIGNAL(valueChanged(int)), this, SLOT(handleTimeLimitChange(int)));
    layout->addWidget(timeLimit, 0, 1);

    layout->addWidget(new QLabel("Coin Rush Time Limit:"), 1, 0, 1, 1, Qt::AlignRight);
    coinRushtimeLimit = new QSpinBox();
    coinRushtimeLimit->setRange(0, 999);
    connect(coinRushtimeLimit, SIGNAL(valueChanged(int)), this, SLOT(handleCoinRushTimeLimitChange(int)));
    layout->addWidget(coinRushtimeLimit, 1, 1);

    layout->addWidget(new HorLine(), 2, 0, 1, 2);

    layout->addWidget(new QLabel("Level Setting 1:"), 3, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag1 = new QComboBox();
    specialLevelFlag1->addItems(specialLevelFlags1.values());
    connect(specialLevelFlag1, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleSpecialLevelFlag1Change(QString)));
    layout->addWidget(specialLevelFlag1, 3, 1);

    layout->addWidget(new QLabel("Level Setting 2:"), 4, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag2 = new QComboBox();
    specialLevelFlag2->addItems(specialLevelFlags2.values());
    connect(specialLevelFlag2, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleSpecialLevelFlag2Change(QString)));
    layout->addWidget(specialLevelFlag2, 4, 1);

    toadhouseFlag = new QCheckBox("Toad House Flag");
    connect(toadhouseFlag, SIGNAL(toggled(bool)), this, SLOT(handleToadHouseFlagChanged(bool)));
    layout->addWidget(toadhouseFlag, 5, 1, 1, 1);

    layout->addWidget(new HorLine(), 6, 0, 1, 2);

    tsChooser = new TilesetChooser(level, game);
    connect(tsChooser, SIGNAL(updateLevelEditor()), this, SLOT(passUpdateLevelView()));
    connect(tsChooser, SIGNAL(relaodTilesetPicker()), this, SLOT(passRelaodTilesetPicker()));
    layout->addWidget(tsChooser, 8, 0, 1, 2);

    updateInfo();
}

void AreaEditorWidget::updateInfo()
{
    handleChanges = false;
    timeLimit->setValue(level->timeLimit);
    coinRushtimeLimit->setValue(level->coinRushTimeLimit);
    toadhouseFlag->setChecked(level->toadHouseFlag == 2);
    specialLevelFlag1->setCurrentText(specialLevelFlags1.value(level->specialLevelFlag, "Unknown"));
    specialLevelFlag2->setCurrentText(specialLevelFlags2.value(level->specialLevelFlag2, "Unknown"));
    handleChanges = true;
}

void AreaEditorWidget::handleTimeLimitChange(int timeLimitVal)
{
    if (!handleChanges) return;
    level->timeLimit = timeLimitVal;
}

void AreaEditorWidget::handleCoinRushTimeLimitChange(int timeLimitVal)
{
    if (!handleChanges) return;
    level->coinRushTimeLimit = timeLimitVal;
}

void AreaEditorWidget::handleSpecialLevelFlag1Change(QString text)
{
    if (!handleChanges) return;
    level->specialLevelFlag = specialLevelFlags1.key(text, 0);
}

void AreaEditorWidget::handleSpecialLevelFlag2Change(QString text)
{
    if (!handleChanges) return;
    level->specialLevelFlag2 = specialLevelFlags2.key(text, 0);
}

void AreaEditorWidget::handleToadHouseFlagChanged(bool flag)
{
    if (!handleChanges) return;
    level->toadHouseFlag = flag ? 2 : 0;

}

TilesetChooser::TilesetChooser(Level *level, Game *game)
{
    this->game = game;
    this->level = level;

    for (int i=0; i<4; i++)
    {
        QTreeView* tab = new QTreeView(this);
        tab->setRootIsDecorated(false);
        tab->setSelectionBehavior (QAbstractItemView::SelectRows);
        tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tab->setModel(game->getTilesetModel(i, true));
        tab->model()->setObjectName(QString("%1").arg(i));
        tab->setColumnWidth(0, 150);

        QString title;
        if (i == 0) title = "Standard";
        else if (i == 1) title = "Stage";
        else if (i == 2) title = "Background";
        else if (i == 3) title = "Interactive";

        if (!level->tilesets[i])
            tab->selectionModel()->setCurrentIndex(tab->model()->index(0,0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        else
        {
            QString tsName = level->tilesets[i]->getName();

            for (int j=1; j<tab->model()->rowCount(); j++)
            {
                QModelIndex index = tab->model()->index(j, 1);
                QString indexName = index.data().toString();

                if (tsName == indexName)
                {
                    tab->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
                    break;
                }
            }
        }

        connect(tab, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTilesetChange(QModelIndex)));

        this->addTab(tab, title);
    }
}

void TilesetChooser::handleTilesetChange(QModelIndex index)
{
    QString tsName = index.model()->data(index.model()->index(index.row(), 1)).toString();
    int tsId = index.model()->objectName().toInt();

    delete level->tilesets[tsId];

    if (tsName == "")
        level->tilesets[tsId] = NULL;
    else
        level->tilesets[tsId] = game->getTileset(tsName);

    emit updateLevelEditor();
    emit relaodTilesetPicker();
}
