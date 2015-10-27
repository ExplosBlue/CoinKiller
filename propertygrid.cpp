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

#include "propertygrid.h"

PropertyGrid::PropertyGrid(QWidget* parent)
    : QTableView(parent)
{

}





PropertyGridModel::PropertyGridModel(PropertyGrid* parent)
{
    this->parent = parent;

    // do shit here
}

PropertyGridModel::~PropertyGridModel()
{
    // undo shit here
}

int PropertyGridModel::rowCount(const QModelIndex & parent) const
{
    return 3;
}

int PropertyGridModel::columnCount(const QModelIndex & parent) const
{
    return 2;
}

QVariant PropertyGridModel::data(const QModelIndex & index, int role) const
{
    //qDebug("data %d %d", index.row(), index.column());

    switch (role)
    {
    case Qt::DecorationRole:
    case Qt::CheckStateRole:
    case Qt::StatusTipRole:
        return QVariant::Invalid;

    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    case Qt::ToolTipRole:
        // TODO!!
        return QVariant::Invalid;

    case Qt::DisplayRole:
        switch (index.row())
        {
        case 0: return QVariant(index.column() ? "zerp" : "derp");
        case 1: return QVariant(index.column() ? "zouble" : "double");
        case 2: return QVariant(index.column() ? "zirc" : "dirc");
        }
        break;
    }

    return QVariant::Invalid;
}

QVariant PropertyGridModel::headerData(int section, Qt::Orientation orientation, int role)
{
    qDebug("header %d %d", section, orientation);
    return QVariant("head0r");
}

