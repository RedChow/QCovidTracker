#include "mainwindow.h"
#include "statestruct.h"
#include <QDebug>

#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::vector<StateInfo> >("std::vector<StateInfo>");
    qRegisterMetaType<QVector<qint64>>("QVector<qint64>");
    qRegisterMetaType<QVector<float>>("QVector<float>");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
