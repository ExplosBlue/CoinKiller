#include "patheditorwidget.h"
#include "unitsconvert.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <cmath>

#include "commands/pathcommands.h"
#include "commands/pathnodecommands.h"

PathEditorWidget::PathEditorWidget(QList<Path*> *paths, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent),
    paths(paths),
    undoStack(undoStack) {

    QVBoxLayout* layout = new QVBoxLayout();

    pathList = new SortableTreeWidget();
    pathList->setColumnCount(2);
    pathList->setHeaderLabels(QStringList() << tr("Path ID") << tr("Position"));
    pathList->setSortableColumns({0, 1});
    pathList->setFlexibleColumn(0);
    layout->addWidget(pathList);

    edits = new QWidget();
    QGridLayout* editsLayout = new QGridLayout();
    editsLayout->setContentsMargins(0, 0, 0, 0);

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

    connect(pathList, &QTreeWidget::itemClicked, this, &PathEditorWidget::handlePathListIndexChanged);
    connect(id, &QSpinBox::valueChanged, this, &PathEditorWidget::handleIDChanged);
    connect(loop, &QCheckBox::stateChanged, this, &PathEditorWidget::handleLoopChanged);
    connect(speed, &QDoubleSpinBox::valueChanged, this, &PathEditorWidget::handleSpeedChanged);
    connect(acceleration, &QDoubleSpinBox::valueChanged, this, &PathEditorWidget::handleAccelChanged);
    connect(delay, &QSpinBox::valueChanged, this, &PathEditorWidget::handleDelayChanged);

    connect(rotation, &QSpinBox::valueChanged, this, &PathEditorWidget::handleRotationChanged);
    connect(variableField, &QSpinBox::valueChanged, this, &PathEditorWidget::handleVariableFieldChanged);
    connect(nextPathID, &QSpinBox::valueChanged, this, &PathEditorWidget::handleNextPathIDChanged);
    connect(nodeID, &QSpinBox::valueChanged, this, &PathEditorWidget::handleNodeIDChanged);

    updateEditor();
}

void PathEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        pathList->setHeaderLabels(QStringList() << tr("Path ID") << tr("Position"));

    QWidget::changeEvent(event);
}

void PathEditorWidget::updateEditor()
{
    updateList();
    updateInfo();
}

void PathEditorWidget::updateList()
{
    Path* selected = nullptr;
    if (!pathList->selectedItems().isEmpty())
        selected = static_cast<Path*>(pathList->selectedItems().first()->data(0, Qt::UserRole).value<void*>());

    pathList->clear();
    foreach (Path* path, *paths)
    {
        double x = 0.0;
        double y = 0.0;
        if (path->getNumberOfNodes() > 0)
        {
            PathNode* node = path->getNode(0);
            x = static_cast<double>(to16(node->getx()));
            y = static_cast<double>(to16(node->gety()));
        }

        SortableTreeItem* item = new SortableTreeItem();
        item->setData(0, Qt::DisplayRole, path->getid());
        item->setSortKey(0, static_cast<double>(path->getid()));
        item->setText(1, tr("(X: %1, Y: %2)").arg(static_cast<int>(x)).arg(static_cast<int>(y)));
        item->setSortKey(1, std::hypot(x, y));
        item->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(path)));
        pathList->addTopLevelItem(item);
    }

    pathList->resort();

    if (selected == nullptr)
        return;

    for (int i = 0; i < pathList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = pathList->topLevelItem(i);
        if (static_cast<Path*>(item->data(0, Qt::UserRole).value<void*>()) == selected)
        {
            pathList->setCurrentItem(item);
            break;
        }
    }
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

    for (int i = 0; i < pathList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = pathList->topLevelItem(i);
        if (static_cast<Path*>(item->data(0, Qt::UserRole).value<void*>()) == editPath)
        {
            pathList->setCurrentItem(item);
            break;
        }
    }

    handleChanges = true;
}

void PathEditorWidget::deselect()
{
    editingAPath = false;
    updateInfo();
    pathList->clearSelection();
}

void PathEditorWidget::handlePathListIndexChanged(QTreeWidgetItem *item)
{
    if (!handleChanges) return;
    editPath = static_cast<Path*>(item->data(0, Qt::UserRole).value<void*>());
    editNode = editPath->getNode(0);
    editingAPath = true;
    updateInfo();
    emit selectedPathChanged(editPath->getNode(0));
}

void PathEditorWidget::handleIDChanged(int id)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathCmd::SetId(editPath, id));
    updateList();
}

void PathEditorWidget::handleLoopChanged()
{
    if (!handleChanges) return;
    if (loop->isChecked()) {
        undoStack->push(new Commands::PathCmd::SetLoop(editPath, 2));
    } else {
        undoStack->push(new Commands::PathCmd::SetLoop(editPath, 0));
    }
}

void PathEditorWidget::handleSpeedChanged(double speed)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathNodeCmd::SetSpeed(editNode, static_cast<float>(speed)));
}

void PathEditorWidget::handleAccelChanged(double accel)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathNodeCmd::SetAccel(editNode, static_cast<float>(accel)));
}

void PathEditorWidget::handleDelayChanged(int delay)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathNodeCmd::SetDelay(editNode, delay));
}

void PathEditorWidget::handleRotationChanged(int rotation)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathNodeCmd::SetRotation(editNode, static_cast<qint16>(rotation)));
}

void PathEditorWidget::handleVariableFieldChanged(int value)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathNodeCmd::SetVariableField(editNode, value));
}

void PathEditorWidget::handleNextPathIDChanged(int nextPathID)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::PathNodeCmd::SetNextPathID(editNode, nextPathID));
}

void PathEditorWidget::handleNodeIDChanged(int nodeID)
{
    if (!handleChanges) return;

    if (nodeID >= editPath->getNumberOfNodes()) {
        this->nodeID->blockSignals(true);
        this->nodeID->setValue(editPath->getIndexOfNode(editNode));
        this->nodeID->blockSignals(false);
        return;
    }

    undoStack->push(new Commands::PathCmd::SwapNodes(editPath, editPath->getIndexOfNode(editNode), nodeID));
    updateInfo();
}
