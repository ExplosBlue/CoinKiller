#ifndef ZONEEDITORWIDGET_H
#define ZONEEDITORWIDGET_H

#include "objects.h"
#include "level.h"
#include "sortabletreewidget.h"
#include "backgroundlist.h"

#include <QWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QFrame>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QScrollArea>
#include <QCheckBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QPushButton>
#include <QToolButton>
#include <QDialog>
#include <QGroupBox>
#include <QStyledItemDelegate>
#include <QUndoStack>

class BgPreview : public QLabel
{
    Q_OBJECT
public:
    BgPreview(QWidget *parent = nullptr);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void resizeEvent(QResizeEvent *) override;
    void setBg(QPixmap bg);
private:
    QPixmap bg;
};

class BackgroundThumbnailDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BackgroundThumbnailDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class BackgroundThumbnailList : public QListWidget
{
    Q_OBJECT
public:
    explicit BackgroundThumbnailList(QWidget *parent = nullptr);

    void relayoutItems();

    void setContentMargins(int left, int right);
    QMargins contentMargins() const;

protected:
    void resizeEvent(QResizeEvent *event) override;
};

class ZoneBackgroundWidget : public QDialog
{
    Q_OBJECT
public:
    ZoneBackgroundWidget(QList<ZoneBackground*> *backgrounds, QUndoStack *undoStack, Level *level, QWidget *parent = nullptr);
    void updateWidget();
    void setSelectedIndex(int index);

signals:
    void selectedBackgroundChanged(int val);

private:
    void loadBackgrounds();
    void populateGroups();
    void populateChoices();
    void populateThumbnails();
    void applyFilter();
    void setSelectionEnabled(bool enabled);

    QList<BackgroundEntry> currentGroupEntries() const;
    QListWidgetItem* backgroundItemById(int id) const;
    QSize thumbnailTileSize() const;
    void updateThumbnailGrid();
    void setThumbnailView(bool enabled);

    bool eventFilter(QObject* watched, QEvent* event) override;

    void updateBgPreview();

    QListWidget* backgroundList;

    QPushButton* addBackgroundBtn;
    QPushButton* removeBackgroundBtn;

    QGroupBox* settingsGroup;
    QSpinBox* backgroundId;
    QList<BackgroundGroup> backgroundGroups;

    QComboBox* groupCombo;
    QToolButton* thumbnailToggleBtn;
    QLineEdit* selectFilter;
    QTreeWidget* selectList;
    BackgroundThumbnailList* thumbnailList;
    QStackedWidget* selectStack;
    BgPreview* backgroundPreview;

    QSpinBox* xPos;
    QSpinBox* yPos;
    QComboBox* parallaxMode;

    QList<ZoneBackground*> *zoneBgs;
    ZoneBackground* editBg = nullptr;
    QMap<QString, QString> backgroundNames;

    void updateList();
    void updateInfo();

    bool editingABg = false;
    bool handleChanges = true;

    QUndoStack *undoStack;
    Level *level;

private slots:
    void handleBackgroundListIndexChange(QListWidgetItem* item);
    void handleBackgroundListDoubleClick(QListWidgetItem* item);
    void handleGroupChange();
    void handleSelectListClicked(QTreeWidgetItem* item, int column);
    void handleThumbnailClicked(QListWidgetItem* item);
    void handleViewToggle();
    void handleSelectFilterChange(const QString& text);
    void handleParallaxModeChange(int val);
    void handleXPosChanged(int val);
    void handleYPosChanged(int val);
    void handleBackgroundIDChanged(int val);
    void handleAddBackgroundClicked();
    void handleRemoveBackgroundClicked();
};

class ZoneBoundingWidget : public QDialog
{
    Q_OBJECT
public:
    ZoneBoundingWidget(QList<ZoneBounding*> *boundings, QUndoStack *undoStack, Level *level, QWidget *parent = nullptr);
    void updateWidget();
    void setSelectedIndex(int index);

signals:
    void selectedBoundingChanged(int val);

private:
    QListWidget* boundingList;

    QPushButton* addBoundingBtn;
    QPushButton* removeBoundingBtn;

    QGroupBox* settingsGroup;
    QSpinBox* boundingId;
    QSpinBox* primaryUpperBound;
    QSpinBox* primaryLowerBound;
    QSpinBox* secondaryUpperBound;
    QSpinBox* secondaryLowerBound;
    QCheckBox* unlimitedScrolling;
    QSpinBox* vertScrollingDistance;

    QList<ZoneBounding*> *zoneBoundings;
    ZoneBounding* editBounding;

    void updateList();
    void updateInfo();

    bool editingABounding = false;
    bool handleChanges = true;

    QUndoStack *undoStack;
    Level *level;

private slots:
    void handleBoundingListIndexChange(QListWidgetItem* item);
    void handleBoundingListDoubleClick(QListWidgetItem* item);
    void handleUnlimitedScrollingChange(bool val);
    void handleVertScrollingDistanceChange(int val);
    void handlePrimaryUpperBoundChange(int val);
    void handlePrimaryLowerBoundChange(int val);
    void handleSecondaryUpperBoundChange(int val);
    void handleSecondaryLowerBoundChange(int val);
    void handleAddBoundingClicked();
    void handleRemoveBoundingClicked();
    void handleBoundingIdChange(int val);
};

class ZoneEditorWidget : public QWidget
{
    Q_OBJECT
public:
    ZoneEditorWidget(QList<Zone*> *zones, QList<ZoneBackground*> *bgs, QList<ZoneBounding*> *bounds, QUndoStack *undoStack, Level *level, QWidget *parent = nullptr);
    ~ZoneEditorWidget() override;
    void deselect();
    void select(Zone* zone);
    void updateEditor();

signals:
    void selectedZoneChanged(Object* zone);
    void selectZoneContents(Zone* zone);
    void screenshot(QRect);

private:
    SortableTreeWidget* zoneList;

    QPushButton* selectContentsBtn;
    QPushButton* screenshotBtn;

    QSpinBox* id;
    QComboBox* multiplayerTracking;
    QMap<int, QString> multiplayerTrackings;
    QSpinBox* progPathId;
    QComboBox* musicId;
    QMap<int, QPair<int, QString>> musicIds;
    QSpinBox* unk1;

    QComboBox* boundingId;
    QPushButton* editBounding;
    ZoneBoundingWidget* boundingWidget;

    QComboBox* backgroundId;
    QPushButton* editBackground;
    ZoneBackgroundWidget* backgroundWidget;
    QMap<QString, QString> backgroundNames;

    QGroupBox* cameraFlagGroup;
    QCheckBox* camFlag1;
    QCheckBox* camFlag2;
    QCheckBox* camFlag3;
    QCheckBox* camFlag4;
    QGroupBox* settingsGroup;

    QList<Zone*> *zones;
    QList<ZoneBounding*> *zoneBoundings;
    QList<ZoneBackground*> *zoneBgs;
    Zone* editZone;

    void changeEvent(QEvent* event) override;

    void loadMusicIDs();

    void updateList();
    void updateInfo();
    void populateBoundingIds();
    void populateBackgroundIds();

    bool editingAZone = false;
    bool handleChanges = true;

    QUndoStack *undoStack;
    Level *level;

private slots:
    void handleZoneListIndexChange(QTreeWidgetItem* item);
    void handleIDChange(int idVal);
    void handleProgPathIDChange(int ppIDVal);
    void handleMusicIDChange(QString text);
    void handleMultiPlayerTrackingChange(QString text);
    void handleUnk1Change(int val);
    void handleCameraFlagsChange(bool checked);
    void handleSelectContentsClicked();
    void handleScreenshotClicked();
    void handleBoundingIDChange(int val);
    void handleBackgroundIDChange(int val);
    void handleBoundingIdComboChange();
    void handleBackgroundIdComboChange();
    void handleEditBoundingClicked();
    void handleEditBackgroundClicked();
};

#endif // ZONEEDITORWIDGET_H
