#-------------------------------------------------
#
# Project created by QtCreator 2019-02-27T10:10:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = syssetting
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    netmanager.cpp \
    smmanager.cpp \
    sharehelper.cpp

HEADERS  += mainwindow.h \
    netmanager.h \
    smmanager.h \
    sharehelper.h

FORMS    += mainwindow.ui

target.path = /home/hqtech/test
INSTALLS += target
