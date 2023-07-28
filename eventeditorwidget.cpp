#include "eventeditorwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "EditorCommands/setvalue.h"

EventEditorWidget::EventEditorWidget(Level *level, QUndoStack* undoStack) :
    level(level), undoStack(undoStack) {

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    // Filters
    QHBoxLayout* filterLayout = new QHBoxLayout();

    QLabel* filterLabel = new QLabel(tr("Filter:"));
    filterLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filterLayout->addWidget(filterLabel);
    viewFilter = new QComboBox();
    QStringList filters;

    filters << tr("All")
            << tr("Enabled")
            << tr("Disabled");

    viewFilter->insertItems(0, filters);
    filterLayout->addWidget(viewFilter);

    layout->addLayout(filterLayout);

    // List
    eventsList = new QListWidget();
    layout->addWidget(eventsList);

    updateEventStates();

    connect(eventsList, &QListWidget::itemChanged, this, &EventEditorWidget::eventListItemChanged);
    connect(viewFilter, &QComboBox::currentIndexChanged, this, &EventEditorWidget::filterChanged);
}

void EventEditorWidget::updateEventStates() {
    quint64 states = level->eventState;

    eventsList->blockSignals(true);
    eventsList->clear();

    for (int i = 0; i < 64; i++) {
        QListWidgetItem* listItem = new QListWidgetItem(tr("Event %1").arg(i+1), eventsList);
        listItem->setCheckState(((states >> i) & 1ULL) ? Qt::Checked : Qt::Unchecked);
        listItem->setData(Qt::UserRole, i);
        eventsList->addItem(listItem); // TODO Should set at index
    }

    eventsList->blockSignals(false);
}

void EventEditorWidget::updateEditor() {

    updateEventStates();
    updateFilter();
}

void EventEditorWidget::updateFilter() {
    switch (filter)
    {
    case 1: // Enabled
    {
        for (int i = 0; i < eventsList->count(); i++)
        {
            QListWidgetItem* item = eventsList->item(i);

            if (item->checkState() == Qt::Checked)
                item->setHidden(false);
            else
                item->setHidden(true);
        }
    }   break;
    case 2: // Disabled
    {
        for (int i = 0; i < eventsList->count(); i++)
        {
            QListWidgetItem* item = eventsList->item(i);

            if (item->checkState() == Qt::Unchecked)
                item->setHidden(false);
            else
                item->setHidden(true);
        }
    }   break;
    default:
    {
        for (int i = 0; i < eventsList->count(); i++)
        {
            QListWidgetItem* item = eventsList->item(i);
            item->setHidden(false);
        }
    }   break;
    }
}

void EventEditorWidget::eventListItemChanged(QListWidgetItem* item) {
    quint64 newState = level->eventState ^ (1ULL << item->data(Qt::UserRole).toInt());
    undoStack->push(new EditorCommand::SetValue<quint64>(&level->eventState, newState, tr("Changed Area Event Flags")));
}

void EventEditorWidget::filterChanged(int newFilter) {
    this->filter = newFilter;
    updateFilter();
}
