#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "level.h"

#include <QObject>

class LevelManager : public QObject
{
    Q_OBJECT
public:
    LevelManager(QWidget* parentWidget, Game *game, QString lvlPath);
    ~LevelManager();

    Level* openArea(int id);
    void closeArea(Level* area);

    int addArea(int id);
    int removeArea(Level* level);

    void openAreaEditor(int id);
    int getOpenedAreaCount();
    bool areaIsOpen(int id);

    int getAreaCount();
    bool hasArea(int id);

    QWidget* getParent();
    Game* getGame();

signals:
    updateLevelEditors();

private:
    SarcFilesystem* archive;

    QWidget* parentWidget;
    Game* game;
    QString lvlPath;

    QList<Level*> openedAreas;
};

#endif // LEVELMANAGER_H
