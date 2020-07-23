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

#ifndef CUSTOMLINESERIES_H
#define CUSTOMLINESERIES_H

#include <QObject>
#include <QtCharts>

//TODO: Make a better class name? ...lol
class CustomLineSeries : public QLineSeries
{
    Q_OBJECT
public:
    CustomLineSeries();
    ~CustomLineSeries();
    qreal getYValue(qreal xValue);
protected:
    void showHovered(QPointF pointF, bool state);
private:
    QGraphicsTextItem *m_coordX;
    QHash<QString, qreal> dateToQReal;
signals:
    void setToolTip(QPointF pointF, bool state, QAbstractSeries *abstractSeries);
};

#endif // CUSTOMLINESERIES_H
