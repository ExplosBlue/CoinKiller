#include "sarcexplorerwindow.h"
#include "ui_sarcexplorerwindow.h"

#include <QMessageBox>

SarcExplorerWindow::SarcExplorerWindow(QWidget *parent, QString path, SettingsManager* settings) :
    QMainWindow(parent),
    ui(new Ui::SarcExplorerWindow)
{
    this->settings = settings;
    this->setAttribute(Qt::WA_DeleteOnClose);
    iconsPath = QCoreApplication::applicationDirPath() + "/coinkiller_data/icons/";
    ui->setupUi(this);

    setWindowTitle(settings->getTranslation("SarcExplorer", "sarcExplorer") + " - " + path);

    sarcFile = new ExternalFile(NULL, path);
    sarc = new SarcFilesystem(sarcFile);

    setupFileTree(fileTree.invisibleRootItem(), "");
    fileTree.setSortRole(Qt::DisplayRole);
    fileTree.invisibleRootItem()->setData("D");
    // ui->fileTreeView->setAnimated(true); Dunno if this looks good...
    ui->fileTreeView->setModel(&fileTree);

    fileTree.sort(Qt::DisplayRole, Qt::AscendingOrder);

    QObject::connect(&fileTree, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(renameFile(QStandardItem*)));
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

    //files.sort();
    //dirs.sort();

    foreach (QString dir, dirs)
    {
        QStandardItem* dirItem = new QStandardItem(dir);
        dirItem->setData("D" + path + "/" + dir);
        dirItem->setFlags(dirItem->flags() | Qt::ItemIsEditable);
        setIconOfNode(dirItem);
        node->appendRow(dirItem);
        setupFileTree(dirItem, path + "/" + dir);
    }

    foreach (QString file, files)
    {
        QStandardItem* fileItem = new QStandardItem(file);

        fileItem->setData("F" + path + "/" + file);
        fileItem->setFlags(fileItem->flags() | Qt::ItemIsEditable);

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
        QString fileExtesion = l.at(l.count()-1);
        QIcon i;

        if (fileExtesion == "ctpk")
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

void SarcExplorerWindow::renameFile(QStandardItem *node)
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
            QMessageBox::information(this, "CoinKiller - " + settings->getTranslation("SarcExplorer", "sarcExplorer"), "A file with that name already exists!", QMessageBox::Ok);
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
            QMessageBox::information(this, "CoinKiller - " + settings->getTranslation("SarcExplorer", "sarcExplorer"), "A folder with that name already exists!", QMessageBox::Ok);
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
