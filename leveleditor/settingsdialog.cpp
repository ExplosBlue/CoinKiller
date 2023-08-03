#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settingsmanager.h"

#include <QColorDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    loadSettings();

    connect(ui->backgroundColorPickerButton, &QPushButton::clicked, this, &SettingsDialog::changeBackgroundColor);
    connect(ui->undoLimitSpinBox, &QSpinBox::valueChanged, this, &SettingsDialog::changeUndoLimit);
    connect(ui->selectOnPlacedCheckBox, &QCheckBox::clicked, this, &SettingsDialog::changeSelectOnPlaced);
    connect(ui->showStatusBarCheckBox, &QCheckBox::clicked, this, &SettingsDialog::changeShowStatusbar);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::handleButtonClicked);
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::loadSettings() {
    backgroundColor = SettingsManager::getInstance()->getLEWindowColor();
    updateColorPickerButton(backgroundColor);

    undoLimit = SettingsManager::getInstance()->getLEUndoLimit();
    ui->undoLimitSpinBox->setValue(static_cast<int>(undoLimit));

    selectOnPlaced = SettingsManager::getInstance()->getLESelectOnPlace();
    ui->selectOnPlacedCheckBox->setChecked(selectOnPlaced);

    showStatusbar = SettingsManager::getInstance()->getLEShowStatusbar();
    ui->showStatusBarCheckBox->setChecked(showStatusbar);
}

void SettingsDialog::restoreDefaults() {
    backgroundColor = SettingsManager::LE_WINDOW_COLOR_DEFAULT;
    updateColorPickerButton(backgroundColor);

    undoLimit = SettingsManager::LE_UNDO_LIMIT_DEFAULT;
    ui->undoLimitSpinBox->setValue(static_cast<int>(undoLimit));

    selectOnPlaced = SettingsManager::LE_SELECT_ON_PLACE_DEFAULT;
    ui->selectOnPlacedCheckBox->setChecked(selectOnPlaced);

    showStatusbar = SettingsManager::LE_SHOW_STATUSBAR_DEFAULT;
    ui->showStatusBarCheckBox->setChecked(showStatusbar);
}

void SettingsDialog::applyChanges() {
    SettingsManager::getInstance()->setLEWindowColor(backgroundColor);
    SettingsManager::getInstance()->setLEUndoLimit(undoLimit);
    SettingsManager::getInstance()->setLESelectOnPlace(selectOnPlaced);
    SettingsManager::getInstance()->setLEShowStatusbar(showStatusbar);

    emit changesApplied();
}

void SettingsDialog::handleButtonClicked(QAbstractButton *button) {
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
        restoreDefaults();
    }

    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        applyChanges();
    }

    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
        applyChanges();
        accept();
    }
}

void SettingsDialog::changeBackgroundColor() {

    QColorDialog::ColorDialogOptions options = QColorDialog::DontUseNativeDialog;

#ifdef USE_KDE_BLUR
    options |= QColorDialog::ShowAlphaChannel;
#endif

    QColor newColor = QColorDialog::getColor(backgroundColor, this, tr("Select Background Color"), options);
    if(newColor.isValid()) {
        backgroundColor = newColor;
        updateColorPickerButton(newColor);
    }
}

void SettingsDialog::changeUndoLimit() {
    undoLimit = ui->undoLimitSpinBox->value();
}

void SettingsDialog::changeSelectOnPlaced() {
    selectOnPlaced = ui->selectOnPlacedCheckBox->isChecked();
}

void SettingsDialog::changeShowStatusbar() {
    showStatusbar = ui->showStatusBarCheckBox->isChecked();
}

void SettingsDialog::updateColorPickerButton(QColor color) {
    const QString buttonStyle("QPushButton { border: none; background-color : %1;}");
    ui->backgroundColorPickerButton->setStyleSheet(buttonStyle.arg(color.name()));
    ui->backgroundColorPickerButton->setAutoFillBackground(true);
    ui->backgroundColorPickerButton->setFlat(true);
}
