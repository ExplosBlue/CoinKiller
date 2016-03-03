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

    layout->addWidget(new HorLine(), 1, 0, 1, 2);

    layout->addWidget(new QLabel("Level Setting 1:"), 2, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag1 = new QComboBox();
    specialLevelFlag1->addItems(specialLevelFlags1.values());
    connect(specialLevelFlag1, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleSpecialLevelFlag1Change(QString)));
    layout->addWidget(specialLevelFlag1, 2, 1);

    layout->addWidget(new QLabel("Level Setting 2:"), 3, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag2 = new QComboBox();
    specialLevelFlag2->addItems(specialLevelFlags2.values());
    connect(specialLevelFlag2, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleSpecialLevelFlag2Change(QString)));
    layout->addWidget(specialLevelFlag2, 3, 1);

    toadhouseFlag = new QCheckBox("Toad House Flag");
    connect(toadhouseFlag, SIGNAL(toggled(bool)), this, SLOT(handleToadHouseFlagChanged(bool)));
    layout->addWidget(toadhouseFlag, 4, 1, 1, 1);

    layout->addWidget(new HorLine(), 5, 0, 1, 2);

    layout->addWidget(new QLabel("Tilesets:"), 6, 0, 1, 2);
    QTabWidget* tabWidget = new QTabWidget();
    layout->addWidget(tabWidget, 7, 0, 1, 2);

    QMap<QString, QString> tsNames;
    QFile file(QCoreApplication::applicationDirPath() + "/coinkiller_data/tilesetnames.txt");
    if(!file.open(QIODevice::ReadOnly)) return;
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QStringList nameStrings = in.readLine().split(":");
        tsNames.insert(nameStrings[0], nameStrings[1]);
    }
    file.close();

    for (int i = 0; i < 4; i++)
    {
        TilesetChooser* tsChooser = new TilesetChooser(level, i, game, &tsNames);
        connect(tsChooser, SIGNAL(updateLevelEditor()), this, SLOT(passUpdateLevelView()));
        connect(tsChooser, SIGNAL(relaodTilesetPicker()), this, SLOT(passRelaodTilesetPicker()));
        tabWidget->addTab(tsChooser, QString("%1").arg(i));
    }

    updateInfo();
}

void AreaEditorWidget::updateInfo()
{
    handleChanges = false;
    timeLimit->setValue(level->timeLimit);
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

TilesetChooser::TilesetChooser(Level* level, int tsNbr, Game* game, QMap<QString, QString> *names)
{
    this->level = level;
    this->game = game;
    this->tsNbr = tsNbr;

    QStringList headers;
    headers << "Name" << "File";
    setHeaderLabels(headers);

    setRootIsDecorated(false);

    QStringList tilesetFileNames;
    game->fs->directoryContents("/Unit/", QDir::Files, tilesetFileNames);

    QString prefix;

    if (tsNbr == 0) prefix = "J_";
    else if (tsNbr == 1) prefix = "M_";
    else if (tsNbr == 2) prefix = "S1_";
    else if (tsNbr == 3) prefix = "S2_";

    foreach (QString fileName, tilesetFileNames)
    {
        if (!fileName.startsWith(prefix)) continue;
        fileName.chop(5);

        QTreeWidgetItem *ts = new QTreeWidgetItem(this);

        QString name;

        ts->setText(0, names->value(fileName, fileName.right(fileName.length() - (tsNbr/2 + 2))));
        ts->setText(1, fileName);
    }

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(handleTilesetChange(QTreeWidgetItem*,int)));
}

void TilesetChooser::handleTilesetChange(QTreeWidgetItem* item, int)
{
    level->tilesets[tsNbr] = game->getTileset(item->text(1));
    emit updateLevelEditor();
    emit relaodTilesetPicker();
}
