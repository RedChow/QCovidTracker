#ifndef JSONMAPPINGSEDITORDIALOG_H
#define JSONMAPPINGSEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class JSONMappingsEditorDialog;
}

class JSONMappingsEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JSONMappingsEditorDialog(QWidget *parent = nullptr);
    ~JSONMappingsEditorDialog();

private:
    Ui::JSONMappingsEditorDialog *ui;
};

#endif // JSONMAPPINGSEDITORDIALOG_H
