#include "newleveldialog.h"
#include "ui_newleveldialog.h"

NewLevelDialog::NewLevelDialog(QWidget *parent, SettingsManager* settings) :
    QDialog(parent),
    ui(new Ui::NewLevelDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Add Level..."));
    ui->okButton->setText(tr("OK"));
    ui->cancelButton->setText(tr("Cancel"));

    ui->okButton->setEnabled(false);

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

NewLevelDialog::~NewLevelDialog()
{
    delete ui;
}

void NewLevelDialog::on_levelNameEdit_textChanged(const QString &text)
{
    QRegularExpression re("^(.*?/)?\\d+-\\d+$");

    ui->okButton->setEnabled(text.length() != 0 && re.match(text).hasMatch());
}

QString NewLevelDialog::getName()
{
    return ui->levelNameEdit->text();
}
