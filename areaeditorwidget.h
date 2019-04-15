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
    void tilesetEditMade();
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
    void editMade();
private:
    Level* level;
    Game* game;

    QSpinBox* timeLimit;
    QSpinBox* coinRushtimeLimit;
    QSpinBox* levelEntranceID;
    QComboBox* specialLevelFlag1;
    QComboBox* specialLevelFlag2;
    QSpinBox* unk1Editor;
    QSpinBox* unk2Editor;
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
    void handleUnk1Change(int unk);
    void handleUnk2Change(int unk);
    void handleLevelEntranceIDChanged(int id);

    void passUpdateLevelView() { emit updateLevelView(); }
    void passRelaodTilesetPicker() { emit relaodTilesetPicker(); }
    void passTilesetEditMade() { emit editMade(); }
};

#endif // AREAEDITORWIDGET_H
