#include "databaseinterface.h"
#include <QDebug>
#include <QThread>

DatabaseInterface::DatabaseInterface(QString p_dbName) :
    dbName(p_dbName),
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL", dbName)))
{
    mDatabase->setHostName(this->hostName);
    mDatabase->setDatabaseName(this->databaseName);
    mDatabase->setUserName(this->username);
    mDatabase->setPassword(this->password);
}

DatabaseInterface::DatabaseInterface(int t) : threadID(t) {

}

QSqlDatabase DatabaseInterface::getThreadDB() {
    QSqlDatabase mdb = QSqlDatabase::database(threadDBName);
    if (!mdb.isOpen()) {
        mdb = QSqlDatabase::addDatabase("QPSQL", threadDBName);
        mdb.setHostName(this->hostName);
        mdb.setDatabaseName(this->databaseName);
        mdb.setUserName(this->username);
        mdb.setPassword(this->password);
        mdb.open();
    }
    return mdb;
}

void DatabaseInterface::setThreadDBName(QString p_ThreadDBName) {
    threadDBName = p_ThreadDBName;
}

QSqlDatabase DatabaseInterface::getDatabase() {
    mDatabase->open();
    return *mDatabase;
}

DatabaseInterface::~DatabaseInterface() {
    mDatabase->close();
}
