#ifndef ENTRANCEEDITOR_H
#define ENTRANCEEDITOR_H

#include "objects.h"

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QListWidget>
#include <QUndoStack>

class EntranceEditorWidget : public QWidget
{
    Q_OBJECT
public:
    EntranceEditorWidget(QList<Entrance*> *entrances, QUndoStack *undoStack, QWidget *parent = nullptr);
    void deselect();
    void select(Entrance* entr);
    void updateEditor();

signals:
    void selectedEntrChanged(Object* entr);

private:
    QListWidget *entrancesList;
    QWidget *edits;
    QComboBox *type;
    QSpinBox *id;
    QSpinBox *destId;
    QSpinBox *destArea;
    QSpinBox *camXOffset;
    QSpinBox *camYOffset;
    QSpinBox *unk1;
    QSpinBox *unk2;
    QCheckBox *enterable;
    QCheckBox *returnToWM;

    QList<Entrance*> *entrances;
    Entrance* editEntrance;
    QList<QString> entranceTypes;

    class HorLine : public QFrame { public: HorLine() { setFrameStyle(QFrame::HLine | QFrame::Sunken); } };

    void changeEvent(QEvent* event);

    void updateList();
    void updateInfo();
    void loadEntranceTypes();

    bool editingAnEntrance = false;
    bool handleChanges = true;

    QUndoStack *undoStack;

private slots:
    void handleEntranceListIndexChanged(QListWidgetItem* item);
    void handleTypeChanged(int typeVal);
    void handleIDChanged(int idVal);
    void handleDestEntrChange(int destEntrVal);
    void handleDestAreaChange(int destAreaVal);
    void handleCamXChange(int camXVal);
    void handleCamYChange(int camYVal);
    void handleUnk1Change(int unk1Val);
    void handleUnk2Change(int unk2Val);
    void handleEnterableChange(bool enterableVal);
    void handleReturnToWMChange(bool returnToWMVal);
};

#endif // ENTRANCEEDITOR_H
