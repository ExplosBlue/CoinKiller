#ifndef LOCATIONEDITORWIDGET_H
#define LOCATIONEDITORWIDGET_H

#include "objects.h"

#include <QWidget>
#include <QListWidget>
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
    void handleLocationListIndexChange(QListWidgetItem *item);
    void handleIDChange(int idVal);

private:
    QListWidget* locationList;
    QWidget* edits;
    QSpinBox* id;

    void updateList();
    void updateInfo();

    QList<Location*> *locations;
    Location* editLocation;
    bool editingALocation = false;
    bool handleChanges = true;

    QUndoStack *undoStack;
};

#endif // LOCATIONEDITORWIDGET_H
