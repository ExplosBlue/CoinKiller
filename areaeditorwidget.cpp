#include "areaeditorwidget.h"
#include "game.h"

#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QTabWidget>
#include <QMessageBox>
#include <QGroupBox>

#include "EditorCommands/setvalue.h"

AreaEditorWidget::AreaEditorWidget(Level *level, Game *game, QUndoStack *undoStack) :
    level(level), game(game), undoStack(undoStack) {

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

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    QGroupBox *areaSettingsBox = new QGroupBox();
    areaSettingsBox->setTitle(tr("Area Settings"));
    layout->addWidget(areaSettingsBox);

    QGridLayout *settingsLayout = new QGridLayout();
    areaSettingsBox->setLayout(settingsLayout);

    settingsLayout->addWidget(new QLabel(tr("Time Limit:")), 0, 0, 1, 1, Qt::AlignRight);
    timeLimit = new QSpinBox();
    timeLimit->setRange(0, 999);
    connect(timeLimit, &QSpinBox::valueChanged, this, qOverload<quint16>(&AreaEditorWidget::timeLimitChanged));
    timeLimit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(timeLimit, 0, 1);

    settingsLayout->addWidget(new QLabel(tr("CoinRush Time Limit:")), 1, 0, 1, 1, Qt::AlignRight);
    coinRushtimeLimit = new QSpinBox();
    coinRushtimeLimit->setRange(0, 999);
    connect(coinRushtimeLimit, &QSpinBox::valueChanged, this, qOverload<quint16>(&AreaEditorWidget::coinRushTimeLimitChanged));
    coinRushtimeLimit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(coinRushtimeLimit, 1, 1);

    settingsLayout->addWidget(new QLabel(tr("Level Setting 1:")), 2, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag1 = new QComboBox();
    specialLevelFlag1->addItems(specialLevelFlags1.values());
    connect(specialLevelFlag1, &QComboBox::currentTextChanged, this, &AreaEditorWidget::specialLevelFlag1Changed);
    specialLevelFlag1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(specialLevelFlag1, 2, 1);

    settingsLayout->addWidget(new QLabel(tr("Level Setting 2:")), 3, 0, 1, 1, Qt::AlignRight);
    specialLevelFlag2 = new QComboBox();
    specialLevelFlag2->addItems(specialLevelFlags2.values());
    connect(specialLevelFlag2, &QComboBox::currentTextChanged, this, &AreaEditorWidget::specialLevelFlag2Changed);
    specialLevelFlag2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(specialLevelFlag2, 3, 1);

    settingsLayout->addWidget(new QLabel(tr("Unknown 1:")), 4, 0, 1, 1, Qt::AlignRight);
    unk1Editor = new QSpinBox();
    unk1Editor->setRange(0, 255);
    connect(unk1Editor, &QSpinBox::valueChanged, this, &AreaEditorWidget::unk1Changed);
    unk1Editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(unk1Editor, 4, 1);

    settingsLayout->addWidget(new QLabel(tr("Unknown 2:")), 5, 0, 1, 1, Qt::AlignRight);
    unk2Editor = new QSpinBox();
    unk2Editor->setRange(0, 255);
    connect(unk2Editor, &QSpinBox::valueChanged, this, &AreaEditorWidget::unk2Changed);
    unk2Editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(unk2Editor, 5, 1);

    settingsLayout->addWidget(new QLabel(tr("Level Entrance ID:")), 6, 0, 1, 1, Qt::AlignRight);
    levelEntranceID = new QSpinBox();
    levelEntranceID->setRange(0, 255);
    connect(levelEntranceID, &QSpinBox::valueChanged, this, &AreaEditorWidget::levelEntranceIDChanged);
    levelEntranceID->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsLayout->addWidget(levelEntranceID, 6, 1, 1, 1);

    QGroupBox *eventEditorBox = new QGroupBox();
    eventEditorBox->setTitle(tr("Initial Area Event States"));
    layout->addWidget(eventEditorBox);

    QVBoxLayout *eventBoxLayout = new QVBoxLayout();
    eventEditorBox->setLayout(eventBoxLayout);

    eventEditor = new EventEditorWidget(level, undoStack);
    eventBoxLayout->addWidget(eventEditor);

    updateInfo();
}

void AreaEditorWidget::updateEditor() {
    this->updateInfo();
    eventEditor->updateEditor();
}

void AreaEditorWidget::updateInfo() {
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

void AreaEditorWidget::timeLimitChanged(quint16 timeLimitVal) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint16>(&level->timeLimit, timeLimitVal, tr("Changed Area Time Limit")));
}

void AreaEditorWidget::coinRushTimeLimitChanged(quint16 timeLimitVal) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint16>(&level->coinRushTimeLimit, timeLimitVal, tr("Changed Area CoinRush Time Limit")));
}

void AreaEditorWidget::specialLevelFlag1Changed(QString text) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint16>(&level->specialLevelFlag, specialLevelFlags1.key(text, 0), tr("Changed Area Special Flag 1")));
}

void AreaEditorWidget::specialLevelFlag2Changed(QString text) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint16>(&level->specialLevelFlag2, specialLevelFlags2.key(text, 0), tr("Changed Area Special Flag 2")));
}

void AreaEditorWidget::levelEntranceIDChanged(quint8 id) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint8>(&level->levelEntranceID, id, tr("Changed Area Entrance ID")));
}

void AreaEditorWidget::unk1Changed(quint16 val) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint16>(&level->unk1, val, tr("Changed Area Unknown 1")));
}

void AreaEditorWidget::unk2Changed(quint8 val) {
    if (!handleChanges) return;
    undoStack->push(new EditorCommand::SetValue<quint8>(&level->unk2, val, tr("Changed Area Unknown 2")));
}
