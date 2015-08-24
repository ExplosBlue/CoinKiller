#ifndef FILEBASE
#define FILEBASE


class FileBase
{
public:

    // base shit
    virtual void open()=0;
    virtual void close()=0;
    virtual quint64 readData(quint8* data, quint64 len)=0;
    virtual quint64 writeData(quint8* data, quint64 len)=0;
    virtual quint64 pos()=0;
    virtual bool seek(quint64 pos)=0;
    virtual quint64 size()=0;
    virtual bool resize(quint64 size)=0;


    // conveniency

    quint8 read8()
    {
        quint8 ret;
        readData((quint8*)&ret, 1);
        return ret;
    }

    quint16 read16()
    {
        quint16 ret;
        readData((quint8*)&ret, 2);
        return ret;
    }

    quint32 read32()
    {
        quint32 ret;
        readData((quint8*)&ret, 4);
        return ret;
    }

    quint32 readStringASCII(QString& ret, quint32 len) // len=0 for NULL terminated string
    {
        char temp8[64];
        QChar temp16[64];
        ret.clear();

        if (!len) len = 0xFFFFFFFF; // lazy

        bool terminated = false;
        quint32 lenread = 0;
        while (lenread < len && !terminated)
        {
            quint32 thislen = (lenread+64 > len) ? len : lenread+64;
            readData((quint8*)temp8, thislen);

            quint32 actuallen = 0;
            for (quint32 i = 0; i < thislen; i++)
            {
                temp16[i] = (QChar)temp8[i];
                if (!temp8[i])
                {
                    terminated = true;
                    break;
                }
                actuallen++;
            }

            ret.append(temp16, actuallen);
            lenread += actuallen;
        }

        return lenread;
    }

    void write8(quint8 val)
    {
        writeData((quint8*)&val, 1);
    }

    void write16(quint16 val)
    {
        writeData((quint8*)&val, 2);
    }

    void write32(quint32 val)
    {
        writeData((quint8*)&val, 4);
    }

    void skip(qint64 num)
    {
        seek(pos()+num);
    }


    FileBase* getSubfile(FilesystemBase* container, quint64 offset, quint64 size);


    void setIdPath(QString path)
    {
        if (idPath.isEmpty())
            idPath = path;
    }


protected:
    FilesystemBase* parent;
    QString idPath; // identifies the file in its parent FS

    int openCount;
};


#endif // FILEBASE

