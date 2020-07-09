#include "jsontablemodel.h"
#include <QDebug>

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
    //qDebug() << index.row() << index.column();
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}














