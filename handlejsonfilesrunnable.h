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
#ifndef HANDLEJSONFILESRUNNABLE_H
#define HANDLEJSONFILESRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QStringList>
#include <QSqlDatabase>
#include "databaseinterface.h"

class HandleJsonFilesRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    HandleJsonFilesRunnable();
    ~HandleJsonFilesRunnable();
    void run() override;
    QString jsonDirectory;
    void setJsonDirectory(QString);
    QString dbName;
    void setDbName(QString);
    QStringList jsonFileList;
    void setJsonFileList(QStringList);
    QString fileName;
    void setFileName(QString);
    int beginning;
    int end;
    void setBeginningAndEnd(int, int);
    QSqlDatabase db();
signals:
    void sendStatusMessage(QString);
private:
    DatabaseInterface *dbi;
};

#endif // HANDLEJSONFILESRUNNABLE_H
