#ifndef LEVELMINIMAP_H
#define LEVELMINIMAP_H

#include "level.h"

#include <QWidget>

class LevelMiniMap : public QWidget
{
    Q_OBJECT
public:
    explicit LevelMiniMap(QWidget *parent, Level* level);
    void setDrawRect(QRect drawRect) { this->drawRect = drawRect; }

public slots:
    void update_(QRect drawRect);
    void updateBounds();

protected:
    void paintEvent(QPaintEvent *evt) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *evt) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *evt) Q_DECL_OVERRIDE;

private:
    Level* level;
    int maxX, maxY;
    float zoom;
    QRect drawRect = QRect();

    void checkBounds(Object* obj);

signals:
    void scrollTo(int x, int y);
};

#endif // LEVELMINIMAP_H
