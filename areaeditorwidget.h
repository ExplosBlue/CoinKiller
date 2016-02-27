#ifndef AREAEDITORWIDGET_H
#define AREAEDITORWIDGET_H

#include "level.h"

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QMap>

class AreaEditorWidget : public QWidget
{
    Q_OBJECT
public:
    AreaEditorWidget(Level* level);

private:
    Level* level;

    QSpinBox* timeLimit;
    QCheckBox* toadhouseFlag;
    QComboBox* specialLevelFlag1;
    QComboBox* specialLevelFlag2;

    QMap<int, QString> specialLevelFlags1;
    QMap<int, QString> specialLevelFlags2;

    bool handleChanges = true;
    void updateInfo();

    class HorLine : public QFrame { public: HorLine() { setFrameStyle(QFrame::HLine | QFrame::Sunken); } };

private slots:
    void handleTimeLimitChange(int timeLimitVal);
    void handleSpecialLevelFlag1Change(QString text);
    void handleSpecialLevelFlag2Change(QString text);
    void handleToadHouseFlagChanged(bool flag);
};

#endif // AREAEDITORWIDGET_H
