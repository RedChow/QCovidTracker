#ifndef DATABASEINTERFACE_H
#define DATABASEINTERFACE_H
#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseInterface
{
public:
    DatabaseInterface(QString p_dbName = "default");
    DatabaseInterface(int);
    ~DatabaseInterface();
    QSqlDatabase getDatabase();
    void setThreadDBName(QString);
    QSqlDatabase getThreadDB();
    int threadID;
private:
    const QString dbName = "default";
    QString threadDBName;

    QSqlDatabase* mDatabase;
    const QString hostName = "localhost";
    const QString databaseName = "coviddata";
    const QString username = "postgres";
    const QString password = "h8Aw72eZhun86pg";
};

#endif // DATABASEINTERFACE_H
