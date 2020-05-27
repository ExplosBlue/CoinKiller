#include "filedownloader.h"


FileDownloader::FileDownloader() : QObject(0)
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
}

FileDownloader::~FileDownloader() { }


void FileDownloader::download(const QUrl& url, QObject* signalReceiver, const char* doneSlot, const QString& userAgent)
{
    FileDownloader* fd = new FileDownloader();

    connect(fd, SIGNAL(done(QNetworkReply::NetworkError, const QByteArray&, const QUrl&)), signalReceiver, doneSlot);

    fd->url = url;

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", userAgent.toUtf8());

    fd->manager.get(request);
}


void FileDownloader::downloadFinished(QNetworkReply* reply)
{
    emit done(reply->error(), reply->readAll(), url);

    disconnect();
    deleteLater();
}
