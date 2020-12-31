#ifndef SPRITESEDITOR_H
#define SPRITESEDITOR_H

#include "spritedata.h"
#include "level.h"
#include "spriteidswidget.h"

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSplitter>
#include <QScrollArea>

class SpriteValueFieldWidget : public QSpinBox
{
    Q_OBJECT
public:
    SpriteValueFieldWidget(Sprite* sprite, Field* field);
    void updateValue();
private slots:
    void handleValueChange(int value);
signals:
    void updateHex();
    void updateFields();
    void editMade();

private:
    Sprite* sprite;
    Field* field;
    bool handleValueChanges;
};

class SpriteCheckboxFieldWidget : public QCheckBox
{
    Q_OBJECT
public:
    SpriteCheckboxFieldWidget(Sprite* sprite, Field* field);
    void updateValue();
private slots:
    void handleValueChange(bool checked);
signals:
    void updateHex();
    void updateFields();
    void editMade();

private:
    Sprite* sprite;
    Field* field;
    bool handleValueChanges;
};

class SpriteListFieldWidget : public QComboBox
{
    Q_OBJECT
public:
    SpriteListFieldWidget(Sprite* sprite, Field* field);
    void updateValue();
private slots:
    void handleIndexChange(int index);
signals:
    void updateHex();
    void updateFields();
    void editMade();

private:
    Sprite* sprite;
    Field* field;
    bool handleValueChanges;
};

class SpriteBitFieldWidget : public QFrame
{
    Q_OBJECT
public:
    SpriteBitFieldWidget(Sprite* sprite, Field* field);
    void updateValue();
private slots:
    void handleValueChange();
signals:
    void updateHex();
    void updateFields();
    void editMade();

private:
    QGridLayout* layout;

    Sprite* sprite;
    Field* field;
    bool handleValueChanges;

    QList<QCheckBox*> checkboxWidgets;

};

class SpriteDataEditorWidget : public QScrollArea
{
    Q_OBJECT
public:
    SpriteDataEditorWidget(SpriteData* spriteData);
    void select(Sprite* sprite);
    void deselect();
    void updateEditor();

private slots:
    void handleRawSpriteDataChange(QString text);
    void updateRawSpriteData();
    void updateFields();
    void handleEditDetected();
    void handleShowNotes();
    void handleLayerChanged(int);

signals:
    void updateLevelView();
    void editMade();

private:
    QGridLayout* layout;
    QLineEdit* rawSpriteData;

    SpriteData* spriteData;
    Sprite* editSprite;
    bool editingASprite = false;

    QLabel* spriteName;
    QPushButton* spriteNotesButton;
    QList<QLabel*> fieldNames;
    QList<SpriteValueFieldWidget*> valueFieldWidgets;
    QList<SpriteListFieldWidget*> listFieldWidgets;
    QList<SpriteCheckboxFieldWidget*> checkboxFieldWidgets;
    QList<SpriteBitFieldWidget*> bitFieldWidgets;

    class HorLine : public QFrame { public: HorLine() { setFrameStyle(QFrame::HLine | QFrame::Sunken); } };

    HorLine* splitterLine;
    QComboBox* layerComboBox;
    QLabel* layerLabel;

    QString spriteNotes;

    void addField(Field* field, int pos);
    void reloadFields();
};

class SpriteEditorWidget : public QSplitter
{
    Q_OBJECT
public:
    SpriteEditorWidget(QList<Sprite*> *sprites);
    void select(Sprite* sprite);
    SpriteDataEditorWidget* spriteDataEditorPtr() { return editor; }
    SpriteIdWidget* spriteIdsPtr() { return spriteIds; }

signals:
    void currentSpriteChanged(int);
    void selectedSpriteChanged(Object*);
    void editMade();
    void updateLevelView();

public slots:
    void setView(int view);
    void search(QString text);
    void handleSelectedSpriteChanged(Object* obj);
    void handleUpdateLevelView();

private slots:
    void handleIndexChange(QTreeWidgetItem *item);
    void handleEditDetected();
    void handleSplitterMoved();

private:
    SpriteData spriteData;
    QComboBox* viewComboBox;
    QLineEdit* searchEdit;
    QTreeWidget* spriteTree;
    SpriteIdWidget* spriteIds;

    SpriteDataEditorWidget* editor;

    void changeEvent(QEvent* event);

    QStringList getViewNames();
};

#endif // SPRITESEDITOR_H
