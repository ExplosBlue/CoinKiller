#ifndef EVENTEDITORWIDGET_H
#define EVENTEDITORWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QUndoStack>

#include "level.h"

class EventEditorWidget : public QWidget
{
    Q_OBJECT
public:
    EventEditorWidget(Level *level, QUndoStack *undoStack);

    void updateEditor();

private:
    void updateEventStates();
    void updateFilter();

    QListWidget *eventsList;
    QComboBox *viewFilter;

    Level *level;
    QUndoStack *undoStack;

    int filter = 0;

private slots:
    void eventListItemChanged(QListWidgetItem *item);
    void filterChanged(int filter);
};

#endif // EVENTEDITORWIDGET_H
