#ifndef GAME_H
#define GAME_H

#include "filesystem.h"
#include "tileset.h"
#include "levelmanager.h"

#include <QStandardItemModel>
#include <QtXml>

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

    FilesystemBase* fs;

private:
    QString path;

    void loadLevelNamesCat(QStandardItem* item, QDomElement node);
};

#endif // GAME_H
