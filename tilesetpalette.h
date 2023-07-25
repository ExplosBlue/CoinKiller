#ifndef TILESETPALETTE_H
#define TILESETPALETTE_H

#include "level.h"
#include "game.h"
#include "editmanager.h"

#include <QWidget>
#include <QListView>
#include <QTabWidget>
#include <QComboBox>

class TilesetPalette : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetPalette(Level* level, EditManager *editManager, Game *game);
    void select(BgdatObject* obj);

signals:
    void updateLevelView();
    void editMade();

private slots:
    void on_objectsListView0_clicked(const QModelIndex &index);
    void on_objectsListView1_clicked(const QModelIndex &index);
    void on_objectsListView2_clicked(const QModelIndex &index);
    void on_objectsListView3_clicked(const QModelIndex &index);
    void on_layerRadioButton_toggled(bool checked);

    void handleTilesetChange(int index);

private:
    Level* level;
    Game* game;
    EditManager* editManager;

    QTabWidget* tabWidget;
    QListView* objectLists[4];

    void loadTileset(int tilesetNbr);
    void updatePalettes(int actualPal);

    void reloadTilesets();
};

#endif // TILESETPALETTE_H
