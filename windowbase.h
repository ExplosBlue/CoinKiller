#ifndef WINDOWBASE_H
#define WINDOWBASE_H

#include <QMainWindow>

class WindowBase : public QMainWindow
{
    Q_OBJECT

public:
    WindowBase(WindowBase* parent = nullptr)
    {
        if(parent)
        {
            connect(parent, SIGNAL(window_closed()), this, SLOT(close()));
            connect(parent, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
        }
    }

    void closeEvent(QCloseEvent* event)
    {
        emit window_closed();
        QMainWindow::closeEvent(event);
    }

signals:
    void window_closed();
};

#endif // WINDOWBASE_H
