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
    QString threadDBName = "";

    QSqlDatabase* mDatabase;
    const QString hostName = "localhost";
    const QString databaseName = "coviddata";
    const QString username = "postgres";
    const QString password = "h8Aw72eZhun86pg";
};

#endif // DATABASEINTERFACE_H
