#ifndef GETCOVIDINFO_H
#define GETCOVIDINFO_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>
#include <vector>
#include "databaseinterface.h"
#include "statestruct.h"


class GetCovidInfo : public QThread
{
    Q_OBJECT
public:
    GetCovidInfo();
    std::vector<StateInfo> stateInfoVector;
    std::vector<StateInfo> stateInfoVectorBatch;
signals:
    void stateInfoVectorsReady(std::vector<StateInfo> stateInfo);
    void workFinished();
public slots:
    void getMissingData();
private:
    DatabaseInterface *dbi;
};

#endif // GETCOVIDINFO_H
