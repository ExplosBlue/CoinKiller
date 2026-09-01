#include "newtilesetdialog.h"
#include "ui_newtilesetdialog.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>

NewTilesetDialog::NewTilesetDialog(QWidget *parent, SettingsManager* settings, QString initialName, int initialSlot, QString title) :
    QDialog(parent),
    ui(new Ui::NewTilesetDialog)
{
    Q_UNUSED(settings);

    ui->setupUi(this);
    setWindowTitle(title.isEmpty() ? tr("Add Tileset...") : title);
    ui->okButton->setText(tr("OK"));
    ui->cancelButton->setText(tr("Cancel"));

    ui->okButton->setEnabled(false);

    ui->tilesetNameEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[^\\s]*"), ui->tilesetNameEdit));

    QStringList slotNames;
    slotNames << tr("Standard Suite") << tr("Stage Suite") << tr("Background Suite") << tr("Interactive Suite");

    ui->tilesetTypeBox->addItems(slotNames);

    if (initialSlot >= 0 && initialSlot < slotNames.size())
        ui->tilesetTypeBox->setCurrentIndex(initialSlot);
    else
        ui->tilesetTypeBox->setCurrentIndex(1);

    if (!initialName.isEmpty())
    {
        ui->tilesetNameEdit->setText(initialName);
        ui->okButton->setEnabled(true);
    }

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

QString NewTilesetDialog::getBaseName()
{
    return ui->tilesetNameEdit->text();
}

int NewTilesetDialog::getSlot()
{
    return ui->tilesetTypeBox->currentIndex();
}
