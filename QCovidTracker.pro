QT       += core gui charts network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    algorithms.cpp \
    barchartsview.cpp \
    chartbuilder.cpp \
    customlineseries.cpp \
    databaseinterface.cpp \
    dataeditordialog.cpp \
    dataeditoritemdelegate.cpp \
    dataeditormodel.cpp \
    getcovidinfo.cpp \
    handlejsonfilesrunnable.cpp \
    jsonfileshandler.cpp \
    jsonitemdelegate.cpp \
    jsonmappingseditordialog.cpp \
    jsontablemodel.cpp \
    logger.cpp \
    main.cpp \
    mainchart.cpp \
    mainchartview.cpp \
    mainwindow.cpp \
    plotconfigform.cpp \
    plotmodel.cpp \
    plotsconfigdialog.cpp \
    tableeditordialog.cpp

HEADERS += \
    algorithms.h \
    barchartsview.h \
    chartbuilder.h \
    customlineseries.h \
    databaseinterface.h \
    dataeditordialog.h \
    dataeditoritemdelegate.h \
    dataeditormodel.h \
    getcovidinfo.h \
    handlejsonfilesrunnable.h \
    jsonfileshandler.h \
    jsonitemdelegate.h \
    jsonmappingseditordialog.h \
    jsontablemodel.h \
    logger.h \
    mainchart.h \
    mainchartview.h \
    mainwindow.h \
    plotconfigform.h \
    plotmodel.h \
    plotsconfigdialog.h \
    statestruct.h \
    tableeditordialog.h

FORMS += \
    dataeditordialog.ui \
    jsonmappingseditordialog.ui \
    mainwindow.ui \
    mainwindow_copy.ui \
    plotconfigform.ui \
    plotsconfigdialog.ui \
    tableeditordialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    QCovidTracker.pro.user
