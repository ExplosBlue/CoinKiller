#include "locationeditorwidget.h"
#include "unitsconvert.h"

#include <QVBoxLayout>
#include <QLabel>
#include <cmath>

#include "commands/locationcommands.h"

LocationEditorWidget::LocationEditorWidget(QList<Location *> *locations, QUndoStack *undoStack, QWidget *parent) :
    QWidget(parent),
    locations(locations),
    undoStack(undoStack) {

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    locationList = new SortableTreeWidget();
    locationList->setColumnCount(2);
    locationList->setHeaderLabels(QStringList() << tr("Location ID") << tr("Position"));
    locationList->setSortableColumns({0, 1});
    locationList->setFlexibleColumn(0);
    connect(locationList, &QTreeWidget::itemClicked, this, &LocationEditorWidget::handleLocationListIndexChange);
    layout->addWidget(locationList);

    edits = new QWidget();
    QGridLayout* subLayout = new QGridLayout();
    subLayout->setContentsMargins(0, 0, 0, 0);
    edits->setLayout(subLayout);

    subLayout->addWidget(new QLabel(tr("ID:")), 0, 0, 1, 1, Qt::AlignRight);
    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, &QSpinBox::valueChanged, this, &LocationEditorWidget::handleIDChange);
    subLayout->addWidget(id, 0, 1);

    layout->addWidget(edits);

    updateList();
    updateInfo();
}

void LocationEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
        locationList->setHeaderLabels(QStringList() << tr("Location ID") << tr("Position"));

    QWidget::changeEvent(event);
}

void LocationEditorWidget::deselect()
{
    editingALocation = false;
    updateInfo();
    locationList->clearSelection();
}

void LocationEditorWidget::select(Location *loc)
{
    editLocation = loc;
    editingALocation = true;
    updateInfo();

    for (int i = 0; i < locationList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = locationList->topLevelItem(i);
        if (static_cast<Location*>(item->data(0, Qt::UserRole).value<void*>()) == loc)
        {
            locationList->setCurrentItem(item);
            break;
        }
    }
}

void LocationEditorWidget::updateList()
{
    Location* selected = nullptr;
    if (!locationList->selectedItems().isEmpty())
        selected = static_cast<Location*>(locationList->selectedItems().first()->data(0, Qt::UserRole).value<void*>());

    locationList->clear();

    foreach (Location* location, *locations)
    {
        SortableTreeItem* item = new SortableTreeItem();
        item->setData(0, Qt::DisplayRole, location->getid());
        item->setSortKey(0, static_cast<double>(location->getid()));
        item->setText(1, tr("(X: %1, Y: %2) (W: %3, H: %4)").arg(to16(location->getx())).arg(to16(location->gety())).arg(to16(location->getwidth())).arg(to16(location->getheight())));
        item->setSortKey(1, std::hypot(static_cast<double>(to16(location->getx())), static_cast<double>(to16(location->gety()))));
        item->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(location)));
        locationList->addTopLevelItem(item);
    }

    locationList->resort();

    if (selected == nullptr)
        return;

    for (int i = 0; i < locationList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = locationList->topLevelItem(i);
        if (static_cast<Location*>(item->data(0, Qt::UserRole).value<void*>()) == selected)
        {
            locationList->setCurrentItem(item);
            break;
        }
    }
}

void LocationEditorWidget::updateInfo()
{
    if (editingALocation == false)
    {
        edits->setHidden(true);
        return;
    }
    edits->setHidden(false);

    handleChanges = false;
    id->setValue(editLocation->getid());
    handleChanges = true;
}

void LocationEditorWidget::updateEditor()
{
    updateList();
}

void LocationEditorWidget::handleIDChange(int id)
{
    if (!handleChanges) return;
    undoStack->push(new Commands::LocationCmd::SetId(editLocation, id));
    updateList();
}

void LocationEditorWidget::handleLocationListIndexChange(QTreeWidgetItem *item)
{
    if (!handleChanges) return;
    editLocation = static_cast<Location*>(item->data(0, Qt::UserRole).value<void*>());
    editingALocation = true;
    updateInfo();
    emit selectedLocChanged(editLocation);
}
