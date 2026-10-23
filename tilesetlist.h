#ifndef TILESETLIST_H
#define TILESETLIST_H

#include <QList>
#include <QString>

class FilesystemBase;

struct TilesetEntry
{
    QString fileName;
    QString name;
};

struct TilesetSubgroup
{
    QString name;
    QList<TilesetEntry> tilesets;
};

struct TilesetGroup
{
    QString name;
    QList<TilesetEntry> tilesets;
    QList<TilesetSubgroup> subgroups;

    QList<TilesetEntry> allTilesets() const;
    TilesetEntry* findTileset(const QString& fileName);
};

class TilesetList
{
public:
    TilesetList(FilesystemBase* fs);

    QList<TilesetGroup> groups() const;

    static bool matchesSlot(const QString& fileName, int slot);
    static int slotForFileName(const QString& fileName);
    static QString stripExtension(const QString& fileName);

    QString displayName(const QString& fileName) const;
    QString groupName(const QString& fileName) const;

private:
    FilesystemBase* fs;
};

#endif
