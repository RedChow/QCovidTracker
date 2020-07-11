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

#include "jsonitemdelegate.h"

#include <QDebug>


JSONItemDelegate::JSONItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget* JSONItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(option);
    if (index.column() == 0) {
        //JSON key
        QLineEdit *editor = new QLineEdit(parent);
        return editor;
    }
    else if (index.column() == 1) {
        //Data Type
        QComboBox *combo = new QComboBox(parent);
        int i{0};
        QHash<int, QString>::iterator dataEnumIterator;
        for (dataEnumIterator = dataEnum->begin(); dataEnumIterator != dataEnum->end(); dataEnumIterator++) {
            combo->insertItem(i, dataEnumIterator.value(), dataEnumIterator.key());
            i++;
        }
        return combo;
    }
    else if (index.column() == 2) {
        //deprecated
        QComboBox *combo = new QComboBox(parent);
        combo->insertItem(0, "No");
        combo->insertItem(1, "Yes");
        return combo;
    }
    else {
        //data source
        QComboBox *combo = new QComboBox(parent);
        int i{0};
        QHash<int, QString>::iterator dataSourcesIterator;
        for (dataSourcesIterator = dataSources->begin(); dataSourcesIterator != dataSources->end(); dataSourcesIterator++) {
            combo->insertItem(i, dataSourcesIterator.value(), dataSourcesIterator.key());
            i++;
        }
        return combo;
    }

}

void JSONItemDelegate::setDataEnum(QHash<int, QString> *dE) {
    dataEnum = dE;
}

void JSONItemDelegate::setDataSources(QHash<int, QString> *dS) {
    dataSources = dS;
}

void JSONItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    if (index.column() == 0) {
        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        lineEdit->setText(value);
    }
    else if (index.column() == 1) {
        QString strValue = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combo = static_cast<QComboBox*>(editor);
        combo->setCurrentText(strValue);
    }
    else if (index.column() == 2) {
        QString strValue = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combo = static_cast<QComboBox*>(editor);
        combo->setCurrentText(strValue);
    }
    else if (index.column() == 3) {
        QString strValue = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox *combo = static_cast<QComboBox*>(editor);
        combo->setCurrentText(strValue);
    }
}

void JSONItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    if (index.column() == 0) {
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        QString text = lineEdit->text();
        model->setData(index, text, Qt::DisplayRole);
    }
    else if (index.column() == 1) {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QVariant currentData = comboBox->currentData();
        model->setData(index, currentData);
    }
    else if (index.column() == 2) {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        int currentIndex = comboBox->currentIndex();
        model->setData(index, currentIndex);
    }
    else {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QVariant currentData = comboBox->currentData();
        model->setData(index, currentData);
    }
}

void JSONItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
