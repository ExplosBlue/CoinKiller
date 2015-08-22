
#include "externalfile.h"

ExternalFile::ExternalFile(ExternalFilesystem* fs, QString path)
{
    parent = fs;
    file = new QFile(path);
}


void ExternalFile::open()
{
    file->open(QIODevice::ReadWrite);
}

void ExternalFile::close()
{
    file->close();
    parent->save(); // we never know
}

quint64 ExternalFile::readData(quint8* data, quint64 len)
{
    return file->read((char*)data, len);
}

quint64 ExternalFile::writeData(quint8* data, quint64 len)
{
    return file->write((const char*)data, len);
}

quint64 ExternalFile::pos()
{
    return file->pos();
}

bool ExternalFile::seek(quint64 pos)
{
    return file->seek(pos);
}

quint64 ExternalFile::size()
{
    return file->size();
}

bool ExternalFile::resize(quint64 size)
{
    return file->resize(size);
}

