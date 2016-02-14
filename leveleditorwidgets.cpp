#include "leveleditorwidgets.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStringListModel>

SpritePicker::SpritePicker()
{
    QLabel* viewLabel = new QLabel("View:");
    viewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    viewComboBox = new QComboBox();
    connect(viewComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setView(int)));
    spriteTree = new SpritePickerTree();

    QHBoxLayout* viewLayout = new QHBoxLayout();
    viewLayout->setSpacing(6);
    viewLayout->addWidget(viewLabel);
    viewLayout->addWidget(viewComboBox);


    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addItem(viewLayout);
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
    }

    viewComboBox->setModel(new QStringListModel(viewNames));
    setLayout(layout);

    connect(spriteTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(handleIndexChange(QTreeWidgetItem*)));
}

void SpritePicker::setView(int view)
{
    for (int i = 0; i < spriteTree->topLevelItemCount(); i++)
    {
        spriteTree->topLevelItem(i)->setHidden(true);
    }
    foreach (QTreeWidgetItem* categoryNode, spriteData.spriteViewPtr(view)->categoryNodes)
    {
        categoryNode->setHidden(false);
    }
}

void SpritePicker::handleIndexChange(QTreeWidgetItem *item)
{
    int data = item->data(0, Qt::UserRole).toInt();
    if (data >= 0 && data <= 325)
        emit(selectedSpriteChanged(data));
}

SpritePicker::SpritePickerTree::SpritePickerTree()
{
    setColumnCount(1);
    setHeaderHidden(true);
}
