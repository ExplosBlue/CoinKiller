#include "filesystem.h"


FileBase* FileBase::getSubfile(FilesystemBase* container, quint64 offset, quint64 size)
{
    open();
    seek(offset);
    FileBase* ret;

    if (size >= 32*1024*1024)
    {
        // backed by temporary file
        ret = new ExternalFile(container);
        ret->open();
        ret->resize(size);
        ret->seek(0);

        quint8* tempbuf = new quint8[4096];
        quint64 pos = 0;
        while (pos < size)
        {
            quint64 toread = 4096;
            if ((pos+toread) > size) toread = size-pos;

            readData(tempbuf, toread);
            ret->writeData(tempbuf, toread);
            pos += toread;
        }

        ret->close();
        delete[] tempbuf;
    }
    else
    {
        // backed by memory
        quint8* data = new quint8[size];
        readData(data, size);
        ret = new MemoryFile(container, data, size);
    }

    close();
    return ret;
}
