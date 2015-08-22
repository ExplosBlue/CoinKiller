#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sarcfilesystem.h"
#include "externalfile.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About that thing you're using", "CoinKiller v1.0 -- by StapleButter\n\nhttp://kuribo64.net/ or whatever\n\noh and this is free software, if you paid for it, you got scammed");
}

void MainWindow::on_actionLoadROM_triggered()
{
    qDebug("load ROM");

    // testing shit
    QFile lol("C:/Documents/1-1.sarc");
    qDebug("%s", lol.exists()?"good":"bad");

    ExternalFilesystem* exfs = new ExternalFilesystem("C:/Documents");
    ExternalFile* exfile = (ExternalFile*)exfs->openFile("/1-1.sarc");
    SarcFilesystem* sarc = new SarcFilesystem(exfile);
}
