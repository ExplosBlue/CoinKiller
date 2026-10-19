#ifndef TILESETPALETTE_H
#define TILESETPALETTE_H

#include "level.h"
#include "game.h"
#include "editmanager.h"

#include <QWidget>
#include <QListView>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QUndoStack>

class TilesetPalette : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetPalette(Level* level, EditManager *editManager, Game *game, QUndoStack *undoStack, QWidget *parent = nullptr);
    void select(BgdatObject* obj);
    void updateEditor();

private slots:
    void objectsListViewClicked(const QModelIndex &index);
    void layerToggled(bool state);
    void changeTileset();
    void tilesetComboChosen(int index);

private:
    Level *level;
    EditManager *editManager;
    Game *game;
    QUndoStack *undoStack;

    QTabWidget *tabWidget;
    QList<QComboBox*> tilesetPickers;
    QList<QPushButton*> tilesetButtons;
    QList<QListView*> objectLists;
    QString lastTilesetGroups[4];

    void loadTileset(int tilesetNbr);
    void updatePalettes(int actualPal);
    void populateTilesetCombo(int tsSlot);
    void updateTilesetPickers();

    void reloadTilesets();

};

#endif // TILESETPALETTE_H
