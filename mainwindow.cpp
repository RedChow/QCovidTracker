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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QThread>
#include <QSqlError>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QPen>
#include <QColor>
#include <QApplication>
#include <QSettings>
#include <QStatusBar>
#include "getcovidinfo.h"
#include "customlineseries.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    dbi = new DatabaseInterface("MainThreadDb");
    mdb = dbi->getDatabase();
    fillComboBox();
    connect(ui->pushButtonGetData, &QPushButton::clicked, this, &MainWindow::getData);
    connect(ui->pushButtonPlot, &QPushButton::clicked, this, &MainWindow::plotPositiveTests);
    connect(ui->pushButtonPlot_2, &QPushButton::clicked, this, &MainWindow::plotPlotGrouping);

    readyReadJsonFiles = new QTimer(this);
    connect(readyReadJsonFiles, &QTimer::timeout, this, &MainWindow::jsonDirChanged);

    filesToDownload = 0;
    countFiles = 0;
    jsonFilesHandler = new JsonFilesHandler();
    futureWatcher = new QFutureWatcher<void>();
    QFuture<void> jsonFileReaderConcurrent = QtConcurrent::run(jsonFilesHandler, &JsonFilesHandler::readJsonFilesDir, jsonFileDir);
    connect(jsonFilesHandler, SIGNAL(jsonSendStatusMessage(QString)), this, SLOT(receiveStatusMessage(QString)), Qt::UniqueConnection);
    connect(jsonFilesHandler, SIGNAL(threadsAreDone()), this, SLOT(parsingFilesDone()), Qt::UniqueConnection);
    connect(jsonFilesHandler, SIGNAL(updateNumberOfFiles(int)), this, SLOT(updateFilesToDownload(int)), Qt::UniqueConnection);
    futureWatcher->setFuture(jsonFileReaderConcurrent);

    //TableViewPlots
    plotModel = new PlotModel();
    ui->tableViewPlots->setModel(plotModel);

    //Menu Actions
    connect(ui->actionEdit_State_Info, &QAction::triggered, this, &MainWindow::openStateInfoEditor);
    connect(ui->actionEdit_JSON_Fields, &QAction::triggered, this, &MainWindow::openJSONEditor);
    connect(ui->actionEdit_History_Data, &QAction::triggered, this, &MainWindow::openDataEditor);
    connect(ui->pushButtonEditPlotGroups, &QPushButton::clicked, this, &MainWindow::openPlotsConfigDialog);

    //in case this ever gets ported to mobile/tablet environment
    this->grabGesture(Qt::PanGesture);
    this->grabGesture(Qt::PinchGesture);

    chartBuilder = new ChartBuilder(mdb);

    //Logger
    getDataLogger = new Logger(this, ui->plainTextEditGetDataLogs);
    connect(ui->pushButtonClearLogs, &QPushButton::clicked, this, &MainWindow::clearLogs);
}

void MainWindow::updateFilesToDownload(int newNumber) {
    filesToDownload = newNumber;
}

void MainWindow::getData() {
    ui->statusbar->showMessage("Getting data...", 10*1000);
    //Set up directories
    if (!appDirectoryOverride.isEmpty()) {
        jsonFileDir = appDirectoryOverride;
    }
    if (!QDir(jsonFileDir).exists()) {
        QDir().mkdir(jsonFileDir);
    }
    if (!QDir(jsonFileDir + "/process_files").exists()) {
        QDir().mkdir(jsonFileDir + "/process_files");
    }
    QThread *thread = new QThread();
    GetCovidInfo *gci = new GetCovidInfo();
    gci->setDatabase(mdb);
    gci->moveToThread(thread);

    connect(thread, SIGNAL(started()), gci, SLOT(getMissingData()));
    connect(gci, SIGNAL(workFinished()), thread, SLOT(quit()));
    connect(gci, SIGNAL(workFinished()), gci, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(gci, SIGNAL(stateInfoVectorsReady(std::vector<StateInfo>)), this, SLOT(stateVectorsReady(std::vector<StateInfo>)));
    thread->start();
}

void MainWindow::stateVectorsReady(std::vector<StateInfo> stateInfoVector) {
    ui->statusbar->showMessage(QString("Querying %1 files...").arg(stateInfoVector.size()), 10*1000);
    getDataLogger->writeLog(QString("Querying %1 files").arg(stateInfoVector.size()));
    filesToDownload = stateInfoVector.size();
    countFiles = 0;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString baseUrl = QString("https://covidtracking.com/api/v1/states/");

    int count{0};
    int p_stateId;
    QString url;
    for (auto state: stateInfoVector) {
        url = baseUrl + state.stateAbbreviation + QString("/") + state.date.toString("yyyyMMdd") + QString(".json");
        if (state.stateAbbreviation == "us") {
           url = QString("https://covidtracking.com/api/v1/us/") + state.date.toString("yyyyMMdd") + QString(".json");
        }
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    p_stateId = state.stateId;
    connect(manager, &QNetworkAccessManager::finished, [this, reply, p_stateId] {this->emit replyFinished(reply, p_stateId);});
    count++;
    }
}

void MainWindow::replyFinished(QNetworkReply *r,  int stateId) {
    QJsonObject json = QJsonDocument::fromJson(r->readAll()).object();
    if (!json.empty()) {
        json.insert("stateId", stateId);
        QFile jsonFile;
        if (json["state"].isNull()) {
            jsonFile.setFileName(jsonFileDir + "/" + "us_" + QString::number(json["date"].toInt()) + QString(".json"));
        }
        else {
            jsonFile.setFileName(jsonFileDir + "/" + json["state"].toString() + QString("_") + QString::number(json["date"].toInt()) + QString(".json"));
        }
        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(QJsonDocument(json).toJson());
        if (readyReadJsonFiles->isActive()) {
            readyReadJsonFiles->stop();
        }
        readyReadJsonFiles->start(5*1000);
    }
}

void MainWindow::jsonDirChanged() {
    if (futureWatcher->isFinished()) {
        readyReadJsonFiles->stop();
        QFuture<void> jsonFileReaderConcurrent = QtConcurrent::run(jsonFilesHandler, &JsonFilesHandler::readJsonFilesDir, jsonFileDir);
        futureWatcher->setFuture(jsonFileReaderConcurrent);
    }
    else {
         if (readyReadJsonFiles->isActive()) {
            readyReadJsonFiles->stop();
         }
         readyReadJsonFiles->setInterval(15000);
         readyReadJsonFiles->start();
    }
}

void MainWindow::fillComboBox() {
    ui->comboBoxStates->addItem("--Select a State--", -1);
    QSqlQuery query("SELECT state_info_id, state_abbreviation, data_source_id FROM state_info", mdb);
    while (query.next()) {
        //only data sources for first tab plots should be from covid tracking project
        //i.e., skip adding states to first tab plots with data sources >= 2
        if (query.value(2).toInt() >= 2) {
            continue;
        }
        ui->comboBoxStates->addItem(query.value(1).toString(), query.value(0).toInt());
    }

    //fill saved plots
    QSqlQuery plotQuery("SELECT plot_grouping_id, plot_grouping_name FROM plot_grouping ORDER BY plot_grouping_name", mdb);
    while (plotQuery.next()) {
        ui->comboBoxPlotGrouping->addItem(plotQuery.value(1).toString(), plotQuery.value(0).toInt());
    }
}

void MainWindow::plotPositiveTests() {
    //ComboBox Data
    int currentIndex = ui->comboBoxStates->currentIndex();
    if (currentIndex <= -1) {
        return;
    }
    int stateId = ui->comboBoxStates->itemData(currentIndex).toInt();
    int days = ui->spinBoxRollingAverage->value();


    plotModel->clearTable();
    ui->tableViewPlots->setModel(plotModel);
    plotModel->setStateId(stateId);
    plotModel->setDays(days);
    QStringList headers = {"Positive", "Pos Diff", "Pos Cum Sum", "Pos Mov Avg"};
    plotModel->setHeaders(headers);
    plotModel->populateData();

    chart = new MainChart();
    chart->legend()->hide();
    CustomLineSeries *series = new CustomLineSeries();
    //std::unique_ptr<CustomLineSeries> series(new CustomLineSeries());
    CustomLineSeries *movingAvgSeries = new CustomLineSeries();
    CustomLineSeries *testsSeries = new CustomLineSeries();
    CustomLineSeries *testsMovingAvgSeries = new CustomLineSeries();
    CustomLineSeries *positivitySeries = new CustomLineSeries();

    QVXYModelMapper *positiveMapper = new QVXYModelMapper();
    positiveMapper->setXColumn(0);
    if (ui->checkBoxShowDifferences->isChecked()) {
        positiveMapper->setYColumn(2);
    } else {
        positiveMapper->setYColumn(1);
    }
    positiveMapper->setSeries(series);
    positiveMapper->setModel(plotModel);
    chart->addSeries(series);

    QVXYModelMapper *movingAvgMapper = new QVXYModelMapper();
    if (days > 2) {
        movingAvgMapper->setXColumn(0);
        movingAvgMapper->setYColumn(4);
        movingAvgMapper->setSeries(movingAvgSeries);
        movingAvgMapper->setModel(plotModel);
        chart->addSeries(movingAvgSeries);
    }

    QVXYModelMapper *positivityMapper = new QVXYModelMapper();
    if (ui->checkBoxPositivityRate->isChecked()) {
        positivityMapper->setXColumn(0);
        positivityMapper->setYColumn(13);
        positivityMapper->setSeries(positivitySeries);
        positivityMapper->setModel(plotModel);
        chart->addSeries(positivitySeries);
    }

    QVXYModelMapper *testsMapper = new QVXYModelMapper();
    testsMapper->setXColumn(0);
    if (ui->checkBoxShowDifferences->isChecked()) {
        testsMapper->setYColumn(8);
    } else {
        testsMapper->setYColumn(7);
    }
    testsMapper->setSeries(testsSeries);
    testsMapper->setModel(plotModel);
    chart->addSeries(testsSeries);

    QVXYModelMapper *testsMovingAvgMapper = new QVXYModelMapper();
    testsMovingAvgMapper->setXColumn(0);
    testsMovingAvgMapper->setYColumn(10);
    testsMovingAvgMapper->setSeries(testsMovingAvgSeries);
    testsMovingAvgMapper->setModel(plotModel);
    chart->addSeries(testsMovingAvgSeries);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("MM-dd");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    if (days > 2) {
        movingAvgSeries->attachAxis(axisX);
    }
    if (ui->checkBoxPositivityRate->isChecked()) {
        positivitySeries->attachAxis(axisX);
    }

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    if (days > 2) {
        movingAvgSeries->attachAxis(axisY);
    }

    //add new y axis for tests
    QValueAxis *axisTestsY = new QValueAxis();
    chart->addAxis(axisTestsY, Qt::AlignRight);
    testsSeries->attachAxis(axisTestsY);
    testsMovingAvgSeries->attachAxis(axisTestsY);

    //add new y axis for positivity
    QValueAxis *axisPositivityAxis = new QValueAxis();
    if (ui->checkBoxPositivityRate->isChecked()) {
        chart->addAxis(axisPositivityAxis, Qt::AlignLeft);
        positivitySeries->attachAxis(axisPositivityAxis);
        //go ahead and attach pen
        QPen positivityPen;
        positivityPen.setStyle(Qt::DashDotDotLine);
        positivityPen.setColor(QColor("#cc3300"));
        positivitySeries->setPen(positivityPen);
    }

    QPen posPen;
    posPen.setColor(QColor(0, 102, 204));
    series->setPen(posPen);

    QPen testsPen;
    //testsPen.setStyle(Qt::DashLine);
    //255, 153, 102
    testsPen.setColor(QColor(255, 153, 102));
    testsSeries->setPen(testsPen);

    if (days > 2) {
        QPen positiveMovingAvgPen;
        positiveMovingAvgPen.setStyle(Qt::DashLine);
        positiveMovingAvgPen.setColor(QColor(0, 102, 204));
        movingAvgSeries->setPen(positiveMovingAvgPen);
    }

    QPen testsMovingAvgPen;
    testsMovingAvgPen.setStyle(Qt::DashDotLine);
    testsMovingAvgPen.setColor(QColor(255, 153, 102));
    testsMovingAvgSeries->setPen(testsMovingAvgPen);

    connect(series, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewPositiveTestsPlots,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));
    connect(movingAvgSeries, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewPositiveTestsPlots,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));
    connect(testsSeries, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewPositiveTestsPlots,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));
    connect(testsMovingAvgSeries, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewPositiveTestsPlots,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));
    connect(positivitySeries, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewPositiveTestsPlots,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));

    ui->graphicsViewPositiveTestsPlots->clearCustomLineSeriesVector();

    ui->graphicsViewPositiveTestsPlots->addCustomLineSeries(series);
    ui->graphicsViewPositiveTestsPlots->addCustomLineSeries(testsSeries);
    if (days > 2) {
        ui->graphicsViewPositiveTestsPlots->addCustomLineSeries(movingAvgSeries);
        ui->graphicsViewPositiveTestsPlots->addCustomLineSeries(testsMovingAvgSeries);
    }
    if (ui->checkBoxPositivityRate->isChecked()) {
        ui->graphicsViewPositiveTestsPlots->addCustomLineSeries(positivitySeries);
    }
    QChart *oldChart = ui->graphicsViewPositiveTestsPlots->chart();
    ui->graphicsViewPositiveTestsPlots->setMainChart(chart);
    ui->graphicsViewPositiveTestsPlots->setChart(chart);
    delete oldChart;
}

void MainWindow::plotPlotGrouping() {
    QSqlQuery getPlotsQuery = QSqlQuery(mdb);
    getPlotsQuery.prepare("SELECT plot_name, "
                        "state_info_id, "
                        "json_mappings_id, "
                        "show_differences, "
                        "moving_average_days, "
                        "axis_id, "
                        "chart_type, "
                        "pen_style, "
                        "pen_color, "
                        "pen_width, "
                        "chart_alignment "
                        "FROM plot "
                        "WHERE plot_grouping_id=:pgi");
    getPlotsQuery.bindValue(":pgi", ui->comboBoxPlotGrouping->currentData().toInt());
    getPlotsQuery.exec();
    chartBuilder->clearBuilder();
    int stateInfoId{0};
    int jsonMappingsId{0};
    QString plotName{""};
    while (getPlotsQuery.next()) {
        stateInfoId = getPlotsQuery.value(1).toInt();
        jsonMappingsId = getPlotsQuery.value(2).toInt();
        plotName = getPlotsQuery.value(0).toString();
        //SeriesInfo(int sII, int jMI, int cT, int aI, int cA, QString sN, bool sD, QString pC, int pW, int pT, int mAD) :
        chartBuilder->addPlot(stateInfoId, jsonMappingsId,  getPlotsQuery.value(6).toInt(),
                              getPlotsQuery.value(5).toInt(), getPlotsQuery.value(10).toInt() , plotName, getPlotsQuery.value(3).toBool(),
                              getPlotsQuery.value(8).toString(), getPlotsQuery.value(9).toInt(), getPlotsQuery.value(7).toInt(),
                              getPlotsQuery.value(4).toInt());
    }
    customChart = new MainChart();
    customChart = chartBuilder->getChart();
    // TODO: Add connections for CustomLineSeries to display labels
    //ui->graphicsViewPositiveTestsPlots->clearCustomLineSeriesVector();
    //ui->graphicsViewPositiveTestsPlots->addCustomLineSeries(series);
    ui->graphicsViewCustom->clearCustomLineSeriesVector();
    QList<QAbstractSeries*> seriesList = customChart->series();
    for (auto sl: seriesList) {
        //connect(series, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewPositiveTestsPlots,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));
        qDebug() << sl->name() << sl->type();
        if (sl->type() == QAbstractSeries::SeriesTypeLine) {
            ui->graphicsViewCustom->addCustomLineSeries( dynamic_cast<CustomLineSeries*>(sl) );
            connect(sl, SIGNAL(setToolTip(QPointF, bool, QAbstractSeries*)), ui->graphicsViewCustom,  SLOT(showThatToolTip(QPointF, bool, QAbstractSeries*)));
        }
    }
    QChart *oldChart = ui->graphicsViewCustom->chart();
    ui->graphicsViewCustom->setMainChart(customChart);
    ui->graphicsViewCustom->setChart(customChart);
    delete oldChart;
}

void MainWindow::receiveStatusMessage(QString message) {
    countFiles++;
    QString newMessage = message + " : " + QString("%1/%2").arg(countFiles).arg(filesToDownload);
    ui->statusbar->showMessage(newMessage, 5*1000);
    getDataLogger->writeLog(newMessage);
}

void MainWindow::parsingFilesDone() {
    ui->statusbar->showMessage(QString("%1/%2 json files parsed").arg(countFiles).arg(filesToDownload), 10*1000);
    connect(ui->statusbar, &QStatusBar::messageChanged, this, &MainWindow::statusBarMessageDone);
    getDataLogger->writeLog(QString("%1/%2 json files parsed").arg(countFiles).arg(filesToDownload));
}

void MainWindow::statusBarMessageDone() {
    filesToDownload = 0;
    countFiles = 0;
    disconnect(ui->statusbar, &QStatusBar::messageChanged, this, &MainWindow::statusBarMessageDone);
}

void MainWindow::openStateInfoEditor() {
    tableEditorDialog = new TableEditorDialog(this);
    tableEditorDialog->show();
}

void MainWindow::openJSONEditor() {
    jsonEditorDialog = new JSONMappingsEditorDialog(this);
    jsonEditorDialog->show();
}

void MainWindow::openDataEditor() {
    dataEditorDialog = new DataEditorDialog(this);
    dataEditorDialog->show();
}

void MainWindow::openPlotsConfigDialog() {
    plotsConfigDialog = new PlotsConfigDialog(mdb, this);
    plotsConfigDialog->show();
}

void MainWindow::clearLogs() {
    ui->plainTextEditGetDataLogs->clear();
}

void MainWindow::loadSettings() {
    QString basePath = QApplication::applicationDirPath();
    settingsFilePath = basePath + "/QCovidTrackerSettings.ini";
    QSettings settings(settingsFilePath, QSettings::NativeFormat);
    jsonFileDir = settings.value("jsonFileDir", basePath + "/json_downloads/").toString();
    //ui->splitter->restoreState(settings.value("splitter").toByteArray());
    move(settings.value("pos", QPoint(100, 100)).toPoint());
    resize(settings.value("size", QSize(800, 600)).toSize());
}

void MainWindow::saveSettings() {
    QSettings settings(settingsFilePath, QSettings::NativeFormat);
    settings.setValue("jsonFileDir", jsonFileDir);
    //settings.setValue("splitter", ui->splitter->saveState());
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

