#include "spritedata.h"

SpriteData::SpriteData()
{
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

    // Load Sprite Views
    QDomDocument xmlSpriteViews;
    QFile f2(QCoreApplication::applicationDirPath() + "/coinkiller_data/spritecategories.xml");
    if (!f2.open(QIODevice::ReadOnly))
        return;
    xmlSpriteViews.setContent(&f2);
    f2.close();

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

SpriteDefinition::SpriteDefinition(QDomElement spriteElement)
{
    id = spriteElement.attribute("id", "999").toInt();
    name = spriteElement.attribute("name", "Invalid Name");
}
