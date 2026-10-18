#ifndef PROGRESSPATHEDITORWIDGET_H
#define PROGRESSPATHEDITORWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QUndoStack>

#include "objects.h"
#include "sortabletreewidget.h"

class ProgressPathEditorWidget : public QWidget
{
    Q_OBJECT
public:
    ProgressPathEditorWidget(QList<ProgressPath*> *paths, QUndoStack *undoStack, QWidget *parent = nullptr);
    void deselect();
    void select(ProgressPathNode* node);
    void updateEditor();

signals:
    void selectedProgPathChanged(Object* pPath);

public slots:

private:
    ProgressPath* editPath;
    bool editingAPath = false;
    bool handleChanges = true;

    QList<ProgressPath*> *paths;

    QWidget* edits;
    SortableTreeWidget* pathList;
    QSpinBox* id;
    QCheckBox* alternatePathFlag;

    QUndoStack *undoStack;

    void changeEvent(QEvent* event) override;

    void updateList();
    void updateInfo();

private slots:
    void handlePathListIndexChanged(QTreeWidgetItem *item);
    void handleIDChanged(int idVal);
    void handleAlternetePathFlagChanged(bool apfVal);
};

#endif // PROGRESSPATHEDITORWIDGET_H
