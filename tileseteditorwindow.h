#ifndef TILESETEDITORWINDOW_H
#define TILESETEDITORWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>

#include "tileset.h"


// Tileset Picker
class TilesetPicker : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetPicker(QWidget *parent);
    void setTilesetImage(QImage *image);

signals:
    void selectedTileChanged(int x, int y);

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    QImage *tilesetImage;
    int selectedX;
    int selectedY;
};


namespace Ui {
class TilesetEditorWindow;
}

class TilesetEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TilesetEditorWindow(QWidget *parent, Tileset *tileset);
    ~TilesetEditorWindow();

public slots:
    void updateSelectedTile(int x, int y);

private slots:
    void on_hexLineEdit_textEdited(const QString &text);

private:
    Ui::TilesetEditorWindow *ui;
    TilesetPicker* tilesetPicker;

    Tileset* tileset;

    int selectedX;
    int selectedY;
};

#endif // TILESETEDITORWINDOW_H
