#include "tileseteditorwindow.h"
#include "ui_tileseteditorwindow.h"

#include <QDebug>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QBrush>
#include <QFileDialog>
#include <QMessageBox>
#include <QtXml>
#include <QColorDialog>

TilesetEditorWindow::TilesetEditorWindow(QWidget *parent, Tileset *tileset) :
    QMainWindow(parent),
    ui(new Ui::TilesetEditorWindow)
{
    this->tileset = tileset;
    this->selectedTile = -1;
    this->selectedSpecialBehavior = -1;
    this->selectedParameter = -1;

    ui->setupUi(this);
    ui->behaviorsTab->setEnabled(false);
    this->setWindowTitle("Tileset Editor - CoinKiller");

    // Load UI Icons
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/");
    ui->actionSave->setIcon(QIcon(basePath + "save.png"));
    ui->actionSetBackgroundColor->setIcon(QIcon(basePath + "colors.png"));
    ui->actionExportImage->setIcon(QIcon(basePath + "export.png"));

    tilesetPicker = new TilesetPicker(this);

    ui->tilesetPicker->setWidget(tilesetPicker);
    tilesetPicker->setMinimumSize(440, 440);
    tilesetPicker->setMaximumSize(440, 440);

    connect(tilesetPicker, SIGNAL(selectedTileChanged(int)), this, SLOT(updateSelectedTile(int)));

    tilesetPicker->setTilesetImage(tileset->getImage());

    ui->hexLineEdit->setInputMask("HH HH HH HH HH HH HH HH");

    loadBehaviors();
    setStaticModels();
}

TilesetEditorWindow::~TilesetEditorWindow()
{
    delete ui;
}

void TilesetEditorWindow::updateSelectedTile(int tile)
{
    if (tile != -1)
        ui->behaviorsTab->setEnabled(true);
    else
    {
        ui->selectedTileLabel->setText("Selected Tile: None");
        ui->behaviorsTab->setEnabled(false);
        return;
    }

    selectedTile = tile;

    QString selTileText("Selected Tile: (%1, %2)");
    ui->selectedTileLabel->setText(selTileText.arg(tile%21).arg(tile/21));

    updateHex();
    updateBehavior();
    updateComboBoxes();
}

void::TilesetEditorWindow::updateHex()
{
    if (selectedTile == -1)
        return;

    QString hexValue;
    for (int i = 0; i < 8; i++)
        hexValue.append(QString("%1").arg(tileset->getBehaviorByte(selectedTile, i), 2, 16, QChar('0')));

    ui->hexLineEdit->setText(hexValue);
}

void TilesetEditorWindow::updateBehavior()
{
    int byte0 = tileset->getBehaviorByte(selectedTile, 0);
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
        updateParameters();
}

void TilesetEditorWindow::updateParameters()
{
    int byte2 = tileset->getBehaviorByte(selectedTile, 2);
    bool check = false;

    for (int i = 0; i < specialBehaviors[selectedSpecialBehavior].parameters.size(); i++)
    {
        if (byte2 == specialBehaviors[selectedSpecialBehavior].parameters[i].byte)
        {
            selectedParameter = i;
            QModelIndex modelIndex = ui->parameterListView->model()->index(i, 0, QModelIndex());
            ui->parameterListView->setCurrentIndex(modelIndex);
            check = true;
            break;
        }
    }
    if (!check)
    {
        ui->parameterListView->clearSelection();
        selectedParameter = -1;
    }
}

void TilesetEditorWindow::updateComboBoxes()
{
    updateComboBox(4, hitboxes, ui->hitBoxComboBox);
    updateComboBox(5, terrainTypes, ui->terrainTypeComboBox);
    updateComboBox(7, depthBehaviors, ui->depthComboBox);
    updateComboBox(3, pipeColors, ui->pipeColorComboBox);
    ui->pipeColorComboBox->setEnabled((specialBehaviors[selectedSpecialBehavior].description == "Pipe"));

}

void TilesetEditorWindow::updateComboBox(int byteNbr, QList<parameter> &list, QComboBox *comboBox)
{
    int byte = tileset->getBehaviorByte(selectedTile, byteNbr);
    bool check = false;

    for (int i = 0; i < list.size(); i++)
    {
        if (byte == list[i].byte)
        {
            comboBox->setCurrentIndex(i);
            check = true;
            break;
        }
    }
    if (!check)
    {
        comboBox->setCurrentIndex(-1);
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

    QDomElement behaviorElement = root.elementsByTagName("special_behaviors").at(0).firstChild().toElement();
    while (!behaviorElement.isNull())
    {
        QList<parameter> parameterL;

        QDomElement valueElement = behaviorElement.firstChild().toElement();
        while (!valueElement.isNull())
        {
            parameterL.append(parameter(valueElement.attribute("id", "-1").toInt(), valueElement.attribute("description", "Error in tilebehaviors.xml")));
            valueElement = valueElement.nextSibling().toElement();
        }

        specialBehavior sBehavior = { behaviorElement.attribute("value", "-1").toInt(), behaviorElement.attribute("description", "Error in tilebehaviors.xml"), parameterL };
        specialBehaviors.append(sBehavior);

        behaviorElement = behaviorElement.nextSibling().toElement();
    }

    QDomElement hitboxElement = root.elementsByTagName("hitboxes").at(0).firstChild().toElement();
    while (!hitboxElement.isNull())
    {
        parameter p(hitboxElement.attribute("id", "-1").toInt(), hitboxElement.attribute("description", "Error in tilebehaviors.xml"));
        hitboxes.append(p);

        hitboxElement = hitboxElement.nextSibling().toElement();
    }

    QDomElement terrainTypeElement = root.elementsByTagName("terrainTypes").at(0).firstChild().toElement();
    while (!terrainTypeElement.isNull())
    {
        parameter p(terrainTypeElement.attribute("id", "-1").toInt(), terrainTypeElement.attribute("description", "Error in tilebehaviors.xml"));
        terrainTypes.append(p);

        terrainTypeElement = terrainTypeElement.nextSibling().toElement();
    }

    QDomElement depthBehaviorElement = root.elementsByTagName("depthBehaviors").at(0).firstChild().toElement();
    while (!depthBehaviorElement.isNull())
    {
        parameter p(depthBehaviorElement.attribute("id", "-1").toInt(), depthBehaviorElement.attribute("description", "Error in tilebehaviors.xml"));
        depthBehaviors.append(p);

        depthBehaviorElement = depthBehaviorElement.nextSibling().toElement();
    }

    QDomElement pipeColorElement = root.elementsByTagName("pipeColors").at(0).firstChild().toElement();
    while (!pipeColorElement.isNull())
    {
        parameter p(pipeColorElement.attribute("id", "-1").toInt(), pipeColorElement.attribute("description", "Error in tilebehaviors.xml"));
        pipeColors.append(p);

        pipeColorElement = pipeColorElement.nextSibling().toElement();
    }
}

void TilesetEditorWindow::setStaticModels()
{
    QStringListModel* sBehaviorsModel = new QStringListModel;
    QStringList sBehaviorsList;
    for (int i = 0; i < specialBehaviors.size(); i++)
        sBehaviorsList.append(specialBehaviors[i].description);
    sBehaviorsModel->setStringList(sBehaviorsList);
    ui->sBehaviorListView->setModel(sBehaviorsModel);

    for (int i = 0; i < hitboxes.size(); i++)
       ui->hitBoxComboBox->addItem(hitboxes[i].description);

    for (int i = 0; i < terrainTypes.size(); i++)
       ui->terrainTypeComboBox->addItem(terrainTypes[i].description);

    for (int i = 0; i < depthBehaviors.size(); i++)
       ui->depthComboBox->addItem(depthBehaviors[i].description);

    for (int i = 0; i < pipeColors.size(); i++)
       ui->pipeColorComboBox->addItem(pipeColors[i].description);
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
    if (selectedTile == -1)
        return;

    QStringList bytes = text.split(" ");

    for (int i = 0; i < 8; i++)
        tileset->setBehaviorByte(selectedTile, i, (quint8)bytes[i].toUInt(0, 16));

    updateBehavior();
    updateComboBoxes();
}

void TilesetEditorWindow::on_sBehaviorListView_clicked(const QModelIndex &index)
{
    if (selectedTile == -1)
        return;

    int oldSelectedSpecialBehavior = selectedSpecialBehavior;

    selectedSpecialBehavior = index.row();

    tileset->setBehaviorByte(selectedTile, 0, specialBehaviors[selectedSpecialBehavior].value);
    if (selectedSpecialBehavior != oldSelectedSpecialBehavior)
        tileset->setBehaviorByte(selectedTile, 2, 0);

    setParametersModel();
    updateParameters();
    updateHex();
}

void TilesetEditorWindow::on_parameterListView_clicked(const QModelIndex &index)
{
    if (selectedTile == -1)
        return;

    selectedParameter = index.row();
    tileset->setBehaviorByte(selectedTile, 2, specialBehaviors[selectedSpecialBehavior].parameters[selectedParameter].byte);

    updateHex();
}

void TilesetEditorWindow::on_hitBoxComboBox_currentIndexChanged(int index)
{
    if (selectedTile == -1 || index == -1)
        return;

    tileset->setBehaviorByte(selectedTile, 4, hitboxes[index].byte);
    updateHex();
}

void TilesetEditorWindow::on_terrainTypeComboBox_currentIndexChanged(int index)
{
    if (selectedTile == -1 || index == -1)
        return;

    tileset->setBehaviorByte(selectedTile, 5, terrainTypes[index].byte);
    updateHex();
}

void TilesetEditorWindow::on_depthComboBox_currentIndexChanged(int index)
{
    if (selectedTile == -1 || index == -1)
        return;

    tileset->setBehaviorByte(selectedTile, 7, depthBehaviors[index].byte);
    updateHex();
}

void TilesetEditorWindow::on_pipeColorComboBox_currentIndexChanged(int index)
{
    if (selectedTile == -1 || index == -1)
        return;

    tileset->setBehaviorByte(selectedTile, 3, pipeColors[index].byte);
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

void TilesetEditorWindow::on_actionSetBackgroundColor_triggered()
{
    QColor bgColor = QColorDialog::getColor(Qt::white, this, "Select Background Color",  QColorDialog::DontUseNativeDialog);
    if(bgColor.isValid())
        tilesetPicker->setBGColor(bgColor);
}

void TilesetEditorWindow::on_actionSave_triggered()
{
    tileset->save();
}

TilesetPicker::TilesetPicker(QWidget *parent) : QWidget(parent)
{
    this->selectedTile = -1;
    tilesetImage = new QImage(420, 420, QImage::Format_RGBA8888);
    bgColor = Qt::white;
}

void TilesetPicker::setTilesetImage(QImage *tilesetImage)
{
    this->tilesetImage = tilesetImage;
}

void TilesetPicker::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.fillRect(evt->rect(), QBrush(bgColor, Qt::SolidPattern));

    for (int x = 0; x < 21; x++)
    {
        for (int y = 0; y < 21; y++)
        {
            painter.drawImage(x*21, y*21, *tilesetImage, x*20 + x*4 + 2, y*20 + y*4 + 2, 20, 20, Qt::AutoColor);
        }
    }

    if (selectedTile != -1)
        painter.fillRect(QRect(selectedTile%21*21, selectedTile/21*21, 20, 20), QBrush(QColor(160,222,255,150), Qt::SolidPattern));
}

void TilesetPicker::mousePressEvent(QMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    if (evt->x() % 21 != 20 && evt->x() % 21 != 20)
    {
        selectedTile = evt->x() / 21 + evt->y() / 21 * 21;

        emit selectedTileChanged(selectedTile);

        update();
    }
}
