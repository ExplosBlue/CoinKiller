#ifndef AREAEDITORWIDGET_H
#define AREAEDITORWIDGET_H

#include "level.h"
#include "eventeditorwidget.h"

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QMap>
#include <QTreeView>
#include <QUndoStack>

class AreaEditorWidget : public QWidget
{
    Q_OBJECT
public:
    AreaEditorWidget(Level *level, Game *game, QUndoStack *undoStack);

    void updateEditor();

signals:
    void relaodTilesetPicker();

private:
    Level *level;
    Game *game;

    QUndoStack *undoStack;

    QSpinBox *timeLimit;
    QSpinBox *coinRushtimeLimit;
    QSpinBox *levelEntranceID;
    QComboBox *specialLevelFlag1;
    QComboBox *specialLevelFlag2;
    QSpinBox *unk1Editor;
    QSpinBox *unk2Editor;

    EventEditorWidget *eventEditor;

    QMap<int, QString> specialLevelFlags1;
    QMap<int, QString> specialLevelFlags2;

    bool handleChanges = true;
    void updateInfo();

    class HorLine : public QFrame { public: HorLine() { setFrameStyle(QFrame::HLine | QFrame::Sunken); } };

private slots:
    void timeLimitChanged(quint16 timeLimitVal);
    void coinRushTimeLimitChanged(quint16 timeLimitVal);
    void specialLevelFlag1Changed(QString text);
    void specialLevelFlag2Changed(QString text);
    void levelEntranceIDChanged(quint8 id);
    void unk1Changed(quint16 val);
    void unk2Changed(quint8 val);
};

#endif // AREAEDITORWIDGET_H
