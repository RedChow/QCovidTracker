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
#include "chartbuilder.h"
#include <QSqlQuery>
#include <QDebug>


ChartBuilder::ChartBuilder(QSqlDatabase &p_mdb) :
    mdb(p_mdb)
{
    //chart = new MainChart();
    xAxisType = QAbstractAxis::AxisTypeDateTime;
}

ChartBuilder::~ChartBuilder() {

}

MainChart* ChartBuilder::getChart() {
    MainChart* chart = new MainChart();
    //add axes to charts and set map
    QBarCategoryAxis *barCategoryAxis = new QBarCategoryAxis();
    QDateTimeAxis *dateTimeAxis = new QDateTimeAxis;
    if (xAxisType == QAbstractAxis::AxisTypeDateTime) {
        delete barCategoryAxis;
        chart->addAxis(dateTimeAxis, Qt::AlignBottom);
    }
    else if (xAxisType == QAbstractAxis::AxisTypeBarCategory) {
        delete dateTimeAxis;

        QStringList categories;
        auto it = sdm.constBegin();
        while (it != sdm.constEnd()) {
            categories << it.key().toString("MM/dd");
            it++;
        }
        barCategoryAxis->append(categories);
        chart->addAxis(barCategoryAxis, Qt::AlignBottom);
    }

    for (int i = 0; i < seriesInfo.size(); i++) {
        QValueAxis *axisY = new QValueAxis();
        chart->addAxis(axisY, (Qt::Alignment) seriesInfo.at(i).chartAlignment);
        //if seriesInfo.yAxisId has not been added, we insert a new (key, value) into the hash
        if (!axisIdToAxis.contains(seriesInfo.at(i).yAxisId)) {
            axisIdToAxis.insert(seriesInfo.at(i).yAxisId, axisY);
        }
        if (seriesInfo.at(i).chartType == BarChart) {
            if (xAxisType == QAbstractAxis::AxisTypeDateTime) {
                attachBarSeries(chart, seriesInfo.at(i).yAxisId, seriesInfo.at(i).jsonMappingId, dateTimeAxis, i);
            }
            else {
                attachBarSeries(chart, seriesInfo.at(i).yAxisId, seriesInfo.at(i).jsonMappingId, barCategoryAxis, i);
            }
        }
        else if (seriesInfo.at(i).chartType == LineChart) {
            if (xAxisType == QAbstractAxis::AxisTypeDateTime) {
                attachLineSeries(chart, seriesInfo.at(i).yAxisId, seriesInfo.at(i).jsonMappingId, dateTimeAxis, i);
            }
            else {
                attachLineSeries(chart, seriesInfo.at(i).yAxisId, seriesInfo.at(i).jsonMappingId, barCategoryAxis, i);
            }
        }
    }
    //TODO: make custom legend
    return chart;
}

void ChartBuilder::setBarCategoryAxis() {
    QStringList categories;
    auto it = sdm.constBegin();
    while (it != sdm.constEnd()) {
        categories << it.key().toString("MM/dd");
        it++;
    }
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
}

void ChartBuilder::attachBarSeries(MainChart*& chart, int axisId, int jsonMappingsId, QAbstractAxis *axis, int index) {
    QBarSeries *barSeries = new QBarSeries();
    barSeries->setName(seriesInfo.at(index).seriesName);;
    makeBarSets(barSeries, jsonMappingsId, index);
    chart->addSeries(barSeries);
    barSeries->attachAxis(axisIdToAxis[axisId]);
    barSeries->attachAxis(axis);
}

void ChartBuilder::attachLineSeries(MainChart*& chart, int axisId, int jsonMappingsId, QAbstractAxis *axis, int index) {
    /*
     * 	TODO: the below code works, but could probably be more efficient
     */
    CustomLineSeries *series = new CustomLineSeries();
    if (seriesInfo.at(index).showDifferences || seriesInfo.at(index).movingAverageDays > 2) {
        std::vector<int> temp;
        std::vector<qint64> dateTime;
        auto it = sdm.constBegin();
        while (it != sdm.constEnd()) {
            temp.push_back(it.value().data[jsonMappingsId]);
            dateTime.push_back(QDateTime(it.key()).toMSecsSinceEpoch());
            it++;
        }
        std::unique_ptr<std::vector<int>> diff = algorithms::differences(temp);
        if (seriesInfo.at(index).movingAverageDays <= 2) {
            for(std::size_t i = 0; i < diff->size(); ++i) {
                if (axis->type() == QAbstractAxis::AxisTypeDateTime) {
                    series->append(dateTime.at(i), diff->at(i));
                }
                else {
                    series->append(i, diff->at(i));
                }
            }
        }
        else {
            std::unique_ptr<std::vector<double>> movingAvg = algorithms::movingAverage(diff, seriesInfo.at(index).movingAverageDays);
            for (std::size_t i = 0; i < movingAvg->size(); ++i) {
                if (axis->type() == QAbstractAxis::AxisTypeDateTime) {
                    series->append(dateTime.at(i), movingAvg->at(i));
                }
                else {
                    series->append(i, movingAvg->at(i));
                }
            }
        }
    }
    else {
        auto it = sdm.constBegin();
        int count{0};
        while (it != sdm.constEnd()) {
            if (axis->type() == QAbstractAxis::AxisTypeDateTime) {
                series->append(QDateTime(it.key()).toMSecsSinceEpoch(), it.value().data[jsonMappingsId]);
            }
            else {
                series->append(count, it.value().data[jsonMappingsId]);
            }
            it++;
            count++;
        }
    }
    chart->addSeries(series);
    series->setName(seriesInfo.at(index).seriesName);
    series->attachAxis(axisIdToAxis[axisId]);
    series->attachAxis(axis);
    QPen seriesPen;
    seriesPen.setColor(QColor(seriesInfo.at(index).penColor));
    seriesPen.setStyle((Qt::PenStyle)seriesInfo.at(index).penType);
    series->setPen(seriesPen);
}

void ChartBuilder::makeBarSets(QBarSeries*& series, int jsonMappingsId, int index) {
    QBarSet *set = new QBarSet(seriesInfo.at(index).seriesName);
    auto it = sdm.constBegin();
    if (seriesInfo.at(index).showDifferences) {
        std::vector<int> temp;
        while (it != sdm.constEnd()) {
            temp.push_back(it.value().data[jsonMappingsId]);
            it++;
        }
        std::unique_ptr<std::vector<int>> diff = algorithms::differences(temp);
        for(std::size_t i = 0; i < diff->size(); ++i) {
            *set << diff->at(i);
        }
    }
    else {
        while (it != sdm.constEnd()) {
            *set << it.value().data[jsonMappingsId];
            it++;
        }
    }
    set->setColor(seriesInfo.at(index).penColor);
    series->append(set);
}

void ChartBuilder::addPlot(int stateInfoId, int jsonMappingsId, int chartType, int axisId, int chartAlignment, QString seriesName, bool showDiff, QString penColor, int penWidth, int penType, int mAD) {
    addData(stateInfoId, jsonMappingsId);
    SeriesInfo thisSeriesInfo = SeriesInfo(stateInfoId, jsonMappingsId, chartType, axisId, chartAlignment, seriesName, showDiff, penColor, penWidth, penType, mAD);
    seriesInfo.push_back(thisSeriesInfo);
    if (chartType == BarChart) {
        xAxisType = QAbstractAxis::AxisTypeBarCategory;
    }
}

void ChartBuilder::addData(int stateInfoId, int jsonMappingId) {
    //we wait until all series are added before constructing vector<int>; this
    //ensures that they will all have the same number of elements
    QSqlQuery query = QSqlQuery(mdb);
    query.prepare("SELECT covid_history_date, integer_value FROM covid_history WHERE json_mappings_id=:jmi AND state_info_id=:sii");
    query.bindValue(":jmi", jsonMappingId);
    query.bindValue(":sii", stateInfoId);
    if (!query.exec()) {
        return;
    }
    while (query.next()) {
        //if the date isn't valid, there's no way to tie a value to it and insert it into the map, so skip it
        SeriesData sd;
        if (!query.value(0).toDate().isValid()) {
            continue;
        }
        if (sdm.contains(query.value(0).toDate())) {
            sd = sdm[query.value(0).toDate()];
        }
        sd.data[jsonMappingId] = query.value(1).toInt();
        sdm[query.value(0).toDate()] = sd;
    }
}

void ChartBuilder::printData() {
    auto i = sdm.constBegin();
    while (i != sdm.constEnd()) {
        qDebug() << i.key(); //i.value();
        auto j = i.value().data.constBegin();
        while (j != i.value().data.constEnd()) {
            qDebug() << " " << j.key() << j.value();
            ++j;
        }
        ++i;
    }
}

void ChartBuilder::clearBuilder() {
    sdm.clear();
    seriesInfo.clear();
    if (axisIdToAxis.size() > 0) {
        qDeleteAll(axisIdToAxis);
    }
    axisIdToAxis.clear();
    //reset
    xAxisType = QAbstractAxis::AxisTypeDateTime;
}



