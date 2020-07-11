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

#ifndef TABLEEDITORDIALOG_H
#define TABLEEDITORDIALOG_H

#include <QSqlTableModel>
#include <QDialog>
#include "databaseinterface.h"

namespace Ui {
class TableEditorDialog;
}

class TableEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableEditorDialog(QWidget *parent = nullptr);
    ~TableEditorDialog();
public slots:
    void addNewRow();
    void reject() override;
    void submitAndRefresh();
    void tableSelectionChanged(int index);
private:
    Ui::TableEditorDialog *ui;
    DatabaseInterface *dbi;
    QSqlDatabase mdb;
    QSqlTableModel *tableModel;
};

#endif // TABLEEDITORDIALOG_H
