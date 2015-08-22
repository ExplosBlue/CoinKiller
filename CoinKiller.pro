#-------------------------------------------------
#
# Project created by QtCreator 2015-08-15T21:23:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CoinKiller
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sarcfilesystem.cpp \
    externalfilesystem.cpp \
    externalfile.cpp

HEADERS  += mainwindow.h \
    filesystembase.h \
    filebase.h \
    sarcfilesystem.h \
    externalfilesystem.h \
    externalfile.h \
    shit.h

FORMS    += mainwindow.ui
