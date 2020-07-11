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
#ifndef MAINCHARTVIEW_H
#define MAINCHARTVIEW_H

#include <QObject>
#include <QtCharts>
#include <QChartView>
#include "mainchart.h"
#include "customlineseries.h"

/*
 *	Thinking of moving adding series and options from mainwindow to this one
 */

class MainChartView : public QChartView
{
    Q_OBJECT
public:
    MainChartView();
    MainChartView(QWidget *widget = nullptr);
    void contextMenuEvent(QContextMenuEvent *event) override;
    void setMainChart(MainChart *m);
    void traceChart();
    void untraceChart();
    qreal getYValue(qreal xValue, QAbstractSeries *seriesPointVector);
    void addCustomLineSeries(CustomLineSeries *customLine);
    void clearCustomLineSeriesVector();
protected:
    bool viewportEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
private:
    MainChart *m_chart;
    QVector<QGraphicsTextItem*> graphicsTextItemVector;
    QGraphicsTextItem *m_coordX; //This is for the hover tooltip when tracing is not enabled
    bool tracePlot;
    int mouseX;
    int mouseY;
    qreal xTrace;
    QLineSeries *verticalLine;
    QVector<CustomLineSeries*> customLineSeriesVector; //holds toolstips for all series added to chart
public slots:
    void showThatToolTip(QPointF pointF, bool state, QAbstractSeries *abstractSeries);
};

#endif // MAINCHARTVIEW_H
