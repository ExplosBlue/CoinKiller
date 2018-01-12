#include "entranceeditorwidget.h"
#include "unitsconvert.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

EntranceEditorWidget::EntranceEditorWidget(QList<Entrance*> *entrances)
{
    this->entrances = entrances;
    loadEntranceTypes();

    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChanged(int)));

    type = new QComboBox();
    type->addItems(entranceTypes);
    connect(type, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTypeChanged(int)));

    destId = new QSpinBox();
    destId->setRange(0, 255);
    connect(destId, SIGNAL(valueChanged(int)), this, SLOT(handleDestEntrChange(int)));

    destArea = new QSpinBox();
    destArea->setRange(0, 4);
    connect(destArea, SIGNAL(valueChanged(int)), this, SLOT(handleDestAreaChange(int)));

    camXOffset = new QSpinBox();
    camXOffset->setRange(-32768, 32767);
    connect(camXOffset, SIGNAL(valueChanged(int)), this, SLOT(handleCamXChange(int)));

    camYOffset = new QSpinBox();
    camYOffset->setRange(-32768, 32767);
    connect(camYOffset, SIGNAL(valueChanged(int)), this, SLOT(handleCamYChange(int)));

    unk1 = new QSpinBox();
    unk1->setRange(0, 255);
    connect(unk1, SIGNAL(valueChanged(int)), this, SLOT(handleUnk1Change(int)));

    unk2 = new QSpinBox();
    unk2->setRange(0, 255);
    connect(unk2, SIGNAL(valueChanged(int)), this, SLOT(handleUnk2Change(int)));

    enterable = new QCheckBox("Enterable");
    connect(enterable, SIGNAL(toggled(bool)), this, SLOT(handleEnterableChange(bool)));

    returnToWM = new QCheckBox("Return to Worldmap");
    connect(returnToWM, SIGNAL(toggled(bool)), this, SLOT(handleReturnToWMChange(bool)));

    entrancesList = new QListWidget();
    connect(entrancesList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleEntranceListIndexChanged(QListWidgetItem*)));

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    layout->addWidget(entrancesList);

    edits = new QWidget;
    QGridLayout* subLayout = new QGridLayout();
    subLayout->setMargin(0);
    edits->setLayout(subLayout);
    layout->addWidget(edits);

    subLayout->addWidget(new QLabel("Type:"), 0, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(type, 0, 1, 1, 3);

    subLayout->addWidget(new HorLine(), 1, 0, 1, 4);

    subLayout->addWidget(new QLabel("ID:"), 2, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(id, 2, 1, 1, 1);

    subLayout->addWidget(new QLabel("Dest. ID"), 3, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(destId, 3, 1, 1, 1);

    subLayout->addWidget(new QLabel("Camera X:"), 3, 2, 1, 1, Qt::AlignRight);
    subLayout->addWidget(camXOffset, 3, 3, 1, 1);

    subLayout->addWidget(new QLabel("Dest. Area:"), 4, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(destArea, 4, 1, 1, 1);

    subLayout->addWidget(new QLabel("Camera Y:"), 4, 2, 1, 1, Qt::AlignRight);
    subLayout->addWidget(camYOffset, 4, 3, 1, 1);

    subLayout->addWidget(new QLabel("Unknown 1:"), 5, 0, 1, 1, Qt::AlignRight);
    subLayout->addWidget(unk1, 5, 1, 1, 1);

    subLayout->addWidget(new QLabel("Unknown 2:"), 5, 2, 1, 1, Qt::AlignRight);
    subLayout->addWidget(unk2, 5, 3, 1, 1);

    subLayout->addWidget(new HorLine(), 6, 0, 1, 4);

    subLayout->addWidget(enterable, 7, 0, 1, 2);

    subLayout->addWidget(returnToWM, 7, 2, 1, 2);

    updateList();
    updateInfo();
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
    QFile file(QCoreApplication::applicationDirPath() + "/coinkiller_data/entrancetypes.txt");
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
            type = "UNKNOWN";
        entrancesList->addItem(QString("%1: %2 (at %3,%4)").arg(entr->getid()).arg(type).arg(to16(entr->getx())).arg(to16(entr->gety())));
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

void EntranceEditorWidget::handleTypeChanged(int typeVal)
{
    if (!handleChanges) return;
    editEntrance->setEntrType(typeVal);
    editEntrance->setRect();
    updateList();
    emit updateLevelView();
    emit editMade();
}

void EntranceEditorWidget::handleIDChanged(int idVal)
{
    if (!handleChanges) return;
    editEntrance->setId(idVal);
    updateList();
    emit updateLevelView();
    emit editMade();
}

void EntranceEditorWidget::handleDestEntrChange(int destEntrVal)
{
    if (!handleChanges) return;
    editEntrance->setDestEntr(destEntrVal);
    emit editMade();
}

void EntranceEditorWidget::handleDestAreaChange(int destAreaVal)
{
    if (!handleChanges) return;
    editEntrance->setDestArea(destAreaVal);
    emit editMade();
}

void EntranceEditorWidget::handleCamXChange(int camXVal)
{
    if (!handleChanges) return;
    editEntrance->setCameraX(camXVal);
    emit editMade();
}

void EntranceEditorWidget::handleCamYChange(int camYVal)
{
    if (!handleChanges) return;
    editEntrance->setCameraY(camYVal);
    emit editMade();
}

void EntranceEditorWidget::handleUnk1Change(int unk1Val)
{
    if (!handleChanges) return;
    editEntrance->setUnk1(unk1Val);
    emit editMade();
}

void EntranceEditorWidget::handleUnk2Change(int unk2Val)
{
    if (!handleChanges) return;
    editEntrance->setUnk2(unk2Val);
    emit editMade();
}

void EntranceEditorWidget::handleEnterableChange(bool enterableVal)
{
    if (!handleChanges) return;
    editEntrance->setSettingsBit(!enterableVal, 7);
    emit editMade();
}

void EntranceEditorWidget::handleReturnToWMChange(bool returnToWMVal)
{
    if (!handleChanges) return;
    editEntrance->setSettingsBit(returnToWMVal, 4);
    emit editMade();
}
