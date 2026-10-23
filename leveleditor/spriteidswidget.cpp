#include "spriteidswidget.h"
#include "level.h"
#include "unitsconvert.h"

#include <QGridLayout>
#include <QLabel>
#include <QStringListModel>
#include <algorithm>
#include <cmath>

SpriteIdWidget::SpriteIdWidget(QList<Sprite*> *sprites)
{
    this->sprites = sprites;

    QLabel* viewLabel = new QLabel(tr("View:"));
    viewLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QStringList viewTypes;
    viewTypes << tr("All in Level")
              << tr("Triggering IDs")
              << tr("Target IDs")
              << tr("Rotation IDs")
              << tr("Group IDs")
              << tr("Movement IDs")
              << tr("Path IDs")
              << tr("Camera Limits");

    viewComboBox = new QComboBox();
    viewComboBox->addItems(viewTypes);
    connect(viewComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setView(int)));

    QLabel* searchLabel = new QLabel(tr("Search:"));
    searchLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchEdit = new QLineEdit();
    connect(searchEdit, SIGNAL(textEdited(QString)), this, SLOT(search(QString)));

    QGridLayout* topLayout = new QGridLayout();
    topLayout->setSpacing(6);
    topLayout->addWidget(viewLabel, 0, 0);
    topLayout->addWidget(viewComboBox, 0, 1);
    topLayout->addWidget(searchLabel, 1, 0);
    topLayout->addWidget(searchEdit, 1, 1);

    spriteTree = new SortableTreeWidget();
    spriteTree->setColumnCount(1);
    spriteTree->setHeaderLabels(QStringList() << tr("Sprite") << tr("ID"));
    spriteTree->setSortableColumns({0});

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addItem(topLayout);
    layout->addWidget(spriteTree);
    setLayout(layout);

    updateList();

    connect(spriteTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(handleIndexChange(QTreeWidgetItem*)));
}

void SpriteIdWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        int currentView = viewComboBox->currentIndex();

        spriteData.loadSpriteDefs();
        spriteData.loadSpriteViews();

        viewComboBox->setCurrentIndex(currentView);

        spriteTree->resetColumnSizing();
        updateList();
    }

    QWidget::changeEvent(event);
}

void SpriteIdWidget::search(QString text)
{
    for (int i = 0; i < spriteTree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = spriteTree->topLevelItem(i);

        if (item->text(0).contains(text, Qt::CaseInsensitive) || item->text(1).contains(text, Qt::CaseInsensitive))
            item->setHidden(false);
        else
            item->setHidden(true);
    }
}

void SpriteIdWidget::updateEditor()
{
    updateList();
}

void SpriteIdWidget::updateList()
{
    handleChanges = false;
    QModelIndex index;
    if (spriteTree->selectionModel()->selectedIndexes().size() != 0) index = spriteTree->selectionModel()->selectedIndexes().at(0);
    spriteTree->clear();

    switch (viewMode)
    {
    case 1: // Has Triggering ID
        addIdView("triggering", tr("Triggering ID"));
        break;
    case 2: // Has Target ID
        addIdView("target", tr("Target ID"));
        break;
    case 3: // Has Rotation ID
        addIdView("rotation", tr("Rotation ID"));
        break;
    case 4: // Has Group ID
        addIdView("group", tr("Group ID"));
        break;
    case 5: // Has Movement ID
        addIdView("movement", tr("Movement ID"));
        break;
    case 6: // Has Path ID
        addIdView("path", tr("Path ID"));
        break;
    case 7: // Camera Limits
        addCameraLimitsView();
        break;
    default: // All in Level
    {
        foreach (Sprite* sprite, *sprites)
        {
            SortableTreeItem* spriteItem = new SortableTreeItem();
            spriteItem->setData(0, Qt::DisplayRole, sprite->getid());
            spriteItem->setSortKey(0, static_cast<double>(sprite->getid()));
            spriteItem->setText(1, spriteData.getSpriteDef(sprite->getid()).getName());
            spriteItem->setSortKey(1, spriteData.getSpriteDef(sprite->getid()).getName());
            spriteItem->setText(2, tr("(X: %1, Y: %2)").arg(to16(sprite->getx())).arg(to16(sprite->gety())));
            spriteItem->setSortKey(2, std::hypot(static_cast<double>(to16(sprite->getx())), static_cast<double>(to16(sprite->gety()))));
            spriteItem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(sprite)));
            spriteTree->addTopLevelItem(spriteItem);
        }
        spriteTree->setColumnCount(3);
        spriteTree->setHeaderLabels(QStringList() << tr("ID") << tr("Name") << tr("Position"));
        spriteTree->setSortableColumns({0, 1, 2});
        spriteTree->setFlexibleColumn(1);
        break;
    }
    }
    spriteTree->resort();
    spriteTree->setCurrentIndex(index);

    handleChanges = true;
}

void SpriteIdWidget::addIdView(const QString& match, const QString& header)
{
    foreach (Sprite* sprite, *sprites)
    {
        for (int i = 0; i < spriteData.getSpriteDef(sprite->getid()).getFieldCount(); i++)
        {
            Field* field = spriteData.getSpriteDef(sprite->getid()).getFieldPtr(i);

            if (!field->title.toLower().contains(match) || !field->title.toLower().contains("id"))
                continue;

            int value = sprite->getNybbleData(field->startPos, field->endPos);
            if (value == 0)
                continue;

            SortableTreeItem* spriteItem = new SortableTreeItem();
            spriteItem->setText(0, QString("%1: %2").arg(sprite->getid()).arg(spriteData.getSpriteDef(sprite->getid()).getName()));
            spriteItem->setSortKey(0, static_cast<double>(sprite->getid()));
            spriteItem->setData(1, Qt::DisplayRole, value);
            spriteItem->setSortKey(1, static_cast<double>(value));
            spriteItem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(sprite)));
            spriteTree->addTopLevelItem(spriteItem);
        }
    }

    spriteTree->setColumnCount(2);
    spriteTree->setHeaderLabels(QStringList() << tr("Sprite") << header);
    spriteTree->setSortableColumns({0, 1});
    spriteTree->setFlexibleColumn(0);
}

void SpriteIdWidget::addCameraLimitsView()
{
    QList<Sprite*> leftLimits;
    QList<Sprite*> rightLimits;
    QList<Sprite*> topLimits;
    QList<Sprite*> bottomLimits;
    QList<Sprite*> pathLimits;

    foreach (Sprite* sprite, *sprites)
    {
        switch (sprite->getid())
        {
        case 156:
            leftLimits.append(sprite);
            break;
        case 157:
            rightLimits.append(sprite);
            break;
        case 160:
            bottomLimits.append(sprite);
            break;
        case 161:
            topLimits.append(sprite);
            break;
        case 202:
            pathLimits.append(sprite);
            break;
        default:
            break;
        }
    }

    std::sort(rightLimits.begin(), rightLimits.end(), [](const Sprite* a, const Sprite* b) -> bool { return a->getx() < b->getx(); });
    std::sort(bottomLimits.begin(), bottomLimits.end(), [](const Sprite* a, const Sprite* b) -> bool { return a->gety() < b->gety(); });

    QList<Sprite*> pairedRights;
    QList<Sprite*> pairedBottoms;

    foreach (Sprite* left, leftLimits)
    {
        Sprite* right = nullptr;

        foreach (Sprite* candidate, rightLimits)
        {
            if (!Level::cameraLimitCanPair(left, candidate))
                continue;

            right = candidate;
            break;
        }

        if (right == nullptr)
        {
            addDisconnectedCameraLimit(left);
            continue;
        }

        pairedRights.append(right);
        addCameraLimitItem(left, right, tr("Vertical"));
    }

    foreach (Sprite* right, rightLimits)
        if (!pairedRights.contains(right))
            addDisconnectedCameraLimit(right);

    foreach (Sprite* top, topLimits)
    {
        Sprite* bottom = nullptr;

        foreach (Sprite* candidate, bottomLimits)
        {
            if (!Level::cameraLimitCanPair(top, candidate))
                continue;

            bottom = candidate;
            break;
        }

        if (bottom == nullptr)
        {
            addDisconnectedCameraLimit(top);
            continue;
        }

        pairedBottoms.append(bottom);
        addCameraLimitItem(top, bottom, tr("Horizontal"));
    }

    foreach (Sprite* bottom, bottomLimits)
        if (!pairedBottoms.contains(bottom))
            addDisconnectedCameraLimit(bottom);

    foreach (Sprite* path, pathLimits)
        addCameraLimitItem(path, nullptr, cameraLimitShortName(path));

    spriteTree->setColumnCount(5);
    spriteTree->setHeaderLabels(QStringList() << tr("Camera Limit") << tr("Priority ID") << tr("Direction") << tr("Permeable") << tr("Position"));
    spriteTree->setSortableColumns({0, 1, 2, 3, 4});
    spriteTree->setFlexibleColumn(0);
}

void SpriteIdWidget::addCameraLimitItem(Sprite* sprite, Sprite* partner, const QString& label)
{
    SortableTreeItem* spriteItem = new SortableTreeItem();

    spriteItem->setText(0, label);
    spriteItem->setSortKey(0, label);

    if (sprite->getid() != 202)
    {
        spriteItem->setData(1, Qt::DisplayRole, static_cast<int>(sprite->getNybble(9)));
        spriteItem->setSortKey(1, static_cast<double>(sprite->getNybble(9)));

        bool permeable = sprite->getNybble(8) == 1 && (partner == nullptr || partner->getNybble(8) == 1);
        spriteItem->setText(3, permeable ? tr("Yes") : tr("No"));
        spriteItem->setSortKey(3, permeable ? 1.0 : 0.0);
    }

    spriteItem->setText(2, cameraLimitDirection(sprite));
    spriteItem->setSortKey(2, static_cast<double>(sprite->getNybble(4)));

    spriteItem->setText(4, tr("(X: %1, Y: %2)").arg(to16(sprite->getx())).arg(to16(sprite->gety())));
    spriteItem->setSortKey(4, std::hypot(static_cast<double>(to16(sprite->getx())), static_cast<double>(to16(sprite->gety()))));

    spriteItem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(sprite)));
    spriteTree->addTopLevelItem(spriteItem);
}

void SpriteIdWidget::addDisconnectedCameraLimit(Sprite* sprite)
{
    addCameraLimitItem(sprite, nullptr, QString("%1 %2")
                       .arg(cameraLimitShortName(sprite))
                       .arg(tr("(Unpaired)")));
}

QString SpriteIdWidget::cameraLimitShortName(Sprite* sprite)
{
    switch (sprite->getid())
    {
    case 156:
        return tr("Vertical L");
    case 157:
        return tr("Vertical R");
    case 160:
        return tr("Horizontal B");
    case 161:
        return tr("Horizontal T");
    case 202:
        return tr("Path");
    default:
        return spriteData.getSpriteDef(sprite->getid()).getName();
    }
}

QString SpriteIdWidget::cameraLimitDirection(Sprite* sprite)
{
    int direction = sprite->getNybble(4);

    if (sprite->getid() == 160 || sprite->getid() == 161)
        return direction == 1 ? tr("Right") : tr("Left");

    return direction == 1 ? tr("Below") : tr("Above");
}

void SpriteIdWidget::deselect()
{
    spriteTree->clearSelection();
    updateList();
}

void SpriteIdWidget::setView(int view)
{
    switch (view) {
    case 1: // Has Triggering ID
        viewMode = 1;
        break;
    case 2: // Has Target ID
        viewMode = 2;
        break;
    case 3: // Has Rotation ID
        viewMode = 3;
        break;
    case 4: // Has Group ID
        viewMode = 4;
        break;
    case 5: // Has Movement ID
        viewMode = 5;
        break;
    case 6: // Has Path ID
        viewMode = 6;
        break;
    case 7: // Camera Limits
        viewMode = 7;
        break;
    default: // All in Level
        viewMode = 0;
        break;
    }
    spriteTree->resetColumnSizing();
    deselect();
}

void SpriteIdWidget::handleIndexChange(QTreeWidgetItem *spriteItem)
{
    if (!handleChanges || spriteItem == nullptr)
        return;

    Sprite* sprite = static_cast<Sprite*>(spriteItem->data(0, Qt::UserRole).value<void*>());
    if (sprite != nullptr)
        emit selectedSpriteChanged(sprite);

    emit updateLevelView();
}
