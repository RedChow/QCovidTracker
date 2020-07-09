#include "jsonmappingseditordialog.h"
#include "ui_jsonmappingseditordialog.h"

JSONMappingsEditorDialog::JSONMappingsEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JSONMappingsEditorDialog)
{
    ui->setupUi(this);
}

JSONMappingsEditorDialog::~JSONMappingsEditorDialog()
{
    delete ui;
}
