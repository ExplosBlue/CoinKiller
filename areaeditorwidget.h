#ifndef AREAEDITORWIDGET_H
#define AREAEDITORWIDGET_H

#include "level.h"

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QMap>
#include <QTreeView>

class TilesetChooser : public QTabWidget
{
    Q_OBJECT
public:
    TilesetChooser(Level* level, Game* game);
signals:
    void updateLevelEditor();
    void relaodTilesetPicker();
private slots:
    void handleTilesetChange(QModelIndex index);
private:
    QTreeView* tilesetTrees[4];
    Game* game;
    Level* level;
};

class AreaEditorWidget : public QWidget
{
    Q_OBJECT
public:
    AreaEditorWidget(Level* level, Game* game);

signals:
    void updateLevelView();
    void relaodTilesetPicker();

private:
    Level* level;
    Game* game;

    QSpinBox* timeLimit;
    QSpinBox* coinRushtimeLimit;
    QCheckBox* toadhouseFlag;
    QComboBox* specialLevelFlag1;
    QComboBox* specialLevelFlag2;
    TilesetChooser* tsChooser;

    QMap<int, QString> specialLevelFlags1;
    QMap<int, QString> specialLevelFlags2;

    bool handleChanges = true;
    void updateInfo();

    class HorLine : public QFrame { public: HorLine() { setFrameStyle(QFrame::HLine | QFrame::Sunken); } };

private slots:
    void handleTimeLimitChange(int timeLimitVal);
    void handleCoinRushTimeLimitChange(int timeLimitVal);
    void handleSpecialLevelFlag1Change(QString text);
    void handleSpecialLevelFlag2Change(QString text);
    void handleToadHouseFlagChanged(bool flag);

    void passUpdateLevelView() { emit updateLevelView(); }
    void passRelaodTilesetPicker() { emit relaodTilesetPicker(); }
};

#endif // AREAEDITORWIDGET_H
