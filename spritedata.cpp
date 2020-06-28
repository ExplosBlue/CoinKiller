#include "spritedata.h"
#include "settingsmanager.h"

SpriteData::SpriteData()
{
    loadSpriteDefs();
    loadSpriteViews();
}

void SpriteData::loadSpriteDefs()
{
    spriteDefs.clear();

    QDomDocument xmlSpriteData;
    QFile f(SettingsManager::getInstance()->getFilePath("spritedata.xml"));
    if (!f.open(QIODevice::ReadOnly))
        return;
    xmlSpriteData.setContent(&f);
    f.close();

    spriteDataVersion =  xmlSpriteData.documentElement().attribute("version", "1.0");

    QDomElement spriteElement = xmlSpriteData.documentElement().firstChild().toElement();
    while (!spriteElement.isNull())
    {
        SpriteDefinition def(spriteElement);
        spriteDefs.insert(def.getID(), def);

        spriteElement = spriteElement.nextSiblingElement();
    }
}

void SpriteData::loadSpriteViews()
{
    spriteViews.clear();

    QDomDocument xmlSpriteViews;
    QFile f2(SettingsManager::getInstance()->getFilePath("spritecategories.xml"));
    if (!f2.open(QIODevice::ReadOnly))
        return;
    xmlSpriteViews.setContent(&f2);
    f2.close();

    // Add "All (Sorted by Number)" View
    spriteView allView;
    allView.name = QObject::tr("All (Sorted by Number)");
    foreach (SpriteDefinition sprDef, spriteDefs)
    {
        allView.simpleSprites.append(sprDef.getID());
    }
    std::sort(allView.simpleSprites.begin(), allView.simpleSprites.end());
    spriteViews.append(allView);

    QDomNodeList views = xmlSpriteViews.documentElement().elementsByTagName("view");
    for (int v = 0; v < views.size(); v++)
    {
        QDomElement viewElement = views.at(v).toElement();

        spriteView view;
        view.name = viewElement.attribute("name");

        QDomNodeList cats = viewElement.elementsByTagName("category");
        for (int c = 0; c < cats.size(); c++)
        {
            QDomElement categoryElement = cats.at(c).toElement();

            spriteCategory category;
            category.name = categoryElement.attribute("name");

            QDomNodeList attachs = categoryElement.elementsByTagName("attach");
            for (int a = 0; a < attachs.size(); a++)
            {
                QString attachStr = attachs.at(a).toElement().attribute("sprite");
                if (attachStr.contains('-'))
                {
                    QStringList a = attachStr.split('-');
                    for (int i = a[0].toInt(); i < a[1].toInt()+1; i++)
                        category.sprites.append(i);
                }
                else if (attachStr.contains(','))
                {
                    QStringList a = attachStr.split(',');
                    foreach (QString str, a)
                        category.sprites.append(str.toInt());
                }
                else
                    category.sprites.append(attachStr.toInt());
            }

            view.categories.append(category);
        }

        spriteViews.append(view);
    }
}

SpriteDefinition& SpriteData::getSpriteDef(int nbr)
{
    if (!spriteDefs.contains(nbr))
    {
        SpriteDefinition def(nbr);
        spriteDefs.insert(nbr, def);
    }

    return spriteDefs[nbr];
}


SpriteDefinition::SpriteDefinition()
{
    throw std::runtime_error("SpriteDefinition::SpriteDefinition() called. SpriteData key error!");
}

SpriteDefinition::SpriteDefinition(int spriteId)
{
    id = spriteId;
    name = QObject::tr("Unknown Sprite %1").arg(spriteId);
    notes = "";
}

SpriteDefinition::SpriteDefinition(QDomElement spriteElement)
{
    id = spriteElement.attribute("id", "999").toInt();
    name = spriteElement.attribute("name", "Invalid Name");
    notes = spriteElement.attribute("notes", "");

    QDomElement fieldElement = spriteElement.firstChild().toElement();
    while (!fieldElement.isNull())
    {
        Field field;
        field.title = fieldElement.attribute("title", "INVALID TITLE");
        field.comment = fieldElement.attribute("comment", "");

        QString posTypeStr = fieldElement.attribute("pos_type");

        if (posTypeStr == "nybbles")
            field.posType = Field::Nybbles;
        else if (posTypeStr == "bits")
            field.posType = Field::Bits;
        else
        {
            qDebug() << QString("Sprite %1 field %2 has invalid pos_type.").arg(id).arg(field.title);
            fieldElement = fieldElement.nextSibling().toElement();
            continue;
        }

        QString posString = fieldElement.attribute("pos");

        if (posString.contains('-'))
        {
            QStringList posParts = posString.split('-');
            field.startPos = posParts[0].toInt();
            field.endPos = posParts[1].toInt();
        }
        else
        {
            field.startPos = posString.toInt();
            field.endPos = field.startPos;
        }

        if (fieldElement.tagName() == "value")
            field.type = Field::Value;

        else if (fieldElement.tagName() == "checkbox")
        {
            field.type = Field::Checkbox;
            quint32 mask = fieldElement.attribute("mask", "1").toUInt();
            if (mask > 255) mask = 1;
            field.mask = mask;
        }

        else if (fieldElement.tagName() == "list")
        {
            field.type = Field::List;
            QDomNodeList entries = fieldElement.elementsByTagName("entry");
            for (int i = 0; i < entries.size(); i++)
            {
                QDomElement entryElement = entries.at(i).toElement();
                QPair<int, QString> entry;
                entry.first = entryElement.attribute("value").toInt();
                entry.second = entryElement.text();
                field.listEntries.append(entry);
            }
        }

        else if (fieldElement.tagName() == "bitfield")
            field.type = Field::Bitfield;

        fields.append(field);

        fieldElement = fieldElement.nextSibling().toElement();
    }
}
