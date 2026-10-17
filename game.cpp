#include "game.h"
#include "settingsmanager.h"

#include <QHash>
#include <QFile>
#include <QDir>

class CourseItem : public QStandardItem
{
public:
    using QStandardItem::QStandardItem;
    bool operator<(const QStandardItem &other) const override
    {
        QVariant a = data(Qt::UserRole);
        QVariant b = other.data(Qt::UserRole);
        if (a.isValid() && b.isValid())
            return a.toString() < b.toString();
        return QStandardItem::operator<(other);
    }
};

Game::Game(const QString& path)
{
    this->fs = new ExternalFilesystem(path);
    this->tilesetList = new TilesetList(fs);
    this->path = path;
}

Game::~Game()
{
    delete tilesetList;
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
    QStandardItemModel* model = new QStandardItemModel();

    QList<QString> dirList;
    fs->directoryContents("/Course/", QDir::Dirs, dirList);
    dirList.push_front("");

    QMap<int, QString> levelNames;
    QFile ln(SettingsManager::getInstance()->getFilePath("levelnames.txt"));
    if (ln.open(QIODevice::ReadOnly))
    {
        QTextStream in(&ln);
        in.setEncoding(QStringConverter::Utf8);

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
        in.setEncoding(QStringConverter::Utf8);

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

            delete dirItem;
            if (courseList.isEmpty()) {
                continue;
            }

            dirItem = model->invisibleRootItem();
        }
        else
        {
            fs->directoryContents("/Course/" + dir, QDir::Files, courseList);

            if (courseList.isEmpty()) {
                delete dirItem;
                continue;
            }

            model->invisibleRootItem()->appendRow(dirItem);
        }

        QStandardItem* worldItemPtr = nullptr;

        foreach(QString filename, courseList)
        {
            QRegularExpression re("^\\d+-\\d+\\.sarc$");
            if (!re.match(filename, 0, QRegularExpression::NormalMatch).hasMatch())
                continue;

            QString worldNum = filename.split(".")[0].split("-")[0];
            QString levelNum = filename.split(".")[0].split("-")[1];

            QString worldName = worldNames.value(worldNum.toInt());

            if (worldName.isEmpty())
                worldName = worldNum;

            // World Categories
            if (worldItemPtr == nullptr || worldItemPtr->text() != QString(worldName))
            {
                CourseItem* subItem = new CourseItem(worldName);
                subItem->setData(worldNum.rightJustified(3, '0'), Qt::UserRole);
                worldItemPtr = subItem;
                dirItem->appendRow(subItem);
            }

            // Levels
            if (worldItemPtr != nullptr)
            {
                QString levelName = levelNames.value(levelNum.toInt());

                if (levelName.isEmpty())
                    levelName = worldName + " - " + levelNum;
                else
                    levelName = levelName.arg(worldName);

                CourseItem* subItem = new CourseItem(levelName);
                subItem->setData(levelNum.rightJustified(3, '0'), Qt::UserRole);
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
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(2);
    QStringList headers;
    headers << QObject::tr("Tileset") << QObject::tr("Filename");
    model->setHorizontalHeaderLabels(headers);

    const QString suiteNames[4] = {
        QObject::tr("Standard"),
        QObject::tr("Stage"),
        QObject::tr("Background"),
        QObject::tr("Interactive")
    };

    for (const TilesetGroup& group : tilesetList->groups())
    {
        QStandardItem* groupItem = new QStandardItem(group.name);

        for (int slot = 0; slot < 4; slot++)
        {
            QStandardItem* suiteItem = new QStandardItem(suiteNames[slot]);

            for (const TilesetEntry& entry : group.tilesets)
            {
                if (!TilesetList::matchesSlot(entry.fileName, slot))
                    continue;

                QList<QStandardItem*> items;

                QStandardItem* tileset = new QStandardItem(entry.name);
                tileset->setData(entry.fileName);
                items.append(tileset);

                QStandardItem* fileNameItem = new QStandardItem(entry.fileName);
                fileNameItem->setData(entry.fileName);
                items.append(fileNameItem);

                suiteItem->appendRow(items);
            }

            if (suiteItem->rowCount() > 0)
                groupItem->appendRow(suiteItem);
            else
                delete suiteItem;
        }

        if (groupItem->rowCount() > 0)
            model->appendRow(groupItem);
        else
            delete groupItem;
    }

    return model;
}

QStandardItemModel* Game::getTilesetModel(int id, bool includeNoneItem)
{
    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(2);
    QStringList headers;

    headers << QObject::tr("Tileset")<< QObject::tr("Filename");

    model->setHorizontalHeaderLabels(headers);

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
    for (const TilesetGroup& group : tilesetList->groups())
        for (const TilesetEntry& entry : group.tilesets)
            defaultNames.insert(entry.fileName, entry.name);

    QList<QString> tilesetfiles;
    fs->directoryContents("/Unit", QDir::Files, tilesetfiles);

    for (int i = 0; i < tilesetfiles.length(); i++)
    {
        QString fileName = TilesetList::stripExtension(tilesetfiles[i]);

        if (!TilesetList::matchesSlot(fileName, id))
            continue;

        QString tilesetname = defaultNames.value(fileName, fileName);

        QList<QStandardItem*> items;

        QStandardItem* tileset = new QStandardItem(tilesetname);
        tileset->setData(fileName);
        items.append(tileset);

        QStandardItem* fileNameItem = new QStandardItem(fileName);
        fileNameItem->setData(fileName);
        items.append(fileNameItem);

        model->appendRow(items);
    }

    return model;
}
