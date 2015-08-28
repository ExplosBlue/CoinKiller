#ifndef SILLYTEST_H
#define SILLYTEST_H

#include <QMainWindow>
#include <QWidget>

class Crapomatic : public QWidget
{
    Q_OBJECT
public:
    explicit Crapomatic(QWidget *parent);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    int mouseX, mouseY;
    int snapX, snapY;
    bool drawing;
    int lastX, lastY;

    QList<QLine> lines;
};


class SillyTest : public QMainWindow
{
    Q_OBJECT
public:
    explicit SillyTest(QWidget *parent = 0);

protected:
   // void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

signals:

public slots:
};

#endif // SILLYTEST_H
