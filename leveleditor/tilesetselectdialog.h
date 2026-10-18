#ifndef TILESETSELECTDIALOG_H
#define TILESETSELECTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QTreeWidget>
#include <QListView>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItemModel>

#include "game.h"

class TilesetSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    TilesetSelectionDialog(Game* game, int slot, const QString& currentTileset, int currentObjectCount = -1, QWidget* parent = nullptr);
    ~TilesetSelectionDialog() override;

    QString selectedTileset() const { return selectedFileName; }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void groupChanged();
    void tilesetChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void tilesetDoubleClicked(QTreeWidgetItem* item, int column);
    void applyFilter();

private:
    void populateTilesets();
    void addTilesets(const QList<TilesetEntry>& entries);
    void loadPreview(const QString& fileName);
    void clearPreview();

    Game* game;
    int slot;
    QString currentTileset;
    int currentObjectCount;

    QString selectedFileName;
    Tileset* previewTileset = nullptr;

    QList<TilesetGroup> groups;

    QListWidget* groupList;
    QTreeWidget* tilesetList;
    QLineEdit* filterBox;
    QLabel* filenameLabel;
    QListView* previewList;
    QStandardItemModel* previewModel;
    QWidget* warningWidget;
};

#endif // TILESETSELECTDIALOG_H
