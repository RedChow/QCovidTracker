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

#include "tableeditordialog.h"
#include "ui_tableeditordialog.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>

TableEditorDialog::TableEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableEditorDialog)
{
    ui->setupUi(this);
    dbi = new DatabaseInterface("JSONFieldEditor");
    mdb = dbi->getDatabase();
    tableModel = new QSqlTableModel(this, mdb);
    ui->tableViewTable->setModel(tableModel);
    connect(ui->pushButtonAddNewRow, &QPushButton::clicked, this, &TableEditorDialog::addNewRow);
    connect(ui->pushButtonCommit, &QPushButton::clicked, this, &TableEditorDialog::submitAndRefresh);
    ui->comboBoxTable->addItem("--SELECT A TABLE--");
    ui->comboBoxTable->addItem("State Info");
    ui->comboBoxTable->addItem("Data Source");
    connect(ui->comboBoxTable, SIGNAL(currentIndexChanged(int)), this, SLOT(tableSelectionChanged(int)));
}

void TableEditorDialog::tableSelectionChanged(int index) {
    if (index == 1) {
        tableModel->setTable("state_info");
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        tableModel->select();
        ui->tableViewTable->hideColumn(0);
    }
    else if (index == 2) {
        tableModel->setTable("data_source");
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        tableModel->select();
    }
}

void TableEditorDialog::submitAndRefresh() {
    tableModel->submitAll();
}

void TableEditorDialog::addNewRow() {
    tableModel->insertRow(tableModel->rowCount());
}

void TableEditorDialog::reject() {
    if (tableModel->isDirty()) {
        QMessageBox msgBox;
        msgBox.setText("The table has unsaved changes.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch(ret) {
            case QMessageBox::Save:
                tableModel->submitAll();
                break;
            default:
                QDialog::reject();
                break;
        }
    }
    QDialog::reject();
}

TableEditorDialog::~TableEditorDialog()
{
    delete ui;
}
