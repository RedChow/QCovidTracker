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
    const QString hostName = "";
    const QString databaseName = "";
    const QString username = "";
    const QString password = "";
};

#endif // DATABASEINTERFACE_H
