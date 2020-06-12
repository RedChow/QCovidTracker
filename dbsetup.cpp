#include "dbsetup.h"
#include "ui_dbsetup.h"

DBSetup::DBSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DBSetup)
{
    ui->setupUi(this);
}

DBSetup::~DBSetup()
{
    delete ui;
}
