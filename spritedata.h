#ifndef SPRITEDATA_H
#define SPRITEDATA_H

#include <QList>
#include <QtXml>
#include <QTreeWidgetItem>
#include <QMap>
#include <QHash>

class Field
{
public:

    enum fieldType
    {
        List,
        Value,
        Checkbox,
        Bitfield
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
    SpriteDefinition();
    SpriteDefinition(int spriteId);
    SpriteDefinition(QDomElement spriteElement);
    QString getName() const { return name; }
    int getID() const { return id; }
    QString getNotes() const { return notes; }
    Field* getFieldPtr(int id) { return &fields[id]; }
    int getFieldCount() const { return fields.size(); }

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
    spriteView* spriteViewPtr(int view) { return &spriteViews[view]; }
    SpriteDefinition& getSpriteDef(int nbr);

    int spriteCount() { return spriteDefs.size(); }

    void loadSpriteDefs();
    void loadSpriteViews();

private:
    QList<spriteView> spriteViews;
    QHash<int, SpriteDefinition> spriteDefs;
};

#endif // SPRITEDATA_H
