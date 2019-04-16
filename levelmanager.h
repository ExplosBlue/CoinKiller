#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "level.h"
#include "windowbase.h"

#include <QObject>

class LevelManager : public QObject
{
    Q_OBJECT
public:
    LevelManager(WindowBase* parentWidget, Game *game, QString lvlPath);
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

    WindowBase* getParent();
    Game* getGame();

signals:
    void updateLevelEditors();

private:
    SarcFilesystem* archive;

    WindowBase* parentWidget;
    Game* game;
    QString lvlPath;

    QList<Level*> openedAreas;
};

#endif // LEVELMANAGER_H
