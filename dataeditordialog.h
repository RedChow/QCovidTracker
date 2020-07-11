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

#ifndef DATAEDITORDIALOG_H
#define DATAEDITORDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include "dataeditoritemdelegate.h"
#include "dataeditormodel.h"

namespace Ui {
class DataEditorDialog;
}

class DataEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataEditorDialog(QWidget *parent = nullptr);
    ~DataEditorDialog();
    void fillStateInfoComboBox(QSqlDatabase mdb);

private:
    Ui::DataEditorDialog *ui;
    QAbstractTableModel *dataEditorModel;
    DataEditorItemDelegate *dataEditorItemDelegate;
    DataEditorTableModel *dataEditorTableModel;
public slots:
    void changeTableModel(int index);
    void addNewRow();
};

#endif // DATAEDITORDIALOG_H
