#include "backgroundlist.h"
#include "settingsmanager.h"

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

static QString backgroundDisplayNameFromFile(const QString& fileName)
{
    return QString(fileName).replace('_', ' ');
}

static bool parseBackgroundsXml(const QString& path, QList<BackgroundGroup>& groups)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument xml;
    if (!xml.setContent(&file))
    {
        file.close();
        return false;
    }

    file.close();

    QDomElement root = xml.documentElement();
    if (root.isNull())
        return false;

    for (QDomElement groupElement = root.firstChildElement("group"); !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("group"))
    {
        BackgroundGroup group;
        group.name = groupElement.attribute("name");

        for (QDomElement bgElement = groupElement.firstChildElement("background"); !bgElement.isNull(); bgElement = bgElement.nextSiblingElement("background"))
        {
            BackgroundEntry entry;
            entry.fileName = bgElement.attribute("file");
            entry.name = bgElement.attribute("name", entry.fileName);

            if (entry.fileName.isEmpty())
                continue;

            group.entries.append(entry);
        }

        groups.append(group);
    }

    return true;
}

static void buildNameFileGroup(QList<BackgroundGroup>& groups)
{
    QFile file(SettingsManager::getInstance()->dataPath("backgrounds/backgroundnames.txt"));
    if (!file.open(QIODevice::ReadOnly))
        return;

    BackgroundGroup group;
    group.name = QObject::tr("Retail");

    QTextStream in(&file);

    while (!in.atEnd())
    {
        const QString line = in.readLine().trimmed();
        int separator = line.indexOf(':');

        if (separator <= 0)
            continue;

        BackgroundEntry entry;
        entry.fileName = line.left(separator);
        entry.name = line.mid(separator + 1);

        if (entry.name.isEmpty())
            entry.name = entry.fileName;

        group.entries.append(entry);
    }

    file.close();

    if (!group.entries.isEmpty())
        groups.append(group);
}

static QStringList existingBackgrounds()
{
    QDir backgroundsFolder(SettingsManager::getInstance()->dataPath("backgrounds"));
    backgroundsFolder.setFilter(QDir::Files);
    backgroundsFolder.setNameFilters(QStringList{"*.png"});

    QStringList existingFiles;

    for (const QString& file : backgroundsFolder.entryList())
    {
        QString fileName = BackgroundList::stripExtension(file);

        if (fileName != "Invalid")
            existingFiles.append(fileName);
    }

    return existingFiles;
}

QString BackgroundList::stripExtension(const QString& fileName)
{
    int dot = fileName.lastIndexOf('.');
    if (dot > 0)
        return fileName.left(dot);

    return fileName;
}

QString BackgroundList::previewPath(const QString& fileName)
{
    if (!fileName.isEmpty())
    {
        QString path = SettingsManager::getInstance()->dataPath("backgrounds/" + fileName + ".png");

        if (QFileInfo(path).isFile())
            return path;
    }

    return invalidPreviewPath();
}

QString BackgroundList::invalidPreviewPath()
{
    return SettingsManager::getInstance()->dataPath("backgrounds/Invalid.png");
}

QList<BackgroundGroup> BackgroundList::groups() const
{
    QList<BackgroundGroup> groups;

    QString basePath = SettingsManager::getInstance()->dataPath("backgrounds.xml");
    parseBackgroundsXml(basePath, groups);

    if (groups.isEmpty())
        buildNameFileGroup(groups);

    QString translatedPath = SettingsManager::getInstance()->getFilePath("backgrounds.xml");
    if (!translatedPath.isEmpty() && translatedPath != basePath)
    {
        QList<BackgroundGroup> translatedGroups;
        if (parseBackgroundsXml(translatedPath, translatedGroups))
        {
            for (const BackgroundGroup& translatedGroup : translatedGroups)
            {
                BackgroundGroup* targetGroup = nullptr;
                for (BackgroundGroup& group : groups)
                {
                    if (group.name == translatedGroup.name)
                    {
                        targetGroup = &group;
                        break;
                    }
                }

                if (targetGroup == nullptr)
                {
                    groups.append(translatedGroup);
                    continue;
                }

                for (const BackgroundEntry& translatedEntry : translatedGroup.entries)
                {
                    bool found = false;
                    for (BackgroundEntry& entry : targetGroup->entries)
                    {
                        if (entry.fileName == translatedEntry.fileName)
                        {
                            entry.name = translatedEntry.name;
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                        targetGroup->entries.append(translatedEntry);
                }
            }
        }
    }

    QStringList existingFiles = existingBackgrounds();

    QList<QString> mapped;
    for (BackgroundGroup& group : groups)
    {
        for (int i = group.entries.size() - 1; i >= 0; i--)
        {
            if (!existingFiles.contains(group.entries[i].fileName))
            {
                group.entries.removeAt(i);
                continue;
            }

            mapped.append(group.entries[i].fileName);
        }
    }

    for (int i = groups.size() - 1; i >= 0; i--)
        if (groups[i].entries.isEmpty())
            groups.removeAt(i);

    BackgroundGroup otherGroup;
    otherGroup.name = QObject::tr("Other");

    for (const QString& fileName : existingFiles)
    {
        if (mapped.contains(fileName))
            continue;

        BackgroundEntry entry;
        entry.fileName = fileName;
        entry.name = backgroundDisplayNameFromFile(fileName);
        otherGroup.entries.append(entry);
    }

    if (!otherGroup.entries.isEmpty())
        groups.append(otherGroup);

    return groups;
}

QList<BackgroundEntry> BackgroundList::entries() const
{
    QList<BackgroundEntry> entries;

    for (const BackgroundGroup& group : groups())
        entries.append(group.entries);

    return entries;
}
