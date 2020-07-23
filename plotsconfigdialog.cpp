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
#include "plotsconfigdialog.h"
#include "ui_plotsconfigdialog.h"
#include <QSqlQuery>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMessageBox>

#include "plotconfigform.h"

#include <QDebug>

PlotsConfigDialog::PlotsConfigDialog(QSqlDatabase &p_mdb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotsConfigDialog),
    mdb(p_mdb)
{
    ui->setupUi(this);
    fillComboBox();

    connect(ui->pushButtonAddNewPlotGroup, &QPushButton::clicked, this, &PlotsConfigDialog::addNewPlotGrouping);
    connect(ui->pushButtonAddPlot, &QPushButton::clicked, this, &PlotsConfigDialog::addPlotToGroup);
    connect(ui->comboBoxPlotGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(populateListWidgetPlots(int)));
    connect(ui->pushButtonDeletePlot, &QPushButton::clicked, this, &PlotsConfigDialog::deletePlot);

    createComboVectors();
    ui->listWidgetPlots->setStyleSheet(
  "QListWidget::item {"
     "border-style: solid;"
     "border-width:1px;"
     "border-color:black;"
  "}"
  "QListWidget::item:selected {"
     "background-color: #ababab;"
  "}");
}

PlotsConfigDialog::~PlotsConfigDialog()
{
    delete ui;
}

void PlotsConfigDialog::fillComboBox() {
    QSqlQuery query = QSqlQuery(mdb);
    query.exec("SELECT plot_grouping_id, plot_grouping_name FROM plot_grouping ORDER BY plot_grouping_name");
    while (query.next()) {
         ui->comboBoxPlotGroup->addItem(query.value(1).toString(),  query.value(0).toInt());
    }
}

void PlotsConfigDialog::addNewPlotGrouping() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("New Plot Group Name:"), QLineEdit::Normal,"", &ok);
    if (ok && !text.isEmpty()) {
        QSqlQuery insertQuery = QSqlQuery(mdb);
        insertQuery.prepare("INSERT INTO plot_grouping (plot_grouping_name) VALUES (:pgn) RETURNING plot_grouping_id");
        insertQuery.bindValue(":pgn", text);
        insertQuery.exec();
        if (insertQuery.next()) {
            ui->comboBoxPlotGroup->addItem(text, insertQuery.value(0).toInt());
            ui->comboBoxPlotGroup->setCurrentIndex(ui->comboBoxPlotGroup->count()-1);
        }
    }
}

void PlotsConfigDialog::addPlotToGroup() {
    if (ui->comboBoxPlotGroup->currentData().toInt() <= 0) {
        return;
    }
    int plotGroupingId = ui->comboBoxPlotGroup->currentData().toInt();
    QSqlQuery query = QSqlQuery(mdb);
    query.prepare("INSERT INTO plot (plot_name, plot_grouping_id) "
                  "VALUES (:pn, :pgi) RETURNING plot_id");
    query.bindValue(":pn", "newPlot");
    query.bindValue(":pgi", plotGroupingId);
    query.exec();
    if (query.next()) {
        addWidget(query.value(0).toInt());
    }
}

void PlotsConfigDialog::populateListWidgetPlots(int currentIndex) {
    Q_UNUSED(currentIndex);
    ui->listWidgetPlots->clear();
    int plotGroupingId = ui->comboBoxPlotGroup->currentData().toInt();
    QSqlQuery queryGetPlotIds = QSqlQuery(mdb);
    queryGetPlotIds.prepare("SELECT plot_id FROM plot WHERE plot_grouping_id=:pgi");
    queryGetPlotIds.bindValue(":pgi", plotGroupingId);
    queryGetPlotIds.exec();
    while (queryGetPlotIds.next()) {
        addWidget(queryGetPlotIds.value(0).toInt());
    }
}

void PlotsConfigDialog::addWidget(int plotId) {
    PlotConfigForm *plotConfigForm = new PlotConfigForm(mdb, plotId);
    plotConfigForm->setStateInfoComboBox(stateInfoVector);
    plotConfigForm->setJsonFieldsComboBox(jsonFieldVector);
    plotConfigForm->fillOutForm();
    QListWidgetItem *lwi = new QListWidgetItem();
    lwi->setData(Qt::UserRole, plotId);
    ui->listWidgetPlots->addItem(lwi);
    ui->listWidgetPlots->setItemWidget(lwi, plotConfigForm);
    lwi->setSizeHint(plotConfigForm->sizeHint());
}

void PlotsConfigDialog::deletePlot() {
    QList<QListWidgetItem*> widgetItems = ui->listWidgetPlots->selectedItems();
    for (auto item: widgetItems) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete?", "Are you sure you want to delete the selected plot?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QSqlQuery deleteQuery(mdb);
            deleteQuery.prepare("DELETE FROM plot WHERE plot_id=:pi");
            deleteQuery.bindValue(":pi", item->data(Qt::UserRole).toInt());
            deleteQuery.exec();
        }
    }
    populateListWidgetPlots(0); //any number will work since argument is unused
}

void PlotsConfigDialog::createComboVectors() {
    QSqlQuery stateInfoQuery = QSqlQuery(mdb);
    stateInfoQuery.exec("SELECT state_abbreviation, state_name, state_info_id FROM state_info");
    while (stateInfoQuery.next()) {
        //using pointers since we need the QPairs to live beyond this function; vector gets passed by reference
        // so that we don't make a copy in plotconfigform; probably could use unique_ptr here
        QSharedPointer<QPair<QString, int>> pair = QSharedPointer<QPair<QString, int>>(new QPair<QString, int>);
        pair->first = stateInfoQuery.value(0).toString() + " - " + stateInfoQuery.value(1).toString();
        pair->second = stateInfoQuery.value(2).toInt();
        stateInfoVector.push_back(pair);
    }
    QSqlQuery jsonQuery = QSqlQuery(mdb);
    jsonQuery.exec("SELECT json_key, json_mappings_id FROM json_mappings");
    while (jsonQuery.next()) {
        QSharedPointer<QPair<QString, int>> pair = QSharedPointer<QPair<QString, int>>(new QPair<QString, int>);
        pair->first = jsonQuery.value(0).toString();
        pair->second = jsonQuery.value(1).toInt();
        jsonFieldVector.push_back(pair);
    }
}












