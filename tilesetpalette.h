#ifndef TILESETPALETTE_H
#define TILESETPALETTE_H

#include "level.h"
#include "editionmode.h"

#include <QWidget>
#include <QListView>

class TilesetPalette : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetPalette(Level* level, ObjectsEditonMode *objEditionMode);

public slots:
    void reloadTilesets();

private slots:
    void on_objectsListView0_clicked(const QModelIndex &index);
    void on_objectsListView1_clicked(const QModelIndex &index);
    void on_objectsListView2_clicked(const QModelIndex &index);
    void on_objectsListView3_clicked(const QModelIndex &index);
    void on_layerRadioButton_toggled(bool checked);

private:
    Level* level;
    ObjectsEditonMode* objEditionMode;

    QListView* objectLists[4];

    void loadTileset(int tilesetNbr);
};

#endif // TILESETPALETTE_H
