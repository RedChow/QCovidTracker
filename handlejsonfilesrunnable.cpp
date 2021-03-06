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


#include "handlejsonfilesrunnable.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QThread>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDate>
#include <memory>
#include <QHash>
#include <QVector>

HandleJsonFilesRunnable::HandleJsonFilesRunnable()
{

}

HandleJsonFilesRunnable::~HandleJsonFilesRunnable() {

}

QSqlDatabase HandleJsonFilesRunnable::db() {
    QString threadDBName = QString::number((qintptr)QThread::currentThreadId());

    dbi = new DatabaseInterface(0);
    dbi->setThreadDBName(threadDBName);
    QSqlDatabase mdb = dbi->getThreadDB();
    return mdb;
}

void HandleJsonFilesRunnable::setJsonDirectory(QString dir) {
    jsonDirectory = dir;
}

void HandleJsonFilesRunnable::setDbName(QString p_dbName) {
    dbName = p_dbName;
}

void HandleJsonFilesRunnable::setJsonFileList(QStringList jsonFiles) {
    jsonFileList = jsonFiles;
}

void HandleJsonFilesRunnable::setFileName(QString p_FileName) {
    fileName = p_FileName;
}

void HandleJsonFilesRunnable::setBeginningAndEnd(int b, int e) {
    beginning = b;
    end = e;
}

void HandleJsonFilesRunnable::run() {
    int start{0};
    QJsonObject json;
    int stateId{-1};
    QSqlQuery query(db());
    QSqlQuery insertQuery(db());
    //get covid_tracking_project json keys
    QSqlQuery covidProjectDS(db());
    covidProjectDS.prepare("SELECT data_source_id FROM data_source WHERE data_source_name=:ctp");
    covidProjectDS.bindValue(":ctp", "covid_tracking_project");
    covidProjectDS.exec();
    if (!covidProjectDS.next()) {
        return;
    }
    int ctp_id = covidProjectDS.value(0).toInt();

    int data_type_enum_id{1};
    int json_mapping_id{-1};
    QDate d_date;
    QHash<QString, QVector<int>> jsonMappingsHash;
    QHash<QString, QVector<int>>::iterator jsonMappingsHashIterator;
    query.prepare("SELECT json_key, json_mappings_id, data_type_enum_id FROM json_mappings WHERE data_source_id=:dsi");
    query.bindValue(":dsi", ctp_id);
    query.exec();
    while (query.next()) {
        QVector<int> qVector;
        qVector.push_back(query.value(1).toInt());
        qVector.push_back(query.value(2).toInt());
        jsonMappingsHash.insert(query.value(0).toString(), qVector);
    }
    query.prepare("SELECT json_key FROM json_mappings WHERE deprecated=:dep AND data_source_id=:dsi");
    query.bindValue(":dep", 1);
    query.bindValue(":dsi", ctp_id);
    query.exec();
    QStringList deprecated;
    while (query.next()) {
        deprecated << query.value(0).toString();
    }
    //QDate batchDate = QDate::currentDate().addDays(-3);
    /*
     * 	TODO:
     * 		Add INSERT INTO covid_history (json_mappings_id, state_info_id, string_value, integer_value, covid_history_date) VALUES
     * 			(jmi_0, sii_0, sv_0, iv_0, chd_0),
     * 			(jmi_1, sii_1, sv_1, iv_1, chd_1),
     * 			...
     * 			(smi_n, sii_n, sv_n, iv_n, chd_n)
     */
    for (start = beginning; start < end; start++) {
        QFile jsonFile(jsonDirectory + jsonFileList[start]);
        jsonFile.open(QFile::ReadOnly);
        if (!jsonFile.isOpen()) {
            continue;
        }

        json = QJsonDocument().fromJson(jsonFile.readAll()).object();
        if (!json["error"].isNull()) {
            if (json["error"].toBool()) {
                continue;
            }
        }
        stateId = json["stateId"].toInt();
        d_date = QDate::fromString(QString::number(json["date"].toInt()), "yyyyMMdd");
        for (auto key: json.keys()) {
            if (deprecated.contains(key, Qt::CaseInsensitive)) {
                continue;
            }
            data_type_enum_id = 1;
            jsonMappingsHashIterator = jsonMappingsHash.find(key);
            if (jsonMappingsHashIterator == jsonMappingsHash.end()) {
                if (!json.value(key).isString()) {
                    data_type_enum_id = 2;
                }
                insertQuery.prepare("INSERT INTO json_mappings (json_key, data_type_enum_id, data_source_id) VALUES (:jk, :dtei, :dsi) RETURNING json_mappings_id;");
                insertQuery.bindValue(":jk", key);
                insertQuery.bindValue(":dtei", data_type_enum_id);
                insertQuery.bindValue(":dsi", ctp_id);
                insertQuery.exec();
                if(insertQuery.next()) {
                    json_mapping_id = insertQuery.value(0).toInt();
                }
                else {
                    query.prepare("SELECT json_mappings_id, data_type_enum_id FROM json_mappings WHERE json_key=:jk AND data_source_id=:dsi");
                    query.bindValue(":jk", key);
                    query.bindValue(":dsi", ctp_id);
                    query.exec();
                    query.next();
                    json_mapping_id = query.value(0).toInt();
                    data_type_enum_id = query.value(1).toInt();
                }
                QVector<int> qVector;
                qVector.push_back(json_mapping_id);
                qVector.push_back(data_type_enum_id);
                jsonMappingsHash.insert(key, qVector);
            }
            else {
                json_mapping_id = jsonMappingsHash[key][0];
                data_type_enum_id = jsonMappingsHash[key][1];
            }
            if (json_mapping_id > -1) {
                    insertQuery.prepare("INSERT INTO covid_history (json_mappings_id, state_info_id, string_value, integer_value, covid_history_date) VALUES (:jmi, :sii, :sv, :iv, :chd)");
                    insertQuery.bindValue(":jmi", json_mapping_id);
                    insertQuery.bindValue(":sii", stateId);
                    //TODO: This snippet checking for data_type_enum appears often; refactor in future
                    if (data_type_enum_id == 2) {
                        insertQuery.bindValue(":sv", QVariant(QVariant::String));
                        insertQuery.bindValue(":iv", json[key].toInt());
                    }
                    else {
                        insertQuery.bindValue(":sv", json[key].toString());
                        insertQuery.bindValue(":iv", QVariant(QVariant::Int));
                    }
                    insertQuery.bindValue(":chd", d_date.toString("yyyy-MM-dd"));
                    if (!insertQuery.exec()) {
                        //add update
                        QSqlQuery updateQuery(db());
                        if (data_type_enum_id == 2) {
                            updateQuery.prepare("UPDATE covid_history SET integer_value=:iv WHERE state_info_id=:sii AND json_mappings_id=:jmi AND covid_history_date=:chd");
                            updateQuery.bindValue(":iv", json[key].toInt());
                        }
                        else {
                            updateQuery.prepare("UPDATE covid_history SET string_value=:sv WHERE state_info_id=:sii AND json_mappings_id=:jmi AND covid_history_date=:chd");
                            updateQuery.bindValue(":sv", json[key].toString());
                        }
                        updateQuery.bindValue(":sii", stateId);
                        updateQuery.bindValue(":jmi", json_mapping_id);
                        updateQuery.bindValue(":chd", d_date.toString("yyyy-MM-dd"));
                        updateQuery.exec();
                    }
                }
            }

            if (!jsonFile.rename(jsonDirectory + "process_files/" + jsonFileList[start])) {
                QFile tempFile(jsonDirectory + "process_files/" + jsonFileList[start]);
                tempFile.remove();
                jsonFile.rename(jsonDirectory + "process_files/" + jsonFileList[start]);
                //TODO: add check for removing jsonFile
            }
            emit sendStatusMessage(jsonFileList[start]);
    }
}
