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
    Game(FilesystemBase* fs);


    Tileset* getTileset(QString name);
    LevelManager* getLevelManager(QWidget* parent, QString path);

    QStandardItemModel* getCourseModel();
    QStandardItemModel* getTilesetModel();
    QStandardItemModel* getTilesetModel(int id, bool includeNoneItem = false);

    FilesystemBase* fs;

private:
    void loadLevelNamesCat(QStandardItem* item, QDomElement node);

};

#endif // GAME_H
