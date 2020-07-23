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

#include "dataeditormodel.h"
#include <QDebug>
#include <QSqlError>
/*
 *	Headers will be dependent on the data_source field in state_info.
 * 		Only show non-deprecated fields
 */

DataEditorTableModel::DataEditorTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    dbi = new DatabaseInterface(1);
    dbi->setThreadDBName("DataEditorModel");
    mdb = dbi->getThreadDB();
}

void DataEditorTableModel::populateData(int stateInfoId, int dataSourceId) {
    thisStateInfoId = stateInfoId;
    beginResetModel();
    QSqlQuery query = QSqlQuery(mdb);
    query.prepare("SELECT covid_history_date, json_mappings_id, string_value, integer_value FROM covid_history "
                    "WHERE state_info_id=:sii AND json_mappings_id IN (SELECT json_mappings_id FROM json_mappings WHERE "
                  "deprecated=0 AND data_source_id=:dsi) "
                  "ORDER BY covid_history_date");
    query.bindValue(":sii", stateInfoId);
    query.bindValue(":dsi", dataSourceId);
    query.exec();
    QDate date = QDate(1969, 1, 1);
    DataTableStruct *dataTableStruct = new DataTableStruct();
    //std::unique_ptr<DataTableStruct> dataTableStruct(new DataTableStruct);
    //std::unique_ptr<std::vector<std::unique_ptr<DataTableStruct>>> list(new std::vector<std::unique_ptr<DataTableStruct>>());
    while (query.next()) {
        if (query.value(0).toDate() > date) {
            //we have gotten to the next date
            if (!dataTableStruct->date.isNull()) {
                tableData.push_back(dataTableStruct);
                //tableData.push_back(dataTableStruct);
                //list->push_back(dataTableStruct);
            }
            dataTableStruct = new DataTableStruct();
            //std::unique_ptr<DataTableStruct> dataTableStruct(new DataTableStruct);
            date = query.value(0).toDate();
            dataTableStruct->date = date;
        }
        if (query.value(2).isNull()) {
            dataTableStruct->data[query.value(1).toInt()] = query.value(3);
        }
        else {
            dataTableStruct->data[query.value(1).toInt()] = query.value(2);
        }
    }
    //The last needs to be inserted
    if (!dataTableStruct->date.isNull()) {
        tableData.push_back(dataTableStruct);
        //tableData.push_back(move(dataTableStruct));
        //list->push_back(dataTableStruct);
    }
    endResetModel();
}

bool DataEditorTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    DataTableStruct *dataTableStruct = new DataTableStruct();
    //std::unique_ptr<DataTableStruct> dataTableStruct(new DataTableStruct);
    dataTableStruct->date = QDate(1970, 1, 1);
    for (auto headerIterator = tableHeaders.begin(); headerIterator != tableHeaders.end(); headerIterator++) {
        dataTableStruct->data[headerIterator->json_mapping_id] = QVariant(0);
        //first header is the date which we assigned a json_mapping_id to -1
        if (headerIterator->json_mapping_id < 0) {
            continue;
        }
        QSqlQuery insertQuery = QSqlQuery(mdb);
        insertQuery.prepare("INSERT INTO covid_history (json_mappings_id, state_info_id, covid_history_date) "
                            "VALUES (:jmi, :sii, :chd);");
        insertQuery.bindValue(":jmi", headerIterator->json_mapping_id);
        insertQuery.bindValue(":sii", thisStateInfoId);
        insertQuery.bindValue(":chd", QDate(1970,1,1));
        //If it fails, return false and quit; don't push dataTableStruct onto tableData
        if (!insertQuery.exec()) {
            return false;
        }
    }
    beginInsertRows(parent, row, row+count-1);
    tableData.push_back(dataTableStruct);
    endInsertRows();
    return false;
}

void DataEditorTableModel::sort(int column, Qt::SortOrder order) {
    Q_UNUSED(order);
    jsonMappingsId = tableHeaders[column].json_mapping_id;
    //lambda sorting functions, seems verbose...
    //recall that json_mappings_id < 0 when it's the date column
    auto ascendingOrder = [this] (const DataTableStruct *left, const DataTableStruct *right) -> bool
    {
        if (jsonMappingsId >= 0) {
            return left->data[jsonMappingsId] < right->data[jsonMappingsId];
        }
        else {
            return left->date < right->date;
        }
    };

    auto descendingOrder = [this] (const DataTableStruct *left, const DataTableStruct *right) -> bool
    {
        if (jsonMappingsId >= 0) {
            return left->data[jsonMappingsId] > right->data[jsonMappingsId];
        }
        else {
            return left->date > right->date;
        }
    };
    emit layoutAboutToBeChanged();
    if (order == Qt::DescendingOrder) {
        std::sort(tableData.begin(), tableData.end(), ascendingOrder);
    }
    else {
        std::sort(tableData.begin(), tableData.end(), descendingOrder);
    }
    emit layoutChanged();
}

void DataEditorTableModel::getTableHeaders(int dataSourceId) {
    QSqlQuery query = QSqlQuery(mdb);
    query.prepare("SELECT json_mappings_id, json_key, data_type_enum_id, deprecated FROM json_mappings WHERE deprecated=0 AND data_source_id=:dsi ORDER BY json_key");
    query.bindValue(":dsi", dataSourceId);
    query.exec();
    tableHeaders.emplace_back(-1, "Date", -1, 0);
    while (query.next()) {
         tableHeaders.emplace_back(query.value(0).toInt(), query.value(1).toString(), query.value(2).toInt(), query.value(3).toInt());
    }
}

int DataEditorTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return tableData.size();
}

int DataEditorTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return tableHeaders.size();
}

QVariant DataEditorTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) { // || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return tableData[index.row()]->date.toString("yyyy-MM-dd");
        }
        else if (index.column() >= 1) {
            return tableData[index.row()]->getDisplayData(tableHeaders[index.column()].json_mapping_id);
        }
    }
    return QVariant();
}

QVariant DataEditorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return tableHeaders[section].json_key;
    }
    return QVariant();
}

bool DataEditorTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row+count-1);
    /*
     *
    int rowCount;
    for (rowCount = count-1; rowCount >= 0; rowCount--) {
        tableData.remove(rowCount);
    }

    beginRemoveRows(parent, row, row + count - 1);
    int countLeft = count;
    while (countLeft--) {
        const Album& album = *mAlbums->at(row + countLeft);
        mDb.albumDao.removeAlbum(album.id());
    }
    mAlbums->erase(mAlbums->begin() + row,
                  mAlbums->begin() + row + count);
    endRemoveRows();
    */
    tableData.erase(tableData.begin() + row, tableData.begin() + row + count);
    endRemoveRows();
    return true;
}

bool DataEditorTableModel::removeRow(int row, const QModelIndex &parent) {
    Q_UNUSED(row);
    Q_UNUSED(parent);
    return false;
}

void DataEditorTableModel::clearTable() {
    removeRows(0, tableData.size());
    tableHeaders.clear();
}

Qt::ItemFlags DataEditorTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

bool DataEditorTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(role);
    QSqlQuery insertQuery = QSqlQuery(mdb);
    if (index.column() == 0) {
        //updating the date
        insertQuery.prepare("UPDATE covid_history SET covid_history_date=:new_chd WHERE state_info_id=:sii AND covid_history_date=:chd");
        insertQuery.bindValue(":new_chd", value.toDate());
        insertQuery.bindValue(":sii", thisStateInfoId);
        insertQuery.bindValue(":chd", tableData[index.row()]->date);
        if (!insertQuery.exec()) {
            return false;
        }
        tableData[index.row()]->date = value.toDate();
    }
    else {
        insertQuery.prepare("UPDATE covid_history SET string_value=:sv, integer_value=:iv WHERE json_mappings_id=:jmi AND state_info_id=:sii AND covid_history_date=:chd");
        insertQuery.bindValue(":jmi", tableHeaders[index.column()].json_mapping_id);
        insertQuery.bindValue(":sii", thisStateInfoId);
        insertQuery.bindValue(":chd", tableData[index.row()]->date);
        if (tableHeaders[index.column()].data_type_enum_id == 2) {
            insertQuery.bindValue(":sv", QVariant(QVariant::String));
            insertQuery.bindValue(":iv", value.toInt());
        }
        else {
            insertQuery.bindValue(":sv", value.toString());
            insertQuery.bindValue(":iv", QVariant(QVariant::Int));
        }
        if (!insertQuery.exec()) {
            return false;
        }
        tableData[index.row()]->data[tableHeaders[index.column()].json_mapping_id] = value;
    }
    return true;
}

QSqlDatabase DataEditorTableModel::lendDatabase() {
    return mdb;
}
