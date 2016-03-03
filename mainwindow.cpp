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
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filesystem.h"

#include "leveleditorwindow.h"
#include "tileseteditorwindow.h"
#include "sillytest.h" // REMOVE ME!!

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CoinKiller");


    QCoreApplication::setOrganizationName("Blarg City");
    QCoreApplication::setApplicationName("CoinKiller");


    // DEFAULT LEVEL NAMES
    //defaultLevelNames["1-1"] = "1-1";
}

MainWindow::~MainWindow()
{
    delete game;
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About that thing you're using",
                             "CoinKiller v1.0 -- by StapleButter\n\nhttp://kuribo64.net/ or whatever\n\nDefault Icons by Icons8\n\noh and this is free software, if you paid for it, you got scammed");
}

void MainWindow::on_actionLoadROM_triggered()
{
    QSettings settings;
    // full tile: 24x24
    // gfx: 20x20

    QString basepath = settings.value("LastRomFSPath", "").toString();

    QString dirpath = QFileDialog::getExistingDirectory(this, "Open a RomFS folder", basepath);
    if (dirpath.isNull())
        return; // whatever

    settings.setValue("LastRomFSPath", dirpath);


    FilesystemBase* fs = new ExternalFilesystem(dirpath);
    game = new Game(fs); // hax!!



    FilesystemBase* test = new SarcFilesystem(game->fs->openFile("/Course/1-1.sarc"));
    QList<QString> testlist;
    test->directoryContents("/course", QDir::Files, testlist);
    qDebug("FILES");
    for (int i = 0; i < testlist.size(); i++)
        qDebug(testlist[i].toStdString().c_str());
    test->directoryContents("/", QDir::Dirs, testlist);
    qDebug("DIRS");
    for (int i = 0; i < testlist.size(); i++)
        qDebug(testlist[i].toStdString().c_str());


    // TODO: move all that crap to the Game class

    QStandardItemModel* levels = new QStandardItemModel();

    QList<QString> coursefiles;
    fs->directoryContents("/Course", QDir::Files, coursefiles);

    int l = 0;
    for (int w = 1; w <= 9; w++)
    {
        QStandardItem* world = new QStandardItem(QString("World %1").arg(w));
        levels->appendRow(world);

        for (;;)
        {
            // incredibly cheap way to name levels
            // way to go, me
            QString levelname = coursefiles[l];
            levelname.replace(".sarc", "");

            QStandardItem* level = new QStandardItem(levelname);
            level->setData(levelname);
            world->appendRow(level);

            //

            l++;
            if (l >= coursefiles.length())
                break;
            if (w != coursefiles[l].section('-',0,0).toInt())
                break;
        }
    }

    ui->levelList->setModel(levels);


    QStandardItemModel* tilesets = new QStandardItemModel();

    QStandardItem* standardSuite = new QStandardItem(QString("Standard Suite"));
    QStandardItem* stageSuite = new QStandardItem(QString("Stage Suite"));
    QStandardItem* backgroundSuite = new QStandardItem(QString("Background Suite"));
    QStandardItem* interactiveSuite = new QStandardItem(QString("Interactive Suite"));
    tilesets->appendRow(standardSuite);
    tilesets->appendRow(stageSuite);
    tilesets->appendRow(backgroundSuite);
    tilesets->appendRow(interactiveSuite);

    QList<QString> tilesetfiles;
    fs->directoryContents("/Unit", QDir::Files, tilesetfiles);

    for (int i = 0; i < tilesetfiles.length(); i++)
    {
        QString tilesetname = tilesetfiles[i];
        tilesetname.chop(5);

        QStandardItem* tileset;
        tileset = new QStandardItem();
        tileset->setData(tilesetname);

        if (tilesetname.startsWith("J_"))
        {
            tileset->setText(tilesetname.right(tilesetname.size() - 2));
            standardSuite->appendRow(tileset);
        }
        else if (tilesetname.startsWith("M_"))
        {
            tileset->setText(tilesetname.right(tilesetname.size() - 2));
            stageSuite->appendRow(tileset);
        }
        else if (tilesetname.startsWith("S1_"))
        {
            tileset->setText(tilesetname.right(tilesetname.size() - 3));
            backgroundSuite->appendRow(tileset);
        }
        else if (tilesetname.startsWith("S2_"))
        {
            tileset->setText(tilesetname.right(tilesetname.size() - 3));
            interactiveSuite->appendRow(tileset);
        }
    }

    ui->tilesetView->setModel(tilesets);
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

    QString data = index.data(Qt::UserRole+1).toString();

    int world = data.left(1).toInt();
    int level = data.mid(2, data.size()-2).toInt();

    LevelEditorWindow* lvlEditor = new LevelEditorWindow(this, game, world, level);
    lvlEditor->show();
}

void MainWindow::on_tilesetView_doubleClicked(const QModelIndex &index)
{
    if (index.data(Qt::UserRole+1).isNull())
        return;

    QString data = index.data(Qt::UserRole+1).toString();
    TilesetEditorWindow* tsEditor = new TilesetEditorWindow(this, new Tileset(game, data));
    tsEditor->show();
}
