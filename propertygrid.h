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

#ifndef PROPERTYGRID_H
#define PROPERTYGRID_H


#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>


class PropertyGrid : public QTableView
{
public:
    PropertyGrid(QWidget* parent = 0);
};


class PropertyGridModel : public QAbstractTableModel
{
public:
    PropertyGridModel(PropertyGrid* parent);
    ~PropertyGridModel();


    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole);

private:
    PropertyGrid* parent;
};

#endif // PROPERTYGRID_H
