#include "progresspatheditorwidget.h"

#include <QLabel>

ProgressPathEditorWidget::ProgressPathEditorWidget(QList<ProgressPath*> *paths)
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

    alternatePathFlag = new QCheckBox("Alternate Path Flag");
    editsLayout->addWidget(alternatePathFlag, 0, 2, 1, 2, Qt::AlignRight);

    edits->setLayout(editsLayout);
    layout->addWidget(edits);
    setLayout(layout);

    connect(pathList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handlePathListIndexChanged(QListWidgetItem*)));
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChanged(int)));
    connect(alternatePathFlag, SIGNAL(toggled(bool)), this, SLOT(handleAlternetePathFlagChanged(bool)));

    updateList();
    updateInfo();
}

void ProgressPathEditorWidget::updateEditor()
{
    updateList();
    updateInfo();
}

void ProgressPathEditorWidget::updateList()
{
    QModelIndex index;
    if (pathList->selectionModel()->selectedIndexes().size() != 0) index = pathList->selectionModel()->selectedIndexes().at(0);
    pathList->clear();
    foreach (ProgressPath* path, *paths)
    {
        QListWidgetItem* pathItem = new QListWidgetItem();
        pathItem->setText(QString("Progress path %1: %2 nodes").arg(path->getid()).arg(path->getNumberOfNodes()));
        pathList->addItem(pathItem);
    }
    pathList->setCurrentIndex(index);
}

void ProgressPathEditorWidget::updateInfo()
{
    if (editingAPath == false)
    {
        edits->setHidden(true);
        return;
    }
    edits->setHidden(false);

    handleChanges = false;
    id->setValue(editPath->getid());
    alternatePathFlag->setChecked(editPath->getAlternatePathFlag() == 1);
    handleChanges = true;
}

void ProgressPathEditorWidget::select(ProgressPathNode* node)
{
    editPath = node->getParentPath();
    editingAPath = true;
    updateInfo();
    handleChanges = false;
    pathList->setCurrentRow(paths->indexOf(editPath));
    handleChanges = true;
}

void ProgressPathEditorWidget::deselect()
{
    editingAPath = false;
    updateInfo();
    pathList->clearSelection();
}

void ProgressPathEditorWidget::handlePathListIndexChanged(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editPath = paths->at(pathList->row(item));
    editingAPath = true;
    updateInfo();
    emit selectedProgPathChanged(editPath->getNode(0));
}

void ProgressPathEditorWidget::handleIDChanged(int idVal)
{
    if (!handleChanges) return;
    editPath->setId(idVal);
    updateList();
    emit updateLevelView();
}

void ProgressPathEditorWidget::handleAlternetePathFlagChanged(bool apfVal)
{
    if (!handleChanges) return;
    editPath->setAlternatePathFlag(apfVal);
}
