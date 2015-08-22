#ifndef EXTERNALFILE_H
#define EXTERNALFILE_H

#include "externalfilesystem.h"


class ExternalFile : public FileBase
{
public:
    ExternalFile(ExternalFilesystem* fs, QString path);

    void open();
    void close();
    quint64 readData(quint8* data, quint64 len);
    quint64 writeData(quint8* data, quint64 len);
    quint64 pos();
    bool seek(quint64 pos);
    quint64 size();
    bool resize(quint64 size);


private:
    ExternalFilesystem* parent;
    QFile* file;
};

#endif // EXTERNALFILE_H
