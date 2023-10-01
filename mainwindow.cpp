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

#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QCoreApplication>
#include <QFileDialog>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QStyleFactory>
#include <QFileSystemWatcher>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imagecache.h"

#include "tileseteditor/tileseteditorwindow.h"
#include "sarcexplorerwindow.h"
#include "newtilesetdialog.h"
#include "newleveldialog.h"
#include "spritedata.h"
#include "filedownloader.h"

MainWindow::MainWindow(WindowBase *parent) :
    WindowBase(parent), ui(new Ui::MainWindow)
{
    settings = SettingsManager::init(this);

    startupClose = checkForMissingFiles();
    if (startupClose)
    {
        QTimer::singleShot(0, this, &MainWindow::close);
        this->move(0, -10000);
        return;
    }

    ImageCache::init();

    settings->loadTranslations();
    initialiseUi();
    createContextMenus();
    checkSpriteDataVersion();
}

MainWindow::~MainWindow()
{
    if (!startupClose)
    {
        if (gameLoaded)
            delete game;

        delete ImageCache::getInstance();
    }
    delete SettingsManager::getInstance();
    delete ui;
}

void MainWindow::initialiseUi()
{
    ui->setupUi(this);

    // ui signals/slots
    connect(ui->actionLoadUnpackedROMFS, &QAction::triggered, this, &MainWindow::loadUnpackedROMFS);
    connect(ui->actionShowROMFSDir,      &QAction::triggered, this, &MainWindow::showROMFSDir);
    connect(ui->actionAbout,             &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->actionSarcExplorer,      &QAction::triggered, this, &MainWindow::openSarcExplorer);

    connect(ui->levelList, &QTreeView::clicked,       this, &MainWindow::levelListSelectedIndexChanged);
    connect(ui->levelList, &QTreeView::doubleClicked, this, &MainWindow::openLevelFromListIndex);

    connect(ui->tilesetList, &QTreeView::clicked,       this, &MainWindow::tilesetListSelectedIndexChanged);
    connect(ui->tilesetList, &QTreeView::doubleClicked, this, &MainWindow::openTilesetFromListIndex);

    connect(ui->addLevelBtn,    &QPushButton::clicked, this, &MainWindow::addLevel);
    connect(ui->removeLevelBtn, &QPushButton::clicked, this, &MainWindow::removeLevel);

    connect(ui->addTilesetBtn,    &QPushButton::clicked, this, &MainWindow::addTileset);
    connect(ui->removeTilesetBtn, &QPushButton::clicked, this, &MainWindow::removeTileset);

    connect(ui->updateSpriteData, &QPushButton::clicked, this, &MainWindow::updateSpriteData);

    connect(ui->darkModeCheckbox,  &QCheckBox::toggled, this, &MainWindow::setDarkMode);
    connect(ui->maximisedCheckbox, &QCheckBox::toggled, this, &MainWindow::setShouldStartMaximized);
    connect(ui->loadLastCheckbox,  &QCheckBox::toggled, this, &MainWindow::setShouldLoadLastROMFS);

    setWindowTitle("CoinKiller");

    statusLabel = new ClickableLabel(this);
    ui->statusBar->addWidget(statusLabel);
    statusLabel->setFrameShape(QFrame::NoFrame);
    connect(statusLabel, &ClickableLabel::doubleClicked, this, &MainWindow::showROMFSDir);

    ui->darkModeCheckbox->setChecked(settings->get("nightmode", false).toBool());
    ui->maximisedCheckbox->setChecked(settings->get("maximised", false).toBool());
    ui->loadLastCheckbox->setChecked(settings->get("loadLastOnStart", false).toBool());

    settings->setupLanguageSelector(ui->languageSelector);

    if (settings->get("loadLastOnStart", false).toBool() && !settings->getLastRomFSPath().isEmpty())
    {
        loadGame(settings->getLastRomFSPath());
        ui->actionShowROMFSDir->setEnabled(true);
    }
    else
    {
        setGameLoaded(false);
    }

    connect(&fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::handleWatchedDirectoryChanged);
}

void MainWindow::createContextMenus()
{
    ui->levelList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->levelList, &QWidget::customContextMenuRequested, this, &MainWindow::createLevelListContextMenu);

    ui->tilesetList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tilesetList, &QWidget::customContextMenuRequested, this, &MainWindow::createTilesetListContextMenu);
}

void MainWindow::checkSpriteDataVersion()
{
    SpriteData spriteData;

    if (spriteData.getVersion() != "2.0")
    {
        QMessageBox message(QMessageBox::Information, "CoinKiller", tr("Spritedata.xml is outdated, CoinKiller will now attempt to update it."), QMessageBox::Ok);
        message.exec();

        this->setEnabled(false);
        FileDownloader::download(QUrl("https://smbnext.net/spritedb/spritexml.php"), this, SLOT(sdDownloadFinished(QNetworkReply::NetworkError, const QByteArray&, const QUrl&)));
    }
}

void MainWindow::loadLevelList()
{
    QStandardItemModel *courseModel = game->getCourseModel();
    courseModel->setParent(this);

    if (ui->levelList->model() != nullptr) {
        delete ui->levelList->model();
    }

    ui->levelList->setModel(courseModel);
}


void MainWindow::loadTilesetList()
{
    QStandardItemModel *tilesetModel = game->getTilesetModel();
    tilesetModel->setParent(this);

    if (ui->tilesetList->model() != nullptr) {
        delete ui->tilesetList->model();
    }

    ui->tilesetList->setModel(tilesetModel);
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);

        if (gameLoaded)
        {
            loadLevelList();
            loadTilesetList();
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::setGameLoaded(bool loaded)
{
    if (!loaded)
        statusLabel->setText(tr("No ROMFS loaded."));
    ui->tab_tilesets->setEnabled(loaded);
    gameLoaded = loaded;
}

void MainWindow::showAboutDialog()
{
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::MarkdownText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowTitle(tr("About CoinKiller"));
    msgBox.setText(tr("# CoinKiller\n\n"
                      "Version: %1\n\n"
                      "Github: https://github.com/Arisotura/CoinKiller\n\n"
                      "## Credits\n"
                      "StapleButter, RicBent and Explos\n\n"
                      "Icons from [Icons8](https://icons8.com/)\n\n"
                      "CoinKiller is free software: "
                      "you can redistribute it and/or modify it under the terms of the GNU General Public License "
                      "as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.").arg(CK_VERSION));
    msgBox.exec();
}

void MainWindow::loadGame(const QString& path)
{
    settings->setLastRomFSPath(path);

    game = new Game(path);

    setGameLoaded(true);
    statusLabel->setText(tr("Loaded: %1").arg(path));

    loadLevelList();

    ui->tilesetList->setHeaderHidden(false);
    loadTilesetList();
    ui->tilesetList->setColumnWidth(0, 200);

    if (!fileSystemWatcher.directories().isEmpty())
        fileSystemWatcher.removePaths(fileSystemWatcher.directories());

    QStringList watchPaths;
    watchPaths
        << path + "/Course/"
        << path + "/Unit/";

    fileSystemWatcher.addPaths(watchPaths);
}

void MainWindow::loadUnpackedROMFS()
{
    QString basepath = settings->getLastRomFSPath();
    QString dirpath = QFileDialog::getExistingDirectory(this, tr("Select unpacked ROMFS Folder..."), basepath);
    if (dirpath.isNull()) return;

    loadGame(dirpath);
}

void MainWindow::showROMFSDir()
{
    if (!gameLoaded || game == nullptr) return;

    QString path = settings->getLastRomFSPath();
    if (path.isNull()) return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::levelListSelectedIndexChanged()
{
    bool selectedIndexIsLevel = ui->levelList->currentIndex().data(Qt::UserRole+1).toString() != "";
    ui->removeLevelBtn->setEnabled(selectedIndexIsLevel);
}

void MainWindow::openLevelFromListIndex(const QModelIndex &index)
{
    if (index.data(Qt::UserRole+1).isNull())
        return;

    QString path = index.data(Qt::UserRole+1).toString();

    LevelManager* lvlMgr = game->getLevelManager(this, path);
    lvlMgr->openAreaEditor(1);
}

void MainWindow::openTilesetFromListIndex(const QModelIndex &index)
{
    if (index.data(Qt::UserRole+1).isNull())
        return;

    QString data = index.data(Qt::UserRole+1).toString();

    TilesetEditorWindow* tsEditor = new TilesetEditorWindow(this, game->getTileset(data));
    tsEditor->setAttribute(Qt::WA_DeleteOnClose);
    tsEditor->show();
}

bool MainWindow::checkForMissingFiles()
{
    QStringList requiredFiles;

    requiredFiles
    << "blank_course.bin"
    << "entrancetypes.txt"
    << "levelnames.txt"
    << "worldnames.txt"
    << "musicids.txt"
    << "spritecategories.xml"
    << "spritedata.xml"
    << "tilebehaviors.xml"
    << "tilesetnames.txt";

    QList<int> missingFilesIds;

    for (int i = 0; i<requiredFiles.size(); i++)
    {
        if (!QFile(settings->dataPath(requiredFiles[i])).exists())
            missingFilesIds.append(i);
    }

    if (!missingFilesIds.isEmpty())
    {
        QString missingFiles;
        for (int i : missingFilesIds)
            missingFiles.append(QString("<li>/coinkiller_data/%1</li>").arg(requiredFiles[i]));

        QString infoText = QString("CoinKiller was unable to find a coinkiller_data folder beside the executable or in the application data folder.<br>"
                                   "<br>"
                                   "Please place the coinkiller_data folder from the CoinKiller download at one of these locations:"
                                   "<ul><li>%1</li><li>%2</li></ul>"
                                   "Missing files:"
                                   "<ul>%3</ul>")
                                   .arg(
                                    QCoreApplication::applicationDirPath() + "/coinkiller_data/",
                                    QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/coinkiller_data/",
                                    missingFiles);

        QMessageBox message(QMessageBox::Information, "CoinKiller", infoText, QMessageBox::Ok);
        message.exec();

        return true;
    }

    return false;
}

void MainWindow::updateSpriteData()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "CoinKiller", tr("Any changes made to your spritedata.xml will be overwritten!\n\nDo you want to proceed?"), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    this->setEnabled(false);

    FileDownloader::download(QUrl("https://smbnext.net/spritedb/spritexml.php"), this, SLOT(sdDownloadFinished(QNetworkReply::NetworkError, const QByteArray&, const QUrl&)));
}

void MainWindow::sdDownloadFinished(QNetworkReply::NetworkError error, const QByteArray& data, const QUrl& url)
{
    if (error == QNetworkReply::NoError)
    {
        QFile file(settings->dataPath("spritedata.xml"));
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(data);
            file.close();
            QMessageBox::information(this, "CoinKiller", tr("Your spritedata was updated successfully!"), QMessageBox::Ok);
        }
        else
        {
            QMessageBox::information(this, "CoinKiller", tr("Updating the spritedata failed:\n\nCould not open spritedata.xml for writing!"), QMessageBox::Ok);
            this->setEnabled(true);
        }
    }
    else
        QMessageBox::information(this, "CoinKiller", tr("Updating the spritedata failed:\n\nConnection to %1 failed!").arg(url.toString()), QMessageBox::Ok);

    this->setEnabled(true);
}

void MainWindow::openSarcExplorer()
{
    QString basePath = "";
    if (!settings->getLastSarcFilePath().isEmpty())
        basePath = settings->getLastSarcFilePath();
    else
        basePath = QCoreApplication::applicationDirPath();

    QString sarcFilePath = QFileDialog::getOpenFileName(this, tr("Select a Sarc Archive"), basePath, tr("Sarc Archives (*.sarc)"));

    if (sarcFilePath.isEmpty() || sarcFilePath.isEmpty())
        return;

    QStringList lastPath = sarcFilePath.split('/');
    lastPath.removeLast();
    QString last = lastPath.join("/");
    settings->setLastSarcFilePath(last);

    SarcExplorerWindow* sarcExplorer = new SarcExplorerWindow(this, sarcFilePath, settings);
    sarcExplorer->show();
}

void MainWindow::addLevel()
{
    QFile blankLvl(settings->dataPath("blank_level.sarc"));
    if (!blankLvl.exists())
    {
        QMessageBox::information(this, "CoinKiller", tr("You cannot create new levels without the level template.") + " (/coinkiller_data/blank_level.sarc).", QMessageBox::StandardButton::Ok);
        return;
    }

    NewLevelDialog nld(this, settings);
    int result = nld.exec();

    if (result != QDialog::Accepted)
        return;

    if (game->fs->fileExists("/Course/" + nld.getName() + ".sarc"))
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(this, "CoinKiller", tr("%1 already exists, would you like to overwrite it? This action cannot be undone.").arg(nld.getName() + ".sarc"), QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            game->fs->deleteFile("/Course/" + nld.getName() + ".sarc");
        }
        else return;
    }

    if (nld.getName().contains("/"))
    {
        QStringList path = nld.getName().split("/");
        path.removeLast();
        QDir().mkpath(settings->getLastRomFSPath() + "/Course/" + path.join("/"));
    }

    blankLvl.copy(settings->getLastRomFSPath() + "/Course/" + nld.getName() + ".sarc");

    loadLevelList();
}

void MainWindow::removeLevel()
{
    QString selLvlName = ui->levelList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString();
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "CoinKiller", tr("Are you sure you want to remove %1? This action cannot be undone.").arg(selLvlName), QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (ui->levelList->selectionModel()->selectedIndexes().length() == 0 || ui->levelList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString() == "")
            return;

        game->fs->deleteFile("/Course/" + selLvlName);
        loadLevelList();

        ui->removeLevelBtn->setDisabled(true);
    }
}

void MainWindow::addTileset()
{
    QFile blankTs(settings->dataPath("blank_tileset.sarc"));
    if (!blankTs.exists())
    {
        QMessageBox::information(this, "CoinKiller", tr("You cannot create new tilesets without the tileset template.") + " (/coinkiller_data/blank_tileset.sarc).", QMessageBox::StandardButton::Ok);
        return;
    }

    NewTilesetDialog ntd(this, settings);
    int result = ntd.exec();

    if (result != QDialog::Accepted)
        return;

    if (game->fs->fileExists("/Unit/" + ntd.getName() + ".sarc"))
    {
        QMessageBox::information(this, "CoinKiller", tr("A tileset with that name already exists."), QMessageBox::StandardButton::Ok);
        return;
    }

    blankTs.copy(settings->getLastRomFSPath() + "/Unit/" + ntd.getName() + ".sarc");

    SarcFilesystem sarc(game->fs->openFile("/Unit/" + ntd.getName() + ".sarc"));
    sarc.renameFile("BG_chk/d_bgchk_REPLACE.bin", "d_bgchk_" + ntd.getName() + ".bin");
    sarc.renameFile("BG_tex/REPLACE.ctpk", ntd.getName() + ".ctpk");
    sarc.renameFile("BG_unt/REPLACE.bin", ntd.getName() + ".bin");
    sarc.renameFile("BG_unt/REPLACE_add.bin", ntd.getName() + "_add.bin");
    sarc.renameFile("BG_unt/REPLACE_hd.bin", ntd.getName() + "_hd.bin");

    Tileset ts(game, ntd.getName());
    ts.setSlot(ntd.getSlot());
    ts.setInternalName(ntd.getName());
    ts.save();

    loadTilesetList();
}

void MainWindow::removeTileset()
{
    if (ui->tilesetList->selectionModel()->selectedIndexes().length() == 0 || ui->tilesetList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString() == "")
        return;

    QString selTsName = ui->tilesetList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString();
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "CoinKiller", tr("Are you sure you want to remove %1? This action cannot be undone.").arg(selTsName), QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (ui->tilesetList->selectionModel()->selectedIndexes().length() == 0 || ui->tilesetList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString() == "")
            return;

        game->fs->deleteFile("/Unit/" + selTsName + ".sarc");
        loadTilesetList();

        ui->removeTilesetBtn->setDisabled(true);
    }
}

void MainWindow::tilesetListSelectedIndexChanged()
{
    bool selectedIndexIsTileset = ui->tilesetList->currentIndex().data(Qt::UserRole+1).toString() != "";
    ui->removeTilesetBtn->setEnabled(selectedIndexIsTileset);
}

void MainWindow::setDarkMode(bool isDarkMode)
{
    settings->set("nightmode", isDarkMode);

    if (isDarkMode)
    {
        qApp->setStyle(QStyleFactory::create("Fusion"));

        QPalette darkPalette;

        QColor primaryColor = QColor(53, 53, 53);
        QColor secondaryColor = QColor(25, 25, 25);
        QColor accentColor = QColor(42, 130, 218);
        QColor disabledColor = QColor(97, 97, 97);
        QColor textColor = QColor(255, 255, 255);

        darkPalette.setColor(QPalette::Window, primaryColor);
        darkPalette.setColor(QPalette::WindowText, textColor);

        darkPalette.setColor(QPalette::Base, secondaryColor);
        darkPalette.setColor(QPalette::AlternateBase, primaryColor);

        darkPalette.setColor(QPalette::ToolTipBase, textColor);
        darkPalette.setColor(QPalette::ToolTipText, textColor);

        darkPalette.setColor(QPalette::PlaceholderText, Qt::lightGray);
        darkPalette.setColor(QPalette::Text, textColor);

        darkPalette.setColor(QPalette::Button, primaryColor);
        darkPalette.setColor(QPalette::ButtonText, textColor);

        darkPalette.setColor(QPalette::BrightText, Qt::red);

        darkPalette.setColor(QPalette::Link, accentColor);

        darkPalette.setColor(QPalette::Highlight, accentColor);
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

        qApp->setPalette(darkPalette);
// TODO: Delete me
//        QString darkStylesheet = QString("QToolTip { color: %1; background-color: %2; border: 1px solid %3; }").arg(
//                                         textColor.name(), primaryColor.name(), textColor.name());

//        qApp->setStyleSheet(darkStylesheet);
    }
    else
    {
        QStringList styleKeys = QStyleFactory::keys();
        qApp->setStyle(styleKeys.first());
        qApp->setStyleSheet("");
        qApp->setPalette(qApp->style()->standardPalette());
        return;
    }
}

void MainWindow::setShouldStartMaximized(bool shouldMaximize)
{
    settings->set("maximised", shouldMaximize);
}

void MainWindow::setShouldLoadLastROMFS(bool shouldLoad)
{
     settings->set("loadLastOnStart", shouldLoad);
}

void MainWindow::createLevelListContextMenu(const QPoint &pos)
{
    if (ui->levelList->indexAt(pos).data(Qt::UserRole+1).isNull())
        return;

    QMenu contextMenu(tr("Context menu"), this);

    QAction openLevel(tr("Open In Level Editor"), this);
    QAction sarcExplorer(tr("Open In Sarc Explorer"), this);
    QAction fileExplorer(tr("Show In File Explorer"), this);
    QAction removeLevel(tr("Remove Level"), this);

    connect(&openLevel,    &QAction::triggered, this, &MainWindow::openLevelFromConextMenu);
    connect(&sarcExplorer, &QAction::triggered, this, &MainWindow::openInSarcExplorer);
    connect(&fileExplorer, &QAction::triggered, this, &MainWindow::showInFileExplorer);
    connect(&removeLevel,  &QAction::triggered, this, &MainWindow::removeLevel);

    openLevel.setData(QVariant(pos));
    sarcExplorer.setData(QVariant(pos));
    fileExplorer.setData(QVariant(pos));

    contextMenu.addAction(&openLevel);
    contextMenu.addAction(&sarcExplorer);
    contextMenu.addAction(&fileExplorer);
    contextMenu.addSeparator();
    contextMenu.addAction(&removeLevel);

    contextMenu.exec(QCursor::pos());
}

void MainWindow::createTilesetListContextMenu(const QPoint &pos)
{
    if (ui->tilesetList->indexAt(pos).data(Qt::UserRole+1).isNull())
        return;

    QMenu contextMenu(tr("Context menu"), this);

    QAction openTileset(tr("Open In Tileset Editor"), this);
    QAction sarcExplorer(tr("Open In Sarc Explorer"), this);
    QAction fileExplorer(tr("Show In File Explorer"), this);
    QAction removeTileset(tr("Remove Tileset"), this);

    connect(&openTileset,   &QAction::triggered, this, &MainWindow::openTilesetFromConextMenu);
    connect(&sarcExplorer,  &QAction::triggered, this, &MainWindow::openInSarcExplorer);
    connect(&fileExplorer,  &QAction::triggered, this, &MainWindow::showInFileExplorer);
    connect(&removeTileset, &QAction::triggered, this, &MainWindow::removeTileset);

    openTileset.setData(QVariant(pos));
    sarcExplorer.setData(QVariant(pos));
    fileExplorer.setData(QVariant(pos));

    contextMenu.addAction(&openTileset);
    contextMenu.addAction(&sarcExplorer);
    contextMenu.addAction(&fileExplorer);
    contextMenu.addSeparator();
    contextMenu.addAction(&removeTileset);

    contextMenu.exec(QCursor::pos());
}

void MainWindow::openLevelFromConextMenu()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QPoint pos = action->data().toPoint();

    QString path = ui->levelList->indexAt(pos).data(Qt::UserRole+1).toString();

    LevelManager* lvlMgr = game->getLevelManager(this, path);
    lvlMgr->openAreaEditor(1);
}

void MainWindow::openTilesetFromConextMenu()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QPoint pos = action->data().toPoint();

    QString path = ui->tilesetList->indexAt(pos).data(Qt::UserRole+1).toString();

    TilesetEditorWindow* tsEditor = new TilesetEditorWindow(this, game->getTileset(path));
    tsEditor->setAttribute(Qt::WA_DeleteOnClose);
    tsEditor->show();
}

void MainWindow::openInSarcExplorer()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString path = getFilePath(action);

    SarcExplorerWindow* sarcExplorer = new SarcExplorerWindow(this, path, settings);
    sarcExplorer->show();
}

void MainWindow::showInFileExplorer()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString path = getFilePath(action);

    QFileInfo info(path);
#if defined(Q_OS_WIN)
    QStringList args;
    if (!info.isDir())
        args << "/select,";
    args << QDir::toNativeSeparators(path);
    if (QProcess::startDetached("explorer", args))
        return;
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \"" + path + "\"";
    args << "-e";
    args << "end tell";
    args << "-e";
    args << "return";
    if (!QProcess::execute("/usr/bin/osascript", args))
        return;
#elif defined(Q_OS_LINUX)
    QStringList args;
    args << "--session";
    args << "--dest=org.freedesktop.FileManager1";
    args << "--type=method_call";
    args << "/org/freedesktop/FileManager1";
    args << "org.freedesktop.FileManager1.ShowItems";
    args << "array:string:file://" + path;
    args << "string:";
    if (QProcess::startDetached("dbus-send", args))
        return;
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
}

QString MainWindow::getFilePath(QAction* action)
{
    QPoint pos = action->data().toPoint();

    QString path;

    if (ui->tabWidget->currentIndex() == 0)
    {
        path = ui->levelList->indexAt(pos).data(Qt::UserRole+1).toString();
        path.prepend(settings->getLastRomFSPath() + "/Course/");
    }
    else
    {
        path = ui->tilesetList->indexAt(pos).data(Qt::UserRole+1).toString();
        path.prepend(settings->getLastRomFSPath() + "/Unit/");
    }

    if (!path.endsWith(".sarc"))
        path.append(".sarc");

    return path;
}


void MainWindow::handleWatchedDirectoryChanged(const QString& path)
{
    if (path.contains("Course"))
    {
        loadLevelList();
    }
    else if (path.contains("Unit"))
    {
        loadTilesetList();
    }
}
