#include "game.h"

#include <QHash>

Game::Game(FilesystemBase* fs, SettingsManager *settingsMgr)
{
    this->fs = fs;
    this->settingsMgr = settingsMgr;
}


Tileset* Game::getTileset(QString name)
{
    return new Tileset(this, name);
}

LevelManager* Game::getLevelManager(QWidget* parent, QString path)
{
    path.prepend("/Course/");
    if (!path.endsWith(".sarc"))
        path.append(".sarc");

    if (!fs->fileExists(path))
        throw std::runtime_error("Level File does not exist.");

    return new LevelManager(parent, this, path);
}

QStandardItemModel* Game::getCourseModel()
{
    QStandardItemModel* model = new QStandardItemModel();

    QDomDocument levelNames;
    QFile f(settingsMgr->getFilePath("levelnames.xml"));
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
    headers << settingsMgr->getTranslation("TilesetEditor", "tileset") << settingsMgr->getTranslation("General", "filename");
    model->setHorizontalHeaderLabels(headers);

    QFile inputFile(settingsMgr->getFilePath("tilesetnames.txt"));
    if (!inputFile.open(QIODevice::ReadOnly))
        return model;

    QHash<QString, QString> defaultNames;

    QTextStream in(&inputFile);
    in.setCodec("UTF-8");
    while (!in.atEnd())
    {
       QStringList parts = in.readLine().split(':');
       defaultNames.insert(parts[0], parts[1]);
    }
    inputFile.close();


    QStandardItem* standardSuite = new QStandardItem(settingsMgr->getTranslation("TilesetEditor", "standard"));
    QStandardItem* stageSuite = new QStandardItem(settingsMgr->getTranslation("TilesetEditor", "stage"));
    QStandardItem* backgroundSuite = new QStandardItem(settingsMgr->getTranslation("TilesetEditor", "background"));
    QStandardItem* interactiveSuite = new QStandardItem(settingsMgr->getTranslation("TilesetEditor", "interactive"));
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

QStandardItemModel* Game::getTilesetModel(int id, bool includeNoneItem)
{
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(2);
    QStringList headers;
    headers << "Tileset" << "Filename";
    model->setHorizontalHeaderLabels(headers);

    QFile inputFile(QCoreApplication::applicationDirPath() + "/coinkiller_data/tilesetnames.txt");
    if (!inputFile.open(QIODevice::ReadOnly))
        return model;

    if (includeNoneItem)
    {
        QList<QStandardItem*> items;

        QStandardItem* tileset = new QStandardItem("<none>");
        tileset->setData("<none>");
        items.append(tileset);

        QStandardItem* fileNameItem = new QStandardItem("");
        fileNameItem->setData("");
        items.append(fileNameItem);

        model->appendRow(items);
    }

    QHash<QString, QString> defaultNames;

    QTextStream in(&inputFile);
    while (!in.atEnd())
    {
       QStringList parts = in.readLine().split(':');
       defaultNames.insert(parts[0], parts[1]);
    }
    inputFile.close();

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

        if (id == 0 && fileName.startsWith("J_"))
        {
            model->appendRow(items);
        }
        else if (id == 1 && fileName.startsWith("M_"))
        {
            model->appendRow(items);
        }
        else if (id == 2 && fileName.startsWith("S1_"))
        {
            model->appendRow(items);
        }
        else if (id == 3 && fileName.startsWith("S2_"))
        {
            model->appendRow(items);
        }
    }

    return model;
}
