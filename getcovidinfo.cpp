/*
 * Copyright (C) 2020 brian DOT l DOT miller DOT ttu AT gmail DOT com
 * This file is part of QCovidTracker.
 *
 * QThis program comes with ABSOLUTELY NO WARRANTY
 * CovidTracker is free software: you can redistribute it and/or modify
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
#include "getcovidinfo.h"
#include <QDebug>
#include <QVariant>
#include <QDate>
#include <QSqlError>

GetCovidInfo::GetCovidInfo()
{

}

void GetCovidInfo::getMissingData() {
    dbi = new DatabaseInterface("GetCovidInfo:MissingDataEdition");
    QSqlDatabase mdb = dbi->getDatabase();
    QSqlQuery covidProjectDS(mdb);
    covidProjectDS.prepare("SELECT data_source_id FROM data_source WHERE data_source_name=:ctp");
    covidProjectDS.bindValue(":ctp", "covid_tracking_project");
    covidProjectDS.exec();
    if (!covidProjectDS.next()) {
        return;
    }
    int ctp_id = covidProjectDS.value(0).toInt();

    QSqlQuery query(mdb);
    query.prepare("SELECT a.state_info_id, b.m, a.state_abbreviation FROM "
        "(SELECT state_info_id, state_abbreviation FROM state_info WHERE data_source_id = :ctid) AS a "
        "FULL OUTER JOIN "
        "(SELECT state_info_id, MAX(covid_history_date) AS m "
        "FROM covid_history "
        "GROUP BY state_info_id) AS b "
        "ON a.state_info_id = b.state_info_id ");
    query.bindValue(":ctid", ctp_id);
    QDate iterDate = QDate(2020, 3, 1);
    QDate threeDaysAgo = QDate::currentDate().addDays(-3);
    query.exec();
    while (query.next()) {
        iterDate = QDate(2020, 3, 1);
        if (!query.value(1).isNull()) {
            iterDate = query.value(1).toDate();
        }
        if (iterDate > threeDaysAgo) {
            iterDate = threeDaysAgo;
        }
        while (iterDate < QDate::currentDate()) {
            stateInfoVector.emplace_back(query.value(2).toString(), query.value(0).toInt(), iterDate);
            iterDate = iterDate.addDays(1);
        }
    }

    emit stateInfoVectorsReady(stateInfoVector);
    emit workFinished();
}


