#include "mainchartview.h"

MainChartView::MainChartView()
{

}

MainChartView::MainChartView(QWidget *widget) :
    QChartView(widget),
    tracePlot(false)
{
    setMouseTracking(true);
    setInteractive(true);
    setRubberBand(RectangleRubberBand);
    m_coordX = new QGraphicsTextItem;
    customLineSeriesVector.clear();
}

void MainChartView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu *contextMenu = new QMenu(this);
    QAction *traceAction = contextMenu->addAction("Trace");
    QAction *untraceAction = contextMenu->addAction("No Trace");
    QAction *action = contextMenu->exec(event->globalPos());
    if (action == traceAction) {
        traceChart();
    }
    else if (action == untraceAction) {
        untraceChart();
    }
}

void MainChartView::traceChart() {
    qDebug() << "I am tracing now";
    tracePlot = true;
}

void MainChartView::untraceChart() {
    tracePlot = false;
}

bool MainChartView::viewportEvent(QEvent *event) {
    return QChartView::viewportEvent(event);
}

void MainChartView::mousePressEvent(QMouseEvent *event) {
    QChartView::mousePressEvent(event);
}

void MainChartView::setMainChart(MainChart *m) {
    m_chart = m;
    m_coordX = new QGraphicsTextItem(m_chart);
    int i;
    graphicsTextItemVector.clear();
    for (i = 0; i < customLineSeriesVector.size(); i++) {
        QGraphicsTextItem *item = new QGraphicsTextItem(m_chart);
        graphicsTextItemVector.push_back(item);
    }
}

void MainChartView::clearCustomLineSeriesVector() {
    customLineSeriesVector.clear();
}

void MainChartView::mouseMoveEvent(QMouseEvent *event) {
    if (tracePlot) {
        mouseX = event->x();
        mouseY = event->y();
    }
    QChartView::mouseMoveEvent(event);
}

void MainChartView::drawForeground(QPainter *painter, const QRectF &rect) {

    if (tracePlot) {
        painter->drawLine(mouseX, 0, mouseX, this->height());

    }
    QChartView::drawForeground(painter, rect);
}

qreal MainChartView::getYValue(qreal xValue, QAbstractSeries *seriesPointVector) {
    Q_UNUSED(xValue);
    Q_UNUSED(seriesPointVector);
    return 0;
}

void MainChartView::mouseReleaseEvent(QMouseEvent *event) {
    //catch and relase that Right Button
    if (event->button() == Qt::RightButton) {
        return;
    }
    QChartView::mouseReleaseEvent(event);
}

void MainChartView::showThatToolTip(QPointF pointF, bool state, QAbstractSeries *abstractSeries) {
    if (state) {
        QDateTime timestamp;
        timestamp.setTime_t(pointF.x()/1000);
        xTrace = pointF.x();
        QPointF point = m_chart->mapToPosition(pointF, abstractSeries);
        int xPos = point.x();
        int yPos = point.y();
        if (xPos + 150 > this->width() ) {
            xPos = xPos - 150;
        }
        m_coordX->setPos(xPos, yPos);
        m_coordX->setHtml(QString("<div style='background-color:#0066cc; color:#ffffff;'>	%1 : %2</div>").arg(timestamp.toString("yyyy-MM-dd")).arg(pointF.y()));
        m_coordX->setZValue(11);
        m_coordX->show();
        m_coordX->setVisible(true);
    }
    else {
        m_coordX->setVisible(false);
    }
    if (tracePlot) {
        m_coordX->setVisible(false);
        int i{0};
        for (i = 0; i < customLineSeriesVector.size(); i++) {
            CustomLineSeries *tempSeries = customLineSeriesVector.at(i);
            qreal tempY = tempSeries->getYValue(xTrace/1000);
            QPointF point = m_chart->mapToPosition(QPointF(xTrace,tempY ), tempSeries);
            int xPos = point.x();
            int yPos = point.y();
            if (xPos + 150 > this->width() ) {
                xPos = xPos - 150;
            }
            QDateTime timestamp;
            timestamp.setTime_t(xTrace/1000);
            graphicsTextItemVector.at(i)->setZValue(10-i);
            if (i == 0) {
                graphicsTextItemVector.at(i)->setHtml(QString("<div style='background-color:#0066cc; color:#ffffff'>	%1 : %2</div>").arg(timestamp.toString("yyyy-MM-dd")).arg(tempY));
            }
            else if (i == 1) {
                graphicsTextItemVector.at(i)->setHtml(QString("<div style='background-color:#ff9966; color:#000000'>	%1 : %2</div>").arg(timestamp.toString("yyyy-MM-dd")).arg(tempY));
            }
           else if (i == 2) {
                graphicsTextItemVector.at(i)->setHtml(QString("<div style='background-color:#a2ceff; color:#000000'>	%1 : %2</div>").arg(timestamp.toString("yyyy-MM-dd")).arg(tempY));
            }
            else if (i == 3) {
                graphicsTextItemVector.at(i)->setHtml(QString("<div style='background-color:#ffd09a; color:#000000'>	%1 : %2</div>").arg(timestamp.toString("yyyy-MM-dd")).arg(tempY));
            }
            //The following sets the y coordinate on the QGraphicsTextItem so that they're all visible
            //TODO: This works most of the time, there are a few cases where it doesn't and the items still overlap
            if (i > 0) {
                int j;
                for (j = 0; j < i; j++) {
                    point = graphicsTextItemVector.at(j)->pos();
                    if (yPos + 30 > point.y() and yPos <= point.y()) {
                        yPos = yPos - 30 - (point.y() - yPos);
                    }
                    else if (yPos - 30 < point.y() and yPos > point.y()) {
                        yPos = yPos - (30 + yPos - point.y());
                    }
                }
                graphicsTextItemVector.at(i)->setPos(xPos, yPos);
            }
            else {
                graphicsTextItemVector.at(i)->setPos(xPos, yPos);
            }
            graphicsTextItemVector.at(i)->show();
            graphicsTextItemVector.at(i)->setVisible(true);
       }

    }
    else {
        int i;
        for (i = 0; i < graphicsTextItemVector.size(); i++) {
            graphicsTextItemVector.at(i)->setVisible(false);
        }
    }
}

void MainChartView::addCustomLineSeries(CustomLineSeries *lineSeries) {
    customLineSeriesVector.push_back(lineSeries);
    QGraphicsTextItem *textItem = new QGraphicsTextItem();
    graphicsTextItemVector.push_back(textItem);
}

void MainChartView::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Plus:
            chart()->zoomIn();
            break;
        case Qt::Key_Minus:
            chart()->zoomOut();
            break;
        case Qt::Key_A:
            chart()->scroll(-2, 0);
            break;
        case Qt::Key_D:
            chart()->scroll(2, 0);
            break;
        case Qt::Key_W:
            chart()->scroll(0, 2);
            break;
        case Qt::Key_S:
            chart()->scroll(0, -2);
            break;
        case Qt::Key_Down:
            chart()->scroll(0, -10);
            break;
        case Qt::Key_Up:
            chart()->scroll(0, 10);
            break;
        case Qt::Key_Right:
            chart()->scroll(10, 0);
            break;
        case Qt::Key_Left:
            chart()->scroll(-10,0);
            break;
    }
}