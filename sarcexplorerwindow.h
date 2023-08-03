#ifndef SARCEXPLORERWINDOW_H
#define SARCEXPLORERWINDOW_H

#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>

#include "filesystem/filesystem.h"
#include "settingsmanager.h"
#include "windowbase.h"

namespace Ui {
class SarcExplorerWindow;
}

class SarcExplorerWindow : public WindowBase
{
    Q_OBJECT

public:
    explicit SarcExplorerWindow(WindowBase *parent, QString path, SettingsManager *settings);
    ~SarcExplorerWindow();

private:
    Ui::SarcExplorerWindow *ui;
    SettingsManager* settings;

    FileBase* sarcFile;
    SarcFilesystem* sarc;
    QStandardItemModel fileTree;

    QString iconsPath;

    QString basePath;

    void setupFileTree(QStandardItem* node, QString path);
    void setIconOfNode(QStandardItem* node);
    void renameChildNodes(QStandardItem* node, QString oldName, QString newName);
    void exportFile(QStandardItem* node, QString filePath);
    void renameFile(QStandardItem* node);
    void changeDirectory(QStandardItem* node);
    void deleteFile(QStandardItem* node);
    void deleteFolder(QStandardItem* node);

private slots:
    void itemChanged(QStandardItem* node);
    void on_exportButton_clicked();
    void on_importButton_clicked();
    void on_insertButton_clicked();
    void on_insertFolderButton_clicked();
    void on_deleteButton_clicked();
    void selectedFileChanged();
};

#endif // SARCEXPLORERWINDOW_H
