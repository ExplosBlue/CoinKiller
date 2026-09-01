#ifndef NEWTILESETDIALOG_H
#define NEWTILESETDIALOG_H

#include "settingsmanager.h"

#include <QDialog>

namespace Ui {
class NewTilesetDialog;
}

class NewTilesetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewTilesetDialog(QWidget *parent, SettingsManager *settings,
                              QString initialName = QString(), int initialSlot = -1,
                              QString title = QString());    ~NewTilesetDialog();
    QString getName();
    QString getBaseName();
    int getSlot();

private slots:
    void on_tilesetNameEdit_textChanged(const QString &text);

private:
    Ui::NewTilesetDialog *ui;
};

#endif // NEWTILESETDIALOG_H
