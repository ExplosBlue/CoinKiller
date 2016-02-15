#ifndef LEVELEDITORWIDGETS_H
#define LEVELEDITORWIDGETS_H

#include "spritedata.h"

#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QLineEdit>

class SpritePicker : public QWidget
{
    Q_OBJECT
public:
    SpritePicker();

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

#endif // LEVELEDITORWIDGETS_H
