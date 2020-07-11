/*
 * Copyright (C) 2020 brian DOT l DOT miller DOT ttu AT gmail DOT com
 * This file is part of QCovidTracker.
 *
 * This program comes with ABSOLUTELY NO WARRANTY
 * QCovidTracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QCovidTracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QCovidTracker.  If not, see <https://www.gnu.org/licenses/.
 */
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
#include "mainchartview.h"
#include "mainchart.h"
#include "tableeditordialog.h"
#include "jsonmappingseditordialog.h"
#include "dataeditordialog.h"

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

    MainChart *chart;
    MainChartView *mainChartView;

    //Menu actions
    void openStateInfoEditor();
    TableEditorDialog *tableEditorDialog;
    void openJSONEditor();
    JSONMappingsEditorDialog *jsonEditorDialog;
    void openDataEditor();
    DataEditorDialog *dataEditorDialog;

public slots:
    void stateVectorsReady(std::vector<StateInfo>);
    void jsonDirChanged();
    void receiveStatusMessage(QString);
    void parsingFilesDone();
    void updateFilesToDownload(int);
    void statusBarMessageDone();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H


