#include "newtilesetdialog.h"
#include "ui_newtilesetdialog.h"

NewTilesetDialog::NewTilesetDialog(QWidget *parent, SettingsManager* settings) :
    QDialog(parent),
    ui(new Ui::NewTilesetDialog)
{
    ui->setupUi(this);
    setWindowTitle(settings->getTranslation("MainWindow", "addTileset") + "...");
    ui->okButton->setText(settings->getTranslation("General", "ok"));
    ui->cancelButton->setText(settings->getTranslation("General", "cancel"));

    ui->okButton->setEnabled(false);

    QStringList slotNames;
    slotNames << "Standard Suite" << "Stage Suite" << "Background Suite" << "Interactive Suite";
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
