#ifndef TILESETPALETTE_H
#define TILESETPALETTE_H

#include "level.h"
#include "game.h"
#include "editmanager.h"

#include <QWidget>
#include <QListView>
#include <QTabWidget>
#include <QComboBox>
#include <QUndoStack>

class TilesetPalette : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetPalette(Level* level, EditManager *editManager, Game *game, QUndoStack *undoStack, QWidget *parent = nullptr);
    void select(BgdatObject* obj);
    void updateEditor();

signals:
    void updateLevelView();
    void editMade();

private slots:
    void objectsListViewClicked(const QModelIndex &index);
    void layerToggled(bool state);
    void tilesetPickerChosen(int index);

private:
    Level *level;
    EditManager *editManager;
    Game *game;
    QUndoStack *undoStack;

    QTabWidget *tabWidget;
    QList<QComboBox*> tilesetPickers;
    QList<QListView*> objectLists;

    void loadTileset(int tilesetNbr);
    void updatePalettes(int actualPal);
    void updateTilesetPickerIndex();

    void reloadTilesets();

};

#endif // TILESETPALETTE_H
