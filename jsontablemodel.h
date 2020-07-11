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

#ifndef JSONTABLEMODEL_H
#define JSONTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include "databaseinterface.h"

struct JSONTableStruct {
    int json_mapping_id;
    QString json_key;
    int data_type_enum_id;
    int deprecated;
    int data_source_id;
    JSONTableStruct(int jmi, QString jk, int dtei, int d, int dsi) : json_mapping_id(jmi), json_key(jk), data_type_enum_id(dtei), deprecated(d), data_source_id(dsi) {}
};

class JSONTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    JSONTableModel(QObject *parent=nullptr);
    ~JSONTableModel();
    //required
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    //helpers
    void populateData();
    void clearTable();
    void setDataEnum();
    void setDataSources();

    //data containers
    std::vector<JSONTableStruct> jsonStructVector;
    QHash<int, QString> dataEnum;
    QHash<int, QString> dataSources;
private:
    DatabaseInterface *dbi;
    QSqlDatabase mdb;
};

#endif // JSONTABLEMODEL_H
