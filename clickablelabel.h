#ifndef CLICKBLELABEL_H
#define CLICKBLELABEL_H

#include <QLabel>
#include <QWidget>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    explicit ClickableLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();

signals:
    void clicked();
    void doubleClicked();

protected:
    void mousePressEvent(QMouseEvent* evt) override;
    void mouseDoubleClickEvent(QMouseEvent* evt) override;
};

#endif // CLICKBLELABEL_H
