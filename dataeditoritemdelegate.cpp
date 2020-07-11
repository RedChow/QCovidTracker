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
#include "dataeditoritemdelegate.h"
#include <QDebug>

DataEditorItemDelegate::DataEditorItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget* DataEditorItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //We're just going to return a line edit for all fields
    Q_UNUSED(option);
    if (index.column() == 0) {
        QDateEdit *dateEdit = new QDateEdit(parent);
        dateEdit->setDisplayFormat("yyyy-MM-dd");
        return dateEdit;
    }
    else {
        QLineEdit *lineEdit = new QLineEdit(parent);
        return lineEdit;
    }
}

void DataEditorItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.column() == 0) {
        QVariant date = index.model()->data(index, Qt::DisplayRole);
        QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
        dateEdit->setDisplayFormat("yyyy-MM-dd");
        dateEdit->setDate(date.toDate());
    }
    else {
        QVariant value = index.model()->data(index, Qt::DisplayRole);
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        lineEdit->setText(value.toString());
    }
}

void DataEditorItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (index.column() == 0) {
        QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
        QDate date = dateEdit->date();
        model->setData(index, date, Qt::DisplayRole);
    }
    else {
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        QString text = lineEdit->text();
        model->setData(index, text, Qt::DisplayRole);
    }
}

void DataEditorItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
