#include "patheditorwidget.h"

#include <QVBoxLayout>
#include <QGridLayout>

PathEditorWidget::PathEditorWidget(QList<Path*> *paths)
{
    this->paths = paths;

    QVBoxLayout* layout = new QVBoxLayout();

    pathList = new QListWidget();
    layout->addWidget(pathList);

    edits = new QWidget();
    QGridLayout* editsLayout = new QGridLayout();
    editsLayout->setMargin(0);

    editsLayout->addWidget(new QLabel(tr("Path ID:")), 0, 0, 1, 1, Qt::AlignRight);
    id = new QSpinBox();
    id->setRange(0, 255);
    id->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editsLayout->addWidget(id, 0, 1, 1, 3);

    loop = new QCheckBox();
    loop->setText(tr("Loop"));
    editsLayout->addWidget(loop, 1, 3, 1, 1, Qt::AlignRight);

    nodeGroupbox = new QGroupBox();
    QGridLayout* nodeGroupLayout = new QGridLayout();
    nodeGroupbox->setTitle(tr("Node: "));

    nodeGroupLayout->addWidget(new QLabel(tr("Node ID:")), 0, 0, 1, 1, Qt::AlignRight);
    nodeID = new QSpinBox();
    nodeID->setRange(0, 255);
    nodeID->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nodeGroupLayout->addWidget(nodeID, 0, 1, 1, 3);

    nodeGroupLayout->addWidget(new QLabel(tr("Speed:")), 1, 0, 1, 1, Qt::AlignRight);
    speed = new QDoubleSpinBox();
    speed->setRange(-1000000, 1000000);
    speed->setDecimals(8);
    speed->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    nodeGroupLayout->addWidget(speed, 1, 1);

    nodeGroupLayout->addWidget(new QLabel(tr("Acceleration:")), 1, 2, 1, 1, Qt::AlignRight);
    acceleration = new QDoubleSpinBox();
    acceleration->setRange(-1000000, 1000000);
    acceleration->setDecimals(8);
    acceleration->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    nodeGroupLayout->addWidget(acceleration, 1, 3);

    nodeGroupLayout->addWidget(new QLabel(tr("Delay:")), 2, 0, 1, 1, Qt::AlignRight);
    delay = new QSpinBox();
    delay->setRange(0, 65535);
    delay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nodeGroupLayout->addWidget(delay, 2, 1);

    nodeGroupLayout->addWidget(new QLabel(tr("Rotation:")), 2, 2, 1, 1, Qt::AlignRight);
    rotation = new QSpinBox();
    rotation->setRange(-32768, 32767);
    rotation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nodeGroupLayout->addWidget(rotation, 2, 3);

    nodeGroupLayout->addWidget(new QLabel(tr("Variable Use:")), 3, 0, 1, 1, Qt::AlignRight);
    variableField = new QSpinBox();
    variableField->setRange(0, 255);
    variableField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    variableField->setToolTip(tr("This is a multi-purpose field. \nCheck the notes of whatever sprite is paired to this path to see how to use this field."));
    nodeGroupLayout->addWidget(variableField, 3, 1);

    nodeGroupLayout->addWidget(new QLabel(tr("Next Path ID:")), 3, 2, 1, 1, Qt::AlignRight);
    nextPathID = new QSpinBox();
    nextPathID->setRange(0, 255);
    nextPathID->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nodeGroupLayout->addWidget(nextPathID, 3, 3);

    nodeGroupbox->setLayout(nodeGroupLayout);
    editsLayout->addWidget(nodeGroupbox, 2, 0, 1, 4);

    edits->setLayout(editsLayout);
    layout->addWidget(edits);
    setLayout(layout);

    connect(pathList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handlePathListIndexChanged(QListWidgetItem*)));
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChanged(int)));
    connect(loop, SIGNAL(stateChanged(int)), this, SLOT(handleLoopChanged()));
    connect(speed, SIGNAL(valueChanged(double)), this, SLOT(handleSpeedChanged(double)));
    connect(acceleration, SIGNAL(valueChanged(double)), this, SLOT(handleAccelChanged(double)));
    connect(delay, SIGNAL(valueChanged(int)), this, SLOT(handleDelayChanged(int)));

    connect(rotation, SIGNAL(valueChanged(int)), this, SLOT(handleRotationChanged(int)));
    connect(variableField, SIGNAL(valueChanged(int)), this, SLOT(handleVariableFieldChanged(int)));
    connect(nextPathID, SIGNAL(valueChanged(int)), this, SLOT(handleNextPathIDChanged(int)));
    connect(nodeID, SIGNAL(valueChanged(int)), this, SLOT(handleNodeIDChanged(int)));

    updateList();
    updateInfo();
}

void PathEditorWidget::updateEditor()
{
    updateList();
    updateInfo();
}

void PathEditorWidget::updateList()
{
    QModelIndex index;
    if (pathList->selectionModel()->selectedIndexes().size() != 0) index = pathList->selectionModel()->selectedIndexes().at(0);
    pathList->clear();
    foreach (Path* path, *paths)
    {
        QListWidgetItem* pathItem = new QListWidgetItem();
        pathItem->setText(tr("Path %1: %n Node(s)", 0, path->getNumberOfNodes()).arg(path->getid()));
        pathList->addItem(pathItem);
    }
    pathList->setCurrentIndex(index);
}

void PathEditorWidget::updateInfo()
{
    if (editingAPath == false)
    {
        edits->setHidden(true);
        return;
    }
    edits->setHidden(false);

    handleChanges = false;
    id->setValue(editPath->getid());
    if (editPath->getLoop() == 2)
        loop->setChecked(true);
    else
        loop->setChecked(false);

    nodeGroupbox->setTitle(tr("Node: %1").arg(editPath->getIndexOfNode(editNode)));
    speed->setValue(editNode->getSpeed());
    acceleration->setValue(editNode->getAccel());
    delay->setValue(editNode->getDelay());
    rotation->setValue(editNode->getRotation());
    variableField->setValue(editNode->getVariableField());
    nextPathID->setValue(editNode->getNextPathID());
    nodeID->setValue(editPath->getIndexOfNode(editNode));
    handleChanges = true;
}

void PathEditorWidget::select(PathNode* node)
{
    editNode = node;
    editPath = node->getParentPath();
    editingAPath = true;
    updateInfo();
    handleChanges = false;
    pathList->setCurrentRow(paths->indexOf(editPath));
    handleChanges = true;
}

void PathEditorWidget::deselect()
{
    editingAPath = false;
    updateInfo();
    pathList->clearSelection();
}

void PathEditorWidget::handlePathListIndexChanged(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editPath = paths->at(pathList->row(item));
    editNode = editPath->getNode(0);
    editingAPath = true;
    updateInfo();
    emit selectedPathChanged(editPath->getNode(0));
}

void PathEditorWidget::handleIDChanged(int idVal)
{
    if (!handleChanges) return;
    editPath->setId(idVal);
    updateList();
    emit updateLevelView();
    emit editMade();
}

void PathEditorWidget::handleLoopChanged()
{
    if (!handleChanges) return;
    if (loop->isChecked())
        editPath->setLoop(2);
    else
        editPath->setLoop(0);

    emit updateLevelView();
    emit editMade();
}

void PathEditorWidget::handleSpeedChanged(double speedVal)
{
    if (!handleChanges) return;
    editNode->setSpeed((float)speedVal);
    emit editMade();
}

void PathEditorWidget::handleAccelChanged(double accelVal)
{
    if (!handleChanges) return;
    editNode->setAccel((float)accelVal);
    emit editMade();
}

void PathEditorWidget::handleDelayChanged(int delayVal)
{
    if (!handleChanges) return;
    editNode->setDelay(delayVal);
    emit editMade();
}

void PathEditorWidget::handleRotationChanged(int rotationVal)
{
    if (!handleChanges) return;
    editNode->setRotation(rotationVal);
    emit editMade();
}

void PathEditorWidget::handleVariableFieldChanged(int flagVal)
{
    if (!handleChanges) return;
    editNode->setVariableField(flagVal);
    emit editMade();
}

void PathEditorWidget::handleNextPathIDChanged(int nextPathID)
{
    if (!handleChanges) return;
    editNode->setNextPathID(nextPathID);
    emit editMade();
}

void PathEditorWidget::handleNodeIDChanged(int nodeID)
{
    if (!handleChanges) return;
    if (nodeID >= editPath->getNumberOfNodes())
    {
        this->nodeID->setValue(editPath->getIndexOfNode(editNode));
        return;
    }

    editPath->swapNodes(editPath->getIndexOfNode(editNode), nodeID);
    emit updateLevelView();
    emit updateInfo();
    emit editMade();
}
