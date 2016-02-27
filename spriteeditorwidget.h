#ifndef SPRITESEDITOR_H
#define SPRITESEDITOR_H

#include "spritedata.h"
#include "level.h"

#include <QWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QLineEdit>

class SpriteEditorWidget : public QWidget
{
    Q_OBJECT
public:
    SpriteEditorWidget();

signals:
    void selectedSpriteChanged(int);

public slots:
    void setView(int view);
    void search(QString text);

private slots:
    void handleIndexChange(QTreeWidgetItem *item);

private:
    SpriteData spriteData;
    QComboBox* viewComboBox;
    QLineEdit* searchEdit;
    QTreeWidget* spriteTree;
};

#endif // SPRITESEDITOR_H
