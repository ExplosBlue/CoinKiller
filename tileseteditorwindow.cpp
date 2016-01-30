#include "tileseteditorwindow.h"
#include "ui_tileseteditorwindow.h"

#include <QDebug>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QBrush>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>

TilesetEditorWindow::TilesetEditorWindow(QWidget *parent, Tileset *tileset) :
    QMainWindow(parent),
    ui(new Ui::TilesetEditorWindow)
{
    this->tileset = tileset;
    this->selectedX = -1;
    this->selectedY = -1;

    ui->setupUi(this);
    this->setWindowTitle("Tileset Editor - CoinKiller");

    tilesetPicker = new TilesetPicker(this);

    ui->tilesetPicker->setWidget(tilesetPicker);
    tilesetPicker->setMinimumSize(440, 440);
    tilesetPicker->setMaximumSize(440, 440);

    connect(tilesetPicker, SIGNAL(selectedTileChanged(int,int)), this, SLOT(updateSelectedTile(int,int)));

    tilesetPicker->setTilesetImage(tileset->getImage());

    ui->hexLineEdit->setInputMask("HH HH HH HH HH HH HH HH");

    loadBehaviors();
    setSBehaviorsModel();
}

TilesetEditorWindow::~TilesetEditorWindow()
{
    delete ui;
}

void TilesetEditorWindow::updateSelectedTile(int x, int y)
{
    selectedX = x;
    selectedY = y;

    QString selTileText("Selected Tile: (%1, %2)");
    ui->selectedTileLabel->setText(selTileText.arg(x).arg(y));

    updateHex();
    updateBehavior();
}

void::TilesetEditorWindow::updateHex()
{
    QString hexValue;
    for (int i = 0; i < 8; i++)
        hexValue.append(QString("%1 ").arg(tileset->getBehaviorByte(selectedX + selectedY*21, i), 2, 16, QChar('0')));

    ui->hexLineEdit->setText(hexValue);
}

void TilesetEditorWindow::updateBehavior()
{
    int byte0 = tileset->getBehaviorByte(selectedX + selectedY*21, 0);
    bool check = false;

    for (int i = 0; i < specialBehaviors.size(); i++)
    {
        if (byte0 == specialBehaviors[i].value)
        {
            selectedSpecialBehavior = i;
            QModelIndex modelIndex = ui->sBehaviorListView->model()->index(i, 0, QModelIndex());
            ui->sBehaviorListView->setCurrentIndex(modelIndex);
            check = true;
            break;
        }
    }

    setParametersModel();

    if (!check)
    {
        ui->sBehaviorListView->clearSelection();
        selectedSpecialBehavior = -1;
    }
    else
    {
        int byte2 = tileset->getBehaviorByte(selectedX + selectedY*21, 2);
        bool checkB = false;

        for (int i = 0; i < specialBehaviors[selectedSpecialBehavior].parameters.size(); i++)
        {
            if (byte2 == specialBehaviors[selectedSpecialBehavior].parameters[i].byte)
            {
                selectedParameter = i;
                QModelIndex modelIndex = ui->parameterListView->model()->index(i, 0, QModelIndex());
                ui->parameterListView->setCurrentIndex(modelIndex);
                checkB = true;
                break;
            }
        }
        if (!checkB)
        {
            ui->parameterListView->clearSelection();
            selectedParameter = -1;
        }
    }
}

void TilesetEditorWindow::loadBehaviors()
{
    QDomDocument xmlBehaviors;
    QFile f(QCoreApplication::applicationDirPath() + "/coinkiller_data/tilebehaviors.xml");
    if (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "CoinKiller", "Failed to load behaviors.xml");
        return;
    }
    xmlBehaviors.setContent(&f);
    f.close();

    QDomElement root = xmlBehaviors.documentElement();
    QString time = root.attribute("timestamp", "Invalid Time");
    qDebug("behaviors.xml from: " + time.toLatin1());

    QDomElement behaviorElement = root.firstChild().toElement();

    while (!behaviorElement.isNull())
    {
        QList<parameter> parameterL;

        QDomElement valueElement = behaviorElement.firstChild().toElement();
        while (!valueElement.isNull())
        {
            parameterL.append(parameter(valueElement.attribute("id", "-1").toInt(), valueElement.attribute("description", "Error in behaviors.xml")));
            valueElement = valueElement.nextSibling().toElement();
        }

        specialBehavior sBehavior = { behaviorElement.attribute("value", "-1").toInt(), behaviorElement.attribute("description", "Error in behaviors.xml"), parameterL };
        specialBehaviors.append(sBehavior);

        behaviorElement = behaviorElement.nextSibling().toElement();
    }
}

void TilesetEditorWindow::setSBehaviorsModel()
{
    QStringListModel* sBehaviorsModel = new QStringListModel;
    QStringList sBehaviorsList;
    for (int i = 0; i < specialBehaviors.size(); i++)
        sBehaviorsList.append(specialBehaviors[i].description);
    sBehaviorsModel->setStringList(sBehaviorsList);

    ui->sBehaviorListView->setModel(sBehaviorsModel);
}

void TilesetEditorWindow::setParametersModel()
{
    QStringListModel* parametersModel = new QStringListModel;

    if (selectedSpecialBehavior != -1)
    {
        QStringList parametersList;
        for (int i = 0; i < specialBehaviors[selectedSpecialBehavior].parameters.size(); i++)
            parametersList.append(specialBehaviors[selectedSpecialBehavior].parameters[i].description);
        parametersModel->setStringList(parametersList);
    }

    ui->parameterListView->setModel(parametersModel);
}

void TilesetEditorWindow::on_hexLineEdit_textEdited(const QString &text)
{
    if (selectedX == -1 || selectedY == -1)
        return;

    QStringList bytes = text.split(" ");

    for (int i = 0; i < 8; i++)
        tileset->setBehaviorByte(selectedX + 21*selectedY, i, (quint8)bytes[i].toUInt(0, 16));
}

void TilesetEditorWindow::on_sBehaviorListView_clicked(const QModelIndex &index)
{
    int oldSelectedSpecialBehavior = selectedSpecialBehavior;

    selectedSpecialBehavior = index.row();

    tileset->setBehaviorByte(selectedX + 21*selectedY, 0, specialBehaviors[selectedSpecialBehavior].value);
    if (selectedSpecialBehavior != oldSelectedSpecialBehavior)
        tileset->setBehaviorByte(selectedX + 21*selectedY, 2, 0);

    setParametersModel();
    updateHex();
}

void TilesetEditorWindow::on_parameterListView_clicked(const QModelIndex &index)
{
    selectedParameter = index.row();
    tileset->setBehaviorByte(selectedX + 21*selectedY, 2, specialBehaviors[selectedSpecialBehavior].parameters[selectedParameter].byte);

    updateHex();
}

void TilesetEditorWindow::on_actionExportImage_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export Tileset Image", QDir::currentPath(), "PNG File (*.png)");

    QImage img = QImage(420, 420, QImage::Format_RGBA8888);
    QPainter painter;
    painter.begin(&img);

    for (int x = 0; x < 21; x++)
    {
        for (int y = 0; y < 21; y++)
        {
            painter.drawImage(x*20, y*20, *tileset->getImage(), x*20 + x*4 + 2, y*20 + y*4 + 2, 20, 20, Qt::AutoColor);
        }
    }

    painter.end();
    img.save(filename);
}

TilesetPicker::TilesetPicker(QWidget *parent) : QWidget(parent)
{
    this->selectedX = -1;
    this->selectedY = -1;
    tilesetImage = new QImage(420, 420, QImage::Format_RGBA8888);
}

void TilesetPicker::setTilesetImage(QImage *tilesetImage)
{
    this->tilesetImage = tilesetImage;
}

void TilesetPicker::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.fillRect(evt->rect(), QBrush(QColor(255, 255, 255), Qt::SolidPattern));

    for (int x = 0; x < 21; x++)
    {
        for (int y = 0; y < 21; y++)
        {
            painter.drawImage(x*21, y*21, *tilesetImage, x*20 + x*4 + 2, y*20 + y*4 + 2, 20, 20, Qt::AutoColor);
        }
    }

    if (selectedX != -1 || selectedY != -1)
        painter.fillRect(QRect(selectedX*21, selectedY*21, 20, 20), QBrush(QColor(160,222,255,150), Qt::SolidPattern));
}

void TilesetPicker::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    if (evt->x() % 21 != 20 && evt->x() % 21 != 20)
    {
        selectedX = evt->x() / 21;
        selectedY = evt->y() / 21;

        emit selectedTileChanged(selectedX, selectedY);

        update();
    }
}
