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
