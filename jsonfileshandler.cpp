#include "jsonfileshandler.h"
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QThreadPool>
#include "handlejsonfilesrunnable.h"

JsonFilesHandler::JsonFilesHandler()
{

}


void JsonFilesHandler::readJsonFilesDir(QString dir) {
     QDir directory(dir);
     QStringList jsonFiles = directory.entryList(QStringList() << "*.json", QDir::Files);
     emit updateNumberOfFiles(jsonFiles.size());
    //void jsonReceiveStatusMessage(QString);
     QThreadPool *threadpool = new QThreadPool();
     int maxThreads = threadpool->maxThreadCount();
     int filesPerThread = jsonFiles.size()/maxThreads;
     int lastThreadCount = jsonFiles.size() - maxThreads*filesPerThread;
     int threadNumber{0};
     qDebug() << maxThreads << " " << filesPerThread << " " <<lastThreadCount;
     if (jsonFiles.size() != 0 && jsonFiles.size() <= maxThreads) {
             HandleJsonFilesRunnable *jsonFileRunnable = new HandleJsonFilesRunnable();
             jsonFileRunnable->setDbName(QString::number(threadNumber));
             jsonFileRunnable->setJsonDirectory(dir);
             jsonFileRunnable->setBeginningAndEnd(0, jsonFiles.size());
             jsonFileRunnable->setJsonFileList(jsonFiles);
             connect(jsonFileRunnable, SIGNAL(sendStatusMessage(QString)), this, SLOT(jsonReceiveStatusMessage(QString)));
             threadpool->waitForDone();
             //TODO emit signal here
     }
     else if (jsonFiles.size() != 0) {
         for (threadNumber = 0; threadNumber < maxThreads; threadNumber++) {
             HandleJsonFilesRunnable *jsonFileRunnable = new HandleJsonFilesRunnable();
             jsonFileRunnable->setDbName(QString::number(threadNumber));
             jsonFileRunnable->setJsonDirectory(dir);
             if (threadNumber == maxThreads -1 ) {
                jsonFileRunnable->setBeginningAndEnd(threadNumber*filesPerThread, threadNumber*filesPerThread + filesPerThread + lastThreadCount);
             }
             else {
                jsonFileRunnable->setBeginningAndEnd(threadNumber*filesPerThread, threadNumber*filesPerThread + filesPerThread);
             }
             jsonFileRunnable->setJsonFileList(jsonFiles);
             connect(jsonFileRunnable, SIGNAL(sendStatusMessage(QString)), this, SLOT(jsonReceiveStatusMessage(QString)));
             threadpool->start(jsonFileRunnable);
         }
         threadpool->waitForDone();
         emit threadsAreDone();
     }
     qDebug() << "FINISHED";
}

void JsonFilesHandler::jsonReceiveStatusMessage(QString message) {
    emit jsonSendStatusMessage(message);
}
