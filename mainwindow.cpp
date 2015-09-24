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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filesystem.h"

#include "leveleditorwindow.h"
#include "sillytest.h" // REMOVE ME!!

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete game;
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About that thing you're using", "CoinKiller v1.0 -- by StapleButter\n\nhttp://kuribo64.net/ or whatever\n\noh and this is free software, if you paid for it, you got scammed");
}

void MainWindow::on_actionLoadROM_triggered()
{
    // full tile: 24x24
    // gfx: 20x20

    game = new Game(new ExternalFilesystem("G:/3DS_ROMs/nsmb2/romfs")); // hax!!


    LevelEditorWindow* crap = new LevelEditorWindow(this, game->getLevel(4, 1, 1));
    crap->show(); // derp
}

void MainWindow::on_actionDebug_test_triggered()
{
    SillyTest* silly = new SillyTest(this);
    silly->show();
}
