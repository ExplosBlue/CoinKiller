#include "areaeditorwidget.h"
#include "game.h"

#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QTabWidget>
#include <QMessageBox>
#include <QGroupBox>

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

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    QGroupBox* areaSettingsBox = new QGroupBox();
    areaSettingsBox->setTitle(tr("Area Settings"));
    layout->addWidget(areaSettingsBox);

    QGridLayout* settingsLayout = new QGridLayout();
    areaSettingsBox->setLayout(settingsLayout);

    settingsLayout->addWidget(new QLabel(tr("Time Limit:")), 0, 0, 1, 1, Qt::AlignRight);
    timeLimit = new QSpinBox();
    timeLimit->setRange(0, 999);
    connect(timeLimit, SIGNAL(valueChanged(int)), this, SLOT(handleTimeLimitChange(int)));
    timeLimit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // All Controls at maximum possible width
    settingsLayout->addWidget(timeLimit, 0, 1);

    settingsLayout->addWidget(new QLabel(tr("CoinRush Time Limit:")), 1, 0, 1, 1, Qt::AlignRight);
    coinRushtimeLimit = new QSpinBox();
    coinRushtimeLimit->setRange(0, 999);
    connect(coinRushtimeLimit, SIGNAL(valueChanged(int)), this, SLOT(handleCoinRushTimeLimitChange(int)));
    coinRushtimeLimit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(coinRushtimeLimit, 1, 1);

    settingsLayout->addWidget(new QLabel(tr("Level Setting 1:")), 2, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag1 = new QComboBox();
    specialLevelFlag1->addItems(specialLevelFlags1.values());
    connect(specialLevelFlag1, SIGNAL(currentTextChanged(QString)), this, SLOT(handleSpecialLevelFlag1Change(QString)));
    specialLevelFlag1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(specialLevelFlag1, 2, 1);

    settingsLayout->addWidget(new QLabel(tr("Level Setting 2:")), 3, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag2 = new QComboBox();
    specialLevelFlag2->addItems(specialLevelFlags2.values());
    connect(specialLevelFlag2, SIGNAL(currentTextChanged(QString)), this, SLOT(handleSpecialLevelFlag2Change(QString)));
    specialLevelFlag2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(specialLevelFlag2, 3, 1);

    settingsLayout->addWidget(new QLabel(tr("Unknown 1:")), 4, 0, 1, 1, Qt::AlignRight);
    unk1Editor = new QSpinBox();
    unk1Editor->setRange(0, 255);
    connect(unk1Editor, SIGNAL(valueChanged(int)), this, SLOT(handleUnk1Change(int)));
    unk1Editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(unk1Editor, 4, 1);

    settingsLayout->addWidget(new QLabel(tr("Unknown 2:")), 5, 0, 1, 1, Qt::AlignRight);
    unk2Editor = new QSpinBox();
    unk2Editor->setRange(0, 255);
    connect(unk2Editor, SIGNAL(valueChanged(int)), this, SLOT(handleUnk2Change(int)));
    unk2Editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(unk2Editor, 5, 1);

    settingsLayout->addWidget(new QLabel(tr("Level Entrance ID:")), 6, 0, 1, 1, Qt::AlignRight);
    levelEntranceID = new QSpinBox();
    levelEntranceID->setRange(0, 255);
    connect(levelEntranceID, SIGNAL(valueChanged(int)), this, SLOT(handlelevelEntranceIDChanged(int)));
    levelEntranceID->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(levelEntranceID, 6, 1, 1, 1);

    QGroupBox* eventEditorBox = new QGroupBox();
    eventEditorBox->setTitle(tr("Initial Area Event States"));
    layout->addWidget(eventEditorBox);

    QVBoxLayout* eventBoxLayout = new QVBoxLayout();
    eventEditorBox->setLayout(eventBoxLayout);

    eventEditor = new EventEditorWidget(level);
    connect(eventEditor, SIGNAL(editMade()), this, SLOT(passEditMade()));
    eventBoxLayout->addWidget(eventEditor);

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
