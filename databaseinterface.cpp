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

DatabaseInterface::DatabaseInterface(int t) :
    threadID(t)
{

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
