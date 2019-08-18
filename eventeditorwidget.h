#ifndef EVENTEDITORWIDGET_H
#define EVENTEDITORWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>

#include "level.h"

class EventEditorWidget : public QWidget
{
    Q_OBJECT
public:
    EventEditorWidget(Level* level);

signals:
    //void selectedEventIdChanged(int* id);

public slots:

private:
    QListWidget* eventsList;
    QComboBox* viewFilter;

    Level* level;

private slots:
    void handleEventListItemChanged(QListWidgetItem* item);
    void handleFilterChanged(int filter);
};

#endif // EVENTEDITORWIDGET_H
