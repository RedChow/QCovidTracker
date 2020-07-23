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
#include "customlineseries.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrentMap>
#include <QDateTime>

#include <math.h>

CustomLineSeries::CustomLineSeries()
{
    dateToQReal.clear();
    connect(this, &QLineSeries::hovered, this, &CustomLineSeries::showHovered);
}

CustomLineSeries::~CustomLineSeries() {

}

void CustomLineSeries::showHovered(QPointF pointF, bool state) {
    QPen pen = this->pen();
    if (state) {
        pen.setWidth(3);
        this->setPen(pen);
    }
    else {
        pen.setWidth(1);
        this->setPen(pen);
    }
    emit setToolTip(pointF, state, this);
}

qreal CustomLineSeries::getYValue(qreal xValue) {
    //build the first time, keep for remaining queries
    if (dateToQReal.size() == 0) {
        //fill the vector
        for (auto point: this->pointsVector()) {
            dateToQReal[QDateTime::fromTime_t(point.x()/1000).toString("yyyy-MM-dd")] = point.y();
        }
    }
    QString findX = QDateTime::fromTime_t(xValue).toString("yyyy-MM-dd");
    QHash<QString, qreal>::const_iterator iterator = dateToQReal.find(findX);
    if (iterator != dateToQReal.end()) {
        return dateToQReal[findX];
    }
    return -1;
}
