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

#include "game.h"
#include "settingsmanager.h"
#include "clickablelabel.h"
#include "windowbase.h"

#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public WindowBase
{
    Q_OBJECT

public:
    explicit MainWindow(WindowBase *parent = nullptr);
    ~MainWindow() override;

private slots:
    void loadUnpackedROMFS();
    void showROMFSDir();
    void showAboutDialog();
    void openSarcExplorer();

    void levelListSelectedIndexChanged();
    void openLevelFromListIndex(const QModelIndex &index);

    void tilesetListSelectedIndexChanged();
    void openTilesetFromListIndex(const QModelIndex &index);

    void addLevel();
    void removeLevel();

    void addTileset();
    void removeTileset();

    void updateSpriteData();

    void setDarkMode(bool isDarkMode);
    void setShouldStartMaximized(bool shouldMaximize);
    void setShouldLoadLastROMFS(bool shouldLoad);

    void createLevelListContextMenu(const QPoint &pos);
    void createTilesetListContextMenu(const QPoint &pos);

    void openLevelFromConextMenu();
    void openTilesetFromConextMenu();

    void openInSarcExplorer();
    void showInFileExplorer();

    void handleWatchedDirectoryChanged(const QString &path);

    void sdDownloadFinished(QNetworkReply::NetworkError error, const QByteArray &data, const QUrl &url);

private:
    void loadGame(const QString &path);
    void setGameLoaded(bool loaded);

    void initialiseUi();
    void createContextMenus();
    void checkSpriteDataVersion();

    void loadLevelList();
    void loadTilesetList();

    void changeEvent(QEvent *event) override;

    bool checkForMissingFiles();

    QString getFilePath(QAction *action);

    Ui::MainWindow* ui {};
    Game* game {};
    SettingsManager* settings {};
    ClickableLabel* statusLabel {};

    QFileSystemWatcher fileSystemWatcher {};

    bool gameLoaded {};
    bool startupClose {};
};

#endif // MAINWINDOW_H
