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
