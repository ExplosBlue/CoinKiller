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

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QHash>
#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QListWidget>

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    static SettingsManager* init(QWidget* parentWidget);
    static SettingsManager* getInstance();
    ~SettingsManager();

    QString dataPath(const QString& path = "");

    void loadTranslations();
    void loadTranslations(QString languageName);
    QString getTranslation(QString category, QString key);
    void setupLanguageSelector(QListWidget* selector);
    QString getFilePath(QString file);

    QString getLastRomFSPath() { return settings.value("LastRomFSPath", "").toString(); }
    void setLastRomFSPath(QString dirpath) { settings.setValue("LastRomFSPath", dirpath); }

    QString getLastSarcFilePath() { return settings.value("LastSarcFilePath", "").toString(); }
    void setLastSarcFilePath(QString dirpath) { settings.setValue("LastSarcFilePath", dirpath); }

    QString getLastSarcExpPath() { return settings.value("LastSarcExpPath", "").toString(); }
    void setLastSarcExpPath(QString dirpath) { settings.setValue("LastSarcExpPath", dirpath); }

    QVariant get(const QString &key, const QVariant &defaultValue = QVariant());
    void set(const QString &key, const QVariant &value);

    QColor getColor(const QString &key, const QColor &defaultColor = Qt::white);
    void setColor(const QString &key, const QColor &color);

protected:
    SettingsManager(QWidget* parentWidget);

private slots:
    void setLanguage(QString language);

private:
    static SettingsManager* instance;

    QSettings settings;

    QString dataBasePath;

    QWidget* parentWidget;
    QHash<QString, QHash<QString, QString>*> translations;
    QHash<QString, QString> translatedFiles;
};

#endif // SETTINGSMANAGER_H
