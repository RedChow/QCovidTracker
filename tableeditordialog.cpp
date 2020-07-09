#include "tableeditordialog.h"
#include "ui_tableeditordialog.h"

TableEditorDialog::TableEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableEditorDialog)
{
    ui->setupUi(this);
}

TableEditorDialog::~TableEditorDialog()
{
    delete ui;
}
