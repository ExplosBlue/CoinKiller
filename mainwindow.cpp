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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filesystem.h"

#include "imagecache.h"

#include "leveleditorwindow.h"
#include "tileseteditorwindow.h"
#include "sarcexplorerwindow.h"
#include "newtilesetdialog.h"
#include "newleveldialog.h"
#include "spritedata.h"

MainWindow::MainWindow(WindowBase *parent) :
    WindowBase(parent),
    ui(new Ui::MainWindow)
{
    settings = SettingsManager::init(this);

    startupClose = checkForMissingFiles();

    ui->setupUi(this);

    if (startupClose)
    {
        QTimer::singleShot(0, this, SLOT(close()));
        this->move(0, -10000);
        return;
    }

    settings->loadTranslations();

    setWindowTitle("CoinKiller");

    statusLabel = new ClickableLabel(this);
    ui->statusBar->addWidget(statusLabel);
    statusLabel->setFrameShape(QFrame::NoFrame);
    connect(statusLabel, SIGNAL(doubleClicked()), this, SLOT(statusLabelClicked()));

    ImageCache::init();

    settings->setupLanguageSelector(ui->languageSelector);
    setGameLoaded(false);

    ui->nightModeCheckbox->setChecked(settings->get("nightmode", false).toBool());
    ui->maximisedCheckbox->setChecked(settings->get("maximised", false).toBool());
    ui->loadLastCheckbox->setChecked(settings->get("loadLastOnStart", false).toBool());

    ui->levelList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->levelList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createLevelListContextMenu(QPoint)));

    ui->tilesetView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tilesetView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createTilesetListContextMenu(QPoint)));

    if (settings->get("loadLastOnStart", false).toBool() && !settings->getLastRomFSPath().isEmpty())
    {
        loadGame(settings->getLastRomFSPath());
        ui->actionOpenlastROMFSDir->setEnabled(true);
    }

    // Check Spritedata.xml is the latest version
    SpriteData spriteData;
    if (spriteData.getVersion() != "2.0")
    {
        QMessageBox message(QMessageBox::Information, "CoinKiller", tr("Spritedata.xml is outdated, CoinKiller will now attempt to update it."), QMessageBox::Ok);
        message.exec();

        this->setEnabled(false);
        FileDownloader::download(QUrl("https://smbnext.net/spritedb/spritexml.php"), this, SLOT(sdDownload_finished(QNetworkReply::NetworkError, const QByteArray&, const QUrl&)));
    }
}

MainWindow::~MainWindow()
{
    if (!startupClose)
    {
        if (gameLoaded)
            delete game;

        delete ImageCache::getInstance();
        delete SettingsManager::getInstance();
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);

        if (gameLoaded)
        {
            ui->levelList->setModel(game->getCourseModel());
            ui->tilesetView->setModel(game->getTilesetModel());
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

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About CoinKiller"),
                             tr("CoinKiller v1.0 -- by StapleButter, RicBent and Explos\n\nhttp://kuribo64.net/\n\nDefault Icons by Icons8\n\nThis is free software, if you paid for it, you got scammed"));
}

void MainWindow::loadGame(const QString& path)
{
    settings->setLastRomFSPath(path);

    game = new Game(path);

    setGameLoaded(true);
    statusLabel->setText(tr("Loaded: %1").arg(path));

    ui->levelList->setModel(game->getCourseModel());

    ui->tilesetView->setHeaderHidden(false);
    ui->tilesetView->setModel(game->getTilesetModel());
    ui->tilesetView->setColumnWidth(0, 200);
}

void MainWindow::on_actionLoadUnpackedROMFS_triggered()
{
    QString basepath = settings->getLastRomFSPath();

    QString dirpath = QFileDialog::getExistingDirectory(this, tr("Select unpacked ROMFS Folder..."), basepath);
    if (dirpath.isNull())
        return;
    loadGame(dirpath);
}

void MainWindow::on_actionOpenlastROMFSDir_triggered()
{
    QString path = settings->getLastRomFSPath();
    if (path.isNull())
        return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::on_levelList_clicked(const QModelIndex &index)
{
    ui->removeLevelBtn->setEnabled(index.data(Qt::UserRole+1).toString() != "");
}

void MainWindow::on_levelList_doubleClicked(const QModelIndex &index)
{
    if (index.data(Qt::UserRole+1).isNull())
        return;

    QString path = index.data(Qt::UserRole+1).toString();

    LevelManager* lvlMgr = game->getLevelManager(this, path);
    lvlMgr->openAreaEditor(1);
}

void MainWindow::on_tilesetView_doubleClicked(const QModelIndex &index)
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
                                   .arg(QCoreApplication::applicationDirPath() + "/coinkiller_data/")
                                   .arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/coinkiller_data/")
                                   .arg(missingFiles);

        QMessageBox message(QMessageBox::Information, "CoinKiller", infoText, QMessageBox::Ok);
        message.exec();

        return true;
    }

    return false;
}

void MainWindow::on_updateSpriteData_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "CoinKiller", tr("Any changes made to your spritedata.xml will be overwritten!\n\nDo you want to proceed?"), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    this->setEnabled(false);

    FileDownloader::download(QUrl("https://smbnext.net/spritedb/spritexml.php"), this, SLOT(sdDownload_finished(QNetworkReply::NetworkError, const QByteArray&, const QUrl&)));
}

void MainWindow::sdDownload_finished(QNetworkReply::NetworkError error, const QByteArray& data, const QUrl& url)
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

void MainWindow::on_actionSarcExplorer_triggered()
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

void MainWindow::on_addLevelBtn_clicked()
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

    ui->levelList->setModel(game->getCourseModel());
}

void MainWindow::on_removeLevelBtn_clicked()
{
    QString selLvlName = ui->levelList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString();
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "CoinKiller", tr("Are you sure you want to remove %1? This action cannot be undone.").arg(selLvlName), QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (ui->levelList->selectionModel()->selectedIndexes().length() == 0 || ui->levelList->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString() == "")
            return;

        game->fs->deleteFile("/Course/" + selLvlName);
        ui->levelList->setModel(game->getCourseModel());

        ui->removeLevelBtn->setDisabled(true);
    }
}

void MainWindow::on_addTilesetBtn_clicked()
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
    ts.save();

    ui->tilesetView->setModel(game->getTilesetModel());
}

void MainWindow::on_removeTilesetBtn_clicked()
{
    if (ui->tilesetView->selectionModel()->selectedIndexes().length() == 0 || ui->tilesetView->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString() == "")
        return;

    QString selTsName = ui->tilesetView->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString();
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "CoinKiller", tr("Are you sure you want to remove %1? This action cannot be undone.").arg(selTsName), QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (ui->tilesetView->selectionModel()->selectedIndexes().length() == 0 || ui->tilesetView->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString() == "")
            return;

        game->fs->deleteFile("/Unit/" + selTsName + ".sarc");
        ui->tilesetView->setModel(game->getTilesetModel());

        ui->removeTilesetBtn->setDisabled(true);
    }
}

void MainWindow::on_tilesetView_clicked(const QModelIndex &index)
{
    ui->removeTilesetBtn->setEnabled(index.data(Qt::UserRole+1).toString() != "");
}

void MainWindow::on_nightModeCheckbox_toggled(bool checked)
{
    setNightmode(checked);
}

void MainWindow::setNightmode(bool nightmode)
{
    settings->set("nightmode", nightmode);

    if (nightmode)
        setStyleSheetFromPath("NightMode.qss");
    else
        setStyleSheetFromPath("");
}
  
void MainWindow::setStyleSheetFromPath(QString path)
{
    QFile styleSheetFile(settings->dataPath(path));

    if (!styleSheetFile.exists())
    {
        qApp->setStyleSheet("");
        return;
    }

    styleSheetFile.open(QFile::ReadOnly | QFile::Text);
    QString styleSheetTxt = QLatin1String(styleSheetFile.readAll());
    styleSheetFile.close();
    qApp->setStyleSheet(styleSheetTxt);
}

void MainWindow::on_maximisedCheckbox_toggled(bool checkState)
{
    settings->set("maximised", checkState);
}

void MainWindow::on_loadLastCheckbox_clicked(bool checked)
{
     settings->set("loadLastOnStart", checked);
}

void MainWindow::statusLabelClicked()
{
    if (!gameLoaded || game == nullptr)
        return;

    QString path = QString("file://%1").arg(QDir::toNativeSeparators(game->getPath()));
    QDesktopServices::openUrl(path);
}


void MainWindow::createLevelListContextMenu(const QPoint &pos)
{
    if (ui->levelList->indexAt(pos).data(Qt::UserRole+1).isNull())
        return;

    QMenu contextMenu(tr("Context menu"), this);

    QAction openLevel(tr("Open In Level Editor"), this);
    QAction sarcExplorer(tr("Open In Sarc Explorer"), this);
    QAction removeLevel(tr("Remove Level"), this);

    connect(&openLevel, SIGNAL(triggered()), this, SLOT(openLevelFromConextMenu()));
    connect(&sarcExplorer, SIGNAL(triggered()), this, SLOT(openInSarcExplorer()));
    connect(&removeLevel, SIGNAL(triggered()), this, SLOT(on_removeLevelBtn_clicked()));

    openLevel.setData(QVariant(pos));
    sarcExplorer.setData(QVariant(pos));

    contextMenu.addAction(&openLevel);
    contextMenu.addAction(&sarcExplorer);
    contextMenu.addSeparator();
    contextMenu.addAction(&removeLevel);

    contextMenu.exec(QCursor::pos());
}

void MainWindow::createTilesetListContextMenu(const QPoint &pos)
{
    if (ui->tilesetView->indexAt(pos).data(Qt::UserRole+1).isNull())
        return;

    QMenu contextMenu(tr("Context menu"), this);

    QAction openTileset(tr("Open In Tileset Editor"), this);
    QAction sarcExplorer(tr("Open In Sarc Explorer"), this);
    QAction removeTileset(tr("Remove Tileset"), this);

    connect(&openTileset, SIGNAL(triggered()), this, SLOT(openTilesetFromConextMenu()));
    connect(&sarcExplorer, SIGNAL(triggered()), this, SLOT(openInSarcExplorer()));
    connect(&removeTileset, SIGNAL(triggered()), this, SLOT(on_removeTilesetBtn_clicked()));

    openTileset.setData(QVariant(pos));
    sarcExplorer.setData(QVariant(pos));

    contextMenu.addAction(&openTileset);
    contextMenu.addAction(&sarcExplorer);
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

    QString path = ui->tilesetView->indexAt(pos).data(Qt::UserRole+1).toString();

    TilesetEditorWindow* tsEditor = new TilesetEditorWindow(this, game->getTileset(path));
    tsEditor->setAttribute(Qt::WA_DeleteOnClose);
    tsEditor->show();
}

void MainWindow::openInSarcExplorer()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QPoint pos = action->data().toPoint();

    QString path;

    if (ui->tabWidget->currentIndex() == 0)
    {
        path = ui->levelList->indexAt(pos).data(Qt::UserRole+1).toString();
        path.prepend(settings->getLastRomFSPath() + "/Course/");
    }
    else
    {
        path = ui->tilesetView->indexAt(pos).data(Qt::UserRole+1).toString();
        path.prepend(settings->getLastRomFSPath() + "/Unit/");
    }

    if (!path.endsWith(".sarc"))
        path.append(".sarc");

    SarcExplorerWindow* sarcExplorer = new SarcExplorerWindow(this, path, settings);
    sarcExplorer->show();
}
