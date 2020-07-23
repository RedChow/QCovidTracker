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
#ifndef CHARTBUILDER_H
#define CHARTBUILDER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QAbstractAxis>
#include <memory>

#include "mainchart.h"
#include "algorithms.h"
#include "customlineseries.h"

struct SeriesData {
    //json_mappings_id, value
    QHash<int, int> data;
    QHash<QPair<int, int>, int> pairedData;

    QVariant getData(int key) {
        if (data.contains(key)) {
            return data[key];
        }
        return 0;
    }
};

struct SeriesInfo {
    int stateInfoId;
    int jsonMappingId;
    int chartType;
    int yAxisId{0};
    int chartAlignment;
    bool showDifferences{false};
    int movingAverageDays{0};
    QString seriesName;
    QString penColor{"#0000FF"};
    int penWidth{1};
    int penType;

    SeriesInfo(int sII, int jMI, int cT, int aI, int cA, QString sN, bool sD, QString pC, int pW, int pT, int mAD) :
        stateInfoId(sII),
        jsonMappingId(jMI),
        chartType(cT),
        yAxisId(aI),
        chartAlignment(cA),
        showDifferences(sD),
        movingAverageDays(mAD),
        seriesName(sN),
        penColor(pC),
        penWidth(pW),
        penType(pT)
    {

    }
};

class ChartBuilder : public QObject
{
    Q_OBJECT
public:
    enum ChartTypes {
        BarChart,
        LineChart
    };

    //TODO: remove enum ChartAlignment and just use Qt::AlignLeft/Right
    enum ChartAlignment {
        left,
        right
    };

    ChartBuilder(QSqlDatabase &p_mdb);
    ~ChartBuilder();
    void addData(int stateInfoId, int jsonMappingId);
    void printData();
    MainChart* getChart();
    //TODO add QString name parameter
    //SeriesInfo(int sII, int jMI, int cT, int aI, int cA, QString sN, bool sD, QString pC, int pW, int pT, int mAD) :
    void addPlot(int stateInfoId, int jsonMappingsId, int chartType, int axisId, int chartAlignment, QString seriesName, bool showDiff, QString penColor, int penWidth, int penType, int mAD);
    void makeBarSets(QBarSeries*& series, int jsonMappingsId, int index);
    void attachBarSeries(MainChart*& chart, int axisId, int jsonMappingsId, QAbstractAxis *axis, int index);
    void attachLineSeries(MainChart*& chart, int axisId, int jsonMappingsId, QAbstractAxis *axis, int index);
    void setBarCategoryAxis();

    void clearBuilder();
    //QVector<QAbstractAxis*> xAxis;
private:
    QSqlDatabase& mdb;
    QMap<QDate, SeriesData> sdm;
    QVector<SeriesInfo> seriesInfo;
    //axisId, QValueAxis
    QHash<int, QValueAxis*> axisIdToAxis;
    int xAxisType;
};

#endif // CHARTBUILDER_H
