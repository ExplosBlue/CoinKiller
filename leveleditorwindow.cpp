#include "leveleditorwindow.h"
#include "ui_leveleditorwindow.h"

#include "levelview.h"

#include <QHBoxLayout>
#include <QSizePolicy>

LevelEditorWindow::LevelEditorWindow(QWidget *parent, SarcFilesystem* level) :
    QMainWindow(parent),
    ui(new Ui::LevelEditorWindow)
{
    levelArchive = level;
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
    delete ui;
}
