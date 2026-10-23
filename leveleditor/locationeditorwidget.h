#ifndef LOCATIONEDITORWIDGET_H
#define LOCATIONEDITORWIDGET_H

#include "objects.h"
#include "sortabletreewidget.h"

#include <QWidget>
#include <QSpinBox>
#include <QUndoStack>

class LocationEditorWidget : public QWidget
{
    Q_OBJECT
public:
    LocationEditorWidget(QList<Location*> *locations, QUndoStack *undoStack, QWidget *parent = nullptr);
    void deselect();
    void select(Location *loc);
    void updateEditor();

signals:
    void selectedLocChanged(Object* loc);

private slots:
    void handleLocationListIndexChange(QTreeWidgetItem *item);
    void handleIDChange(int idVal);

private:
    SortableTreeWidget* locationList;
    QWidget* edits;
    QSpinBox* id;

    void changeEvent(QEvent* event);

    void updateList();
    void updateInfo();

    QList<Location*> *locations;
    Location* editLocation;
    bool editingALocation = false;
    bool handleChanges = true;

    QUndoStack *undoStack;
};

#endif // LOCATIONEDITORWIDGET_H
