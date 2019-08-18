#include "newleveldialog.h"
#include "ui_newleveldialog.h"

NewLevelDialog::NewLevelDialog(QWidget *parent, SettingsManager* settings) :
    QDialog(parent),
    ui(new Ui::NewLevelDialog)
{
    ui->setupUi(this);
    setWindowTitle(settings->getTranslation("MainWindow", "addLevel") + "...");
    ui->okButton->setText(settings->getTranslation("General", "ok"));
    ui->cancelButton->setText(settings->getTranslation("General", "cancel"));

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
