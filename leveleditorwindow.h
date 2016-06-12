/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

#ifndef LEVELEDITORWINDOW_H
#define LEVELEDITORWINDOW_H

#include <QMainWindow>
#include <QListView>

#include "filesystem.h"
#include "levelview.h"
#include "ctpk.h"
#include "level.h"
#include "areaeditorwidget.h"
#include "tilesetpalette.h"
#include "spriteeditorwidget.h"
#include "entranceeditorwidget.h"
#include "zoneeditorwidget.h"
#include "locationeditorwidget.h"

#include "propertygrid.h"

namespace Ui {
class LevelEditorWindow;
}

class LevelEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LevelEditorWindow(QWidget *parent, Game* game, int worldNbr, int levelNbr);
    ~LevelEditorWindow();

public slots:
    void setObjectEdition(Object* obj);
    void deselect();
    void updateEditors();

private slots:
    void selectedAreaChanged(int area);

    void on_actionToggleLayer1_toggled(bool arg1);

    void on_actionToggleLayer2_toggled(bool arg1);

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionZoom_100_triggered();

    void on_actionZoom_Maximum_triggered();

    void on_actionZoom_Minimum_triggered();

    void on_actionSave_triggered();

    void on_actionCopy_triggered();

    void on_actionFullscreen_toggled(bool arg1);

    void on_actionGrid_toggled(bool arg1);

    void on_actionPaste_triggered();

    void on_actionCut_triggered();

    void on_actionDelete_triggered();

    void setSelSprite(int spriteId);

    void on_sidebarTabWidget_currentChanged(int index);

    void on_actionAddArea_triggered();

    void on_actionDeleteCurrentArea_triggered();

private:
    Ui::LevelEditorWindow *ui;

    //SarcFilesystem* levelArchive;
    Level* level;
    Game* game;
    Tileset* tileset;
    LevelView* levelView;
    QComboBox* areaSelector;

    PropertyGrid* propGrid;

    AreaEditorWidget* areaEditor;
    TilesetPalette* tilesetPalette;
    SpriteEditorWidget* spriteEditor;
    EntranceEditorWidget* entranceEditor;
    ZoneEditorWidget* zoneEditor;
    LocationEditorWidget* locationEditor;

    int worldNbr;
    int levelNbr;
    int currArea;

    quint8 layerMask;
    float zoom;

    void loadArea(int area);
    void updateAreaSelector(int index=-1);
};

#endif // LEVELEDITORWINDOW_H
