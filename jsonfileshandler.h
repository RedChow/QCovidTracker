#ifndef JSONFILESHANDLER_H
#define JSONFILESHANDLER_H
#include <QObject>

class JsonFilesHandler : public QObject
{
    Q_OBJECT
public:
    JsonFilesHandler();
    void readJsonFilesDir(QString);
    QString jsonDir;
    void setJsonDir();
signals:
    void threadsAreDone();
    void jsonSendStatusMessage(QString);
    void updateNumberOfFiles(int);
public slots:
    void jsonReceiveStatusMessage(QString);

};

#endif // JSONFILESHANDLER_H
