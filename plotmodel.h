#ifndef PLOTMODEL_H
#define PLOTMODEL_H
#include <QObject>
#include <QAbstractTableModel>
#include <QVector>
#include <QThread>
#include "databaseinterface.h"

#include <vector>

class PlotModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PlotModel(QObject *parent = nullptr);
    ~PlotModel();
    //Rquired QTableAbstractModel functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    //helpers
    void populateData();
    void clearTable();
    QVector<QVector<int>> tableData;
    QVector<QDate> tableQDates;
    int stateId;
    void setStateId(int p_stateId);
    int days;
    void setDays(int p_days);
    QStringList headers;
    void setHeaders(QStringList p_Headers);
signals:

private:
    DatabaseInterface *dbi;
    QSqlDatabase mdb;
};

#endif // PLOTMODEL_H
