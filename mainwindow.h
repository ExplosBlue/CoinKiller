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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "game.h"
#include "settingsmanager.h"
#include "filedownloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    
    void sdDownload_finished(QNetworkReply::NetworkError error);

    void on_actionAbout_triggered();

    void on_actionLoadUnpackedROMFS_triggered();

    void on_actionDebug_test_triggered();

    void on_levelList_doubleClicked(const QModelIndex &index);

    void on_tilesetView_doubleClicked(const QModelIndex &index);

    void on_updateSpriteData_clicked();

    void on_openSarcExplorerBtn_clicked();

    void on_addTilesetBtn_clicked();

    void on_nightModeBtn_clicked();

    void on_removeTilesetBtn_clicked();

    void on_tilesetView_clicked(const QModelIndex &index);

    void on_testButton_clicked();

private:
    Ui::MainWindow *ui;

    Game* game;
    bool gameLoaded;

    SettingsManager* settings;

    FileDownloader* sdDownloader;

    void setGameLoaded(bool loaded);
    bool startupClose = false;
    bool checkForMissingFiles();
    void loadTranslations();
};

#endif // MAINWINDOW_H
