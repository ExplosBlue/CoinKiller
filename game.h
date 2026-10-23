#ifndef GAME_H
#define GAME_H

#include "filesystem/filesystem.h"
#include "tileset.h"
#include "tilesetlist.h"
#include "levelmanager.h"

#include <QStandardItemModel>
#include <QList>

class Game
{
public:
    Game(const QString& path);
    ~Game();

    const QString& getPath() { return path; }

    Tileset* getTileset(QString name);
    LevelManager* getLevelManager(WindowBase* parent, QString path);

    QStandardItemModel* getCourseModel();
    QStandardItemModel* getTilesetModel();
    QStandardItemModel* getTilesetModel(int id, bool includeNoneItem = false);

    TilesetList* getTilesetList() { return tilesetList; }

    FilesystemBase* fs;

private:
    QString path;
    TilesetList* tilesetList;
};

#endif // GAME_H
