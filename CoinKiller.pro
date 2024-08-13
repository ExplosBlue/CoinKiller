#-------------------------------------------------
#
# Project created by QtCreator 2015-08-15T21:23:38
#
#-------------------------------------------------

KDE_BLUR = 0    # 1 to enable blur (requires KDE)

greaterThan(KDE_BLUR, 0) {
    QT      += KWindowSystem
    DEFINES += USE_KDE_BLUR
}

greaterThan(QT_MAJOR_VERSION, 5) {
    QT  += core gui\
           xml\
           network\
           widgets
}

TARGET = CoinKiller
RC_FILE = resource/ck_icon.rc
ICON = resource/app.icns
TEMPLATE = app

linux-g++ {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path = $$PREFIX/bin

    ck_data.path = $$PREFIX/bin/coinkiller_data
    ck_data.files += coinkiller_data/*
    desktop.path = $$PREFIX/share/applications/
    desktop.files += resource/io.github.ExplosBlue.CoinKiller.desktop
    icon512.path = $$PREFIX/share/icons/hicolor/512x512/apps
    icon512.files += resource/io.github.ExplosBlue.CoinKiller.png

    INSTALLS += icon512
    INSTALLS += desktop
    INSTALLS += ck_data
    INSTALLS += target
}

isEmpty(CK_VERSION) {
    CK_VERSION = Unknown
}

DEFINES += CK_VERSION=\\\"$$CK_VERSION\\\"

SOURCES += \
    filesystem/externalfile.cpp \
    filesystem/externalfilesystem.cpp \
    filesystem/filebase.cpp \
    filedownloader.cpp \
    filesystem/memoryfile.cpp \
    filesystem/sarcfilesystem.cpp \
    leveleditor/commands/bgdatcommands.cpp \
    leveleditor/commands/editorcommands.cpp \
    leveleditor/commands/entrancecommands.cpp \
    leveleditor/commands/levelcommands.cpp \
    leveleditor/commands/locationcommands.cpp \
    leveleditor/commands/objectcommands.cpp \
    leveleditor/commands/pathcommands.cpp \
    leveleditor/commands/pathnodecommands.cpp \
    leveleditor/commands/progresspathcommands.cpp \
    leveleditor/commands/spritecommands.cpp \
    leveleditor/commands/zonebgcommands.cpp \
    leveleditor/commands/zoneboundingcommands.cpp \
    leveleditor/commands/zonecommands.cpp \
    leveleditor/areaeditorwidget.cpp \
    leveleditor/editmanager.cpp \
    leveleditor/entranceeditorwidget.cpp \
    leveleditor/eventeditorwidget.cpp \
    leveleditor/leveleditorwindow.cpp \
    leveleditor/levelminimap.cpp \
    leveleditor/levelview.cpp \
    leveleditor/locationeditorwidget.cpp \
    leveleditor/patheditorwidget.cpp \
    leveleditor/progresspatheditorwidget.cpp \
    leveleditor/settingsdialog.cpp \
    leveleditor/spriteeditorwidget.cpp \
    leveleditor/spriteidswidget.cpp \
    leveleditor/tilesetpalette.cpp \
    leveleditor/zoneeditorwidget.cpp \
    tileseteditor/tileseteditorwidgets.cpp \
    tileseteditor/tileseteditorwindow.cpp \
    clickablelabel.cpp \
    ctpk.cpp \
    game.cpp \
    imagecache.cpp \
    level.cpp \
    levelmanager.cpp \
    main.cpp\
    mainwindow.cpp \
    newleveldialog.cpp \
    newtilesetdialog.cpp \
    objectrenderer.cpp \
    objects.cpp \
    rg_etc1.cpp \
    sarcexplorerwindow.cpp \
    settingsmanager.cpp \
    spritedata.cpp \
    tileset.cpp \
    unitsconvert.cpp

HEADERS += \
    filesystem/externalfile.h \
    filesystem/externalfilesystem.h \
    filesystem/filebase.h \
    filesystem/filesystem.h \
    filesystem/filesystembase.h \
    filesystem/memoryfile.h \
    filesystem/sarcfilesystem.h \
    leveleditor/commands/bgdatcommands.h \
    leveleditor/commands/commandids.h \
    leveleditor/commands/editorcommands.h \
    leveleditor/commands/entrancecommands.h \
    leveleditor/commands/levelcommands.h \
    leveleditor/commands/locationcommands.h \
    leveleditor/commands/objectcommands.h \
    leveleditor/commands/pathcommands.h \
    leveleditor/commands/pathnodecommands.h \
    leveleditor/commands/progresspathcommands.h \
    leveleditor/commands/setvalue.h \
    leveleditor/commands/spritecommands.h \
    leveleditor/commands/zonebgcommands.h \
    leveleditor/commands/zoneboundingcommands.h \
    leveleditor/commands/zonecommands.h \
    leveleditor/areaeditorwidget.h \
    leveleditor/editmanager.h \
    leveleditor/entranceeditorwidget.h \
    leveleditor/eventeditorwidget.h \
    leveleditor/layermask.h \
    leveleditor/leveleditorwindow.h \
    leveleditor/levelminimap.h \
    leveleditor/levelview.h \
    leveleditor/locationeditorwidget.h \
    leveleditor/patheditorwidget.h \
    leveleditor/progresspatheditorwidget.h \
    leveleditor/settingsdialog.h \
    leveleditor/spriteeditorwidget.h \
    leveleditor/spriteidswidget.h \
    leveleditor/tilesetpalette.h \
    leveleditor/zoneeditorwidget.h \
    tileseteditor/tileseteditorwidgets.h \
    tileseteditor/tileseteditorwindow.h \
    clickablelabel.h \
    crc32.h \
    ctpk.h \
    filedownloader.h \
    game.h \
    imagecache.h \
    is.h \
    level.h \
    levelmanager.h \
    mainwindow.h \
    newleveldialog.h \
    newtilesetdialog.h \
    objectrenderer.h \
    objects.h \
    rg_etc1.h \
    sarcexplorerwindow.h \
    settingsmanager.h \
    shit.h \
    spritedata.h \
    tileset.h \
    unitsconvert.h \
    windowbase.h

FORMS += \
    leveleditor/leveleditorwindow.ui \
    leveleditor/settingsdialog.ui \
    tileseteditor/tileseteditorwindow.ui \
    mainwindow.ui \
    newleveldialog.ui \
    newtilesetdialog.ui \
    sarcexplorerwindow.ui

TRANSLATIONS += \
    resource/translations/English.ts \
    resource/translations/German.ts \
    resource/translations/Italian.ts

CONFIG += c++17
