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

    specialLevelFlags1.insert(0, tr("Normal"));
    specialLevelFlags1.insert(2, tr("Powerup Toad House"));
    specialLevelFlags1.insert(3, tr("1-Up Toad House"));
    specialLevelFlags1.insert(4, tr("Rainbow Level"));
    specialLevelFlags1.insert(5, tr("Credits"));
    specialLevelFlags1.insert(6, tr("Cannon Level"));
    specialLevelFlags1.insert(7, tr("Frozen Timer"));

    specialLevelFlags2.insert(0, tr("Normal"));
    specialLevelFlags2.insert(1, tr("Ghost House"));
    specialLevelFlags2.insert(7, tr("Reznor Battle"));

    QGridLayout* layout = new QGridLayout();
    setLayout(layout);

    layout->addWidget(new QLabel(tr("Time Limit:")), 0, 0, 1, 1, Qt::AlignRight);
    timeLimit = new QSpinBox();
    timeLimit->setRange(0, 999);
    connect(timeLimit, SIGNAL(valueChanged(int)), this, SLOT(handleTimeLimitChange(int)));
    timeLimit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width
    layout->addWidget(timeLimit, 0, 1);

    layout->addWidget(new QLabel(tr("CoinRush Time Limit:")), 1, 0, 1, 1, Qt::AlignRight);
    coinRushtimeLimit = new QSpinBox();
    coinRushtimeLimit->setRange(0, 999);
    connect(coinRushtimeLimit, SIGNAL(valueChanged(int)), this, SLOT(handleCoinRushTimeLimitChange(int)));
    layout->addWidget(coinRushtimeLimit, 1, 1);

    layout->addWidget(new HorLine(), 2, 0, 1, 2);

    layout->addWidget(new QLabel(tr("Level Setting 1:")), 3, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag1 = new QComboBox();
    specialLevelFlag1->addItems(specialLevelFlags1.values());
    connect(specialLevelFlag1, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleSpecialLevelFlag1Change(QString)));
    layout->addWidget(specialLevelFlag1, 3, 1);

    layout->addWidget(new QLabel(tr("Level Setting 2:")), 4, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag2 = new QComboBox();
    specialLevelFlag2->addItems(specialLevelFlags2.values());
    connect(specialLevelFlag2, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleSpecialLevelFlag2Change(QString)));
    layout->addWidget(specialLevelFlag2, 4, 1);

    layout->addWidget(new QLabel(tr("Level Entrance ID:")), 5, 0, 1, 1, Qt::AlignRight);
    levelEntranceID = new QSpinBox();
    levelEntranceID->setRange(0, 255);
    connect(levelEntranceID, SIGNAL(valueChanged(int)), this, SLOT(handlelevelEntranceIDChanged(int)));
    layout->addWidget(levelEntranceID, 5, 1);

    layout->addWidget(new QLabel(tr("Unknown 1:")), 6, 0, 1, 1, Qt::AlignRight);
    unk1Editor = new QSpinBox();
    unk1Editor->setRange(0, 255);
    connect(unk1Editor, SIGNAL(valueChanged(int)), this, SLOT(handleUnk1Change(int)));
    layout->addWidget(unk1Editor, 6, 1);

    layout->addWidget(new QLabel(tr("Unknown 2:")), 7, 0, 1, 1, Qt::AlignRight);
    unk2Editor = new QSpinBox();
    unk2Editor->setRange(0, 255);
    connect(unk2Editor, SIGNAL(valueChanged(int)), this, SLOT(handleUnk2Change(int)));
    layout->addWidget(unk2Editor, 7, 1);

    layout->addWidget(new HorLine(), 8, 0, 1, 2);

    tsChooser = new TilesetChooser(level, game);
    connect(tsChooser, SIGNAL(updateLevelEditor()), this, SLOT(passUpdateLevelView()));
    connect(tsChooser, SIGNAL(relaodTilesetPicker()), this, SLOT(passRelaodTilesetPicker()));
    connect(tsChooser, SIGNAL(tilesetEditMade()), this, SLOT(passTilesetEditMade()));
    layout->addWidget(tsChooser, 9, 0, 1, 2);

    updateInfo();
}

void AreaEditorWidget::updateInfo()
{
    handleChanges = false;
    timeLimit->setValue(level->timeLimit);
    coinRushtimeLimit->setValue(level->coinRushTimeLimit);
    levelEntranceID->setValue(level->levelEntranceID);
    specialLevelFlag1->setCurrentText(specialLevelFlags1.value(level->specialLevelFlag, tr("Unknown")));
    specialLevelFlag2->setCurrentText(specialLevelFlags2.value(level->specialLevelFlag2, tr("Unknown")));
    unk1Editor->setValue(level->unk1);
    unk2Editor->setValue(level->unk2);
    handleChanges = true;
}

void AreaEditorWidget::handleTimeLimitChange(int timeLimitVal)
{
    if (!handleChanges) return;
    level->timeLimit = timeLimitVal;
    emit editMade();
}

void AreaEditorWidget::handleCoinRushTimeLimitChange(int timeLimitVal)
{
    if (!handleChanges) return;
    level->coinRushTimeLimit = timeLimitVal;
    emit editMade();
}

void AreaEditorWidget::handleSpecialLevelFlag1Change(QString text)
{
    if (!handleChanges) return;
    level->specialLevelFlag = specialLevelFlags1.key(text, 0);
    emit editMade();
}

void AreaEditorWidget::handleSpecialLevelFlag2Change(QString text)
{
    if (!handleChanges) return;
    level->specialLevelFlag2 = specialLevelFlags2.key(text, 0);
    emit editMade();
}

void AreaEditorWidget::handlelevelEntranceIDChanged(int id)
{
    if (!handleChanges) return;
    level->levelEntranceID = id;
    emit editMade();
}

void AreaEditorWidget::handleUnk1Change(int unk)
{
    if (!handleChanges) return;
    level->unk1 = unk;
    emit editMade();
}

void AreaEditorWidget::handleUnk2Change(int unk)
{
    if (!handleChanges) return;
    level->unk2 = unk;
    emit editMade();
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
        if (i == 0) title = tr("Standard");
        else if (i == 1) title = tr("Stage");
        else if (i == 2) title = tr("Background");
        else if (i == 3) title = tr("Interactive");

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
        tab->setSortingEnabled(true);

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
    emit tilesetEditMade();
}
