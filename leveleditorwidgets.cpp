#include "leveleditorwidgets.h"

#include <QGridLayout>
#include <QLabel>
#include <QStringListModel>

SpritePicker::SpritePicker()
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
    layout->setMargin(0);
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
    search(searchEdit->text());
}

void SpritePicker::search(QString text)
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

void SpritePicker::handleIndexChange(QTreeWidgetItem *item)
{
    int data = item->data(0, Qt::UserRole).toInt();
    if (data >= 0 && data <= 325)
        emit(selectedSpriteChanged(data));
}
