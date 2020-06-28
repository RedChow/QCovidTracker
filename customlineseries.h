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
