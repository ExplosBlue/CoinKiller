#include "spritedata.h"

SpriteData::SpriteData()
{
    bool customSprites = true;

    // Load Sprite Definitions
    QDomDocument xmlSpriteData;
    QFile f1(QCoreApplication::applicationDirPath() + "/coinkiller_data/spritedata.xml");
    if (!f1.open(QIODevice::ReadOnly))
        return;
    xmlSpriteData.setContent(&f1);
    f1.close();

    QDomElement spriteElement = xmlSpriteData.documentElement().firstChild().toElement();
    while (!spriteElement.isNull())
    {
        spriteDefs.append(SpriteDefinition(spriteElement));

        spriteElement = spriteElement.nextSiblingElement();
    }

    // Load Custom Sprite Definitions
    QDomDocument xmlSpriteDataNext;
    QFile f2(QCoreApplication::applicationDirPath() + "/coinkiller_data/spritedataNext.xml");
    if (!f2.open(QIODevice::ReadOnly))
        customSprites = false;
    else
    {
        xmlSpriteDataNext.setContent(&f2);
        f1.close();
    }

    if (customSprites)
    {
        QDomElement spriteElementNext = xmlSpriteDataNext.documentElement().firstChild().toElement();
        while (!spriteElementNext.isNull())
        {
            spriteDefsNext.append(SpriteDefinition(spriteElementNext));

            spriteElementNext = spriteElementNext.nextSiblingElement();
        }

        // Replace Spritedata For Existing Sprites With Custom Data
        foreach (SpriteDefinition retailSpr, spriteDefs)
        {
            foreach (SpriteDefinition nextSpr, spriteDefsNext)
            {
                if (retailSpr.getID() == nextSpr.getID())
                    spriteDefs.replace(retailSpr.getID(), nextSpr);
            }
        }
        // Add Spritedata For New Sprites
        foreach (SpriteDefinition nextSpr, spriteDefsNext)
        {
            if (nextSpr.getID() > 325)
                spriteDefs.append(nextSpr);
        }
    }

    // Load Sprite Views
    QDomDocument xmlSpriteViews;
    QFile f3(QCoreApplication::applicationDirPath() + "/coinkiller_data/spritecategories.xml");
    if (!f3.open(QIODevice::ReadOnly))
        return;
    xmlSpriteViews.setContent(&f3);
    f3.close();

    // Add "All (Sorted by Number)" View
    spriteView allView;
    allView.name = "All (Sorted by Number)";
    foreach (SpriteDefinition sprDef, spriteDefs)
    {
        allView.simpleSprites.append(sprDef.getID());
    }
    spriteViews.append(allView);

    QDomNodeList views = xmlSpriteViews.documentElement().elementsByTagName("view");
    for (int v = 0; v < views.size(); v++)
    {
        QDomElement viewElement = views.at(v).toElement();

        spriteView view;
        view.name = viewElement.attribute("name");

        if (view.name == "SMBNext Code Hacks" && !customSprites)
            break;

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
                    {
                        if (i > 325 && !customSprites)
                            break;

                        category.sprites.append(i);
                    }
                }
                else if (attachStr.contains(','))
                {
                    QStringList a = attachStr.split(',');
                    foreach (QString str, a)
                    {
                        if (str.toInt() > 325 && !customSprites)
                            break;

                        category.sprites.append(str.toInt());
                    }
                }
                else
                {
                    if (attachStr.toInt() > 325 && !customSprites)
                        break;

                    category.sprites.append(attachStr.toInt());
                }
            }

            view.categories.append(category);
        }

        spriteViews.append(view);
    }
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

        QString nybbleStr = fieldElement.attribute("nybble");
        if (nybbleStr.contains('-'))
        {
            QStringList nybbleParts = nybbleStr.split('-');
            field.startNybble = nybbleParts[0].toInt()-1;
            field.endNybble = nybbleParts[1].toInt()-1;
        }
        else
        {
            field.startNybble = fieldElement.attribute("nybble").toInt()-1;
            field.endNybble = field.startNybble;
        }

        if (fieldElement.tagName() == "value") field.type = Field::Value;

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
                field.listEntries.insert(entryElement.attribute("value").toInt(), entryElement.text());
            }
        }

        fields.append(field);

        fieldElement = fieldElement.nextSibling().toElement();
    }
}
