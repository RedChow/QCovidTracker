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
#include "plotconfigform.h"
#include "ui_plotconfigform.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QPainter>
#include <QColorDialog>
#include <QDebug>

PlotConfigForm::PlotConfigForm(QSqlDatabase &p_mdb, int p_plotId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotConfigForm),
    mdb(p_mdb),
    plotId(p_plotId)
{
    ui->setupUi(this);
    initializeSelf();
    setLineStyleComboBox();
    setChartTypeComboBox();
    setChartAlignmentComboBox();

    connect(ui->penColor, &QPushButton::clicked, this, &PlotConfigForm::setLineColor);
    connect(ui->comboBoxStateInfo, SIGNAL(currentIndexChanged(int)), this, SLOT(stateInfoChanged(int)));
    connect(ui->comboBoxJSONFields, SIGNAL(currentIndexChanged(int)), this, SLOT(jsonFieldChanged(int)));
    connect(ui->comboBoxChartType, SIGNAL(currentIndexChanged(int)), this , SLOT(chartTypeChanged(int)));
    connect(ui->comboBoxLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(lineStyleChanged(int)));
    connect(ui->lineEdit, SIGNAL(editingFinished()), this, SLOT(plotNameEditingFinished()));
    connect(ui->checkBoxShowDifference, SIGNAL(stateChanged(int)), this, SLOT(showDifferencesChanged(int)));
    connect(ui->spinBoxPenWidth, SIGNAL(valueChanged(int)), this, SLOT(penWidthChanged(int)));
    connect(ui->comboBoxChartAlignment, SIGNAL(currentIndexChanged(int)), this, SLOT(chartAligmentChanged(int)));

    movingAverageDaysTimer = new QTimer(this);
    connect(ui->spinBoxMovingAverageDays, SIGNAL(valueChanged(int)), this, SLOT(movingAverageChanged(int)));
    connect(movingAverageDaysTimer, SIGNAL(timeout()), this, SLOT(movingAverageUpdated()));

    yAxisIdTimer = new QTimer(this);
    connect(yAxisIdTimer, SIGNAL(timeout()), this, SLOT(yAxisIdUpdated()));
    connect(ui->spinBoxAxisId, SIGNAL(valueChanged(int)), this, SLOT(yAxisIdChanged(int)));
}

PlotConfigForm::~PlotConfigForm()
{
    //movingAverageDaysTimer->stop();
    //delete movingAverageDaysTimer;
    //yAxisIdTimer->stop();
    //delete yAxisIdTimer;
    delete ui;
}

void PlotConfigForm::initializeSelf() {
    QSqlQuery query = QSqlQuery(mdb);
    query.prepare("SELECT "
                "plot_name, "
                "state_info_id, "
                "json_mappings_id, "
                "show_differences, "
                "moving_average_days, "
                "axis_id, "
                "chart_type, "
                "pen_style, "
                "pen_color, "
                "pen_width, "
                "chart_alignment "
                "FROM plot WHERE plot_id=:pi");
    query.bindValue(":pi", plotId);
    query.exec();
    if (query.next()) {
        plotName = query.value(0).toString();
        stateInfoId = query.value(1).toInt();
        jsonMappingsId = query.value(2).toInt();
        showDifferences = query.value(3).toBool();
        movingAverageDays = query.value(4).toInt();
        axisId = query.value(5).toInt();
        chartType = query.value(6).toInt();
        penStyle = query.value(7).toInt();
        penColorNotTheButton = query.value(8).toString();
        penWidth = query.value(9).toInt();
        chartAlignment = query.value(10).toInt();
    }

}

void PlotConfigForm::fillOutForm() {
    turnOffEditing = true;
    int index = ui->comboBoxStateInfo->findData(stateInfoId);
    ui->comboBoxStateInfo->setCurrentIndex(index > 0 ? index : 0);
    index = ui->comboBoxJSONFields->findData(jsonMappingsId);
    ui->comboBoxJSONFields->setCurrentIndex(index > 0 ? index : 0);
    index = ui->comboBoxChartType->findData(chartType);
    ui->comboBoxChartType->setCurrentIndex(index > 0 ? index : 0);
    index = ui->comboBoxLineStyle->findData(penStyle);
    ui->comboBoxLineStyle->setCurrentIndex(index > 0 ? index : 0);
    index = ui->comboBoxChartAlignment->findData(chartAlignment);
    ui->comboBoxChartAlignment->setCurrentIndex(index > 0 ? index : 0);

    ui->spinBoxAxisId->setValue(axisId > 0 ? axisId : 0);
    ui->spinBoxMovingAverageDays->setValue(movingAverageDays > 0 ? movingAverageDays : 0);
    ui->spinBoxPenWidth->setValue(penWidth > 0 ? penWidth : 1);

    QString qss = QString("background-color: %1").arg(penColorNotTheButton);
    ui->penColor->setStyleSheet(qss);

    ui->lineEdit->setText(plotName);

    if (showDifferences) {
        ui->checkBoxShowDifference->setCheckState(Qt::CheckState::Checked);
    }
    else {
        ui->checkBoxShowDifference->setCheckState(Qt::CheckState::Unchecked);
    }

    turnOffEditing = false;
}

//void PlotConfigForm::setStateInfoComboBox(QVector<QPair<QString, int>>& stateInfoVector) {
void PlotConfigForm::setStateInfoComboBox(const std::vector<QSharedPointer<QPair<QString, int>>>& stateInfoVector) {
    for(std::size_t i = 0; i < stateInfoVector.size(); ++i) {
        ui->comboBoxStateInfo->addItem(stateInfoVector.at(i)->first, stateInfoVector.at(i)->second);
    }
}


void PlotConfigForm::setJsonFieldsComboBox(const QVector<QSharedPointer<QPair<QString, int>>>& jsonFieldVector) {
    for (int i = 0; i < jsonFieldVector.size(); i++) {
        ui->comboBoxJSONFields->addItem(jsonFieldVector.at(i)->first, jsonFieldVector.at(i)->second);
    }
}

void PlotConfigForm::setLineStyleComboBox() {
    //There's got to be a better way to do this...
    QVector<QPair<int, QString>> lineTypes = {QPair<int, QString>(Qt::SolidLine, "Solid Line"), QPair<int, QString>(Qt::DashLine, "Dash Line"), QPair<int, QString>(Qt::DotLine, "Dot Line"),
                                              QPair<int, QString>(Qt::DashDotLine, "Dash Dot Line"), QPair<int, QString>(Qt::DashDotDotLine, "Dash Dot Dot Line")};
    for (int i = 0; i < lineTypes.size(); i++) {
        QPixmap icon(64, 32);
        icon.fill(Qt::white);

        QBrush brush(Qt::black);
        QPen pen(brush, 1, (Qt::PenStyle) lineTypes.at(i).first);

        QPainter painter(&icon);
        painter.setPen(pen);
        painter.drawLine(0, 16, 64, 16);

        ui->comboBoxLineStyle->addItem(QIcon(icon), lineTypes.at(i).second, lineTypes.at(i).first);
    }
}

void PlotConfigForm::setChartTypeComboBox() {
    ui->comboBoxChartType->addItem("Bar Chart", BarChart);
    ui->comboBoxChartType->addItem("Line Chart", LineChart);
}

void PlotConfigForm::setChartAlignmentComboBox() {
    ui->comboBoxChartAlignment->addItem("Y-Axis Left", Qt::AlignLeft);
    ui->comboBoxChartAlignment->addItem("Y-Axis Right", Qt::AlignRight);
}

void PlotConfigForm::setLineColor() {
    QColor color = QColorDialog::getColor(Qt::blue, this);
    if(color.isValid() ) {
        QString qss = QString("background-color: %1").arg(color.name());
        ui->penColor->setStyleSheet(qss);
        updateStatement("pen_color", color.name());
    }
}

void PlotConfigForm::updateStatement(QString field, QVariant value) {
    if (!turnOffEditing) {
        QSqlQuery update = QSqlQuery(mdb);
        update.prepare("UPDATE plot SET " + field + "= :value WHERE plot_id=:pi");
        update.bindValue(":value", value);
        update.bindValue(":pi", plotId);
        if (!update.exec()) {
            qDebug() << update.lastError().text();
        }
    }
}

void PlotConfigForm::stateInfoChanged(int index) {
    Q_UNUSED(index);
    updateStatement("state_info_id", ui->comboBoxStateInfo->currentData());
}

void PlotConfigForm::jsonFieldChanged(int index) {
    Q_UNUSED(index);
    updateStatement("json_mappings_id", ui->comboBoxJSONFields->currentData());
}

void PlotConfigForm::chartTypeChanged(int index) {
    Q_UNUSED(index);
    updateStatement("chart_type", ui->comboBoxChartType->currentData());
}

void PlotConfigForm::lineStyleChanged(int index) {
    Q_UNUSED(index);
    updateStatement("pen_style", ui->comboBoxLineStyle->currentData());
}

void PlotConfigForm::chartAligmentChanged(int index) {
    Q_UNUSED(index);
    updateStatement("chart_alignment", ui->comboBoxChartAlignment->currentData());
}

void PlotConfigForm::plotNameEditingFinished() {
    updateStatement("plot_name", ui->lineEdit->text());
}

void PlotConfigForm::movingAverageUpdated() {
    updateStatement("moving_average_days", ui->spinBoxMovingAverageDays->value());
}

void PlotConfigForm::movingAverageChanged(int days) {
    Q_UNUSED(days);
    if (movingAverageDaysTimer->isActive()) {
        movingAverageDaysTimer->stop();
    }
    movingAverageDaysTimer->setInterval(3*1000);
    movingAverageDaysTimer->start();
}

void PlotConfigForm::yAxisIdUpdated() {
    updateStatement("axis_id", ui->spinBoxAxisId->value());
}

void PlotConfigForm::yAxisIdChanged(int yAxisId) {
    Q_UNUSED(yAxisId);
    if (yAxisIdTimer->isActive()) {
        yAxisIdTimer->stop();
    }
    yAxisIdTimer->setInterval(3*1000);
    yAxisIdTimer->start();
}

void PlotConfigForm::showDifferencesChanged(int state) {
    QVariant stateBool = false;
    if (state == Qt::CheckState::Checked) {
        stateBool = true;
    }
    updateStatement("show_differences", stateBool);
}

void PlotConfigForm::penWidthChanged(int width) {
    updateStatement("pen_width", width);
}

int PlotConfigForm::getPlotId() {
    qDebug() << "i was called"; // << plotId;
    return plotId;
}

void PlotConfigForm::printStuff() {
    qDebug() << movingAverageDaysTimer->isActive() << "YES";
}
