/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

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
