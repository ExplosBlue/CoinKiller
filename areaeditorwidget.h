#ifndef AREAEDITORWIDGET_H
#define AREAEDITORWIDGET_H

#include "level.h"

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QMap>
#include <QTreeWidget>

class TilesetChooser : public QTreeWidget
{
    Q_OBJECT
public:
    TilesetChooser(Level* level, int tsNbr, Game *game, QMap<QString, QString>* names);
private:
    Level* level;
    Game* game;
    int tsNbr;
private slots:
    void handleTilesetChange(QTreeWidgetItem* item, int);
signals:
    void updateLevelEditor();
    void relaodTilesetPicker();
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
