#ifndef BACKGROUNDLIST_H
#define BACKGROUNDLIST_H

#include <QList>
#include <QString>

struct BackgroundEntry
{
    QString fileName;
    QString name;
};

struct BackgroundGroup
{
    QString name;
    QList<BackgroundEntry> entries;
};

class BackgroundList
{
public:
    QList<BackgroundGroup> groups() const;
    QList<BackgroundEntry> entries() const;

    static QString stripExtension(const QString& fileName);
    static QString previewPath(const QString& fileName);
    static QString invalidPreviewPath();
};

#endif // BACKGROUNDLIST_H
