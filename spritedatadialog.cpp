#include "spritedatadialog.h"
#include "ui_spritedatadialog.h"
#include "settingsmanager.h"

#include <QMessageBox>
#include <QUrl>

SpriteDataDialog::SpriteDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpriteDataDialog)
{
    ui->setupUi(this);

    ui->urlEdit->setText(SettingsManager::getInstance()->getSpriteDataUrl());

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton *button) {
        switch (ui->buttonBox->buttonRole(button))
        {
        case QDialogButtonBox::AcceptRole:
        case QDialogButtonBox::ApplyRole:
            save();
            break;
        case QDialogButtonBox::ResetRole:
            restoreDefaults();
            break;
        case QDialogButtonBox::RejectRole:
            reject();
            break;
        default:
            break;
        }
    });
}

SpriteDataDialog::~SpriteDataDialog()
{
    delete ui;
}

void SpriteDataDialog::save()
{
    QString url = ui->urlEdit->text().trimmed();

    QUrl parsedUrl(url);
    if (url.isEmpty() || !parsedUrl.isValid() || parsedUrl.scheme().isEmpty() || parsedUrl.host().isEmpty())
    {
        QMessageBox::warning(this, "CoinKiller", tr("Please enter a valid URL."), QMessageBox::Ok);
        return;
    }

    SettingsManager::getInstance()->setSpriteDataUrl(url);
    accept();
}

void SpriteDataDialog::restoreDefaults()
{
    ui->urlEdit->setText(SettingsManager::SPRITEDATA_URL_DEFAULT);
    ui->urlEdit->setFocus();
}
