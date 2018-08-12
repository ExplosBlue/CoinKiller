#ifndef SPRITEDATA_H
#define SPRITEDATA_H

#include <QList>
#include <QtXml>
#include <QTreeWidgetItem>
#include <QMap>

class Field
{
public:

    enum fieldType
    {
        List,
        Value,
        Checkbox
    };

    Field() {}
    QString title;
    QString comment;
    fieldType type;
    int startNybble;
    int endNybble;

    QMap<int, QString> listEntries;
    quint8 mask;
};

class SpriteDefinition
{
public:
    SpriteDefinition(QDomElement spriteElement);
    QString getName() const { return name; }
    int getID() const { return id; }
    QString getNotes() const { return notes; }
    Field* getFieldPtr(int id) { return &fields[id]; }
    int getFieldCount() { return fields.size(); }

private:
    QString name;
    int id;
    QString notes;
    QList<Field> fields;
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
    QList<int> simpleSprites;
    QList<QTreeWidgetItem*> categoryNodes;
};

class SpriteData
{
public:
    SpriteData();
    int spriteViewCount() { return spriteViews.count(); }
    spriteView *spriteViewPtr(int view) { return &spriteViews[view]; }
    SpriteDefinition getSpriteDef(int nbr) { return spriteDefs[nbr]; }
    SpriteDefinition* getSpriteDefPtr(int nbr) { return &spriteDefs[nbr]; }

    int spriteCount() { return spriteDefs.size(); }

private:
    QList<spriteView> spriteViews;
    QList<SpriteDefinition> spriteDefs;
};

#endif // SPRITEDATA_H
