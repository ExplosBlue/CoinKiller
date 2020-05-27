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

    editsLayout->addWidget(new QLabel("ID:"), 0, 0, 1, 1, Qt::AlignRight);

    id = new QSpinBox();
    id->setRange(0, 255);
    editsLayout->addWidget(id, 0, 1);

    editsLayout->addWidget(new QLabel("Loops:"), 1, 0, 1, 1, Qt::AlignRight);

    loop = new QCheckBox();
    editsLayout->addWidget(loop, 1, 1);

    editsLayout->addWidget(new HorLine(), 2, 0, 1, 2);

    currentNode = new QLabel("<b>Node: None</b>");
    editsLayout->addWidget(currentNode, 3, 0, 1, 1);

    editsLayout->addWidget(new QLabel("Speed:"), 4, 0, 1, 1, Qt::AlignRight);

    speed = new QDoubleSpinBox();
    speed->setRange(-1000000, 1000000);
    speed->setDecimals(8);
    speed->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editsLayout->addWidget(speed, 4, 1);

    editsLayout->addWidget(new QLabel("Acceleration:"), 5, 0, 1, 1, Qt::AlignRight);

    acceleration = new QDoubleSpinBox();
    acceleration->setRange(-1000000, 1000000);
    acceleration->setDecimals(8);
    editsLayout->addWidget(acceleration, 5, 1);

    editsLayout->addWidget(new QLabel("Delay:"), 6, 0, 1, 1, Qt::AlignRight);

    delay = new QDoubleSpinBox();
    delay->setRange(0, 4209999872);
    delay->setDecimals(0);
    editsLayout->addWidget(delay, 6, 1);

    edits->setLayout(editsLayout);
    layout->addWidget(edits);
    setLayout(layout);

    connect(pathList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handlePathListIndexChanged(QListWidgetItem*)));
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChanged(int)));
    connect(loop, SIGNAL(stateChanged(int)), this, SLOT(handleLoopChanged()));
    connect(speed, SIGNAL(valueChanged(double)), this, SLOT(handleSpeedChanged(double)));
    connect(acceleration, SIGNAL(valueChanged(double)), this, SLOT(handleAccelChanged(double)));
    connect(delay, SIGNAL(valueChanged(double)), this, SLOT(handleDelayChanged(double)));

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
        pathItem->setText(QString("Path %1: %2 nodes").arg(path->getid()).arg(path->getNumberOfNodes()));
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

    currentNode->setText(QString("<b>Node: %1</b>").arg(editPath->getIndexOfNode(editNode) + 1));
    speed->setValue(editNode->getSpeed());
    acceleration->setValue(editNode->getAccel());
    delay->setValue(editNode->getDelay());
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

void PathEditorWidget::handleDelayChanged(double delayVal)
{
    if (!handleChanges) return;
    editNode->setDelay((quint32)delayVal);
    emit editMade();
}
