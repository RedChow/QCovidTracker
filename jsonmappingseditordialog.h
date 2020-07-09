#ifndef JSONMAPPINGSEDITORDIALOG_H
#define JSONMAPPINGSEDITORDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include "jsontablemodel.h"
#include "jsonitemdelegate.h"

namespace Ui {
class JSONMappingsEditorDialog;
}

class JSONMappingsEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JSONMappingsEditorDialog(QWidget *parent = nullptr);
    ~JSONMappingsEditorDialog();
    void setModelData();
private:
    Ui::JSONMappingsEditorDialog *ui;
    JSONTableModel *jsonTableModel;
    JSONItemDelegate *jsonItemDelegate;

};

#endif // JSONMAPPINGSEDITORDIALOG_H
