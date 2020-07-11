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

#include "dataeditordialog.h"
#include "ui_dataeditordialog.h"
#include <QDebug>
#include <QSqlError>

DataEditorDialog::DataEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataEditorDialog)
{
    ui->setupUi(this);

    dataEditorTableModel = new DataEditorTableModel(this);
    ui->tableViewDataEditor->setModel(dataEditorTableModel);
    dataEditorItemDelegate = new DataEditorItemDelegate(this);
    ui->tableViewDataEditor->setItemDelegate(dataEditorItemDelegate);
    fillStateInfoComboBox(dataEditorTableModel->lendDatabase());
    connect(ui->comboBoxStateInfo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTableModel(int)));
    connect(ui->pushButtonAddNewRow, &QPushButton::clicked, this, &DataEditorDialog::addNewRow);
}

void DataEditorDialog::addNewRow() {
    dataEditorTableModel->insertRows(dataEditorTableModel->tableData.size(), 1);
}

void DataEditorDialog::changeTableModel(int index) {
    QVariant dataSourceId = ui->comboBoxStateInfo->itemData(index, Qt::UserRole | 0x0001);
    QVariant stateInfoId = ui->comboBoxStateInfo->currentData();
    if (ui->comboBoxStateInfo->currentIndex() >= 0) {
        dataEditorTableModel->clearTable();
        dataEditorTableModel->getTableHeaders(dataSourceId.toInt());
        dataEditorTableModel->populateData(stateInfoId.toInt(), dataSourceId.toInt());
    }
}

void DataEditorDialog::fillStateInfoComboBox(QSqlDatabase mdb) {
    QSqlQuery query = QSqlQuery(mdb);
    query.exec("SELECT state_info_id, state_abbreviation, state_name, data_source_id FROM state_info");
    qDebug() << query.lastError().text();
    int itemCount{0};
    ui->comboBoxStateInfo->addItem("--SELECT--");
    while (query.next()) {
        itemCount++;
        ui->comboBoxStateInfo->addItem(query.value(1).toString() + " - " + query.value(2).toString(), query.value(0).toInt());
        ui->comboBoxStateInfo->setItemData(itemCount, query.value(3).toInt(), Qt::UserRole | 0x0001);
    }
}

DataEditorDialog::~DataEditorDialog()
{
    delete ui;
}
