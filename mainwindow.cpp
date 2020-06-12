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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    fillComboBox();
    connect(ui->pushButtonGetData, &QPushButton::clicked, this, &MainWindow::getData);
    connect(ui->pushButtonPlot, &QPushButton::clicked, this, &MainWindow::plotPositiveTests);

    readyReadJsonFiles = new QTimer(this);
    connect(readyReadJsonFiles, &QTimer::timeout, this, &MainWindow::jsonDirChanged);

    filesToDownload = 0;
    countFiles = 0;
    jsonFilesHandler = new JsonFilesHandler();
    futureWatcher = new QFutureWatcher<void>();
    QFuture<void> jsonFileReaderConcurrent = QtConcurrent::run(jsonFilesHandler, &JsonFilesHandler::readJsonFilesDir, jsonFileDir);
    connect(jsonFilesHandler, SIGNAL(jsonSendStatusMessage(QString)), this, SLOT(receiveStatusMessage(QString)));
    connect(jsonFilesHandler, SIGNAL(threadsAreDone()), this, SLOT(parsingFilesDone()));
    connect(jsonFilesHandler, SIGNAL(updateNumberOfFiles(int)), this, SLOT(updateFilesToDownload(int)));
    futureWatcher->setFuture(jsonFileReaderConcurrent);

    //TableView stuff
    plotModel = new PlotModel();
    ui->tableViewPlots->setModel(plotModel);
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
    gci->moveToThread(thread);

    connect(thread, SIGNAL(started()), gci, SLOT(getMissingData()));
    connect(gci, SIGNAL(workFinished()), thread, SLOT(quit()));
    connect(gci, SIGNAL(workFinished()), gci, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(gci, SIGNAL(stateInfoVectorsReady(std::vector<StateInfo>)), this, SLOT(stateVectorsReady(std::vector<StateInfo>)));
    thread->start();
}

void MainWindow::stateVectorsReady(std::vector<StateInfo> stateInfoVector) {
    ui->statusbar->showMessage(QString("%1 files to download...").arg(stateInfoVector.size()), 10*1000);
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
        readyReadJsonFiles->start(1*1000);
    }
}

void MainWindow::jsonDirChanged() {
    if (futureWatcher->isFinished()) {
        readyReadJsonFiles->stop();
        QFuture<void> jsonFileReaderConcurrent = QtConcurrent::run(jsonFilesHandler, &JsonFilesHandler::readJsonFilesDir, jsonFileDir);
        connect(jsonFilesHandler, SIGNAL(jsonSendStatusMessage(QString)), this, SLOT(receiveStatusMessage(QString)));
        connect(jsonFilesHandler, SIGNAL(threadsAreDone()), this, SLOT(parsingFilesDone()));
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
    DatabaseInterface *dbi = new DatabaseInterface("fillTheComboBox");
    QSqlDatabase mdb = dbi->getDatabase();

    ui->comboBoxStates->addItem("--Select a State--", -1);
    QSqlQuery query("SELECT state_info_id, state_abbreviation FROM state_info", mdb);
    while (query.next()) {
        ui->comboBoxStates->addItem(query.value(1).toString(), query.value(0).toInt());
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

    QChart *chart = new QChart();
    QLineSeries *series = new QLineSeries;
    QLineSeries *movingAvgSeries = new QLineSeries();
    QLineSeries *testsSeries = new QLineSeries();
    QLineSeries *testsMovingAvgSeries = new QLineSeries();

    QVXYModelMapper *positiveMapper = new QVXYModelMapper();
    positiveMapper->setXColumn(0);
    positiveMapper->setYColumn(2);
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

    QVXYModelMapper *testsMapper = new QVXYModelMapper();
    testsMapper->setXColumn(0);
    testsMapper->setYColumn(8);
    testsMapper->setSeries(testsSeries);
    testsMapper->setModel(plotModel);
    chart->addSeries(testsSeries);

    QVXYModelMapper *testsMovingAvgMapper = new QVXYModelMapper();
    testsMovingAvgMapper->setXColumn(0);
    testsMovingAvgMapper->setYColumn(10);
    testsMovingAvgMapper->setSeries(testsMovingAvgSeries);
    testsMovingAvgMapper->setModel(plotModel);
    chart->addSeries(testsMovingAvgSeries);

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

    QPen testsPen;
    testsPen.setStyle(Qt::DashLine);
    testsPen.setColor(QColor(0, 51, 153));
    testsSeries->setPen(testsPen);

    QPen testsMovingAvgPen;
    testsMovingAvgPen.setStyle(Qt::DashDotLine);
    testsMovingAvgPen.setColor(QColor(0, 51, 102));
    testsMovingAvgSeries->setPen(testsMovingAvgPen);

    ui->graphicsViewPositiveTestsPlots->setChart(chart);
}

void MainWindow::receiveStatusMessage(QString message) {
    countFiles++;
    QString newMessage = message + " : " + QString("%1/%2").arg(countFiles).arg(filesToDownload);
    ui->statusbar->showMessage(newMessage, 5*1000);
}

void MainWindow::parsingFilesDone() {
    ui->statusbar->showMessage(QString("%1/%2 json files parsed").arg(countFiles).arg(filesToDownload), 10*1000);
    filesToDownload = 0;
    countFiles = 0;
}

void MainWindow::loadSettings() {
    QString basePath = QApplication::applicationDirPath();
    settingsFilePath = basePath + "/QCovidTrackerSettings.ini";
    QSettings settings(settingsFilePath, QSettings::NativeFormat);
    jsonFileDir = settings.value("jsonFileDir", basePath + "/json_downloads/").toString();

}

void MainWindow::saveSettings() {
    QSettings settings(settingsFilePath, QSettings::NativeFormat);
    settings.setValue("jsonFileDir", jsonFileDir);
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

