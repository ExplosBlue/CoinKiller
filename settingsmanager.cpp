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

SettingsManager::SettingsManager(QWidget* parentWidget)
{
    this->parentWidget = parentWidget;

    QString language = settings.value("Language", "English").toString();
    loadTranslations(language);
}

SettingsManager::~SettingsManager()
{
    QList<QHash<QString, QString>*> cats = translations.values();

    for (int i = 0; i < cats.length(); i++)
        delete cats[i];
}

void SettingsManager::loadTranslations(QString languageName)
{
    QFile file(QCoreApplication::applicationDirPath() + "/coinkiller_data/languages/"+languageName+"/translations.txt");

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(parentWidget, "CoinKiller", "The language \""+languageName+"\" is not present in the /coinkiller_data/languages/ folder.\n\nEnglish will be selected as fallback language.");
        loadTranslations("English");
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    translations.clear();

    QHash<QString, QString>* actualCat;

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.startsWith(';'))
            continue;

        else if (line.startsWith('['))
        {
            QString actualCatName = line.mid(1, line.length()-2);
            QHash<QString, QString>* cat = new QHash<QString, QString>;
            actualCat = cat;
            translations.insert(actualCatName, cat);
        }

        else if (!line.isEmpty())
        {
            QStringList fields = line.split('=');
            if (fields.size() == 2)
                actualCat->insert(fields[0], fields[1]);
        }
    }

    QStringList translateFiles;
    translateFiles << "entrancetypes.txt"
                   << "levelnames.xml"
                   << "musicids.txt"
                   << "spritecategories.xml"
                   << "spritedata.xml"
                   << "tilebehaviors.xml"
                   << "tilesetnames.txt";

    foreach (QString transFile, translateFiles)
    {
        QString path = QCoreApplication::applicationDirPath() + "/coinkiller_data/languages/"+languageName+"/"+transFile;
        if(!QFile(path).exists())
            path = QCoreApplication::applicationDirPath() + "/coinkiller_data/" + transFile;

        translatedFiles.insert(transFile, path);
    }

    file.close();
}

QString SettingsManager::getTranslation(QString category, QString key)
{
    return translations.value(category, new QHash<QString, QString>)->value(key, "<NOT LOADED>");
}

void SettingsManager::setupLanguageSelector(QListWidget* selector)
{
    selector->blockSignals(true);
    selector->clear();

    QDir translationsFolder(QCoreApplication::applicationDirPath() + "/coinkiller_data/languages/");
    translationsFolder.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QDirIterator directories(translationsFolder, QDirIterator::NoIteratorFlags);

    while(directories.hasNext())
    {
        directories.next();
        if (QFile(directories.filePath()+"/translations.txt").exists())
        {
            QListWidgetItem* item = new QListWidgetItem(directories.fileName());

            selector->addItem(item);

            if (directories.fileName() == settings.value("Language", "English").toString())
                selector->setCurrentItem(item);
        }
    }

    connect(selector, SIGNAL(currentTextChanged(QString)), this, SLOT(setLanguage(QString)));
    selector->blockSignals(false);
}

QString SettingsManager::getFilePath(QString file)
{
    return translatedFiles.value(file);
}

void SettingsManager::setLanguage(QString language)
{
    settings.setValue("Language", language);
    QMessageBox::information(parentWidget, "CoinKiller", QString("The Language has been changed to %1.\n\nPlease restart CoinKiller for the changes to take effect.").arg(language));
}
