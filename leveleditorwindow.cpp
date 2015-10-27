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
#include "game.h"

#include <QHBoxLayout>
#include <QSizePolicy>

LevelEditorWindow::LevelEditorWindow(QWidget *parent, Level* level) :
    QMainWindow(parent),
    ui(new Ui::LevelEditorWindow)
{
    this->level = level;
    // some extra shit here


    ui->setupUi(this);

    QString title;
    level->getName(title);
    setWindowTitle(title + " - CoinKiller");

    levelView = new LevelView(this, level);
    /*levelView->setMinimumHeight(600);
    levelView->setMaximumWidth(800);*/
    //QHBoxLayout* crappyshit = new QHBoxLayout(this);
    //this->setLayout(crappyshit);
    //this->ui->widget->setLayout(crappyshit);
    //crappyshit->addWidget(levelView);
    ui->levelViewArea->setWidget(levelView);
    levelView->setMinimumSize(4096*20, 4096*20);
    levelView->setMaximumSize(4096*20, 4096*20);


    // TEST ZONE

    propGrid = new PropertyGrid(this);
    QHBoxLayout* crappyshit = new QHBoxLayout(ui->widget_2);
    ui->widget_2->setLayout(crappyshit);
    crappyshit->addWidget(propGrid);
    //ui->widget_2->layout()->addWidget(propGrid);

    propGrid->verticalHeader()->hide();
    propGrid->horizontalHeader()->hide();

    PropertyGridModel* model = new PropertyGridModel(propGrid);
    propGrid->setModel(model);

    // TEST ZONE END



    QList<int> derpshit;
    derpshit.append(200);
    derpshit.append(999999999);
    ui->splitter->setSizes(derpshit);

    ui->sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->sidebar->setMinimumSize(200, 20);

    //this->ui->splitter->setStretchFactor(0, 0); // useless

    //this->ui->widget->layout()->addWidget(levelView);


    layerMask = 0x7;
    ui->actionToggleLayer1->setChecked(true);
    ui->actionToggleLayer2->setChecked(true);
    levelView->setLayerMask(layerMask);
}

LevelEditorWindow::~LevelEditorWindow()
{
    delete level;
    delete ui;
}

void LevelEditorWindow::on_actionToggleLayer1_toggled(bool toggle)
{
    if (toggle) layerMask |=  0x1;
    else        layerMask &= ~0x1;
    levelView->setLayerMask(layerMask);
    update();
}

void LevelEditorWindow::on_actionToggleLayer2_toggled(bool toggle)
{
    if (toggle) layerMask |=  0x2;
    else        layerMask &= ~0x2;
    levelView->setLayerMask(layerMask);
    update();
}
