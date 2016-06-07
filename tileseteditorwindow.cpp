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
#include <QInputDialog>

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
    ui->actionImportImage->setIcon(QIcon(basePath + "import.png"));
    ui->actionDeleteAllObjects->setIcon(QIcon(basePath + "delete_objects.png"));
    ui->actionDeleteAll3DOverlays->setIcon(QIcon(basePath + "delete_overlays.png"));
    ui->actionSetTilesetSlot->setIcon(QIcon(basePath + "edit_slot.png"));
    ui->actionShowObjectMarkers->setIcon(QIcon(basePath + "marker_red.png"));


    // Setup Behaviors Editor
    tilesetPicker = new TilesetPicker(this);

    ui->tilesetPicker->setWidget(tilesetPicker);
    tilesetPicker->setMinimumSize(440, 440);
    tilesetPicker->setMaximumSize(440, 440);

    connect(tilesetPicker, SIGNAL(selectedTileChanged(int)), this, SLOT(updateSelectedTile(int)));
    connect(tilesetPicker, SIGNAL(selectedOvTileChanged(int)), this, SLOT(updateSelectedOvTile(int)));

    tilesetPicker->setTilesetImage(tileset->getImage());

    ui->hexLineEdit->setInputMask("HH HH HH HH HH HH HH HH");

    loadBehaviors();
    setStaticModels();


    // Setup Objects Editor
    setupObjectBehaviorModel();
    ui->objectEditor->removeItem(ui->objectEditorSpacer);
    objectEditor = new ObjectEditor(tileset, this);
    connect(this, SIGNAL(selectedObjectChanged(int)), objectEditor, SLOT(selectedObjectChanged(int)));
    connect(objectEditor, SIGNAL(updateSelTileLabel(QString)), this, SLOT(setSelTileData(QString)));
    connect(tilesetPicker, SIGNAL(selectedTileChanged(int)), objectEditor, SLOT(selectedPaintTileChanged(int)));
    connect(objectEditor, SIGNAL(tilesetChanged()), this, SLOT(updateObjectEditor()));
    ui->objectEditor->insertWidget(1, objectEditor);
    ui->actionShowObjectMarkers->setChecked(true);

    ui->objectsListView->setIconSize(QSize(140,140));
    ui->objectsListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setupObjectsModel(false);
    updateObjectInfo();

    this->adjustSize();

    hideHorizEdits(true);
    hideVertEdits(true);
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
    tilesetPicker->setOvTile(tileset->getOverlayTile(selectedTile));

    QString selTileText("Selected Tile: (%1, %2)");
    ui->selectedTileLabel->setText(selTileText.arg(tile%21).arg(tile/21));

    updateHex();
    updateBehavior();
    updateComboBoxes();
}

void TilesetEditorWindow::updateSelectedOvTile(int ovTile)
{
    if (selectedTile != -1)
        tileset->setOverlayTile(selectedTile, ovTile, tileset->getSlot());
}


// Behaviors Editor Funtions

void TilesetEditorWindow::updateHex()
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

    if (!check)
    {
        ui->sBehaviorListView->clearSelection();
        selectedSpecialBehavior = -1;
    }

    setParametersModel();
    updateParameters();
}

void TilesetEditorWindow::updateParameters()
{
    int byte2 = tileset->getBehaviorByte(selectedTile, 2);
    bool check = false;

    if (selectedSpecialBehavior != -1)
    {
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
    }
    if (!check)
    {
        ui->parameterListView->clearSelection();
    }
}

void TilesetEditorWindow::updateComboBoxes()
{
    updateComboBox(4, hitboxes, ui->hitBoxComboBox);
    updateComboBox(5, terrainTypes, ui->terrainTypeComboBox);
    updateComboBox(7, depthBehaviors, ui->depthComboBox);
    updateComboBox(3, pipeColors, ui->pipeColorComboBox);

    if (selectedSpecialBehavior != -1)
        ui->pipeColorComboBox->setEnabled((specialBehaviors[selectedSpecialBehavior].description == "Pipe"));
    else
        ui->pipeColorComboBox->setEnabled(false);
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


// Objects Editor Funtions

void TilesetEditorWindow::setupObjectsModel(bool keepIndex)
{
    int selObj = ui->objectsListView->currentIndex().row();

    QStandardItemModel* objectsModel = new QStandardItemModel();

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/");

    for (int i = 0; i < tileset->getNumObjects(); i++)
    {
        ObjectDef* obj = tileset->getObjectDef(i);
        QPixmap objPixmap(obj->width*20, obj->height*20);
        objPixmap.fill(Qt::transparent);

        QPainter p(&objPixmap);
        TileGrid tileGrid;
        tileGrid.clear();
        tileGrid[0xFFFFFFFF] = 1;
        tileset->drawObject(p, tileGrid, i, 0, 0, obj->width, obj->height, 1);
        p.end();

        QStandardItem *objItem = new QStandardItem(QIcon(objPixmap), QString("Object %1").arg(i));
        objectsModel->appendRow(objItem);
    }

    ui->objectsListView->setModel(objectsModel);

    if (keepIndex)
    {
        QModelIndex oldIndex = ui->objectsListView->model()->index(selObj, 0, QModelIndex());
        ui->objectsListView->setCurrentIndex(oldIndex);
        ui->objectsListView->selectionModel()->select(oldIndex, QItemSelectionModel::SelectCurrent);
    }
}

void TilesetEditorWindow::setSelTileData(QString text)
{
    ui->selTileDataLabel->setText(text);
}

void TilesetEditorWindow::updateObjectEditor()
{
    setupObjectsModel(true);
}

void TilesetEditorWindow::updateObjectInfo()
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1)
    {
        ui->objectGroupBox->setEnabled(false);
        return;
    }

    ui->objectGroupBox->setEnabled(true);

    ObjectDef& obj = *tileset->getObjectDef(selObj);

    bool oldStateW = ui->oWidthSpinBox->blockSignals(true);
    ui->oWidthSpinBox->setValue(obj.width);
    ui->oWidthSpinBox->blockSignals(oldStateW);
    bool oldStateH = ui->oHeightSpinBox->blockSignals(true);
    ui->oHeightSpinBox->setValue(obj.height);
    ui->oHeightSpinBox->blockSignals(oldStateH);

    ui->randHorizontalCheckBox->setChecked(tileset->getRandomizeH(selObj));
    ui->randVerticalCheckBox->setChecked(tileset->getRandomizeV(selObj));
    ui->randTilesSpinBox->setValue(tileset->getRandomizeTiles(selObj));

    setObectBehavior();
    clampOBehaviorSpinBoxes();
}

void TilesetEditorWindow::setObectBehavior()
{
    // Indexes
    // -1: None
    //  0: Tile
    //  1: Repeat Horizontally
    //  2: Repeat Vertically
    //  3: Repeat Horizontally and Vertically
    //  4: Slope (Up)
    //  5: Slope (Down)
    //  6: Upside-Down Slope (Down)
    //  7: Upside-Down Slope (Up)

    hideHorizEdits(true);
    hideVertEdits(true);

    bool repeatH = false;
    bool repeatV = false;
    bool slopeFlag = false;

    ObjectDef& obj = *tileset->getObjectDef(ui->objectsListView->currentIndex().row());

    for (int r = 0; r < obj.rows.size(); r++)
    {
        for (int d = 0; d < obj.rows[r].data.size(); d += 3)
        {
            if (obj.rows[r].data[d] & 1) repeatH = true;
            if ((obj.rows[r].data[d] & 2) >> 1) repeatV = true;
            if ((obj.rows[r].data[d] & 4) >> 2) slopeFlag = true;
        }
    }

    if (repeatH && !repeatV)
    {
        ui->oBehaviorComboBox->setCurrentIndex(1);
        hideHorizEdits(false);
        setRepeatSpinBox(0, obj.rows[0].xRepeatStart, true);
        setRepeatSpinBox(1, obj.rows[0].xRepeatEnd, true);
        return;
    }

    if (!repeatH && repeatV)
    {
        ui->oBehaviorComboBox->setCurrentIndex(2);
        hideVertEdits(false);
        setRepeatSpinBox(2, obj.yRepeatStart, true);
        setRepeatSpinBox(3, obj.yRepeatEnd, true);
        return;
    }

    if (repeatH && repeatV)
    {
        ui->oBehaviorComboBox->setCurrentIndex(3);
        hideHorizEdits(false);
        hideVertEdits(false);
        setRepeatSpinBox(0, obj.rows[0].xRepeatStart, true);
        setRepeatSpinBox(1, obj.rows[0].xRepeatEnd, true);
        setRepeatSpinBox(2, obj.yRepeatStart, true);
        setRepeatSpinBox(3, obj.yRepeatEnd, true);
        return;
    }

    if (obj.rows[0].slopeFlags != 0 && slopeFlag)
    {
        quint8 slopeType = obj.rows[0].slopeFlags & 15;
        if (slopeType == 0) ui->oBehaviorComboBox->setCurrentIndex(4);
        else if (slopeType == 1) ui->oBehaviorComboBox->setCurrentIndex(5);
        else if (slopeType == 2) ui->oBehaviorComboBox->setCurrentIndex(6);
        else if (slopeType == 3) ui->oBehaviorComboBox->setCurrentIndex(7);
        return;
    }

    if (slopeFlag)
    {
        ui->oBehaviorComboBox->setCurrentIndex(-1);
        return;
    }

    ui->oBehaviorComboBox->setCurrentIndex(0);
}

void TilesetEditorWindow::setRepeatSpinBox(int nbr, int value, bool block)
{
    QSpinBox* spinBox;
    if (nbr == 0) spinBox = ui->hStartSpinBox;
    if (nbr == 1) spinBox = ui->hEndSpinBox;
    if (nbr == 2) spinBox = ui->vStartSpinBox;
    if (nbr == 3) spinBox = ui->vEndSpinBox;

    if (block)
    {
        bool oldState = spinBox->blockSignals(true);
        spinBox->setValue(value);
        spinBox->blockSignals(oldState);
    }

    spinBox->setValue(value);
}

void TilesetEditorWindow::setupObjectBehaviorModel()
{
    QStringList oBehaviorsList;
    oBehaviorsList
            << "Tile"
            << "Repeat Horizontally"
            << "Repeat Vertically"
            << "Repeat Horizontally and Vertically"
            << "Slope (Up)"
            << "Slope (Down)"
            << "Upside-Down Slope (Down)"
            << "Upside-Down Slope (Up)";
    ui->oBehaviorComboBox->setModel(new QStringListModel(oBehaviorsList));

    ui->oBehaviorComboBox->setCurrentIndex(-1);
}

void TilesetEditorWindow::hideHorizEdits(bool toggle)
{
    ui->label_o1->setHidden(toggle);
    ui->label_o2->setHidden(toggle);
    ui->label_o3->setHidden(toggle);
    ui->hStartSpinBox->setHidden(toggle);
    ui->hEndSpinBox->setHidden(toggle);
}

void TilesetEditorWindow::hideVertEdits(bool toggle)
{
    ui->label_o4->setHidden(toggle);
    ui->label_o5->setHidden(toggle);
    ui->label_o6->setHidden(toggle);
    ui->vStartSpinBox->setHidden(toggle);
    ui->vEndSpinBox->setHidden(toggle);
}

void TilesetEditorWindow::clampOBehaviorSpinBoxes()
{
    ObjectDef& obj = *tileset->getObjectDef(ui->objectsListView->currentIndex().row());

    ui->hStartSpinBox->setMinimum(0);
    ui->hStartSpinBox->setMaximum(obj.width-1);
    ui->hEndSpinBox->setMaximum(obj.width);
    ui->hEndSpinBox->setMinimum(ui->hStartSpinBox->value()+1);

    ui->vStartSpinBox->setMinimum(0);
    ui->vStartSpinBox->setMaximum(obj.height-1);
    ui->vEndSpinBox->setMaximum(obj.height);
    ui->vEndSpinBox->setMinimum(ui->vStartSpinBox->value()+1);
}


// Behaviors Editor Actions

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

void TilesetEditorWindow::on_actionDeleteAll3DOverlays_triggered()
{
    QMessageBox::StandardButton warning = QMessageBox::warning(this, "CoinKiller", "Do you realy want to delete all 3D Overlays?", QMessageBox::Yes|QMessageBox::No);

    if (warning != QMessageBox::Yes)
        return;

    for (int i = 0; i < 441; i++)
        tileset->setOverlayTile(i, 0, 0);

    tilesetPicker->setOvTile(0);
}

void TilesetEditorWindow::on_actionDeleteAllBehaviors_triggered()
{
    QMessageBox::StandardButton warning = QMessageBox::warning(this, "CoinKiller", "Do you realy want to delete all Behaviors?", QMessageBox::Yes|QMessageBox::No);

    if (warning != QMessageBox::Yes)
        return;

    for (int i = 0; i < 441; i++)
        for (int j = 0; j < 8; j++)
            tileset->setBehaviorByte(i, j, 0);
}


// Objects Editor Actions

void TilesetEditorWindow::on_objectsListView_clicked(const QModelIndex &index)
{
    emit selectedObjectChanged(index.row());
    bool oldState1 = ui->oBehaviorComboBox->blockSignals(true);
    bool oldState2 = ui->hEndSpinBox->blockSignals(true);
    bool oldState3 = ui->vEndSpinBox->blockSignals(true);
    updateObjectInfo();
    ui->oBehaviorComboBox->blockSignals(oldState1);
    ui->hEndSpinBox->blockSignals(oldState2);
    ui->vEndSpinBox->blockSignals(oldState3);
}

void TilesetEditorWindow::on_addObjectPushButton_clicked()
{
    int selObj = ui->objectsListView->currentIndex().row();

    tileset->addObject(selObj);
    setupObjectsModel(false);

    QModelIndex newIndex = ui->objectsListView->model()->index(selObj+1, 0, QModelIndex());
    ui->objectsListView->setCurrentIndex(newIndex);
    ui->objectsListView->selectionModel()->select(newIndex, QItemSelectionModel::Select);
    emit on_objectsListView_clicked(newIndex);
}

void TilesetEditorWindow::on_removeObjectButton_clicked()
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1)
        return;

    tileset->removeObject(selObj);
    setupObjectsModel(false);
    ui->objectsListView->clearSelection();
    emit selectedObjectChanged(-1);
}

void TilesetEditorWindow::on_moveObjectUpButton_clicked()
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || selObj == 0)
        return;

    tileset->moveObjectUp(selObj);

    setupObjectsModel(false);
    QModelIndex newIndex = ui->objectsListView->model()->index(selObj-1, 0, QModelIndex());
    ui->objectsListView->setCurrentIndex(newIndex);
    ui->objectsListView->selectionModel()->select(newIndex, QItemSelectionModel::Select);
    emit selectedObjectChanged(newIndex.row());
}

void TilesetEditorWindow::on_moveObjectDownButton_clicked()
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || selObj+1 >= tileset->getNumObjects())
        return;

    tileset->moveObjectDown(selObj);

    setupObjectsModel(false);
    QModelIndex newIndex = ui->objectsListView->model()->index(selObj+1, 0, QModelIndex());
    ui->objectsListView->setCurrentIndex(newIndex);
    ui->objectsListView->selectionModel()->select(newIndex, QItemSelectionModel::Select);
    emit selectedObjectChanged(newIndex.row());
}

void TilesetEditorWindow::on_actionDeleteAllObjects_triggered()
{
    QMessageBox::StandardButton warning = QMessageBox::warning(this, "CoinKiller", "Do you realy want to delete all objects?", QMessageBox::Yes|QMessageBox::No);

    if (warning != QMessageBox::Yes)
        return;

    for (int i = tileset->getNumObjects()-1; i >= 0; i--)
        tileset->removeObject(i);

    setupObjectsModel(false);
    emit selectedObjectChanged(-1);
}

void TilesetEditorWindow::on_oWidthSpinBox_valueChanged(int width)
{
    tileset->resizeObject(ui->objectsListView->currentIndex().row(), width, -1);
    clampOBehaviorSpinBoxes();
    objectEditor->update();
    setupObjectsModel(true);
}

void TilesetEditorWindow::on_oHeightSpinBox_valueChanged(int height)
{
    tileset->resizeObject(ui->objectsListView->currentIndex().row(), -1, height);
    clampOBehaviorSpinBoxes();
    objectEditor->update();
    setupObjectsModel(true);
}

void TilesetEditorWindow::on_actionSetTilesetSlot_triggered()
{
    QStringList slotNames;
    slotNames << "Standard Suite" << "Stage Suite" << "Background Suite" << "Interactive Suite";
    int slot = -1;
    slot = slotNames.indexOf(QInputDialog::getItem(this, "CoinKiller", "Set Tileset Slot:", slotNames, 0, false, 0, Qt::WindowTitleHint));

    if (slot == -1)
        return;

    tileset->setSlot(slot);

    objectEditor->update();
}

void TilesetEditorWindow::on_randHorizontalCheckBox_toggled(bool checked)
{
    tileset->setRandomizeH(ui->objectsListView->currentIndex().row(), checked);
}

void TilesetEditorWindow::on_randVerticalCheckBox_toggled(bool checked)
{
    tileset->setRandomizeV(ui->objectsListView->currentIndex().row(), checked);
}

void TilesetEditorWindow::on_randTilesSpinBox_valueChanged(int tiles)
{
    tileset->setRandomizeTiles(ui->objectsListView->currentIndex().row(), tiles);
}

void TilesetEditorWindow::on_actionShowObjectMarkers_toggled(bool value)
{
    objectEditor->setMarkers(value);
}

void TilesetEditorWindow::on_oBehaviorComboBox_currentIndexChanged(int index)
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || index == -1)
        return;

    ObjectDef& obj = *tileset->getObjectDef(selObj);

    if (index == 1 || index == 3)
    {
        int s = 1;
        int e = obj.width-1;
        if (obj.width < 3)
        {
            s = obj.width-1;
            e = obj.width;
        }
        setRepeatSpinBox(0, s, true);
        setRepeatSpinBox(1, e, true);
    }
    if (index == 2 || index == 3)
    {
        int s = 1;
        int e = obj.height-1;
        if (obj.height < 3)
        {
            s = obj.height-1;
            e = obj.height;
        }
        setRepeatSpinBox(2, s, true);
        setRepeatSpinBox(3, e, true);
    }

    tileset->setObjectBehavior(selObj, index, ui->hStartSpinBox->value(), ui->hEndSpinBox->value(), ui->vStartSpinBox->value(), ui->vEndSpinBox->value());
    bool oldState = ui->oBehaviorComboBox->blockSignals(true);
    updateObjectInfo();
    ui->oBehaviorComboBox->blockSignals(oldState);
    objectEditor->update();
}

void TilesetEditorWindow::on_hStartSpinBox_valueChanged(int value)
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || value == -1)
        return;

    clampOBehaviorSpinBoxes();
    tileset->setObjectBehavior(selObj, ui->oBehaviorComboBox->currentIndex(), value, ui->hEndSpinBox->value(), ui->vStartSpinBox->value(), ui->vEndSpinBox->value());
    objectEditor->update();
}

void TilesetEditorWindow::on_hEndSpinBox_valueChanged(int value)
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || value == -1)
        return;

    clampOBehaviorSpinBoxes();
    tileset->setObjectBehavior(selObj, ui->oBehaviorComboBox->currentIndex(), ui->hStartSpinBox->value(), value, ui->vStartSpinBox->value(), ui->vEndSpinBox->value());
    objectEditor->update();
}

void TilesetEditorWindow::on_vStartSpinBox_valueChanged(int value)
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || value == -1)
        return;

    clampOBehaviorSpinBoxes();
    tileset->setObjectBehavior(selObj, ui->oBehaviorComboBox->currentIndex(), ui->hStartSpinBox->value(), ui->hEndSpinBox->value(), value, ui->vEndSpinBox->value());
    objectEditor->update();
}

void TilesetEditorWindow::on_vEndSpinBox_valueChanged(int value)
{
    int selObj = ui->objectsListView->currentIndex().row();

    if (selObj == -1 || value == -1)
        return;

    clampOBehaviorSpinBoxes();
    tileset->setObjectBehavior(selObj, ui->oBehaviorComboBox->currentIndex(), ui->hStartSpinBox->value(), ui->hEndSpinBox->value(), ui->vStartSpinBox->value(), value);
    objectEditor->update();
}


// General Actions

void TilesetEditorWindow::on_actionExportImage_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Export Tileset Image", QDir::currentPath(), "PNG Files (*.png)");

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
    {
        tilesetPicker->setBGColor(bgColor);
        objectEditor->setBGColor(bgColor);
    }
}

void TilesetEditorWindow::on_actionSave_triggered()
{
    tileset->save();
}
