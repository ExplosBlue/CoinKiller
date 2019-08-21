#include "game.h"
#include "settingsmanager.h"

#include <QHash>

Game::Game(const QString& path)
{
    this->fs = new ExternalFilesystem(path);
    this->path = path;
}

Game::~Game()
{
    delete fs;
}


Tileset* Game::getTileset(QString name)
{
    QString path = name;
    path.prepend("/Unit/");
    if (!path.endsWith(".sarc"))
        path.append(".sarc");

    if (!fs->fileExists(path))
        throw std::runtime_error("Tileset File does not exist.");

    return new Tileset(this, name);
}

LevelManager* Game::getLevelManager(WindowBase* parent, QString path)
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
    SettingsManager* settingsMgr = SettingsManager::getInstance();

    QStandardItemModel* model = new QStandardItemModel();

    QList<QString> dirList;
    fs->directoryContents("/Course/", QDir::Dirs, dirList);
    dirList.push_front("");

    QMap<int, QString> levelNames;
    QFile ln(SettingsManager::getInstance()->getFilePath("levelnames.txt"));
    if (ln.open(QIODevice::ReadOnly))
    {
        QTextStream in(&ln);
        in.setCodec("UTF-8");

        levelNames.clear();
        while(!in.atEnd())
        {
            QStringList parts = in.readLine().split(":");
            if (parts.count() == 2)
                levelNames.insert(parts[0].toInt(), parts[1]);
        }

        ln.close();
    }

    QMap<int, QString> worldNames;
    QFile wn(SettingsManager::getInstance()->getFilePath("worldnames.txt"));
    if (wn.open(QIODevice::ReadOnly))
    {
        QTextStream in(&wn);
        in.setCodec("UTF-8");

        worldNames.clear();
        while(!in.atEnd())
        {
            QStringList parts = in.readLine().split(":");
            if (parts.count() == 2)
                worldNames.insert(parts[0].toInt(), parts[1]);
        }

        wn.close();
    }
    foreach (QString dir, dirList)
    {
        // Sub Directories
        QList<QString> courseList;
        QStandardItem* dirItem = new QStandardItem(dir);

        if (dir == "")
        {
            fs->directoryContents("/Course/", QDir::Files, courseList);

            if (courseList.isEmpty())
                continue;

            dirItem = model->invisibleRootItem();
        }
        else
        {
            fs->directoryContents("/Course/" + dir, QDir::Files, courseList);

            if (courseList.isEmpty())
                continue;

            model->invisibleRootItem()->appendRow(dirItem);
        }

        QStandardItem* worldItemPtr = nullptr;

        foreach(QString filename, courseList)
        {
            QRegularExpression re("^\\d+-\\d+\\.sarc$");
            if (!re.match(filename, 0, QRegularExpression::NormalMatch).hasMatch())
                continue;

            QString worldName = worldNames.value(filename.split(".")[0].split("-")[0].toInt());

            QString worldNum = filename.split(".")[0].split("-")[0];

            if (worldName.isEmpty())
                worldName = worldNum;

            // World Categories
            if (worldItemPtr == nullptr || worldItemPtr->text() != QString(worldName))
            {
                QStandardItem* subItem = new QStandardItem(worldName);
                worldItemPtr = subItem;
                dirItem->appendRow(subItem);
            }

            // Levels
            if (worldItemPtr != nullptr)
            {
                QString levelName = levelNames.value(filename.split(".")[0].split("-")[1].toInt()).arg(worldName);

                if (levelName.isEmpty())
                    levelName = filename;

                QStandardItem* subItem = new QStandardItem(levelName);
                if (dir != "")
                    subItem->setData(dir + "/" + filename);
                else
                    subItem->setData(filename);
                worldItemPtr->appendRow(subItem);
            }
        }
    }

    model->sort(0, Qt::AscendingOrder);

    return model;
}

QStandardItemModel* Game::getTilesetModel()
{
    SettingsManager* settingsMgr = SettingsManager::getInstance();

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
