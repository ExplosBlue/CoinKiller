#ifndef SARCEXPLORERWINDOW_H
#define SARCEXPLORERWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>

#include "filesystem.h"
#include "settingsmanager.h"

namespace Ui {
class SarcExplorerWindow;
}

class SarcExplorerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SarcExplorerWindow(QWidget *parent, QString path, SettingsManager *settings);
    ~SarcExplorerWindow();

private:
    Ui::SarcExplorerWindow *ui;
    SettingsManager* settings;

    FileBase* sarcFile;
    SarcFilesystem* sarc;
    QStandardItemModel fileTree;

    QString iconsPath;

    void setupFileTree(QStandardItem* node, QString path);
    void setIconOfNode(QStandardItem *node);
    void renameChildNodes(QStandardItem *node, QString oldName, QString newName);

private slots:
    void renameFile(QStandardItem* node);
};

#endif // SARCEXPLORERWINDOW_H
