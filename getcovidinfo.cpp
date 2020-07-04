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
    QSqlQuery query(mdb);
    QDate iterDate = QDate(2020, 3, 1);
    while (iterDate < QDate::currentDate()) {
        query.prepare("SELECT DISTINCT(state_info_id), state_abbreviation FROM state_info WHERE "
                        "state_info_id NOT IN ("
                        "SELECT state_info_id FROM covid_history "
                        "WHERE covid_history_date = :chd)");
        query.bindValue(":chd", iterDate.toString("yyyy-MM-dd"));
        query.exec();
        while (query.next()) {
            if (!query.value(0).isNull() && !query.value(1).isNull()) {
                stateInfoVector.emplace_back(query.value(1).toString(), query.value(0).toInt(), iterDate);
            }
        }
        iterDate = iterDate.addDays(1);
    }
    //now get the last three days; sometimes the data is updated a day or two late
    iterDate = QDate::currentDate().addDays(-3);
    query.exec("SELECT state_info_id, state_abbreviation FROM state_info");
    while (query.next()) {
        iterDate = QDate::currentDate().addDays(-3);
        while (iterDate < QDate::currentDate()) {
            stateInfoVector.emplace_back(query.value(1).toString(), query.value(0).toInt(), iterDate);
            iterDate = iterDate.addDays(1);
        }
    }
    emit stateInfoVectorsReady(stateInfoVector);
    emit workFinished();
}


