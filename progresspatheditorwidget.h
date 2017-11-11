#ifndef PROGRESSPATHEDITORWIDGET_H
#define PROGRESSPATHEDITORWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QListWidget>
#include <QSpinBox>
#include <QCheckBox>

#include "objects.h"

class ProgressPathEditorWidget : public QWidget
{
    Q_OBJECT
public:
    ProgressPathEditorWidget(QList<ProgressPath*> *paths);
    void deselect();
    void select(ProgressPathNode* node);
    void updateEditor();

signals:
    void updateLevelView();
    void selectedProgPathChanged(Object* pPath);
    void editMade();

public slots:

private:
    ProgressPath* editPath;
    bool editingAPath = false;
    bool handleChanges = true;

    QList<ProgressPath*> *paths;

    QWidget* edits;
    QListWidget* pathList;
    QSpinBox* id;
    QCheckBox* alternatePathFlag;

    void updateList();
    void updateInfo();

private slots:
    void handlePathListIndexChanged(QListWidgetItem *item);
    void handleIDChanged(int idVal);
    void handleAlternetePathFlagChanged(bool apfVal);
};

#endif // PROGRESSPATHEDITORWIDGET_H
