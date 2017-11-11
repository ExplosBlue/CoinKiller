#include "spriteeditorwidget.h"
#include "unitsconvert.h"

#include <QGridLayout>
#include <QLabel>
#include <QStringListModel>

SpriteEditorWidget::SpriteEditorWidget()
{
    QLabel* viewLabel = new QLabel("View:");
    viewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    viewComboBox = new QComboBox();
    connect(viewComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setView(int)));

    QLabel* searchLabel = new QLabel("Search:");
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

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(topLayout);
    layout->addWidget(spriteTree);

    QStringList viewNames;

    for (int v = 0; v < spriteData.spriteViewCount(); v++)
    {
        spriteView* view = spriteData.spriteViewPtr(v);

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

    viewComboBox->setModel(new QStringListModel(viewNames));

    editor = new SpriteDataEditorWidget(&spriteData);
    layout->addWidget(editor);

    setLayout(layout);

    connect(spriteTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(handleIndexChange(QTreeWidgetItem*)));
    connect(editor, SIGNAL(editMade()), this, SLOT(handleEditDetected()));

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
    if (data >= 0 && data <= 325)
        emit(selectedSpriteChanged(data));
    emit editMade();
}

void SpriteEditorWidget::select(Sprite *sprite)
{
    emit(selectedSpriteChanged(sprite->getid()));
}

void SpriteEditorWidget::handleEditDetected()
{
    emit editMade();
}


SpriteDataEditorWidget::SpriteDataEditorWidget(SpriteData *spriteData)
{
    this->spriteData = spriteData;
    layout = new QGridLayout();
    layout->setMargin(0);

    spriteName = new QLabel();
    layout->addWidget(spriteName, 0, 0, 1, 2);

    rawSpriteData = new QLineEdit();
    rawSpriteData->setInputMask("HHHH HHHH HHHH HHHH HHHH HHHH");
    layout->addWidget(new QLabel("Raw Sprite Data:"), 1, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(rawSpriteData, 1, 1);
    connect(rawSpriteData, SIGNAL(textEdited(QString)), this, SLOT(handleRawSpriteDataChange(QString)));

    this->setLayout(layout);
    setHidden(true);
}

void SpriteDataEditorWidget::select(Sprite *sprite)
{
    editSprite = sprite;
    editingASprite = true;
    setHidden(false);

    updateRawSpriteData();

    SpriteDefinition* def = spriteData->getSpriteDefPtr(sprite->getid());

    spriteName->setText(QString("<b>%1 (%2)</b>").arg(def->getName()).arg(sprite->getid()));
    spriteName->setToolTip(def->getNotes());

    for (int i = 0; i < spriteData->getSpriteDefPtr(sprite->getid())->getFieldCount(); i++)
    {
        Field* field = spriteData->getSpriteDefPtr(sprite->getid())->getFieldPtr(i);

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
    }
}

void SpriteDataEditorWidget::deselect()
{
    editingASprite = false;
    setHidden(true);

    foreach (QLabel* fieldName, fieldNames) { layout->removeWidget(fieldName); delete fieldName; } fieldNames.clear();
    foreach (SpriteValueFieldWidget* field, valueFieldWidgets) { layout->removeWidget(field); delete field; } valueFieldWidgets.clear();
    foreach (SpriteListFieldWidget* field, listFieldWidgets) { layout->removeWidget(field); delete field; } listFieldWidgets.clear();
    foreach (SpriteCheckboxFieldWidget* field, checkboxFieldWidgets) { layout->removeWidget(field); delete field; } checkboxFieldWidgets.clear();
}

void SpriteDataEditorWidget::updateFields()
{
    foreach (SpriteValueFieldWidget* field, valueFieldWidgets) field->updateValue();
    foreach (SpriteListFieldWidget* field, listFieldWidgets) field->updateValue();
    foreach (SpriteCheckboxFieldWidget* field, checkboxFieldWidgets) field->updateValue();
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


// Field Widgets

SpriteValueFieldWidget::SpriteValueFieldWidget(Sprite *sprite, Field *field)
{
    this->sprite = sprite;
    this->field = field;
    this->setToolTip(field->comment);
    setRange(0, (1 << ((field->endNybble - field->startNybble + 1) * 4)) - 1);

    updateValue();
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange(int)));
}

void SpriteValueFieldWidget::updateValue()
{
    handleValueChanges = false;
    setValue(sprite->getNybbleData(field->startNybble, field->endNybble));
    handleValueChanges = true;
}

void SpriteValueFieldWidget::handleValueChange(int value)
{
    if (!handleValueChanges) return;
    sprite->setNybbleData(value, field->startNybble, field->endNybble);
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
    setChecked((sprite->getNybbleData(field->startNybble, field->endNybble) & field->mask) == field->mask);
    handleValueChanges = true;
}

void SpriteCheckboxFieldWidget::handleValueChange(bool checked)
{
    if (!handleValueChanges) return;

    quint8 newData = sprite->getNybbleData(field->startNybble, field->endNybble) & (~field->mask);
    if (checked) newData |= field->mask;
    sprite->setNybbleData((int)newData, field->startNybble, field->endNybble);
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

    addItems(field->listEntries.values());

    updateValue();
    connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleIndexChange(QString)));
}

void SpriteListFieldWidget::updateValue()
{
    handleValueChanges = false;
    if (field->listEntries.keys().contains(sprite->getNybbleData(field->startNybble, field->endNybble))) setCurrentText(field->listEntries.value(sprite->getNybbleData(field->startNybble, field->endNybble)));
    else setCurrentIndex(-1);
    handleValueChanges = true;
}

void SpriteListFieldWidget::handleIndexChange(QString text)
{
    if (!handleValueChanges) return;
    sprite->setNybbleData(field->listEntries.key(text, 0), field->startNybble, field->endNybble);
    sprite->setRect();
    emit updateHex();
    emit updateFields();
    emit editMade();
}
