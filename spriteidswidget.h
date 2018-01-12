#ifndef SPRITESIDS_H
#define SPRITESIDS_H

#include "objects.h"
#include "spritedata.h"

#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QListWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>

class SpriteIdWidget : public QWidget
{
    Q_OBJECT
public:
    SpriteIdWidget(QList<Sprite*> *sprites);
    void deselect();
    void updateEditor();

signals:
    void selectedSpriteChanged(Object*);
    void updateLevelView();

public slots:
    void setView(int view);

private slots:
    void handleIndexChange(QTreeWidgetItem* spriteItem);
    void search(QString text);


private:
    bool handleChanges = false;
    SpriteData spriteData;
    QList<Sprite*> *sprites;
    QTreeWidget* spriteTree;
    QComboBox* viewComboBox;
    QLineEdit* searchEdit;

    QStringList headerLables;

    void updateList();

    int viewMode = 0;
};

#endif // SPRITESIDS_H
