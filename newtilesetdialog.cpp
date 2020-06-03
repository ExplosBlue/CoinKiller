#include "newtilesetdialog.h"
#include "ui_newtilesetdialog.h"

NewTilesetDialog::NewTilesetDialog(QWidget *parent, SettingsManager* settings) :
    QDialog(parent),
    ui(new Ui::NewTilesetDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Add Tileset..."));
    ui->okButton->setText(tr("OK"));
    ui->cancelButton->setText(tr("Cancel"));

    ui->okButton->setEnabled(false);

    QStringList slotNames;
    slotNames << tr("Standard Suite") << tr("Stage Suite") << tr("Background Suite") << tr("Interactive Suite");

    ui->tilesetTypeBox->addItems(slotNames);
    ui->tilesetTypeBox->setCurrentIndex(1);

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

NewTilesetDialog::~NewTilesetDialog()
{
    delete ui;
}

void NewTilesetDialog::on_tilesetNameEdit_textChanged(const QString &text)
{
    ui->okButton->setEnabled(text.length() != 0);
}

QString NewTilesetDialog::getName()
{
    QString prefix;
    switch (ui->tilesetTypeBox->currentIndex())
    {
        case 0: prefix = "J_"; break;
        case 2: prefix = "S1_"; break;
        case 3: prefix = "S2_"; break;
        default: prefix = "M_"; break;
    }
    return (prefix + ui->tilesetNameEdit->text());
}

int NewTilesetDialog::getSlot()
{
    return ui->tilesetTypeBox->currentIndex();
}
