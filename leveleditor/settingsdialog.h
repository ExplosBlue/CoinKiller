#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog() override;

signals:
    void changesApplied();

private slots:
    void changeBackgroundColor();
    void changeUndoLimit();
    void changeRenderTransparentLiquidAboveTiles();
    void changeSelectOnPlaced();
    void changeShowStatusbar();
    void handleButtonClicked(QAbstractButton *button);

private:
    Ui::SettingsDialog *ui;

    void loadSettings();
    void restoreDefaults();
    void applyChanges();

    void updateColorPickerButton(QColor color);

    QColor backgroundColor;
    quint32 undoLimit;
    bool renderTransparentLiquidAboveTiles;
    bool selectOnPlaced;
    bool showStatusbar;
};

#endif // SETTINGSDIALOG_H
