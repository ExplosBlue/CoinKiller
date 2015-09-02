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

#include "leveleditorwindow.h"
#include "ui_leveleditorwindow.h"

#include "levelview.h"

#include <QHBoxLayout>
#include <QSizePolicy>

LevelEditorWindow::LevelEditorWindow(QWidget *parent, Level* level) :
    QMainWindow(parent),
    ui(new Ui::LevelEditorWindow)
{
    this->level = level;
    // some extra shit here

    ui->setupUi(this);

    //crapshit = testcrap;

    levelView = new LevelView(this, level);//crapshit->getTexture(0));
    /*levelView->setMinimumHeight(600);
    levelView->setMaximumWidth(800);*/
    //QHBoxLayout* crappyshit = new QHBoxLayout(this);
    //this->setLayout(crappyshit);
    //this->ui->widget->setLayout(crappyshit);
    //crappyshit->addWidget(levelView);
    ui->levelViewArea->setWidget(levelView);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);

    QList<int> derpshit;
    derpshit.append(200);
    derpshit.append(999999999);
    ui->splitter->setSizes(derpshit);

    ui->sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->sidebar->setMinimumSize(200, 20);

    //this->ui->splitter->setStretchFactor(0, 0); // useless

    //this->ui->widget->layout()->addWidget(levelView);

}

LevelEditorWindow::~LevelEditorWindow()
{
    delete level;
    delete ui;
}
