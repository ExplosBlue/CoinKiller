/*
    Copyright 2015 StapleButter

    This file is part of CoinKiller.

    CoinKiller is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    CoinKiller is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with CoinKiller. If not, see http://www.gnu.org/licenses/.
*/

#include "settingsmanager.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>
#include <QStandardPaths>
#include <stdexcept>

SettingsManager* SettingsManager::instance = nullptr;

SettingsManager* SettingsManager::init(QWidget* parentWidget) {
    QCoreApplication::setOrganizationName("Blarg City");
    QCoreApplication::setApplicationName("CoinKiller");

    if (instance != nullptr) {
        throw std::runtime_error("SettingsManager already inited.");
    }

    instance = new SettingsManager(parentWidget);
    return instance;
}

SettingsManager* SettingsManager::getInstance() {
    if (instance == nullptr) {
        throw std::runtime_error("SettingsManager not inited.");
    }

    return instance;
}


SettingsManager::SettingsManager(QWidget *parent) :
    QObject(parent) {

    QFileInfo localDataDir(QCoreApplication::applicationDirPath() + "/coinkiller_data");
    if (localDataDir.isDir() && localDataDir.isReadable()) {
        dataBasePath = localDataDir.absoluteFilePath();
    } else {
        dataBasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/coinkiller_data";
        QDir().mkpath(dataBasePath);
    }
}

SettingsManager::~SettingsManager() {

}

QString SettingsManager::dataPath(const QString& path) {
    return QString("%1/%2").arg(dataBasePath).arg(path);
}

void SettingsManager::loadTranslations() {
    QString language = settings.value("Language", "English").toString();
    loadTranslations(language);
}

void SettingsManager::loadTranslations(QString languageName) {
    QString file = dataPath("languages/"+languageName+"/"+languageName);
    bool loaded = translator.load(file);

    if (loaded) {
       qApp->installTranslator(&translator);
    } else {
       qApp->removeTranslator(&translator);
    }


    QStringList translateFiles;
    translateFiles << "entrancetypes.txt"
                   << "levelnames.txt"
                   << "worldnames.txt"
                   << "musicids.txt"
                   << "spritecategories.xml"
                   << "spritedata.xml"
                   << "tilebehaviors.xml"
                   << "tilesetnames.txt";

    foreach (QString transFile, translateFiles) {
        QString path = dataPath("languages/"+languageName+"/"+transFile);
       if(!QFile(path).exists()) {
            path = dataPath(transFile);
       }

        translatedFiles.insert(transFile, path);
    }
}

void SettingsManager::setupLanguageSelector(QListWidget* selector) {
    selector->blockSignals(true);
    selector->clear();

    QDir translationsFolder(dataPath("languages/"));
    translationsFolder.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QDirIterator directories(translationsFolder, QDirIterator::NoIteratorFlags);

    while(directories.hasNext()) {
        directories.next();

        QListWidgetItem* item = new QListWidgetItem(directories.fileName());

        selector->addItem(item);

        if (directories.fileName() == settings.value("Language", "English").toString()) {
            selector->setCurrentItem(item);
        }

    }

    connect(selector, &QListWidget::currentTextChanged, this, &SettingsManager::setLanguage);
    selector->blockSignals(false);
}

QString SettingsManager::getFilePath(QString file) {
    return translatedFiles.value(file);
}

void SettingsManager::setLanguage(QString language) {
    settings.setValue("Language", language);
    loadTranslations(language);
}

QVariant SettingsManager::get(const QString &key, const QVariant &defaultValue) {
    return settings.value(key, defaultValue);
}

void SettingsManager::set(const QString &key, const QVariant &value) {
    settings.setValue(key, value);
}

QColor SettingsManager::getColor(const QString &key, const QColor &defaultColor) {
   return QColor::fromRgba(settings.value(key, defaultColor.rgba()).toUInt());
}

void SettingsManager::setColor(const QString &key, const QColor &color) {
    settings.setValue(key, color.rgba());
}

// Level Editor settings
QColor SettingsManager::getLEWindowColor() {
    return QColor::fromRgba(settings.value("LE_WINDOW_COLOR", LE_WINDOW_COLOR_DEFAULT.rgba()).toUInt());
}

void SettingsManager::setLEWindowColor(const QColor &color) {
    settings.setValue("LE_WINDOW_COLOR", color.rgba());
}

quint32 SettingsManager::getLEUndoLimit() {
    return settings.value("LE_UNDO_LIMIT", LE_UNDO_LIMIT_DEFAULT).toUInt();
}

void SettingsManager::setLEUndoLimit(const quint32 &value) {
    settings.setValue("LE_UNDO_LIMIT", value);
}

bool SettingsManager::getLERenderTransparentLiquidAboveTiles() {
    return settings.value("LE_RENDER_TRANSPARENT_LIQUID_ABOVE_TILES", LE_RENDER_TRANSPARENT_LIQUID_ABOVE_TILES_DEFAULT).toBool();
}

void SettingsManager::setLERenderTransparentLiquidAboveTiles(const bool &value) {
    settings.setValue("LE_RENDER_TRANSPARENT_LIQUID_ABOVE_TILES", value);
}

bool SettingsManager::getLESelectOnPlace() {
    return settings.value("LE_SELECT_ON_PLACE", LE_SELECT_ON_PLACE_DEFAULT).toBool();
}

void SettingsManager::setLESelectOnPlace(const bool &value) {
    settings.setValue("LE_SELECT_ON_PLACE", value);
}

bool SettingsManager::getLEShowStatusbar() {
    return settings.value("LE_SHOW_STATUSBAR", LE_SHOW_STATUSBAR_DEFAULT).toBool();
}

void SettingsManager::setLEShowStatusbar(const bool &value) {
    settings.setValue("LE_SHOW_STATUSBAR", value);
}
