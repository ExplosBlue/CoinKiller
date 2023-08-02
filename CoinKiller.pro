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
    desktop.files += resource/coinkiller.desktop
    icon512.path = $$PREFIX/share/icons/hicolor/512x512/apps
    icon512.files += resource/ck_icon.png

    INSTALLS += icon512
    INSTALLS += desktop
    INSTALLS += ck_data
    INSTALLS += target
}

isEmpty(CK_VERSION) {
    CK_VERSION = Unknown
}

DEFINES += CK_VERSION=\\\"$$CK_VERSION\\\"

SOURCES += main.cpp\
    EditorCommands/changebgdatobject.cpp \
    EditorCommands/changespriteid.cpp \
    EditorCommands/deletebgdatobject.cpp \
    EditorCommands/deleteentrance.cpp \
    EditorCommands/deletelocation.cpp \
    EditorCommands/deletepathnode.cpp \
    EditorCommands/deleteprogresspathnode.cpp \
    EditorCommands/deletesprite.cpp \
    EditorCommands/deletezone.cpp \
    EditorCommands/entrancecommands.cpp \
    EditorCommands/increaseobjectposition.cpp \
    EditorCommands/increaseobjectsize.cpp \
    EditorCommands/insertbgdatobj.cpp \
    EditorCommands/insertentrance.cpp \
    EditorCommands/insertlocation.cpp \
    EditorCommands/insertpath.cpp \
    EditorCommands/insertpathnode.cpp \
    EditorCommands/insertprogresspath.cpp \
    EditorCommands/insertprogresspathnode.cpp \
    EditorCommands/insertsprite.cpp \
    EditorCommands/insertzone.cpp \
    EditorCommands/insertzonebackground.cpp \
    EditorCommands/insertzonebounding.cpp \
    EditorCommands/locationcommands.cpp \
    EditorCommands/lowerlayer.cpp \
    EditorCommands/lowerobject.cpp \
    EditorCommands/moveobject.cpp \
    EditorCommands/raiselayer.cpp \
    EditorCommands/raiseobject.cpp \
    EditorCommands/resizeobject.cpp \
    EditorCommands/setentranceinteraction.cpp \
    EditorCommands/setlayermask.cpp \
    EditorCommands/setlocationinteraction.cpp \
    EditorCommands/setpathinteraction.cpp \
    EditorCommands/setspriteinteraction.cpp \
    EditorCommands/settileset.cpp \
    EditorCommands/spritecommands.cpp \
    EditorCommands/zonebgcommands.cpp \
    EditorCommands/zoneboundingcommands.cpp \
    EditorCommands/zonecommands.cpp \
    editmanager.cpp \
    eventeditorwidget.cpp \
    mainwindow.cpp \
    newleveldialog.cpp \
    rg_etc1.cpp \
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
    sarcexplorerwindow.cpp \
    newtilesetdialog.cpp \
    imagecache.cpp \
    spriteidswidget.cpp \
    clickablelabel.cpp

HEADERS  += mainwindow.h \
    EditorCommands/changebgdatobject.h \
    EditorCommands/changespriteid.h \
    EditorCommands/commandids.h \
    EditorCommands/deletebgdatobject.h \
    EditorCommands/deleteentrance.h \
    EditorCommands/deletelocation.h \
    EditorCommands/deletepathnode.h \
    EditorCommands/deleteprogresspathnode.h \
    EditorCommands/deletesprite.h \
    EditorCommands/deletezone.h \
    EditorCommands/editorcommands.h \
    EditorCommands/entrancecommands.h \
    EditorCommands/increaseobjectposition.h \
    EditorCommands/increaseobjectsize.h \
    EditorCommands/insertbgdatobj.h \
    EditorCommands/insertentrance.h \
    EditorCommands/insertlocation.h \
    EditorCommands/insertpath.h \
    EditorCommands/insertpathnode.h \
    EditorCommands/insertprogresspath.h \
    EditorCommands/insertprogresspathnode.h \
    EditorCommands/insertsprite.h \
    EditorCommands/insertzone.h \
    EditorCommands/insertzonebackground.h \
    EditorCommands/insertzonebounding.h \
    EditorCommands/locationcommands.h \
    EditorCommands/lowerlayer.h \
    EditorCommands/lowerobject.h \
    EditorCommands/moveobject.h \
    EditorCommands/raiselayer.h \
    EditorCommands/raiseobject.h \
    EditorCommands/resizeobject.h \
    EditorCommands/setentranceinteraction.h \
    EditorCommands/setlayermask.h \
    EditorCommands/setlocationinteraction.h \
    EditorCommands/setpathinteraction.h \
    EditorCommands/setspriteinteraction.h \
    EditorCommands/settileset.h \
    EditorCommands/setvalue.h \
    EditorCommands/spritecommands.h \
    EditorCommands/zonebgcommands.h \
    EditorCommands/zoneboundingcommands.h \
    EditorCommands/zonecommands.h \
    crc32.h \
    editmanager.h \
    eventeditorwidget.h \
    filesystembase.h \
    filebase.h \
    layermask.h \
    newleveldialog.h \
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
    sarcexplorerwindow.h \
    newtilesetdialog.h \
    imagecache.h \
    spriteidswidget.h \
    clickablelabel.h \
    windowbase.h \
    rg_etc1.h \

FORMS    += mainwindow.ui \
    leveleditorwindow.ui \
    newleveldialog.ui \
    tileseteditorwindow.ui \
    sarcexplorerwindow.ui \
    newtilesetdialog.ui

TRANSLATIONS += English.ts \
    German.ts \
    Italian.ts

CONFIG += c++17
