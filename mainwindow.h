#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtConcurrent/QtConcurrent>
#include <vector>
#include <QList>
#include <QVector>
#include <QTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QtCharts>

#include "databaseinterface.h"
#include "statestruct.h"
#include "jsonfileshandler.h"
#include "plotmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getData();
    void replyFinished(QNetworkReply *r,int p_stateId);
    void testRun(int a);
    QList<QString> *jsonColumns;
    void fillComboBox();
    void plotPositiveTests();

    int filesToDownload;
    int countFiles;

    QVector<QJsonObject> *allJSONObjects;
    QString appDirectory;
    QString appDirectoryOverride;
    QString jsonFileDir;
    QTimer *readyReadJsonFiles;
    JsonFilesHandler *jsonFilesHandler;

    QFuture<void> jsonFileReaderConcurrent;
    QFutureWatcher<void> *futureWatcher;

    PlotModel *plotModel;

    QString settingsFilePath;
    void loadSettings();
    void saveSettings();

public slots:
    void stateVectorsReady(std::vector<StateInfo>);
    void jsonDirChanged();
    void receiveStatusMessage(QString);
    void parsingFilesDone();
    void updateFilesToDownload(int);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H


