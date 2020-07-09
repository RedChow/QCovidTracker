#ifndef TABLEEDITORDIALOG_H
#define TABLEEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class TableEditorDialog;
}

class TableEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableEditorDialog(QWidget *parent = nullptr);
    ~TableEditorDialog();

private:
    Ui::TableEditorDialog *ui;
};

#endif // TABLEEDITORDIALOG_H
