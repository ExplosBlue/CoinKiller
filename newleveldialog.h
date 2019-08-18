#ifndef NEWLEVELDIALOG_H
#define NEWLEVELDIALOG_H

#include "settingsmanager.h"

#include <QDialog>

namespace Ui {
class NewLevelDialog;
}

class NewLevelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewLevelDialog(QWidget *parent, SettingsManager *settings);
    ~NewLevelDialog();
    QString getName();

private slots:
    void on_levelNameEdit_textChanged(const QString &text);

private:
    Ui::NewLevelDialog *ui;
};

#endif // NEWLEVELDIALOG_H
