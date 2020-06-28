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


