#ifndef MEMORYFILE_H
#define MEMORYFILE_H


class MemoryFile : public FileBase
{
public:
    MemoryFile(FilesystemBase *fs, quint8* blob, quint32 size);
    ~MemoryFile();

    void open();
    void close();
    quint64 readData(quint8* data, quint64 len);
    quint64 writeData(quint8* data, quint64 len);
    quint64 pos();
    bool seek(quint64 pos);
    quint64 size();
    bool resize(quint64 size);


private:
    quint8* data;
    quint32 _size;

    quint32 _pos;
};

#endif // MEMORYFILE_H
