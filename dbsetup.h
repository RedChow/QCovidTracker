#ifndef DBSETUP_H
#define DBSETUP_H

#include <QDialog>

namespace Ui {
class DBSetup;
}

class DBSetup : public QDialog
{
    Q_OBJECT

public:
    explicit DBSetup(QWidget *parent = nullptr);
    ~DBSetup();

private:
    Ui::DBSetup *ui;
};

#endif // DBSETUP_H
