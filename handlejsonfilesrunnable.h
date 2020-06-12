#ifndef HANDLEJSONFILESRUNNABLE_H
#define HANDLEJSONFILESRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QStringList>
#include <QSqlDatabase>
#include "databaseinterface.h"

class HandleJsonFilesRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    HandleJsonFilesRunnable();
    ~HandleJsonFilesRunnable();
    void run() override;
    QString jsonDirectory;
    void setJsonDirectory(QString);
    QString dbName;
    void setDbName(QString);
    QStringList jsonFileList;
    void setJsonFileList(QStringList);
    QString fileName;
    void setFileName(QString);
    int beginning;
    int end;
    void setBeginningAndEnd(int, int);
    QSqlDatabase db();
signals:
    void sendStatusMessage(QString);
private:
    DatabaseInterface *dbi;
};

#endif // HANDLEJSONFILESRUNNABLE_H
