#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f) {}

ClickableLabel::ClickableLabel(const QString& text, QWidget* parent, Qt::WindowFlags f)
    : QLabel(text, parent, f) {}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::mousePressEvent(QMouseEvent* evt)
{
    Q_UNUSED(evt);
    emit clicked();
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* evt)
{
    Q_UNUSED(evt);
    emit doubleClicked();
}
