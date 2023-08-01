#include "entranceeditorwidget.h"
#include "unitsconvert.h"
#include "settingsmanager.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "EditorCommands/entrancecommands.h"

EntranceEditorWidget::EntranceEditorWidget(QList<Entrance*> *entrances, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent),
    entrances(entrances),
    undoStack(undoStack) {
    loadEntranceTypes();

    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleIDChanged);

    type = new QComboBox();
    type->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    type->addItems(entranceTypes);
    connect(type, &QComboBox::currentIndexChanged, this, &EntranceEditorWidget::handleTypeChanged);

    destId = new QSpinBox();
    destId->setRange(0, 255);
    connect(destId, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleDestEntrChange);

    destArea = new QSpinBox();
    destArea->setRange(0, 4);
    connect(destArea, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleDestAreaChange);

    camXOffset = new QSpinBox();
    camXOffset->setRange(-32768, 32767);
    connect(camXOffset, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleCamXChange);

    camYOffset = new QSpinBox();
    camYOffset->setRange(-32768, 32767);
    connect(camYOffset, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleCamYChange);

    unk1 = new QSpinBox();
    unk1->setRange(0, 255);
    connect(unk1, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleUnk1Change);

    unk2 = new QSpinBox();
    unk2->setRange(0, 255);
    connect(unk2, &QSpinBox::valueChanged, this, &EntranceEditorWidget::handleUnk2Change);

    enterable = new QCheckBox(tr("Enterable"));
    connect(enterable, &QAbstractButton::toggled, this, &EntranceEditorWidget::handleEnterableChange);

    returnToWM = new QCheckBox(tr("Return to Worldmap"));
    connect(returnToWM, &QAbstractButton::toggled, this, &EntranceEditorWidget::handleReturnToWMChange);

    entrancesList = new QListWidget();
    connect(entrancesList, &QListWidget::itemClicked, this, &EntranceEditorWidget::handleEntranceListIndexChanged);

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(entrancesList);

    edits = new QWidget;
    QGridLayout* subLayout = new QGridLayout();
    subLayout->setContentsMargins(0, 0, 0, 0);
    edits->setLayout(subLayout);
    layout->addWidget(edits);

    subLayout->addWidget(new QLabel(tr("Type:")), 0, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(type, 0, 1, 1, 3);

    subLayout->addWidget(new HorLine(), 1, 0, 1, 4);

    subLayout->addWidget(new QLabel(tr("ID:")), 2, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(id, 2, 1, 1, 1);

    subLayout->addWidget(new QLabel(tr("Dest. ID:")), 3, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(destId, 3, 1, 1, 1);

    subLayout->addWidget(new QLabel(tr("Camera X")), 3, 2, 1, 1, Qt::AlignRight);
    subLayout->addWidget(camXOffset, 3, 3, 1, 1);

    subLayout->addWidget(new QLabel(tr("Dest. Area")), 4, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(destArea, 4, 1, 1, 1);

    subLayout->addWidget(new QLabel(tr("Camera Y")), 4, 2, 1, 1, Qt::AlignRight);
    subLayout->addWidget(camYOffset, 4, 3, 1, 1);

    subLayout->addWidget(new QLabel(tr("Unknown 1")), 5, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(unk1, 5, 1, 1, 1);

    subLayout->addWidget(new QLabel(tr("Unknown 2")), 5, 2, 1, 1, Qt::AlignRight);
    subLayout->addWidget(unk2, 5, 3, 1, 1);

    subLayout->addWidget(new HorLine(), 6, 0, 1, 4);

    subLayout->addWidget(enterable, 7, 0, 1, 2);

    subLayout->addWidget(returnToWM, 7, 2, 1, 2);

    updateList();
    updateInfo();
}

void EntranceEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        QFile file(SettingsManager::getInstance()->getFilePath("entrancetypes.txt"));
        if(!file.open(QIODevice::ReadOnly))
            return;

        QTextStream in(&file);

        for (int i = 0; i < entranceTypes.length(); i++)
        {
            QString name = in.readLine();
            entranceTypes[i] = name;
            type->setItemText(i, name);
        }

        file.close();

        updateEditor();
    }

    QWidget::changeEvent(event);
}

void EntranceEditorWidget::deselect()
{
    editingAnEntrance = false;
    updateInfo();
    entrancesList->clearSelection();
}

void EntranceEditorWidget::select(Entrance* entr)
{
    this->editEntrance = entr;
    editingAnEntrance = true;
    updateInfo();
    handleChanges = false;
    entrancesList->setCurrentRow(entrances->indexOf(entr));
    handleChanges = true;
}

void EntranceEditorWidget::loadEntranceTypes()
{
    QFile file(SettingsManager::getInstance()->getFilePath("entrancetypes.txt"));
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

    while(!in.atEnd())
        entranceTypes.append(in.readLine());

    file.close();
}

void EntranceEditorWidget::updateEditor()
{
    updateList();
    updateInfo();
}

void EntranceEditorWidget::updateList()
{
    QModelIndex index;
    if (entrancesList->selectionModel()->selectedIndexes().size() != 0) index = entrancesList->selectionModel()->selectedIndexes().at(0);
    entrancesList->clear();
    foreach (Entrance* entr, *entrances)
    {
        QString type;
        if (entr->getEntrType() < entranceTypes.size())
            type = entranceTypes.at(entr->getEntrType());
        else
            type = tr("UNKNOWN");

        entrancesList->addItem(tr("Entrance %1: %2 (X: %3, Y: %4)").arg(entr->getid()).arg(type).arg(to16(entr->getx())).arg(to16(entr->gety())));
    }
    entrancesList->setCurrentIndex(index);
}

void EntranceEditorWidget::updateInfo()
{
    if (editingAnEntrance == false)
    {
        edits->setHidden(true);
        return;
    }
    edits->setHidden(false);

    handleChanges = false;
    type->setCurrentIndex(editEntrance->getEntrType());
    id->setValue(editEntrance->getid());
    destId->setValue(editEntrance->getDestEntr());
    destArea->setValue(editEntrance->getDestArea());
    camXOffset->setValue(editEntrance->getCameraX());
    camYOffset->setValue(editEntrance->getCameraY());
    unk1->setValue(editEntrance->getUnk1());
    unk2->setValue(editEntrance->getUnk2());
    enterable->setChecked((editEntrance->getSettings() & 128) >> 7 == false);
    returnToWM->setChecked((editEntrance->getSettings() & 16) >> 4 == 1);
    handleChanges = true;
}

void EntranceEditorWidget::handleEntranceListIndexChanged(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editEntrance = entrances->at(entrancesList->row(item));
    editingAnEntrance = true;
    updateInfo();
    emit selectedEntrChanged(editEntrance);
}

void EntranceEditorWidget::handleTypeChanged(int type)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetType(editEntrance, type));
    updateList();
}

void EntranceEditorWidget::handleIDChanged(int id)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetId(editEntrance, id));
    updateList();
}

void EntranceEditorWidget::handleDestEntrChange(int destEntrId)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetDestEntr(editEntrance, destEntrId));
}

void EntranceEditorWidget::handleDestAreaChange(int destAreaId)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetDestArea(editEntrance, destAreaId));
}

void EntranceEditorWidget::handleCamXChange(int xOffset)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetCamXOffset(editEntrance, static_cast<qint16>(xOffset)));
}

void EntranceEditorWidget::handleCamYChange(int yOffset)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetCamYOffset(editEntrance, static_cast<qint16>(yOffset)));
}

void EntranceEditorWidget::handleUnk1Change(int unk1)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetUnk1(editEntrance, unk1));
}

void EntranceEditorWidget::handleUnk2Change(int unk2)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetUnk2(editEntrance, unk2));
}

void EntranceEditorWidget::handleEnterableChange(bool enterable)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetSettingsBit(editEntrance, !enterable, 7));
}

void EntranceEditorWidget::handleReturnToWMChange(bool returnToWM)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::EntranceCmd::SetSettingsBit(editEntrance, returnToWM, 4));
}
