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
}

JSONMappingsEditorDialog::~JSONMappingsEditorDialog()
{
    delete ui;
}
