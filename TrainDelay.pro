#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T12:43:11
#
#-------------------------------------------------

QT       += core gui network webkit sql

TARGET = TrainDelay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    webpageloader.cpp \
    dbaccess.cpp

HEADERS  += mainwindow.h \
    webpageloader.h \
    dbaccess.h

FORMS    += mainwindow.ui
