#ifndef DATAEDITORDIALOG_H
#define DATAEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class DataEditorDialog;
}

class DataEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataEditorDialog(QWidget *parent = nullptr);
    ~DataEditorDialog();

private:
    Ui::DataEditorDialog *ui;
};

#endif // DATAEDITORDIALOG_H
