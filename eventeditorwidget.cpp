#include "eventeditorwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>

EventEditorWidget::EventEditorWidget(Level* level)
{
    this->level = level;

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);

    // Filters
    QHBoxLayout* filterLayout = new QHBoxLayout();

    QLabel* filterLabel = new QLabel("Filter: ");
    filterLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filterLayout->addWidget(filterLabel);
    viewFilter = new QComboBox();
    QStringList filters;
    filters << "All" << "Checked" << "Unchecked";
    viewFilter->insertItems(0, filters);
    filterLayout->addWidget(viewFilter);

    layout->addLayout(filterLayout);

    // List
    eventsList = new QListWidget();
    layout->addWidget(eventsList);

    quint64 states = level->eventState;

    for (int i = 0; i < 64; i++)
    {
        QListWidgetItem* listItem = new QListWidgetItem(QString("Event %1").arg(i), eventsList);
        listItem->setCheckState(((states >> i) & 0x1) ? Qt::Checked : Qt::Unchecked);
        listItem->setData(Qt::UserRole, i);
        eventsList->addItem(listItem);
    }

    connect(eventsList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(handleEventListItemChanged(QListWidgetItem*)));
    connect(viewFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(handleFilterChanged(int)));
}

void EventEditorWidget::handleEventListItemChanged(QListWidgetItem* item)
{
    level->eventState ^= 1 << item->data(Qt::UserRole).toInt();
}

void EventEditorWidget::handleFilterChanged(int filter)
{
    switch (filter)
    {
        case 1: // Checked
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
        case 2: // UnChecked
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
