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
#include <QDesktopWidget>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filesystem.h"

#include "leveleditorwindow.h"
#include "tileseteditorwindow.h"
#include "sarcexplorerwindow.h"
#include "newtilesetdialog.h"
#include "sillytest.h" // REMOVE ME!!

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    startupClose = checkForMissingFiles();

    ui->setupUi(this);

    if (startupClose)
    {
        QTimer::singleShot(0, this, SLOT(close()));
        this->move(0, -10000);
        return;
    }

    setWindowTitle("CoinKiller");

    QCoreApplication::setOrganizationName("Blarg City");
    QCoreApplication::setApplicationName("CoinKiller");
    settings = SettingsManager::init(this);

    loadTranslations();
    settings->setupLanguageSelector(ui->languageSelector);
    setGameLoaded(false);

    ui->nightModeCheckbox->setChecked(settings->get("nightmode", false).toBool());
}

MainWindow::~MainWindow()
{
    if (!startupClose)
    {
        if (gameLoaded)
            delete game;
    }
    delete ui;
}

void MainWindow::setGameLoaded(bool loaded)
{
    ui->tab_tilesets->setEnabled(loaded);
    gameLoaded = loaded;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About that thing you're using",
                             "CoinKiller v1.0 -- by StapleButter and RicBent\n\nhttp://kuribo64.net/ or whatever\n\nDefault Icons by Icons8\n\noh and this is free software, if you paid for it, you got scammed");
}

void MainWindow::on_actionLoadUnpackedROMFS_triggered()
{
    // full tile: 24x24
    // gfx: 20x20

    QString basepath = settings->getLastRomFSPath();

    QString dirpath = QFileDialog::getExistingDirectory(this, settings->getTranslation("MainWindow", "selectUnpackedRomFSFolder"), basepath);
    if (dirpath.isNull())
        return; // whatever

    settings->setLastRomFSPath(dirpath);


    FilesystemBase* fs = new ExternalFilesystem(dirpath);
    game = new Game(fs);

    setGameLoaded(true);

    ui->levelList->setModel(game->getCourseModel());

    ui->tilesetView->setHeaderHidden(false);
    ui->tilesetView->setModel(game->getTilesetModel());
    ui->tilesetView->setColumnWidth(0, 200);
}

void MainWindow::on_actionDebug_test_triggered()
{
    SillyTest* silly = new SillyTest(this);
    silly->show();
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
    tsEditor->show();
}

bool MainWindow::checkForMissingFiles()
{
    QStringList requiredFiles;

    requiredFiles
    << "blank_course.bin"
    << "entrancetypes.txt"
    << "levelnames.xml"
    << "musicids.txt"
    << "spritecategories.xml"
    << "spritedata.xml"
    << "tilebehaviors.xml"
    << "tilesetnames.txt"
    << "languages/English/translations.txt";

    QString basePath = QCoreApplication::applicationDirPath();
    QString missingFiles;
    for (int i=0; i<requiredFiles.size(); i++)
    {
        if (!QFile(basePath + "/coinkiller_data/" + requiredFiles[i]).exists())
        {
            missingFiles.append(QString("/coinkiller_data/%1\n").arg(requiredFiles[i]));
        }
    }
    if (!missingFiles.isEmpty())
    {
        QString infoText("There are files missing which are required for CoinKiller to work properly:\n%1\nPlease redownload your copy of the editor.");
        QMessageBox message(QMessageBox::Information, "CoinKiller", infoText.arg(missingFiles), QMessageBox::Ok, QDesktopWidget().screen());
        message.exec();
        return true;
    }

    return false;
}

void MainWindow::loadTranslations()
{
    ui->menuFile->setTitle(settings->getTranslation("General", "file"));
    ui->menuHelp->setTitle(settings->getTranslation("General", "help"));
    ui->actionAbout->setText(settings->getTranslation("MainWindow", "aboutCoinKiller"));
    ui->actionLoadUnpackedROMFS->setText(settings->getTranslation("MainWindow", "loadUnpackedRomFS"));
    ui->tabWidget->setTabText(0, settings->getTranslation("MainWindow", "levels"));
    ui->tabWidget->setTabText(1, settings->getTranslation("MainWindow", "tilesets"));
    ui->addTilesetBtn->setText(settings->getTranslation("MainWindow", "addTileset"));
    ui->removeTilesetBtn->setText(settings->getTranslation("MainWindow", "removeTileset"));
    ui->tabWidget->setTabText(2, settings->getTranslation("General", "settings"));
    ui->languagesLabel->setText(settings->getTranslation("MainWindow", "languages")+":");
    ui->updateSpriteData->setText(settings->getTranslation("MainWindow", "updateSDat"));
    ui->tabWidget->setTabText(3, settings->getTranslation("General", "tools"));
    ui->openSarcExplorerBtn->setText(settings->getTranslation("SarcExplorer", "sarcExplorer"));
    ui->nightModeCheckbox->setText(settings->getTranslation("MainWindow", "NightMode"));
}

void MainWindow::on_updateSpriteData_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "CoinKiller", settings->getTranslation("MainWindow", "sDatWarning"), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    this->setEnabled(false);

    QUrl sdUrl("http://kuribo64.net/nsmb2/spritexml2.php");
    sdDownloader = new FileDownloader(sdUrl, this);

    connect(sdDownloader, SIGNAL(downloaded(QNetworkReply::NetworkError)), this, SLOT(sdDownload_finished(QNetworkReply::NetworkError)));
}

void MainWindow::sdDownload_finished(QNetworkReply::NetworkError error)
{
    if (error == QNetworkReply::NoError)
    {
        QFile file(QCoreApplication::applicationDirPath() + "/coinkiller_data/spritedata.xml");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(sdDownloader->downloadedData());
            file.close();
            QMessageBox::information(this, "CoinKiller", settings->getTranslation("MainWindow", "sDatSuccess"), QMessageBox::Ok);
        }
        else
        {
            QMessageBox::information(this, "CoinKiller", settings->getTranslation("MainWindow", "sDatErrorFile"), QMessageBox::Ok);
        }
    }
    else
        QMessageBox::information(this, "CoinKiller", settings->getTranslation("MainWindow", "sDatErrorNetwork"), QMessageBox::Ok);

    this->setEnabled(true);
}

void MainWindow::on_openSarcExplorerBtn_clicked()
{
    QString sarcFilePath = QFileDialog::getOpenFileName(this, settings->getTranslation("SarcExplorer", "selectArchive"), settings->getLastRomFSPath(), settings->getTranslation("SarcExplorer", "sarcArchives") + " (*.sarc)");

    if (sarcFilePath.isEmpty() || sarcFilePath.isEmpty())
        return;

    SarcExplorerWindow* sarcExplorer = new SarcExplorerWindow(this, sarcFilePath, settings);
    sarcExplorer->show();
}

void MainWindow::on_addTilesetBtn_clicked()
{
    QFile blankTs(QCoreApplication::applicationDirPath() + "/coinkiller_data/blank_tileset.sarc");
    if (!blankTs.exists())
    {
        QMessageBox::information(this, "CoinKiller", settings->getTranslation("MainWindow", "blankTilesetMissing") + " (/coinkiller_data/blank_tileset.sarc).", QMessageBox::StandardButton::Ok);
        return;
    }

    NewTilesetDialog ntd(this, settings);
    int result = ntd.exec();

    if (result != QDialog::Accepted)
        return;

    if (game->fs->fileExists("/Unit/" + ntd.getName() + ".sarc"))
    {
        QMessageBox::information(this, "CoinKiller", settings->getTranslation("MainWindow", "tilesetExists"), QMessageBox::StandardButton::Ok);
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

    QString selTsName =ui->tilesetView->selectionModel()->selectedIndexes().at(0).data(Qt::UserRole+1).toString();

    game->fs->deleteFile("/Unit/" + selTsName + ".sarc");
    ui->tilesetView->setModel(game->getTilesetModel());
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
        setStyleSheetFromPath("nightmode.qss");
    else
        setStyleSheetFromPath("lightmode.qss");
}
  
void MainWindow::setStyleSheetFromPath(QString path)
{
    QFile styleSheetFile(QCoreApplication::applicationDirPath() + "/coinkiller_data/" + path);

    if (!styleSheetFile.exists())
    {
        setStyleSheet("");
        return;
    }

    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheetTxt = QLatin1String(styleSheetFile.readAll());
    setStyleSheet(styleSheetTxt);
}

void MainWindow::on_testButton_clicked()
{
    if (!gameLoaded)
        return;

    QString testPath = "/etc1.ctpk";
    QString intPath = "test.tga";

    Ctpk* ctpk = new Ctpk(game->fs->openFile(testPath));


    QImage* img = ctpk->getTexture(intPath);

    img->save("Z:/TestFolder" + testPath + ".png");

    delete img;
    delete ctpk;
}
