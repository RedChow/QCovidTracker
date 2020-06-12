#ifndef STATESTRUCT_H
#define STATESTRUCT_H
#include <QString>
#include <QDate>
#include <vector>

struct StateInfo {
    QString stateAbbreviation;
    int stateId;
    QDate date;
    StateInfo(QString p_stateAbbreviation, int p_stateId, QDate p_date) : stateAbbreviation(p_stateAbbreviation), stateId(p_stateId), date(p_date) {}
};

#endif // STATESTRUCT_H
