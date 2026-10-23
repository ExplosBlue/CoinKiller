#ifndef SPRITEDATADIALOG_H
#define SPRITEDATADIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SpriteDataDialog;
}

class SpriteDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpriteDataDialog(QWidget *parent = nullptr);
    ~SpriteDataDialog() override;

private slots:
    void save();
    void restoreDefaults();

private:
    Ui::SpriteDataDialog *ui;
};

#endif // SPRITEDATADIALOG_H
