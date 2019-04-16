#include "locationeditorwidget.h"
#include "unitsconvert.h"

#include <QVBoxLayout>
#include <QLabel>

LocationEditorWidget::LocationEditorWidget(QList<Location *> *locations)
{
    this->locations = locations;

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    locationList = new QListWidget();
    connect(locationList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleLocationListIndexChange(QListWidgetItem*)));
    layout->addWidget(locationList);

    edits = new QWidget();
    QGridLayout* subLayout = new QGridLayout();
    subLayout->setMargin(0);
    edits->setLayout(subLayout);

    subLayout->addWidget(new QLabel("ID:"), 0, 0, 1, 1, Qt::AlignRight);
    id = new QSpinBox();
    id->setRange(0, 255);
    connect(id, SIGNAL(valueChanged(int)), this, SLOT(handleIDChange(int)));
    subLayout->addWidget(id, 0, 1);

    layout->addWidget(edits);

    updateList();
    updateInfo();
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
    locationList->setCurrentRow(locations->indexOf(loc));
}

void LocationEditorWidget::updateList()
{
    QModelIndex index;
    if (locationList->selectionModel()->selectedIndexes().size() != 0) index = locationList->selectionModel()->selectedIndexes().at(0);

    locationList->clear();
    foreach (Location* location, *locations)
        locationList->addItem(QString("Location %1: (at %2,%3) (W: %4 H: %5)").arg(location->getid()).arg(to16(location->getx())).arg(to16(location->gety())).arg(to16(location->getwidth())).arg(to16(location->getheight())));

    locationList->setCurrentIndex(index);
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

void LocationEditorWidget::handleIDChange(int idVal)
{
    if (!handleChanges) return;
    editLocation->setId(idVal);
    updateList();
    emit updateLevelView();
    emit editMade();
}

void LocationEditorWidget::handleLocationListIndexChange(QListWidgetItem *item)
{
    if (!handleChanges) return;
    editLocation = locations->at(locationList->row(item));
    editingALocation = true;
    updateInfo();
    emit selectedLocChanged(editLocation);
}
