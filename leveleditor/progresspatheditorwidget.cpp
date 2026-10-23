#include "progresspatheditorwidget.h"
#include "unitsconvert.h"

#include <QLabel>
#include <cmath>

#include "commands/progresspathcommands.h"

ProgressPathEditorWidget::ProgressPathEditorWidget(QList<ProgressPath*> *paths, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent),
    paths(paths),
    undoStack(undoStack) {

    QVBoxLayout* layout = new QVBoxLayout();

    pathList = new SortableTreeWidget();
    pathList->setColumnCount(2);
    pathList->setHeaderLabels(QStringList() << tr("Progress Path ID") << tr("Position"));
    pathList->setSortableColumns({0, 1});
    pathList->setFlexibleColumn(0);
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

    connect(pathList, &QTreeWidget::itemClicked, this, &ProgressPathEditorWidget::handlePathListIndexChanged);
    connect(id, &QSpinBox::valueChanged, this, &ProgressPathEditorWidget::handleIDChanged);
    connect(alternatePathFlag, &QAbstractButton::toggled, this, &ProgressPathEditorWidget::handleAlternetePathFlagChanged);

    updateList();
    updateInfo();
}

void ProgressPathEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        pathList->setHeaderLabels(QStringList() << tr("Progress Path ID") << tr("Position"));

    QWidget::changeEvent(event);
}

void ProgressPathEditorWidget::updateEditor()
{
    updateList();
    updateInfo();
}

void ProgressPathEditorWidget::updateList()
{
    ProgressPath* selected = nullptr;
    if (!pathList->selectedItems().isEmpty())
        selected = static_cast<ProgressPath*>(pathList->selectedItems().first()->data(0, Qt::UserRole).value<void*>());

    pathList->clear();
    foreach (ProgressPath* path, *paths)
    {
        double x = 0.0;
        double y = 0.0;
        if (path->getNumberOfNodes() > 0)
        {
            ProgressPathNode* node = path->getNode(0);
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
        if (static_cast<ProgressPath*>(item->data(0, Qt::UserRole).value<void*>()) == selected)
        {
            pathList->setCurrentItem(item);
            break;
        }
    }
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

    for (int i = 0; i < pathList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = pathList->topLevelItem(i);
        if (static_cast<ProgressPath*>(item->data(0, Qt::UserRole).value<void*>()) == editPath)
        {
            pathList->setCurrentItem(item);
            break;
        }
    }

    handleChanges = true;
}

void ProgressPathEditorWidget::deselect()
{
    editingAPath = false;
    updateInfo();
    pathList->clearSelection();
}

void ProgressPathEditorWidget::handlePathListIndexChanged(QTreeWidgetItem *item)
{
    if (!handleChanges) return;
    editPath = static_cast<ProgressPath*>(item->data(0, Qt::UserRole).value<void*>());
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
