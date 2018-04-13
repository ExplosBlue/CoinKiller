#ifndef CLICKBLELABEL_H
#define CLICKBLELABEL_H

#include <QLabel>
#include <QWidget>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
    explicit ClickableLabel(const QString& text, QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~ClickableLabel();

signals:
    void clicked();
    void doubleClicked();

protected:
    void mousePressEvent(QMouseEvent* evt) override;
    void mouseDoubleClickEvent(QMouseEvent* evt) override;
};

#endif // CLICKBLELABEL_H
