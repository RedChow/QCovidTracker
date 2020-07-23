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
#ifndef PLOTCONFIGFORM_H
#define PLOTCONFIGFORM_H

#include <QWidget>
#include <QSqlDatabase>
#include <QTimer>
#include <memory>
#include <vector>

namespace Ui {
class PlotConfigForm;
}

class PlotConfigForm : public QWidget
{
    Q_OBJECT

public:
    enum ChartTypes {
        BarChart,
        LineChart
    };
    explicit PlotConfigForm(QSqlDatabase &p_mdb, int p_plotId, QWidget *parent = nullptr);
    ~PlotConfigForm();
    void initializeSelf();
    void fillOutForm();
    //void setStateInfoComboBox(QVector<QPair<QString, int>>& stateInfoVector);
    void setStateInfoComboBox(const std::vector<QSharedPointer<QPair<QString, int>>>& stateInfoVector);
    void setJsonFieldsComboBox(const QVector<QSharedPointer<QPair<QString, int>>>& jsonFieldVector);
    void setLineStyleComboBox();
    void setChartTypeComboBox();
    void setChartAlignmentComboBox();
    void updateStatement(QString field, QVariant value);

    //timers for spin boxes
    //using timers so that we're not updating the database after every single change on the spinboxes
    QTimer *movingAverageDaysTimer;
    QTimer *yAxisIdTimer;
    //QTimer *y;


public slots:
    void setLineColor();
    void stateInfoChanged(int index);
    void jsonFieldChanged(int index);
    void chartTypeChanged(int index);
    void lineStyleChanged(int index);
    void plotNameEditingFinished();
    void movingAverageUpdated();
    void movingAverageChanged(int days);
    void yAxisIdUpdated();
    void yAxisIdChanged(int yAxisId);
    void showDifferencesChanged(int state);
    void penWidthChanged(int);
    void chartAligmentChanged(int index);
    int getPlotId();
    void printStuff();
private:
    Ui::PlotConfigForm *ui;
    QSqlDatabase &mdb;

    //database fields
    int plotId;
    QString plotName;
    int stateInfoId{0};
    int jsonMappingsId{0};
    bool showDifferences{false};
    int movingAverageDays{0};
    int axisId{0};
    int chartType{0};
    int penStyle{0};
    QString penColorNotTheButton{"#0000FF"};
    int penWidth{0};
    int chartAlignment{0};

    //editing variables
    bool turnOffEditing{false};
};

#endif // PLOTCONFIGFORM_H
