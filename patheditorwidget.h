#ifndef PATHEDITORWIDGET_H
#define PATHEDITORWIDGET_H

#include "objects.h"

#include <QWidget>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>

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

public slots:

private:
    Path* editPath;
    PathNode* editNode;
    bool editingAPath = false;
    bool handleChanges = true;

    QList<Path*> *paths;

    QWidget* edits;
    QListWidget* pathList;

    QSpinBox* id;
    QDoubleSpinBox* speed;
    QDoubleSpinBox* acceleration;

    class HorLine : public QFrame { public: HorLine() { setFrameStyle(QFrame::HLine | QFrame::Sunken); } };

    void updateList();
    void updateInfo();

private slots:
    void handlePathListIndexChanged(QListWidgetItem *item);
    void handleIDChanged(int idVal);
    void handleSpeedChanged(double speedVal);
    void handleAccelChanged(double accelVal);

};

#endif // PATHEDITORWIDGET_H
