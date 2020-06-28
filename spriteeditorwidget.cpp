#include "spriteeditorwidget.h"
#include "unitsconvert.h"

#include <QGridLayout>
#include <QLabel>
#include <QStringListModel>
#include <QMessageBox>

SpriteEditorWidget::SpriteEditorWidget(QList<Sprite*> *sprites)
{
    QWidget* addSpriteView = new QWidget;
    QTabWidget* tabs = new QTabWidget;

    QLabel* viewLabel = new QLabel(tr("View:"));
    viewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    viewComboBox = new QComboBox();
    connect(viewComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setView(int)));

    QLabel* searchLabel = new QLabel(tr("Search:"));
    searchLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchEdit = new QLineEdit();
    connect(searchEdit, SIGNAL(textEdited(QString)), this, SLOT(search(QString)));

    spriteTree = new QTreeWidget();
    spriteTree->setColumnCount(1);
    spriteTree->setHeaderHidden(true);

    QGridLayout* topLayout = new QGridLayout();
    topLayout->setSpacing(6);
    topLayout->addWidget(viewLabel, 0, 0);
    topLayout->addWidget(viewComboBox, 0, 1);
    topLayout->addWidget(searchLabel, 1, 0);
    topLayout->addWidget(searchEdit, 1, 1);

    QVBoxLayout* addSpriteLayout = new QVBoxLayout();
    addSpriteLayout->addItem(topLayout);
    addSpriteLayout->addWidget(spriteTree);

    addSpriteView->setLayout(addSpriteLayout);
    tabs->addTab(addSpriteView, tr("Add"));

    QStringList viewNames = getViewNames();

    viewComboBox->setModel(new QStringListModel(viewNames));

    spriteIds = new SpriteIdWidget(sprites);
    tabs->addTab(spriteIds, tr("In Level"));

    editor = new SpriteDataEditorWidget(&spriteData);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tabs);
    layout->addWidget(editor);

    setLayout(layout);

    connect(spriteTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(handleIndexChange(QTreeWidgetItem*)));
    connect(editor, SIGNAL(editMade()), this, SLOT(handleEditDetected()));
    connect(spriteIds, SIGNAL(selectedSpriteChanged(Object*)), this, SLOT(handleSelectedSpriteChanged(Object*)));
    connect(spriteIds, SIGNAL(updateLevelView()), this, SLOT(handleUpdateLevelView()));

}

QStringList SpriteEditorWidget::getViewNames()
{
    QStringList viewNames;

    for (int v = 0; v < spriteData.spriteViewCount(); v++)
    {
        spriteView* view = spriteData.spriteViewPtr(v);

        view->categoryNodes.clear();

        viewNames.append(view->name);
        foreach (spriteCategory category, view->categories)
        {
            QTreeWidgetItem* categoryNode = new QTreeWidgetItem();
            categoryNode->setText(0, category.name);
            categoryNode->setData(0, Qt::UserRole, -1);

            foreach (int id, category.sprites)
            {
                QTreeWidgetItem* spriteNode = new QTreeWidgetItem();
                spriteNode->setText(0, QString("%1: %2").arg(id).arg(spriteData.getSpriteDef(id).getName()));
                spriteNode->setData(0, Qt::UserRole, id);
                categoryNode->addChild(spriteNode);
            }

            spriteTree->addTopLevelItem(categoryNode);
            view->categoryNodes.append(categoryNode);
        }
        foreach (int id, view->simpleSprites)
        {
            QTreeWidgetItem* spriteNode = new QTreeWidgetItem();
            spriteNode->setText(0, QString("%1: %2").arg(id).arg(spriteData.getSpriteDef(id).getName()));
            spriteNode->setData(0, Qt::UserRole, id);
            spriteTree->addTopLevelItem(spriteNode);
            view->categoryNodes.append(spriteNode);
        }
    }

    return viewNames;
}

void SpriteEditorWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        int currentView = viewComboBox->currentIndex();

        spriteData.loadSpriteDefs();
        spriteData.loadSpriteViews();

        QStringList viewNames = getViewNames();

        viewComboBox->setModel(new QStringListModel(viewNames));
        viewComboBox->setCurrentIndex(currentView);
    }

    QWidget::changeEvent(event);
}

void SpriteEditorWidget::setView(int view)
{
    for (int i = 0; i < spriteTree->topLevelItemCount(); i++)
    {
        spriteTree->topLevelItem(i)->setHidden(true);
    }
    foreach (QTreeWidgetItem* categoryNode, spriteData.spriteViewPtr(view)->categoryNodes)
    {
        categoryNode->setHidden(false);
    }
    search(searchEdit->text());
}

void SpriteEditorWidget::search(QString text)
{
    foreach (QTreeWidgetItem* categoryNode, spriteData.spriteViewPtr(viewComboBox->currentIndex())->categoryNodes)
    {
        if (categoryNode->data(0, Qt::UserRole).toInt() != -1)
            categoryNode->setHidden(!categoryNode->text(0).contains(text, Qt::CaseInsensitive));
        else
        {
            bool check = false;
            for (int s = 0; s < categoryNode->childCount(); s++)
            {
                categoryNode->child(s)->setHidden(true);
                if (categoryNode->child(s)->text(0).contains(text, Qt::CaseInsensitive))
                {
                    categoryNode->child(s)->setHidden(false);
                    check = true;
                }
            }
            categoryNode->setHidden(!check);
        }
    }
}

void SpriteEditorWidget::handleIndexChange(QTreeWidgetItem *item)
{
    int data = item->data(0, Qt::UserRole).toInt();

    emit(currentSpriteChanged(data));
    editor->updateEditor();
    emit editMade();
}

void SpriteEditorWidget::select(Sprite *sprite)
{
    emit(currentSpriteChanged(sprite->getid()));
}

void SpriteEditorWidget::handleEditDetected()
{
    emit editMade();
}

void SpriteEditorWidget::handleSelectedSpriteChanged(Object* obj)
{
    if(dynamic_cast<Sprite*>(obj) != nullptr)
    {
        spriteDataEditorPtr()->deselect();
        spriteDataEditorPtr()->select(dynamic_cast<Sprite*>(obj));
        emit selectedSpriteChanged(obj);
    }
}

void SpriteEditorWidget::handleUpdateLevelView()
{
    emit updateLevelView();
}

SpriteDataEditorWidget::SpriteDataEditorWidget(SpriteData *spriteData)
{
    this->spriteData = spriteData;

    layout = new QGridLayout();
    layout->setMargin(0);

    spriteName = new QLabel();
    layout->addWidget(spriteName, 0, 0, 1, 2);

    spriteNotesButton = new QPushButton();
    spriteNotesButton->setText(tr("Notes"));
    spriteNotesButton->setToolTipDuration(10000);
    spriteNotesButton->setMaximumHeight(20);
    spriteNotesButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    layout->addWidget(spriteNotesButton, 0, 1, 1, 1, Qt::AlignRight);
    connect(spriteNotesButton, SIGNAL (pressed()),this, SLOT (handleShowNotes()));

    rawSpriteData = new QLineEdit();
    rawSpriteData->setInputMask("HHHH HHHH HHHH HHHH HHHH HHHH");
    layout->addWidget(new QLabel(tr("Raw Sprite Data:")), 1, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(rawSpriteData, 1, 1);
    connect(rawSpriteData, SIGNAL(textEdited(QString)), this, SLOT(handleRawSpriteDataChange(QString)));

    splitterLine = new HorLine();

    layerLabel = new QLabel(tr("Layer:"));
    layerComboBox = new QComboBox();
    QStringList layerNames;
    layerNames << tr("Layer 1") << tr("Layer 2");
    layerComboBox->addItems(layerNames);
    connect(layerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleLayerChanged(int)));

    this->setLayout(layout);
    setHidden(true);
}

void SpriteDataEditorWidget::select(Sprite *sprite)
{
    editSprite = sprite;
    editingASprite = true;
    setHidden(false);

    updateRawSpriteData();

    SpriteDefinition& def = spriteData->getSpriteDef(sprite->getid());

    spriteName->setText(QString("<b>%1 (%2)</b>").arg(def.getName()).arg(sprite->getid()));
    spriteName->setToolTip(def.getNotes());
    spriteNotes = def.getNotes();

    if (spriteNotes == "")
        spriteNotesButton->setToolTip(tr("No notes are available for this sprite."));
    else
        spriteNotesButton->setToolTip(spriteNotes);

    for (int i = 0; i < def.getFieldCount(); i++)
    {
        Field* field = def.getFieldPtr(i);

        if (field->type != Field::Checkbox)
        {
            QLabel* label = new QLabel(field->title + ":");
            layout->addWidget(label, i+2, 0, 1, 1, Qt::AlignRight);
            fieldNames.append(label);
        }

        if (field->type == Field::List)
        {
            SpriteListFieldWidget* fieldWidget = new SpriteListFieldWidget(sprite, field);
            layout->addWidget(fieldWidget, i+2, 1);
            listFieldWidgets.append(fieldWidget);
            connect(fieldWidget, SIGNAL(updateHex()), this, SLOT(updateRawSpriteData()));
            connect(fieldWidget, SIGNAL(updateFields()), this, SLOT(updateFields()));
            connect(fieldWidget, SIGNAL (editMade()), this, SLOT(handleEditDetected()));
        }
        else if (field->type == Field::Checkbox)
        {
            SpriteCheckboxFieldWidget* fieldWidget = new SpriteCheckboxFieldWidget(sprite, field);
            layout->addWidget(fieldWidget, i+2, 1);
            checkboxFieldWidgets.append(fieldWidget);
            connect(fieldWidget, SIGNAL(updateHex()), this, SLOT(updateRawSpriteData()));
            connect(fieldWidget, SIGNAL(updateFields()), this, SLOT(updateFields()));
            connect(fieldWidget, SIGNAL (editMade()), this, SLOT(handleEditDetected()));
        }
        else if (field->type == Field::Value)
        {
            SpriteValueFieldWidget* fieldWidget = new SpriteValueFieldWidget(sprite, field);
            layout->addWidget(fieldWidget, i+2, 1);
            valueFieldWidgets.append(fieldWidget);
            connect(fieldWidget, SIGNAL(updateHex()), this, SLOT(updateRawSpriteData()));
            connect(fieldWidget, SIGNAL(updateFields()), this, SLOT(updateFields()));
            connect(fieldWidget, SIGNAL (editMade()), this, SLOT(handleEditDetected()));
        }
        else if (field->type == Field::Bitfield)
        {
            SpriteBitFieldWidget* fieldWidget = new SpriteBitFieldWidget(sprite, field);
            layout->addWidget(fieldWidget, i+2, 1);
            bitFieldWidgets.append(fieldWidget);
            connect(fieldWidget, SIGNAL(updateHex()), this, SLOT(updateRawSpriteData()));
            connect(fieldWidget, SIGNAL(updateFields()), this, SLOT(updateFields()));
            connect(fieldWidget, SIGNAL (editMade()), this, SLOT(handleEditDetected()));
        }
    }

    // TODO: Only show layer combo box for sprites that use the layer setting
    layerComboBox->setCurrentIndex(editSprite->getLayer());

    int fieldCount = def.getFieldCount();
    layout->addWidget(splitterLine, fieldCount+2, 0, 1, 4);
    layout->addWidget(layerLabel, fieldCount+3, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(layerComboBox, fieldCount+3, 1);
}

void SpriteDataEditorWidget::deselect()
{
    editingASprite = false;
    setHidden(true);

    foreach (QLabel* fieldName, fieldNames) { layout->removeWidget(fieldName); delete fieldName; } fieldNames.clear();
    foreach (SpriteValueFieldWidget* field, valueFieldWidgets) { layout->removeWidget(field); delete field; } valueFieldWidgets.clear();
    foreach (SpriteListFieldWidget* field, listFieldWidgets) { layout->removeWidget(field); delete field; } listFieldWidgets.clear();
    foreach (SpriteCheckboxFieldWidget* field, checkboxFieldWidgets) { layout->removeWidget(field); delete field; } checkboxFieldWidgets.clear();
    foreach (SpriteBitFieldWidget* field, bitFieldWidgets) { layout->removeWidget(field); delete field; } bitFieldWidgets.clear();

    layout->removeWidget(splitterLine);
    layout->removeWidget(layerComboBox);
    layout->removeWidget(layerLabel);
}

void SpriteDataEditorWidget::updateFields()
{
    foreach (SpriteValueFieldWidget* field, valueFieldWidgets) field->updateValue();
    foreach (SpriteListFieldWidget* field, listFieldWidgets) field->updateValue();
    foreach (SpriteCheckboxFieldWidget* field, checkboxFieldWidgets) field->updateValue();
    foreach (SpriteBitFieldWidget* field, bitFieldWidgets) field->updateValue();
}

void SpriteDataEditorWidget::updateRawSpriteData()
{
    QString rawSpriteDataStr;
    for (int i = 0; i < 12; i++) rawSpriteDataStr.append(QString("%1").arg(editSprite->getByte(i), 2, 16, QChar('0')));
    rawSpriteData->setText(rawSpriteDataStr);
    emit updateLevelView();
}

void SpriteDataEditorWidget::handleRawSpriteDataChange(QString text)
{
    QString bytes = text.remove(" ");

    for (int i = 0; i < 12; i++)
        editSprite->setByte(i, (quint8)bytes.mid(i*2, 2).toUInt(0, 16));

    editSprite->setRect();

    updateFields();

    emit updateLevelView();
    emit editMade();
}

void SpriteDataEditorWidget::handleEditDetected()
{
    emit editMade();
}

void SpriteDataEditorWidget::handleShowNotes()
{
    QString name = spriteName->text();
    name.remove(QRegExp("<[^>]*>"));

    QMessageBox notes;
    notes.setWindowTitle(name);

    if (spriteNotes == "")
        notes.setText(tr("No notes are available for this sprite."));
    else
        notes.setText(spriteNotes);

    notes.setDefaultButton(QMessageBox::Ok);
    notes.setIcon(QMessageBox::Information);

    QSpacerItem* spacer = new QSpacerItem(750, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)notes.layout();
    layout->addItem(spacer, layout->rowCount(), 0, 1, layout->columnCount());

    notes.exec();
}

void SpriteDataEditorWidget::handleLayerChanged(int layer)
{
    editSprite->setLayer(layer);
}

void SpriteDataEditorWidget::updateEditor()
{
    if (editingASprite)
        reloadFields();
}

void SpriteDataEditorWidget::reloadFields()
{
    if (editSprite == nullptr)
        return;

    foreach (QLabel* fieldName, fieldNames) { layout->removeWidget(fieldName); delete fieldName; } fieldNames.clear();
    foreach (SpriteValueFieldWidget* field, valueFieldWidgets) { layout->removeWidget(field); delete field; } valueFieldWidgets.clear();
    foreach (SpriteListFieldWidget* field, listFieldWidgets) { layout->removeWidget(field); delete field; } listFieldWidgets.clear();
    foreach (SpriteCheckboxFieldWidget* field, checkboxFieldWidgets) { layout->removeWidget(field); delete field; } checkboxFieldWidgets.clear();
    foreach (SpriteBitFieldWidget* field, bitFieldWidgets) { layout->removeWidget(field); delete field; } bitFieldWidgets.clear();

    layout->removeWidget(splitterLine);
    layout->removeWidget(layerComboBox);
    layout->removeWidget(layerLabel);

    select(editSprite);
}

// Field Widgets

SpriteValueFieldWidget::SpriteValueFieldWidget(Sprite *sprite, Field *field)
{
    this->sprite = sprite;
    this->field = field;
    this->setToolTip(field->comment);

    if (field->posType == Field::Bits)
        setRange(0, (1 << (field->endPos - field->startPos + 1)) - 1);
    else
        setRange(0, (1 << ((field->endPos - field->startPos + 1) * 4)) - 1);

    updateValue();
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange(int)));
}

void SpriteValueFieldWidget::updateValue()
{
    handleValueChanges = false;

    if (field->posType == Field::Bits)
        setValue(sprite->getBits(field->startPos, field->endPos));
    else
        setValue(sprite->getNybbleData(field->startPos, field->endPos));

    handleValueChanges = true;
}

void SpriteValueFieldWidget::handleValueChange(int value)
{
    if (!handleValueChanges) return;

    if (field->posType == Field::Bits)
        sprite->setBits(value, field->startPos, field->endPos);
    else
        sprite->setNybbleData(value, field->startPos, field->endPos);

    sprite->setRect();
    emit updateHex();
    emit updateFields();
    emit editMade();
}


SpriteCheckboxFieldWidget::SpriteCheckboxFieldWidget(Sprite *sprite, Field *field)
{
    this->sprite = sprite;
    this->field = field;
    this->setToolTip(field->comment);
    setText(field->title);

    updateValue();
    connect(this, SIGNAL(toggled(bool)), this, SLOT(handleValueChange(bool)));
}

void SpriteCheckboxFieldWidget::updateValue()
{
    handleValueChanges = false;

    if (field->posType == Field::Bits)
        setChecked(sprite->getBits(field->startPos, field->endPos));
    else
        setChecked((sprite->getNybbleData(field->startPos, field->endPos) & field->mask) == field->mask);

    handleValueChanges = true;
}

void SpriteCheckboxFieldWidget::handleValueChange(bool checked)
{
    if (!handleValueChanges) return;

    if (field->posType == Field::Bits)
    {
        sprite->setBits(checked, field->startPos, field->endPos);
    }
    else
    {
        quint8 newData = sprite->getNybbleData(field->startPos, field->endPos) & (~field->mask);
        if (checked) newData |= field->mask;
        sprite->setNybbleData((int)newData, field->startPos, field->endPos);
    }

    sprite->setRect();
    emit updateHex();
    emit updateFields();
    emit editMade();
}


SpriteListFieldWidget::SpriteListFieldWidget(Sprite *sprite, Field *field)
{
    this->sprite = sprite;
    this->field = field;
    this->setToolTip(field->comment);

    for (int i = 0; i < field->listEntries.count(); i++)
    {
        addItem(field->listEntries[i].second);
        setItemData(i, field->listEntries[i].first);
    }

    updateValue();
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(handleIndexChange(int)));
}

void SpriteListFieldWidget::updateValue()
{
    handleValueChanges = false;

    int fieldData = 0;
    if (field->posType == Field::Bits)
        fieldData = sprite->getBits(field->startPos, field->endPos);
    else
        fieldData = sprite->getNybbleData(field->startPos, field->endPos);

    bool found = false;
    for (int i = 0; i < field->listEntries.count(); i++)
    {
            if (field->listEntries[i].first == fieldData)
            {
                setCurrentIndex(i);
                found = true;
                break;
            }
    }
    if (!found)
        setCurrentIndex(-1);

    handleValueChanges = true;
}

void SpriteListFieldWidget::handleIndexChange(int index)
{
    if (!handleValueChanges) return;

    if (field->posType == Field::Bits)
        sprite->setBits(itemData(index).toInt(), field->startPos, field->endPos);
    else
        sprite->setNybbleData(itemData(index).toInt(), field->startPos, field->endPos);

    sprite->setRect();
    emit updateHex();
    emit updateFields();
    emit editMade();
}


SpriteBitFieldWidget::SpriteBitFieldWidget(Sprite *sprite, Field *field)
{
    this->sprite = sprite;
    this->field = field;
    this->setToolTip(field->comment);

    this->setFrameShape(QFrame::StyledPanel);

    QGridLayout* layout = new QGridLayout();
    layout->setAlignment(this, Qt::AlignCenter);

    int length = field->endPos - field->startPos + 1;
    int numBits = length;

    if (field->posType == Field::Nybbles)
        numBits *= 4;

    for (int i = 0; i < numBits;)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i >= numBits)
                break;

            QCheckBox* checkbox = new QCheckBox();
            layout->addWidget(checkbox, length - i/4, 4 - j);
            checkboxWidgets.append(checkbox);
            connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(handleValueChange()));
            i++;
        }
    }

   updateValue();
    this->setLayout(layout);
}

void SpriteBitFieldWidget::updateValue()
{
    handleValueChanges = false;

    int value = 0;
    int length = field->endPos - field->startPos + 1;

    if (field->posType == Field::Bits)
        value = sprite->getBits(field->startPos, field->endPos);
    else
    {
        value = sprite->getNybbleData(field->startPos, field->endPos);
        length *= 4;
    }

    for(int i = 0; i < length; i++, value = value >> 1)
        checkboxWidgets[i]->setChecked(value & 1);

    handleValueChanges = true;
}

void SpriteBitFieldWidget::handleValueChange()
{
    if (!handleValueChanges) return;

    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
    int bit = checkboxWidgets.indexOf(checkbox);

    if (field->posType == Field::Bits)
    {
        int value = sprite->getBits(field->startPos, field->endPos);
        value ^= 1 << bit;
        sprite->setBits(value, field->startPos, field->endPos);

    }
    else
    {
        int value = sprite->getNybbleData(field->startPos, field->endPos);
        value ^= 1 << bit;
        sprite->setNybbleData(value, field->startPos, field->endPos);
    }

    sprite->setRect();
    emit updateHex();
    emit updateFields();
    emit editMade();
}
