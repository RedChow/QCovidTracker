#include "dataeditordialog.h"
#include "ui_dataeditordialog.h"

DataEditorDialog::DataEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataEditorDialog)
{
    ui->setupUi(this);
}

DataEditorDialog::~DataEditorDialog()
{
    delete ui;
}
