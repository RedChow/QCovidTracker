#include "dataeditordialog.h"
#include "ui_dataeditordialog.h"

DataEditorDialog::DataEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataEditorDialog)
{
    ui->setupUi(this);
    //dataEditorItemDelegate = new DataEditorItemDelegate(this);

    dataEditorTableModel = new DataEditorTableModel(this);
    ui->tableViewDataEditor->setModel(dataEditorTableModel);
}

DataEditorDialog::~DataEditorDialog()
{
    delete ui;
}
