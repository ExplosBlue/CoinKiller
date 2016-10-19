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
    settings = new SettingsManager(this);
    loadTranslations();
    settings->setupLanguageSelector(ui->languageSelector);
}

MainWindow::~MainWindow()
{
    if (!startupClose)
    {
        delete settings;
        delete game;
    }
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About that thing you're using",
                             "CoinKiller v1.0 -- by StapleButter and Mariomaster\n\nhttp://kuribo64.net/ or whatever\n\nDefault Icons by Icons8\n\noh and this is free software, if you paid for it, you got scammed");
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
    game = new Game(fs, settings);


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
    TilesetEditorWindow* tsEditor = new TilesetEditorWindow(this, game->getTileset(data), settings);
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
    ui->tabWidget->setTabText(2, settings->getTranslation("General", "settings"));
    ui->languagesLabel->setText(settings->getTranslation("MainWindow", "languages")+":");
    ui->updateSpriteData->setText(settings->getTranslation("MainWindow", "updateSDat"));
    ui->tabWidget->setTabText(3, settings->getTranslation("General", "tools"));
    ui->openSarcExplorerBtn->setText(settings->getTranslation("SarcExplorer", "sarcExplorer"));
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
