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
#include "plotmodel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

#include <memory>

PlotModel::PlotModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    dbi = new DatabaseInterface("PlotModel");
    mdb = dbi->getDatabase();
}

PlotModel::~PlotModel() {
    delete dbi;
}

void PlotModel::setStateId(int p_StateId) {
    stateId = p_StateId;
}

void PlotModel::setDays(int p_days) {
    days = p_days;
}

void PlotModel::setHeaders(QStringList p_Headers) {
    headers = p_Headers;
}

void PlotModel::populateData() {
    beginResetModel();
    QString query_str = "(SELECT json_mappings_id, json_key FROM json_mappings WHERE json_key='positive')"
                        "UNION"
                        "(SELECT json_mappings_id, json_key FROM json_mappings WHERE json_key='negative')"
                        "ORDER BY json_key ASC";
    QSqlQuery query(query_str, mdb);
    QSqlQuery queryNeg(mdb);
    QSqlQuery megaQuery(mdb);
    //TODO: Add error checking here and escape route if positive and negative do not exist
    query.next();
    int negative_id = query.value(0).toInt();
    query.next();
    int positive_id = query.value(0).toInt();

    megaQuery.prepare("SELECT a.covid_history_date, a.integer_value AS positive, b.integer_value AS negative FROM "
        "(SELECT integer_value, covid_history_date "
        "FROM covid_history "
        "WHERE json_mappings_id = :jmiPos "
        "AND state_info_id = :sii) AS a "
        "FULL OUTER JOIN "
        "(SELECT integer_value, covid_history_date "
        "FROM covid_history "
        "WHERE json_mappings_id = :jmiNeg "
        "AND state_info_id = :sii) AS b "
        "ON a.covid_history_date = b.covid_history_date "
        "ORDER BY covid_history_date");
    megaQuery.bindValue(":jmiPos", positive_id);
    megaQuery.bindValue(":jmiNeg", negative_id);
    megaQuery.bindValue(":sii", stateId);
    megaQuery.exec();
    QVector<int> positiveVector(megaQuery.size());
    QVector<int> negativeVector(megaQuery.size());
    int vectorPosition{0};
    int pos{0};
    int neg{0};
    while (megaQuery.next()) {
        tableQDates.push_back(megaQuery.value(0).toDate());
        pos = megaQuery.value(1).toInt();
        if (megaQuery.isNull(1)) {
             pos = 0;
        }
        neg = megaQuery.value(2).toInt();
        if (megaQuery.isNull(2)) {
            neg = 0;
        }
        positiveVector[vectorPosition] = pos;
        negativeVector[vectorPosition] = pos + neg;
        vectorPosition++;
    }

    tableData.push_back(positiveVector);
    tableData.push_back(negativeVector);
    endResetModel();
}

int PlotModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return tableQDates.size();
}

bool PlotModel::removeRows(int row, int count, const QModelIndex &parent) {
    beginRemoveRows(parent, row, row+count-1);
    int rowCount;
    for (rowCount = count-1; rowCount >= 0; rowCount--) {
        tableQDates.remove(rowCount);
    }
    endRemoveRows();
    for (rowCount = tableData.size() - 1; rowCount >= 0; rowCount--) {
        tableData.remove(rowCount);
    }
    return true;
}

void PlotModel::clearTable() {
    removeRows(0, tableQDates.size());
}

int PlotModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    //TODO: Add dynamic headers
    return 13;
}

QVariant PlotModel::headerData(int section, Qt::Orientation orientation, int role) const {
    //TODO: Add dnymaic headers
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return QString("Date");
            case 1:
                return QString("Positive");
            case 2:
                return QString("Pos Diff");
            case 3:
                return QString("Pos Cum Sum");
            case 4:
                return QString("Pos Mov Avg");
            case 5:
                return QString("Slope of Pos Diff");
            case 6:
                return QString("Slope of Pos Mov Avg");
            case 7:
                return QString("Tests (Pos+Neg)");
            case 8:
                return QString("Tests Diff");
            case 9:
                return QString("Tests Cum Sum");
            case 10:
                return QString("Test Mov Avg");
            case 11:
                return QString("Slope of Tests");
            case 12:
                return QString("Slope of Test Mov Avg");
            default:
                return QString("Pos Mov Avg");
        }
    }
    return QVariant();
}

/*
 * QDate
 * 1(0)         2                    3               4                       5                6
 * positive, positiveDifferences, positiveCumSum, positiveMovingAverage, slope of pos diff, slope of moving avg
 * 7(1)         8                    9
 * tests: positive + negative, negativeDifferences, negativeCumSum, negativeMovingAverage, slopes(?)
 * 13(2)
 * host
*/
QVariant PlotModel::data(const QModelIndex &index, int role ) const {
    int columnIndex = (index.column()-1)/6;
    int columnModded = (index.column() - 1) % 6;
     if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        return tableQDates[index.row()];
    }
    else if (columnModded == 0) {
        //negative will be vector column 1, but column in table will be 7 (7-1) % 6 = 0
        return tableData[columnIndex][index.row()];
    }
    else if (columnModded == 1) { //index.column() == 2, 8
        if (index.row() >= 1) {
            return tableData[columnIndex][index.row()] - tableData[columnIndex][index.row()-1];
        }
        return 0;
    }
    else if (columnModded == 2) { //index.column() == 3, 9

        //s_m = (N_1 - N_0) + (N_2 - N_1) + ... + (N_{m-1} - N_{m-2}) + (N_m - N_{m-1}) =
        //      N_m - N_0
         /*  moving_average 	= (s_m - s_{m-days})/days
         *  				= ((N_m - N_0) - (N_{m-days} - N_0)) / days
         *      			= (N_m - N_0 - N_{m-days} + N_0 )    / days
         * 			   		= (N_m - N_{m-days})/days
         */
        return tableData[columnIndex][index.row()] - tableData[index.column()/6][0];
    }
    else if (columnModded == 3) { //index.column() == 4, 10
        if (index.row() < days || days <= 2) {
            if (index.row() > 0) {
                return tableData[columnIndex][index.row()] - tableData[columnIndex][index.row()-1];
            }
            return 0;
        }
        else {
            return float(tableData[columnIndex][index.row()] - tableData[columnIndex][index.row()-days])/float(days);
        }
    }
    else if (columnModded == 4) {
        //slope of positive diff
        if (index.row() < days + 1 || days <= 2) {
            return 0;
        }
        else {
             float num = float(tableData[columnIndex][index.row()] - tableData[columnIndex][index.row()-1] - tableData[columnIndex][index.row()-days] + tableData[columnIndex][index.row()-days-1]);
             return num/float(days);
        }
    }
    else if (columnModded == 5) {
        //slope of moving average
        if (index.row() < 2*days || days <= 2) {
            return 0;
        }
        else {
            float numPartOne = float(tableData[columnIndex][index.row()] - tableData[columnIndex][index.row()-days])/float(days);
            float numPartTwo = float(tableData[columnIndex][index.row()-days] - tableData[columnIndex][index.row()-2*days])/float(days);
            return float(numPartOne-numPartTwo)/float(days);
        }
    }
    return QVariant();
}

