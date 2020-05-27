#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    static void download(const QUrl& url, QObject* signalReceiver, const char* doneSlot, const QString& userAgent = "CoinKiller");

private:
    explicit FileDownloader();
    ~FileDownloader();

    QNetworkAccessManager manager;
    QUrl url;

signals:
    void done(QNetworkReply::NetworkError error, const QByteArray& data, const QUrl& url);

private slots:
    void downloadFinished(QNetworkReply* reply);
};

#endif // FILEDOWNLOADER_H
