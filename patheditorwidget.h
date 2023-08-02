#ifndef PATHEDITORWIDGET_H
#define PATHEDITORWIDGET_H

#include "objects.h"

#include <QWidget>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QUndoStack>

class PathEditorWidget : public QWidget
{
    Q_OBJECT
public:
    PathEditorWidget(QList<Path*> *paths, QUndoStack *undoStack, QWidget *parent = nullptr);
    void deselect();
    void select(PathNode* node);
    void updateEditor();

signals:
    void selectedPathChanged(Object* pPath);

private:
    Path* editPath;
    PathNode* editNode;
    bool editingAPath = false;
    bool handleChanges = true;

    QList<Path*> *paths;

    QWidget* edits;
    QGroupBox* nodeGroupbox;
    QListWidget* pathList;

    QSpinBox* id;
    QCheckBox* loop;
    QDoubleSpinBox* speed;
    QDoubleSpinBox* acceleration;
    QSpinBox* delay;
    QSpinBox* rotation;
    QSpinBox* variableField;
    QSpinBox* nextPathID;

    QSpinBox* nodeID;

    QUndoStack *undoStack;

    void updateList();
    void updateInfo();

private slots:
    void handlePathListIndexChanged(QListWidgetItem *item);
    void handleIDChanged(int idVal);
    void handleLoopChanged();
    void handleSpeedChanged(double speedVal);
    void handleAccelChanged(double accelVal);
    void handleDelayChanged(int delayVal);
    void handleRotationChanged(int rotation);
    void handleVariableFieldChanged(int variableField);
    void handleNextPathIDChanged(int nextPathID);

    void handleNodeIDChanged(int nodeID);

};

#endif // PATHEDITORWIDGET_H
