#-------------------------------------------------
#
# Project created by QtCreator 2015-08-15T21:23:38
#
#-------------------------------------------------

KDE_BLUR = 0    # 1 to enable blur (requires KDE)

QT       += core gui\
            xml\
            network

greaterThan(KDE_BLUR, 0) {
    QT      += KWindowSystem
    DEFINES += USE_KDE_BLUR
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CoinKiller
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sarcfilesystem.cpp \
    externalfilesystem.cpp \
    externalfile.cpp \
    memoryfile.cpp \
    filebase.cpp \
    leveleditorwindow.cpp \
    levelview.cpp \
    ctpk.cpp \
    level.cpp \
    tileset.cpp \
    game.cpp \
    objects.cpp \
    unitsconvert.cpp \
    objectrenderer.cpp \
    tileseteditorwindow.cpp \
    tileseteditorwidgets.cpp \
    spritedata.cpp \
    tilesetpalette.cpp \
    entranceeditorwidget.cpp \
    spriteeditorwidget.cpp \
    areaeditorwidget.cpp \
    zoneeditorwidget.cpp \
    locationeditorwidget.cpp \
    levelmanager.cpp \
    patheditorwidget.cpp \
    progresspatheditorwidget.cpp \
    settingsmanager.cpp \
    filedownloader.cpp \
    levelminimap.cpp \
    objectseditionmode.cpp \
    sarcexplorerwindow.cpp \
    newtilesetdialog.cpp \
    imagecache.cpp \
    spriteidswidget.cpp

HEADERS  += mainwindow.h \
    filesystembase.h \
    filebase.h \
    sarcfilesystem.h \
    externalfilesystem.h \
    externalfile.h \
    shit.h \
    memoryfile.h \
    filesystem.h \
    leveleditorwindow.h \
    levelview.h \
    ctpk.h \
    level.h \
    tileset.h \
    game.h \
    objects.h \
    unitsconvert.h \
    objectrenderer.h \
    tileseteditorwindow.h \
    tileseteditorwidgets.h \
    is.h \
    editionmode.h \
    spritedata.h \
    tilesetpalette.h \
    entranceeditorwidget.h \
    spriteeditorwidget.h \
    areaeditorwidget.h \
    zoneeditorwidget.h \
    levelmanager.h \
    patheditorwidget.h \
    progresspatheditorwidget.h \
    locationeditorwidget.h \
    settingsmanager.h \
    filedownloader.h \
    levelminimap.h \
    objectseditionmode.h \
    sarcexplorerwindow.h \
    newtilesetdialog.h \
    imagecache.h \
    spriteidswidget.h

FORMS    += mainwindow.ui \
    leveleditorwindow.ui \
    tileseteditorwindow.ui \
    sarcexplorerwindow.ui \
    newtilesetdialog.ui

CONFIG += c++11
