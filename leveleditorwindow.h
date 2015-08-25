#ifndef LEVELEDITORWINDOW_H
#define LEVELEDITORWINDOW_H

#include <QMainWindow>

#include "levelview.h"
#include "ctpk.h"

namespace Ui {
class LevelEditorWindow;
}

class LevelEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LevelEditorWindow(QWidget *parent, Ctpk* testcrap);
    ~LevelEditorWindow();

    Ctpk* crapshit; // REMOVE ME!!!!!

private:
    Ui::LevelEditorWindow *ui;

    LevelView* levelView;
};

#endif // LEVELEDITORWINDOW_H
