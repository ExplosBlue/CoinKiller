#ifndef LEVELEDITORWIDGETS_H
#define LEVELEDITORWIDGETS_H

#include "spritedata.h"

#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>

class SpritePicker : public QWidget
{
    Q_OBJECT
public:
    SpritePicker();
signals:
    void selectedSpriteChanged(int);

public slots:
    void setView(int view);

private slots:
    void handleIndexChange(QTreeWidgetItem *item);

private:
    class SpritePickerTree : public QTreeWidget
    {
    public:
        SpritePickerTree();
    };

    SpriteData spriteData;
    QComboBox* viewComboBox;
    SpritePickerTree* spriteTree;
};

#endif // LEVELEDITORWIDGETS_H
