#include "sarcexplorerwindow.h"
#include "ui_sarcexplorerwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

SarcExplorerWindow::SarcExplorerWindow(WindowBase *parent, QString path, SettingsManager* settings) :
    WindowBase(parent),
    ui(new Ui::SarcExplorerWindow)
{
    this->settings = settings;
    this->setAttribute(Qt::WA_DeleteOnClose);
    iconsPath = SettingsManager::getInstance()->dataPath("icons/");
    ui->setupUi(this);

    setWindowTitle(tr("CoinKiller - Sarc Explorer - %1").arg(path));

    sarcFile = new ExternalFile(NULL, path);
    sarc = new SarcFilesystem(sarcFile);

    setupFileTree(fileTree.invisibleRootItem(), "");
    fileTree.setSortRole(Qt::DisplayRole);
    fileTree.invisibleRootItem()->setData("D");
    fileTree.invisibleRootItem()->setFlags(fileTree.invisibleRootItem()->flags() | Qt::ItemIsEditable);

    fileTree.invisibleRootItem()->setDropEnabled(true);

    ui->fileTreeView->setModel(&fileTree);
    ui->fileTreeView->setDragEnabled(true);
    ui->fileTreeView->setDropIndicatorShown(true);
    ui->fileTreeView->setDragDropMode(QAbstractItemView::InternalMove);

    ui->importButton->setEnabled(false);
    ui->exportButton->setEnabled(false);
    ui->insertButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->insertFolderButton->setEnabled(false);

    fileTree.sort(Qt::DisplayRole, Qt::AscendingOrder);

    QObject::connect(&fileTree, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
    QObject::connect(ui->fileTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectedFileChanged()));

    if (!settings->getLastSarcExpPath().isEmpty())
        basePath = settings->getLastSarcExpPath();
    else
        basePath = QCoreApplication::applicationDirPath();   
}

SarcExplorerWindow::~SarcExplorerWindow()
{
    delete ui;
    delete sarcFile;
}

void SarcExplorerWindow::setupFileTree(QStandardItem* node, QString path)
{
    QStringList files;
    QStringList dirs;

    sarc->directoryContents(path, QDir::Files, files);
    sarc->directoryContents(path, QDir::Dirs, dirs);

    foreach (QString dir, dirs)
    {
        QStandardItem* dirItem = new QStandardItem(dir);
        dirItem->setData("D" + path + "/" + dir);
        dirItem->setFlags(dirItem->flags() | Qt::ItemIsEditable);
        dirItem->setDragEnabled(true);
        dirItem->setDropEnabled(true);
        setIconOfNode(dirItem);
        node->appendRow(dirItem);
        setupFileTree(dirItem, path + "/" + dir);
    }

    foreach (QString file, files)
    {
        QStandardItem* fileItem = new QStandardItem(file);

        fileItem->setData("F" + path + "/" + file);
        fileItem->setFlags(fileItem->flags() | Qt::ItemIsEditable);
        fileItem->setDragEnabled(true);
        fileItem->setDropEnabled(false);

        setIconOfNode(fileItem);

        node->appendRow(fileItem);
    }
}

void SarcExplorerWindow::setIconOfNode(QStandardItem *node)
{
    if (node->data().toString().at(0) == 'D')
    {
        node->setIcon(QIcon(iconsPath + "folder.png"));
        return;
    }

    QStringList l = node->text().split(".");

    if (l.count() > 1)
    {
        QString fileExtension = l.at(l.count()-1);
        QIcon i;

        if (fileExtension == "ctpk")
            i = QIcon(iconsPath + "file_image.png");
        else
            i = QIcon(iconsPath + "file.png");

        node->setIcon(i);
    }
    else
        node->setIcon(QIcon(iconsPath + "file.png"));
}

void SarcExplorerWindow::renameChildNodes(QStandardItem *node, QString oldName, QString newName)
{
    for (int i = 0; i < node->rowCount(); i++)
    {
        QStandardItem* child = node->child(i);
        QString newData = child->data().toString().replace(oldName, newName);
        child->setData(newData);
        renameChildNodes(child, oldName, newName);
    }
}

void SarcExplorerWindow::itemChanged(QStandardItem* node)
{
    QString fileName = node->data().toString().remove(0, 1).split("/").last();

    if (fileName != node->text()) // Renamed
        renameFile(node);
    else
        changeDirectory(node);  // Drag-Dropped
}

void SarcExplorerWindow::renameFile(QStandardItem* node)
{
    QStandardItem* parent = node->parent();

    QString path;

    if (parent == NULL)
        parent = fileTree.invisibleRootItem();

    path = parent->data().toString();
    path.remove(0, 1);

    QChar type = node->data().toString().at(0);
    QString fileName = node->data().toString().remove(0, 1);

    if (type == 'F')
    {
        if (sarc->fileExists(path + "/" + node->text()))
        {
            fileTree.blockSignals(true);
            QStringList l = node->data().toString().split("/");
            node->setText(l.at(l.length()-1));
            fileTree.blockSignals(false);
            QMessageBox::information(this, tr("CoinKiller - Sarc Explorer"), tr("A file with that name already exists!"), QMessageBox::Ok);
            return;
        }

        sarc->renameFile(fileName, node->text());
        fileTree.blockSignals(true);
        node->setData("F" + path + "/" + node->text());
        setIconOfNode(node);
        fileTree.sort(Qt::DisplayRole, Qt::AscendingOrder);
        fileTree.blockSignals(false);
    }

    if (type == 'D')
    {
        if (sarc->directoryExists(path + "/" + node->text()))
        {
            fileTree.blockSignals(true);
            QStringList l = node->data().toString().split("/");
            node->setText(l.at(l.length()-1));
            fileTree.blockSignals(false);
            QMessageBox::information(this, tr("CoinKiller - Sarc Explorer"), tr("A folder with that name already exists!"), QMessageBox::Ok);
            return;
        }

        sarc->renameDir(path + fileName, path + node->text());
        fileTree.blockSignals(true);
        renameChildNodes(node, path + fileName, path + node->text());
        node->setData("D" + path + "/" + node->text());
        fileTree.blockSignals(false);
        fileTree.sort(Qt::DisplayRole, Qt::AscendingOrder);
    }
}

void SarcExplorerWindow::changeDirectory(QStandardItem *node)
{
    QStandardItem* parent = node->parent();
    QChar type = node->data().toString().at(0);

    if (parent == NULL)
        parent = fileTree.invisibleRootItem();

    QString fileName = node->data().toString().remove(0, 1);

    QString tempString;
    QStandardItem* temp = node;

    while(true)
    {
       if (temp->parent() != NULL)
       {
           tempString = temp->parent()->data().toString().remove(0, 2).split("/").last() + "/" + tempString;
           temp = temp->parent();
       }
       else
           break;
    }

    QString newPath = tempString + node->text();

    if (!newPath.startsWith("/"))
        newPath = "/" + newPath;

    if (type == 'F')
    {
        sarc->changeFileDir(fileName, newPath);
        fileTree.blockSignals(true);
        node->setData("F" + newPath);
        setIconOfNode(node);
        fileTree.sort(Qt::DisplayRole, Qt::AscendingOrder);
        fileTree.blockSignals(false);
    }

    if (type == 'D')
    {
        for (int i=0; i < node->rowCount(); i++)
        {
            QStandardItem* childNode = node->child(i);
            changeDirectory(childNode);
        }

        fileTree.blockSignals(true);
        node->setData("D" + newPath);
        setIconOfNode(node);
        fileTree.sort(Qt::DisplayRole, Qt::AscendingOrder);
        fileTree.blockSignals(false);
    }
}

void SarcExplorerWindow::on_exportButton_clicked()
{
    this->setEnabled(false);

    QStandardItem* currentItem = fileTree.itemFromIndex(ui->fileTreeView->currentIndex());
    QChar type = currentItem->data().toString().at(0);
    QString name = currentItem->data().toString().remove(0, 1);

    if (type == 'F')
    {
        QStringList l = currentItem->text().split(".");

        QString fileExtension = "";

        if (l.count() > 1)
            fileExtension = l.at(l.count()-1);

        QString filePath = QFileDialog::getSaveFileName(this, tr("Export File"), basePath + name, " (*.%1)").arg(fileExtension);

        if (filePath.isEmpty() || filePath.isEmpty())
        {
            this->setEnabled(true);
            return;
        }

        QStringList lastPath = filePath.split('/');
        lastPath.removeLast();
        QString last = lastPath.join("/");
        settings->setLastSarcExpPath(last);
        basePath = settings->getLastSarcExpPath();

        exportFile(currentItem, filePath);
    }

    if (type == 'D')
    {
        QString dirPath = QFileDialog::getExistingDirectory(this, tr("Export Folder"), basePath + name, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (dirPath.isEmpty() || dirPath.isEmpty())
        {
            this->setEnabled(true);
            return;
        }

        settings->setLastSarcExpPath(dirPath);
        basePath = settings->getLastSarcExpPath();

        for (int i = 0; i < currentItem->rowCount(); i++)
        {
            QStandardItem* child = currentItem->child(i);
            QChar childType = child->data().toString().at(0);
            QString childName = child->data().toString().remove(0, name.length()+1);

            if (childType == 'F')
            {
                QStringList l = child->text().split(".");

                QString fileExtension = "";

                if (l.count() > 1)
                    fileExtension = l.at(l.count()-1);

                exportFile(child, dirPath + childName);
            }
        }
    }

    this->setEnabled(true);
}

void SarcExplorerWindow::exportFile(QStandardItem* node, QString filePath)
{
    FileBase* itemData = sarc->openFile(node->data().toString().remove(0, 1));

    QFile outFile(filePath);
    if (outFile.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&outFile);
        itemData->open();
        itemData->seek(0);
        while(itemData->pos() != itemData->size())
        {
            quint8 temp = itemData->read8();
            stream << temp;
        }
        itemData->close();
        outFile.close();
    }
}

void SarcExplorerWindow::on_importButton_clicked()
{
    this->setEnabled(false);

    QStandardItem* currentItem = fileTree.itemFromIndex(ui->fileTreeView->currentIndex());
    QString name = currentItem->data().toString().remove(0, 1);

    QStringList l = currentItem->text().split(".");
    QString fileExtension = "";
    if (l.count() > 1)
        fileExtension = l.at(l.count()-1);

    QString importPath = QFileDialog::getOpenFileName(this, tr("Import"), basePath + name, " (*.%1)").arg(fileExtension);

    if (importPath.isEmpty() || importPath.isEmpty())
    {
        this->setEnabled(true);
        return;
    }

    QStringList lastPath = importPath.split('/');
    lastPath.removeLast();
    QString last = lastPath.join("/");
    settings->setLastSarcExpPath(last);
    basePath = settings->getLastSarcExpPath();

    FileBase* itemData = sarc->openFile(currentItem->data().toString().remove(0, 1));

    QFile importFile(importPath);
    if (importFile.open(QIODevice::ReadOnly))
    {
        QDataStream stream(&importFile);
        itemData->open();
        itemData->resize(importFile.size());
        itemData->seek(0);

        while(itemData->pos() != itemData->size())
        {
            quint8 temp;
            stream >> temp;
            itemData->write8(temp);
        }
        itemData->save();
        itemData->close();
        delete itemData;
        importFile.close();

        QMessageBox msgBox;
        msgBox.setText(tr("File Imported Successfully."));
        msgBox.exec();
    }
    this->setEnabled(true);
}

void SarcExplorerWindow::on_insertButton_clicked()
{
    this->setEnabled(false);

    QStandardItem* currentItem = fileTree.itemFromIndex(ui->fileTreeView->currentIndex());
    QChar type = currentItem->data().toString().at(0);
    QString dirName = currentItem->data().toString().remove(0, 1);

    QString inFilePath = QFileDialog::getOpenFileName(this, tr("Insert File"), basePath, "All Files(*)");

    if (inFilePath.isEmpty() || inFilePath.isEmpty())
    {
        this->setEnabled(true);
        return;
    }
    QStringList lastPath = inFilePath.split('/');
    lastPath.removeLast();
    QString last = lastPath.join("/");
    settings->setLastSarcExpPath(last);
    basePath = settings->getLastSarcExpPath();

    QString idPath = dirName + '/' + inFilePath.split('/').last();

    FileBase* newItem;
    newItem = new MemoryFile(sarc);
    newItem->setIdPath(idPath);

    QFile inFile(inFilePath);
    if (inFile.open(QIODevice::ReadOnly))
    {
        QDataStream stream(&inFile);
        newItem->open();
        newItem->resize(inFile.size());
        newItem->seek(0);

        while(newItem->pos() != newItem->size())
        {
            quint8 temp;
            stream >> temp;
            newItem->write8(temp);
        }
        newItem->save();
        newItem->close();
        inFile.close();
    }
    delete newItem;

    QStandardItem* fileItem = new QStandardItem(inFilePath.split('/').last());
    fileItem->setData("F" + idPath);
    fileItem->setFlags(fileItem->flags() | Qt::ItemIsEditable);
    fileItem->setDragEnabled(true);
    fileItem->setDropEnabled(false);
    setIconOfNode(fileItem);

    if (type == 'F')
        currentItem->parent()->appendRow(fileItem);
    else
        currentItem->appendRow(fileItem);

    this->setEnabled(true);
}

void SarcExplorerWindow::on_insertFolderButton_clicked()
{
    this->setEnabled(false);

    QStandardItem* currentItem = fileTree.itemFromIndex(ui->fileTreeView->currentIndex());
    QChar type = currentItem->data().toString().at(0);

    bool ok;
    QString dirName = QInputDialog::getText(this, tr("Name Folder"), tr("Enter Folder Name:"), QLineEdit::Normal, "NewFolder", &ok);

    if (!ok || dirName.isEmpty())
    {
        this->setEnabled(true);
        return;
    }

    QStandardItem* dirItem = new QStandardItem(dirName);
    dirItem->setData("D/" + dirName);
    dirItem->setFlags(dirItem->flags() | Qt::ItemIsEditable);
    dirItem->setDragEnabled(true);
    dirItem->setDropEnabled(true);
    setIconOfNode(dirItem);

    if (type == 'F')
        currentItem->parent()->appendRow(dirItem);
    else
        currentItem->appendRow(dirItem);

    this->setEnabled(true);
}

void SarcExplorerWindow::on_deleteButton_clicked()
{
    this->setEnabled(false);

    QStandardItem* currentItem = fileTree.itemFromIndex(ui->fileTreeView->currentIndex());
    QString name = currentItem->data().toString().remove(0, 1);
    QChar type = currentItem->data().toString().at(0);

    QMessageBox::StandardButton deleteFileDlg;
    deleteFileDlg = QMessageBox::question(this, "CoinKiller", tr("Are you sure you wish to delete %1?").arg(name.split('/').last()), QMessageBox::Cancel|QMessageBox::Ok);

    if (deleteFileDlg == QMessageBox::Ok)
    {
        if (type == 'F')
            deleteFile(currentItem);

        if (type == 'D')
            deleteFolder(currentItem);
    }

    this->setEnabled(true);
}

void SarcExplorerWindow::deleteFolder(QStandardItem* node)
{
    while (node->rowCount() > 0)
    {
        QStandardItem* child = node->child(0);
        QChar childType = child->data().toString().at(0);

        if (childType == 'F')
            deleteFile(child);

        if (childType == 'D')
            deleteFolder(child);
    }
}

void SarcExplorerWindow::deleteFile(QStandardItem* node)
{
        QString name = node->data().toString().remove(0, 1);
        sarc->deleteFile(name);

        QStandardItem* parent = node->parent();

        parent->removeRow(node->row());

        if (parent->rowCount() <= 0)
        {
           if (!parent->parent() == NULL)
               parent->parent()->removeRow(parent->row());
           else
               fileTree.invisibleRootItem()->removeRow(parent->row());
        }
}

void SarcExplorerWindow::selectedFileChanged()
{
    QStandardItem* currentItem = fileTree.itemFromIndex(ui->fileTreeView->currentIndex());
    QChar type = currentItem->data().toString().at(0);

    if (type == 'D')
    {
        ui->exportButton->setEnabled(true);
        ui->importButton->setEnabled(false);
        ui->insertButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->insertFolderButton->setEnabled(true);
    }

    else if (type == 'F')
    {
        ui->exportButton->setEnabled(true);
        ui->importButton->setEnabled(true);
        ui->insertButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->insertFolderButton->setEnabled(true);
    }
    else
    {
        ui->importButton->setEnabled(false);
        ui->exportButton->setEnabled(false);
        ui->insertButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->insertFolderButton->setEnabled(false);
    }

}





