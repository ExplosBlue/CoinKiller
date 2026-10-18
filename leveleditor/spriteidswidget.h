#ifndef SPRITESIDS_H
#define SPRITESIDS_H

#include "objects.h"
#include "spritedata.h"
#include "sortabletreewidget.h"

#include <QWidget>
#include <QLabel>
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
    SortableTreeWidget* spriteTree;
    QComboBox* viewComboBox;
    QLineEdit* searchEdit;

    void changeEvent(QEvent* event);

    void updateList();
    void addIdView(const QString& match, const QString& header);
    void addCameraLimitsView();
    void addCameraLimitItem(Sprite* sprite, Sprite* partner, const QString& label);
    void addDisconnectedCameraLimit(Sprite* sprite);
    QString cameraLimitShortName(Sprite* sprite);
    QString cameraLimitDirection(Sprite* sprite);

    int viewMode = 0;
};

#endif // SPRITESIDS_H
