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

#include "jsonmappingseditordialog.h"
#include "ui_jsonmappingseditordialog.h"

JSONMappingsEditorDialog::JSONMappingsEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JSONMappingsEditorDialog)
{
    ui->setupUi(this);
    jsonTableModel = new JSONTableModel(this);
    jsonTableModel->populateData();
    ui->tableViewJSONMappings->setModel(jsonTableModel);
    jsonItemDelegate = new JSONItemDelegate(this);
    jsonItemDelegate->setDataEnum(&jsonTableModel->dataEnum);
    jsonItemDelegate->setDataSources(&jsonTableModel->dataSources);
    ui->tableViewJSONMappings->setItemDelegate(jsonItemDelegate);
    connect(ui->pushButtonAddNewRow, &QPushButton::clicked, this, &JSONMappingsEditorDialog::addNewRow);
}

void JSONMappingsEditorDialog::addNewRow() {
    jsonTableModel->insertRows(jsonTableModel->jsonStructVector.size(), 1);
}

JSONMappingsEditorDialog::~JSONMappingsEditorDialog()
{
    delete ui;
}
