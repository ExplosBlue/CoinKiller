#include "tilesetlist.h"
#include "filesystem/filesystem.h"
#include "settingsmanager.h"

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QObject>
#include <QSet>

static QString displayNameFromFile(const QString& fileName)
{
    QString name = fileName;

    if (name.startsWith("S1_") || name.startsWith("S2_"))
        name = name.mid(3);
    else if (name.startsWith("M_") || name.startsWith("J_"))
        name = name.mid(2);

    return name.replace('_', ' ');
}

static bool parseTilesetGroups(const QString& path, QList<TilesetGroup>& groups)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument xml;
    xml.setContent(&file);
    file.close();

    QDomElement root = xml.documentElement();
    if (root.isNull())
        return false;

    for (QDomElement groupElement = root.firstChildElement("group"); !groupElement.isNull(); groupElement = groupElement.nextSiblingElement("group"))
    {
        TilesetGroup group;
        group.name = groupElement.attribute("name");

        for (QDomElement child = groupElement.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        {
            if (child.tagName() == "tileset")
            {
                TilesetEntry entry;
                entry.fileName = child.attribute("file");
                entry.name = child.attribute("name", entry.fileName);

                if (entry.fileName.isEmpty())
                    continue;

                group.tilesets.append(entry);
            }
            else if (child.tagName() == "subgroup")
            {
                TilesetSubgroup subgroup;
                subgroup.name = child.attribute("name");

                for (QDomElement tsElement = child.firstChildElement("tileset"); !tsElement.isNull(); tsElement = tsElement.nextSiblingElement("tileset"))
                {
                    TilesetEntry entry;
                    entry.fileName = tsElement.attribute("file");
                    entry.name = tsElement.attribute("name", entry.fileName);

                    if (entry.fileName.isEmpty())
                        continue;

                    subgroup.tilesets.append(entry);
                }

                if (!subgroup.tilesets.isEmpty())
                    group.subgroups.append(subgroup);
            }
        }

        groups.append(group);
    }

    return true;
}

QList<TilesetEntry> TilesetGroup::allTilesets() const
{
    QList<TilesetEntry> entries = tilesets;

    for (const TilesetSubgroup& subgroup : subgroups)
        entries.append(subgroup.tilesets);

    return entries;
}

TilesetEntry* TilesetGroup::findTileset(const QString& fileName)
{
    for (TilesetEntry& entry : tilesets)
        if (entry.fileName == fileName)
            return &entry;

    for (TilesetSubgroup& subgroup : subgroups)
        for (TilesetEntry& entry : subgroup.tilesets)
            if (entry.fileName == fileName)
                return &entry;

    return nullptr;
}

TilesetList::TilesetList(FilesystemBase* fs) :
    fs(fs)
{
}

QList<TilesetGroup> TilesetList::groups() const
{
    QList<TilesetGroup> groups;

    const QString basePath = SettingsManager::getInstance()->dataPath("tilesets.xml");
    parseTilesetGroups(basePath, groups);

    const QString translatedPath = SettingsManager::getInstance()->getFilePath("tilesets.xml");
    if (!translatedPath.isEmpty() && translatedPath != basePath)
    {
        QList<TilesetGroup> translatedGroups;
        if (parseTilesetGroups(translatedPath, translatedGroups))
        {
            for (const TilesetGroup& translatedGroup : translatedGroups)
            {
                TilesetGroup* targetGroup = nullptr;
                for (TilesetGroup& group : groups)
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

                for (const TilesetEntry& translatedEntry : translatedGroup.tilesets)
                {
                    TilesetEntry* entry = targetGroup->findTileset(translatedEntry.fileName);
                    if (entry)
                        entry->name = translatedEntry.name;
                    else
                        targetGroup->tilesets.append(translatedEntry);
                }

                for (const TilesetSubgroup& translatedSubgroup : translatedGroup.subgroups)
                {
                    TilesetSubgroup* targetSubgroup = nullptr;
                    for (TilesetSubgroup& subgroup : targetGroup->subgroups)
                    {
                        if (subgroup.name == translatedSubgroup.name)
                        {
                            targetSubgroup = &subgroup;
                            break;
                        }
                    }

                    if (targetSubgroup == nullptr)
                    {
                        targetGroup->subgroups.append(translatedSubgroup);
                        continue;
                    }

                    for (const TilesetEntry& translatedEntry : translatedSubgroup.tilesets)
                    {
                        TilesetEntry* entry = targetGroup->findTileset(translatedEntry.fileName);
                        if (entry)
                            entry->name = translatedEntry.name;
                        else
                            targetSubgroup->tilesets.append(translatedEntry);
                    }
                }
            }
        }
    }

    QList<QString> tilesetFiles;
    fs->directoryContents("/Unit", QDir::Files, tilesetFiles);

    QList<QString> existingFiles;
    for (const QString& file : tilesetFiles)
        existingFiles.append(stripExtension(file));

    QSet<QString> mapped;
    for (TilesetGroup& group : groups)
    {
        for (int i = group.tilesets.size() - 1; i >= 0; i--)
        {
            if (!existingFiles.contains(group.tilesets[i].fileName))
            {
                group.tilesets.removeAt(i);
                continue;
            }

            mapped.insert(group.tilesets[i].fileName);
        }

        for (TilesetSubgroup& subgroup : group.subgroups)
        {
            for (int i = subgroup.tilesets.size() - 1; i >= 0; i--)
            {
                if (!existingFiles.contains(subgroup.tilesets[i].fileName))
                {
                    subgroup.tilesets.removeAt(i);
                    continue;
                }

                mapped.insert(subgroup.tilesets[i].fileName);
            }
        }

        for (int i = group.subgroups.size() - 1; i >= 0; i--)
            if (group.subgroups[i].tilesets.isEmpty())
                group.subgroups.removeAt(i);
    }

    for (int i = groups.size() - 1; i >= 0; i--)
        if (groups[i].allTilesets().isEmpty())
            groups.removeAt(i);

    TilesetGroup otherGroup;
    otherGroup.name = QObject::tr("Other");

    for (const QString& fileName : existingFiles)
    {
        if (mapped.contains(fileName))
            continue;

        TilesetEntry entry;
        entry.fileName = fileName;
        entry.name = displayNameFromFile(fileName);
        otherGroup.tilesets.append(entry);
    }

    if (!otherGroup.tilesets.isEmpty())
        groups.append(otherGroup);

    return groups;
}

bool TilesetList::matchesSlot(const QString& fileName, int slot)
{
    switch (slot)
    {
    case 0: return fileName.startsWith("J_");
    case 1: return fileName.startsWith("M_");
    case 2: return fileName.startsWith("S1_");
    case 3: return fileName.startsWith("S2_");
    }

    return true;
}

int TilesetList::slotForFileName(const QString& fileName)
{
    for (int slot = 0; slot < 4; slot++)
    {
        if (matchesSlot(fileName, slot))
            return slot;
    }

    return -1;
}

QString TilesetList::stripExtension(const QString& fileName)
{
    int dot = fileName.lastIndexOf('.');
    if (dot > 0)
        return fileName.left(dot);

    return fileName;
}

QString TilesetList::displayName(const QString& fileName) const
{
    for (const TilesetGroup& group : groups())
        for (const TilesetEntry& entry : group.allTilesets())
            if (entry.fileName == fileName)
                return entry.name;

    return fileName;
}

QString TilesetList::groupName(const QString& fileName) const
{
    for (const TilesetGroup& group : groups())
        for (const TilesetEntry& entry : group.allTilesets())
            if (entry.fileName == fileName)
                return group.name;

    return QString();
}
