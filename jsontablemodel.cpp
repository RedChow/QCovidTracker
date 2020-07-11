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

#include "jsontablemodel.h"
#include <QDebug>
#include <QSqlError>

JSONTableModel::JSONTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    dbi = new DatabaseInterface(1);
    dbi->setThreadDBName("JSONTableModel");
    mdb = dbi->getThreadDB();
    setDataEnum();
    setDataSources();
}

JSONTableModel::~JSONTableModel() {

}

void JSONTableModel::setDataEnum() {
    QString query_str = "SELECT data_type_enum_id, data_type FROM data_type_enum";
    QSqlQuery query(query_str, mdb);
    while (query.next()) {
        dataEnum[query.value(0).toInt()] = query.value(1).toString();
    }
}

void JSONTableModel::setDataSources() {
    QString query_str = "SELECT data_source_id, data_source_name FROM data_source";
    QSqlQuery query(query_str, mdb);
    while (query.next()) {
        dataSources[query.value(0).toInt()] = query.value(1).toString();
    }
}

void JSONTableModel::populateData() {
    beginResetModel();
    QString query_str = "SELECT json_mappings_id, json_key, data_type_enum_id, deprecated, data_source_id FROM json_mappings ORDER BY json_key";
    QSqlQuery query(query_str, mdb);
    while (query.next()) {
         jsonStructVector.emplace_back(query.value(0).toInt(), query.value(1).toString(), query.value(2).toInt(), query.value(3).toInt(), query.value(4).toInt());
    }
    endResetModel();
}

bool JSONTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, row+count);
    QSqlQuery query = QSqlQuery(mdb);
    query.prepare("INSERT INTO json_mappings (json_key, deprecated) "
                  "VALUES (:jk, :d) RETURNING json_mappings_id;");
    query.bindValue(":jk", "");
    query.bindValue(":d", 0);
    query.exec();
    if (query.next()) {
        int json_mappings_id = query.value(0).toInt();
        jsonStructVector.emplace_back(json_mappings_id, "", 1, 0, 2);
    }
    else {
        return false; //failed to get json_mappings_id
    }
    endInsertRows();
    return true;
}

int JSONTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return jsonStructVector.size();
}

int JSONTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    //json_key, data_type_enum_id, deprecated, data source
    return 4;
}

bool JSONTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row+count-1);
    jsonStructVector.erase(jsonStructVector.begin() + row, jsonStructVector.begin() + row + count);
    endRemoveRows();
    return true;
}

void JSONTableModel::clearTable() {
    jsonStructVector.clear();
}

QVariant JSONTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return QString("JSON/History Key");
            case 1:
                return QString("Data Type");
            case 2:
                return QString("Deprecated");
            case 3:
                return QString("Data Source");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

QVariant JSONTableModel::data(const QModelIndex &index, int role ) const {
    if (!index.isValid()) { // || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return jsonStructVector[index.row()].json_key;
        }
        else if (index.column() == 1) {
            QString returnDataType = dataEnum[jsonStructVector[index.row()].data_type_enum_id];
            if (returnDataType.isEmpty()) {
                return QString("Not Set");
            }
            return returnDataType;
        }
        else if (index.column() == 2) {
            int deprecated = jsonStructVector[index.row()].deprecated;
            if (deprecated == 0) {
                return QString("No");
            }
            return QString("Yes");
        }
        else if (index.column() == 3) {
            QString dataSourceStr = dataSources[jsonStructVector[index.row()].data_source_id];
            if (dataSourceStr.isEmpty()) {
                return QString("Not Set");
            }
            return dataSourceStr;
        }
    }
    return QVariant();
}


void JSONTableModel::sort(int column, Qt::SortOrder order) {
    //lambda sorting functions, seems verbose...
    //recall that json_mappings_id < 0 when it's the date column
    auto ascendingOrder = [column] (const JSONTableStruct left, const JSONTableStruct right) -> bool
    {
        if (column == 0) {
            return left.json_mapping_id < right.json_mapping_id;
        }
        else if (column == 1) {
            return left.data_type_enum_id < right.data_type_enum_id;
        }
        else if (column == 2) {
            return left.deprecated < right.deprecated;
        }
        else  {
            return left.data_source_id < right.data_source_id;
        }
    };

    auto descendingOrder = [column] (const JSONTableStruct left, const JSONTableStruct right) -> bool
    {
        if (column == 0) {
            return left.json_mapping_id > right.json_mapping_id;
        }
        else if (column == 1) {
            return left.data_type_enum_id > right.data_type_enum_id;
        }
        else if (column == 2) {
            return left.deprecated > right.deprecated;
        }
        else  {
            return left.data_source_id > right.data_source_id;
        }
    };
    emit layoutAboutToBeChanged();
    if (order == Qt::DescendingOrder) {
        std::sort(jsonStructVector.begin(), jsonStructVector.end(), ascendingOrder);
    }
    else {
        std::sort(jsonStructVector.begin(), jsonStructVector.end(), descendingOrder);
    }
    emit layoutChanged();
}

bool JSONTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(role);
    if (index.column() == 0) {
        jsonStructVector[index.row()].json_key = value.toString();
        QSqlQuery query = QSqlQuery(mdb);
        query.prepare("UPDATE json_mappings SET json_key = :jk WHERE json_mappings_id = :jmi");
        query.bindValue(":jk", value.toString());
        query.bindValue(":jmi", jsonStructVector[index.row()].json_mapping_id);
        return query.exec();
    }
    else if (index.column() == 1) {
        jsonStructVector[index.row()].data_type_enum_id = value.toInt();
        QSqlQuery query = QSqlQuery(mdb);
        query.prepare("UPDATE json_mappings SET data_type_enum_id = :dtei WHERE json_mappings_id = :jmi");
        query.bindValue(":dtei", value.toInt());
        query.bindValue(":jmi", jsonStructVector[index.row()].json_mapping_id);
        return query.exec();
    }
    else if (index.column() == 2) {
        jsonStructVector[index.row()].deprecated = value.toInt();
        QSqlQuery query = QSqlQuery(mdb);
        query.prepare("UPDATE json_mappings SET deprecated = :d WHERE json_mappings_id = :jmi");
        query.bindValue(":d", value.toInt());
        query.bindValue(":jmi", jsonStructVector[index.row()].json_mapping_id);
        return query.exec();
    }
    else {
        jsonStructVector[index.row()].data_source_id = value.toInt();
        QSqlQuery query = QSqlQuery(mdb);
        query.prepare("UPDATE json_mappings SET data_source_id=:dsi WHERE json_mappings_id=:jmi");
        query.bindValue(":dsi", value.toInt());
        query.bindValue(":jmi", jsonStructVector[index.row()].json_mapping_id);
        return query.exec();
    }
    return false;
}


Qt::ItemFlags JSONTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}














