#ifndef SPRITESEDITOR_H
#define SPRITESEDITOR_H

#include "spritedata.h"
#include "objects.h"
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
#include <QUndoStack>

class SpriteValueFieldWidget : public QSpinBox
{
    Q_OBJECT
public:
    SpriteValueFieldWidget(Sprite *sprite, Field *field, QUndoStack *undoStack, QWidget *parent = nullptr);
    void updateValue();

private slots:
    void handleValueChange(int value);

signals:
    void updateHex();
    void updateFields();

private:
    Sprite* sprite;
    Field* field;
    QUndoStack *undoStack;
    bool handleValueChanges;
};

class SpriteCheckboxFieldWidget : public QCheckBox
{
    Q_OBJECT
public:
    SpriteCheckboxFieldWidget(Sprite *sprite, Field *field, QUndoStack *undoStack, QWidget *parent = nullptr);
    void updateValue();
private slots:
    void handleValueChange(bool checked);
signals:
    void updateHex();
    void updateFields();

private:
    Sprite *sprite;
    Field *field;
    QUndoStack *undoStack;
    bool handleValueChanges;
};

class SpriteListFieldWidget : public QComboBox
{
    Q_OBJECT
public:
    SpriteListFieldWidget(Sprite *sprite, Field *field, QUndoStack *undoStack, QWidget *parent = nullptr);
    void updateValue();
private slots:
    void handleIndexChange(int index);
signals:
    void updateHex();
    void updateFields();

private:
    Sprite *sprite;
    Field *field;
    QUndoStack *undoStack;
    bool handleValueChanges;
};

class SpriteBitFieldWidget : public QFrame
{
    Q_OBJECT
public:
    SpriteBitFieldWidget(Sprite *sprite, Field *field, QUndoStack *undoStack, QWidget *parent = nullptr);
    void updateValue();
private slots:
    void handleValueChange();
signals:
    void updateHex();
    void updateFields();

private:
    QGridLayout *layout;
    QList<QCheckBox*> checkboxWidgets;

    Sprite *sprite;
    Field *field;
    QUndoStack *undoStack;
    bool handleValueChanges;
};

class SpriteDataEditorWidget : public QScrollArea
{
    Q_OBJECT
public:
    SpriteDataEditorWidget(SpriteData* spriteData, QUndoStack *undoStack, QWidget *parent = nullptr);
    void select(Sprite* sprite);
    void deselect();
    void updateEditor();

private slots:
    void handleRawSpriteDataChange(QString text);
    void updateRawSpriteData();
    void updateFields();
    void handleShowNotes();
    void handleLayerChanged(int);

signals:
    void updateLevelView();

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

    QUndoStack *undoStack;
};

class SpriteEditorWidget : public QSplitter
{
    Q_OBJECT
public:
    SpriteEditorWidget(QList<Sprite*> *sprites, QUndoStack *undoStack, QWidget *parent = nullptr);
    void select(Sprite* sprite);
    SpriteDataEditorWidget* spriteDataEditorPtr() { return editor; }
    SpriteIdWidget* spriteIdsPtr() { return spriteIds; }

signals:
    void currentSpriteChanged(int);
    void selectedSpriteChanged(Object*);
    void updateLevelView();

public slots:
    void setView(int view);
    void search(QString text);
    void handleSelectedSpriteChanged(Object* obj);
    void handleUpdateLevelView();

private slots:
    void handleIndexChange(QTreeWidgetItem *item);
    void handleSplitterMoved();

private:
    SpriteData spriteData;
    QComboBox* viewComboBox;
    QLineEdit* searchEdit;
    QTreeWidget* spriteTree;
    SpriteIdWidget* spriteIds;

    SpriteDataEditorWidget* editor;

    void changeEvent(QEvent* event) override;

    QStringList getViewNames();
};

#endif // SPRITESEDITOR_H
