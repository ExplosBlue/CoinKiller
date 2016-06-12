#ifndef GAME_H
#define GAME_H

#include "filesystem.h"
#include "tileset.h"
#include "level.h"

#include <QStandardItemModel>
#include <QtXml>

class Game
{
public:
    Game(FilesystemBase* fs);


    Tileset* getTileset(QString name);
    Level* getLevel(QString path, int area);

    QStandardItemModel* getCourseModel();
    QStandardItemModel* getTilesetModel();

    FilesystemBase* fs;

private:
    void loadLevelNamesCat(QStandardItem* item, QDomElement node);

};

#endif // GAME_H
