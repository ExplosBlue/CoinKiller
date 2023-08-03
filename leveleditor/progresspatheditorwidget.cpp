#include "progresspatheditorwidget.h"

#include <QLabel>

#include "commands/progresspathcommands.h"

ProgressPathEditorWidget::ProgressPathEditorWidget(QList<ProgressPath*> *paths, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent),
    paths(paths),
    undoStack(undoStack) {

    QVBoxLayout* layout = new QVBoxLayout();

    pathList = new QListWidget();
    layout->addWidget(pathList);

    edits = new QWidget();
    QGridLayout* editsLayout = new QGridLayout();
    editsLayout->setContentsMargins(0, 0, 0, 0);

    editsLayout->addWidget(new QLabel(tr("ID:")), 0, 0, 1, 1, Qt::AlignRight);
    id = new QSpinBox();
    id->setRange(0, 255);
    id->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editsLayout->addWidget(id, 0, 1);

    alternatePathFlag = new QCheckBox(tr("Alternate Path Flag"));
    editsLayout->addWidget(alternatePathFlag, 0, 2, 1, 2, Qt::AlignRight);

    edits->setLayout(editsLayout);
    layout->addWidget(edits);
    setLayout(layout);

    connect(pathList, &QListWidget::itemClicked, this, &ProgressPathEditorWidget::handlePathListIndexChanged);
    connect(id, &QSpinBox::valueChanged, this, &ProgressPathEditorWidget::handleIDChanged);
    connect(alternatePathFlag, &QAbstractButton::toggled, this, &ProgressPathEditorWidget::handleAlternetePathFlagChanged);

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
        pathItem->setText(tr("Progress Path %1: %n Node(s)", nullptr, path->getNumberOfNodes()).arg(path->getid()));
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
    pathList->setCurrentRow(static_cast<int>(paths->indexOf(editPath)));
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

void ProgressPathEditorWidget::handleIDChanged(int id)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ProgressPathCmd::SetId(editPath, id));
    updateList();
}

void ProgressPathEditorWidget::handleAlternetePathFlagChanged(bool flag)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::ProgressPathCmd::SetAltPathFlag(editPath, flag));
}
