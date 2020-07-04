/*
 * Copyright (C) 2020 brian DOT l DOT miller DOT ttu AT gmail DOT com
 * This file is part of QCovidTracker.
 *
 * QThis program comes with ABSOLUTELY NO WARRANTY
 * CovidTracker is free software: you can redistribute it and/or modify
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
     QThreadPool *threadpool = new QThreadPool();
     int maxThreads = threadpool->maxThreadCount();
     int filesPerThread = jsonFiles.size()/maxThreads;
     int lastThreadCount = jsonFiles.size() - maxThreads*filesPerThread;
     int threadNumber{0};
     /* There might be an optimum number of files here in which the overhead in creating
      * threads outweighs just using one
      * For example splitting 80 files into 4 threads might be slower than just using
      * one thread; don't know if this can be done in general, or if there's a heuristic
      * argument
      */
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
         //We're in a thread outside of the main event loop, waiting here won't slow the GUI
         threadpool->waitForDone();
         emit threadsAreDone();
     }
}

void JsonFilesHandler::jsonReceiveStatusMessage(QString message) {
    emit jsonSendStatusMessage(message);
}
