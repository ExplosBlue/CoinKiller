#include "sarcfilesystem.h"

SarcFilesystem::SarcFilesystem(FileBase* file)
{
    quint32 header[8];

    // TODO: someshit if the file was already opened??
    file->open();

    /*file->readData((quint8*)header, 8*4);
    qDebug("SARC: %08X %08X %08X %08X | %08X %08X %08X %08X",
           header[0], header[1], header[2], header[3],
           header[4], header[5], header[6], header[7]);*/
    file->seek(0);
    quint32 tag = file->read32();
    if (tag != 0x43524153)
    {
        qDebug("SARC: bad tag %08X", tag);
        return; // TODO proper error handling!!!
    }

    file->seek(0x98);
    QString perefouras;
    file->readStringASCII(perefouras, 0);
    file->seek(0xB4);
    QString maitredestenebres;
    file->readStringASCII(maitredestenebres, 0);

    qDebug("père fouras");
    qDebug(perefouras.toStdString().c_str());
    qDebug("maître des ténèbres");
    qDebug(maitredestenebres.toStdString().c_str());

    file->close();
}

