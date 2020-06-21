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

class PathEditorWidget : public QWidget
{
    Q_OBJECT
public:
    PathEditorWidget(QList<Path*> *paths);
    void deselect();
    void select(PathNode* node);
    void updateEditor();

signals:
    void updateLevelView();
    void selectedPathChanged(Object* pPath);
    void editMade();

public slots:

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
};

#endif // PATHEDITORWIDGET_H
