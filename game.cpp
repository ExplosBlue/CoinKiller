#include "game.h"

#include <QHash>

Game::Game(FilesystemBase* fs)
{
    this->fs = fs;
}


Tileset* Game::getTileset(QString name)
{
    return new Tileset(this, name);
}

Level* Game::getLevel(QString path, int area)
{
    return new Level(this, path, area);
}

QStandardItemModel* Game::getCourseModel()
{
    QStandardItemModel* model = new QStandardItemModel();

    QDomDocument levelNames;
    QFile f(QCoreApplication::applicationDirPath() + "/coinkiller_data/levelnames.xml");
    if (!f.open(QIODevice::ReadOnly))
        return model;
    levelNames.setContent(&f);
    f.close();

    loadLevelNamesCat(model->invisibleRootItem(), levelNames.documentElement());

    return model;
}

void Game::loadLevelNamesCat(QStandardItem* item, QDomElement node)
{
    QDomElement element = node.firstChild().toElement();
    while (!element.isNull())
    {
        if (element.tagName() == "level")
        {
            QStandardItem* subItem = new QStandardItem(element.attribute("name"));
            subItem->setData(element.attribute("file"));
            item->appendRow(subItem);
        }
        else if (element.tagName() == "category")
        {
            QStandardItem* subItem = new QStandardItem(element.attribute("name"));
            loadLevelNamesCat(subItem, element);
            item->appendRow(subItem);
        }

        element = element.nextSibling().toElement();
    }
}

QStandardItemModel* Game::getTilesetModel()
{
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(2);
    QStringList headers;
    headers << "Tileset" << "Filename";
    model->setHorizontalHeaderLabels(headers);

    QFile inputFile(QCoreApplication::applicationDirPath() + "/coinkiller_data/tilesetnames.txt");
    if (!inputFile.open(QIODevice::ReadOnly))
        return model;

    QHash<QString, QString> defaultNames;

    QTextStream in(&inputFile);
    while (!in.atEnd())
    {
       QStringList parts = in.readLine().split(':');
       defaultNames.insert(parts[0], parts[1]);
    }
    inputFile.close();


    QStandardItem* standardSuite = new QStandardItem(QString("Standard Suite"));
    QStandardItem* stageSuite = new QStandardItem(QString("Stage Suite"));
    QStandardItem* backgroundSuite = new QStandardItem(QString("Background Suite"));
    QStandardItem* interactiveSuite = new QStandardItem(QString("Interactive Suite"));
    model->appendRow(standardSuite);
    model->appendRow(stageSuite);
    model->appendRow(backgroundSuite);
    model->appendRow(interactiveSuite);

    QList<QString> tilesetfiles;
    fs->directoryContents("/Unit", QDir::Files, tilesetfiles);

    for (int i = 0; i < tilesetfiles.length(); i++)
    {
        QString fileName = tilesetfiles[i];
        fileName.chop(5);

        QString tilesetname;

        tilesetname = defaultNames.value(fileName, fileName);

        QList<QStandardItem*> items;

        QStandardItem* tileset = new QStandardItem(tilesetname);
        tileset->setData(fileName);
        items.append(tileset);

        QStandardItem* fileNameItem = new QStandardItem(fileName);
        fileNameItem->setData(fileName);
        items.append(fileNameItem);

        if (fileName.startsWith("J_"))
        {
            standardSuite->appendRow(items);
        }
        else if (fileName.startsWith("M_"))
        {
            stageSuite->appendRow(items);
        }
        else if (fileName.startsWith("S1_"))
        {
            backgroundSuite->appendRow(items);
        }
        else if (fileName.startsWith("S2_"))
        {
            interactiveSuite->appendRow(items);
        }
    }

    return model;
}
