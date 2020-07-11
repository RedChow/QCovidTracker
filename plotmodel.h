/*
 * Copyright (C) 2020 brian DOT l DOT miller DOT ttu AT gmail DOT com
 * This file is part of QCovidTracker.
 *
 * This program comes with ABSOLUTELY NO WARRANTY
 * QCovidTracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QCovidTracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QCovidTracker.  If not, see <https://www.gnu.org/licenses/.
 */
#ifndef PLOTMODEL_H
#define PLOTMODEL_H
#include <QObject>
#include <QAbstractTableModel>
#include <QVector>
#include <QThread>
#include "databaseinterface.h"

#include <vector>

class PlotModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PlotModel(QObject *parent = nullptr);
    ~PlotModel();
    //Rquired QTableAbstractModel functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    //helpers
    void populateData();
    void clearTable();
    QVector<QVector<int>> tableData;
    QVector<QDate> tableQDates;
    int stateId;
    void setStateId(int p_stateId);
    int days;
    void setDays(int p_days);
    QStringList headers;
    void setHeaders(QStringList p_Headers);
signals:

private:
    DatabaseInterface *dbi;
    QSqlDatabase mdb;
};

#endif // PLOTMODEL_H
