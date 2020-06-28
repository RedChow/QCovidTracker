#ifndef MAINCHART_H
#define MAINCHART_H

#include <QObject>
#include <QtCharts>
#include <QChart>

class MainChart : public QChart
{
    Q_OBJECT
public:
    explicit MainChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
protected:
    bool sceneEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);
};

#endif // MAINCHART_H
