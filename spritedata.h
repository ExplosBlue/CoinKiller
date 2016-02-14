#ifndef SPRITEDATA_H
#define SPRITEDATA_H

#include <QList>
#include <QtXml>
#include <QTreeWidgetItem>

class SpriteDefinition
{
public:
    SpriteDefinition(QDomElement spriteElement);
    QString getName() const { return name; }

private:
    QString name;
    int id;
};

struct spriteCategory
{
    QList<int> sprites;
    QString name;
};

struct spriteView
{
    QString name;
    QList<spriteCategory> categories;
    QList<QTreeWidgetItem*> categoryNodes;
};

class SpriteData
{
public:
    SpriteData();
    int spriteViewCount() { return spriteViews.count(); }
    spriteView *spriteViewPtr(int view) { return &spriteViews[view]; }
    SpriteDefinition getSpriteDef(int nbr) { return spriteDefs[nbr]; }

private:
    QList<spriteView> spriteViews;
    QList<SpriteDefinition> spriteDefs;
};

#endif // SPRITEDATA_H
