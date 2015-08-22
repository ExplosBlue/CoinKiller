#include "sarcfilesystem.h"

SarcFilesystem::SarcFilesystem(FileBase* file)
{
    quint32 header[8];
    file->open();
    file->readData((quint8*)header, 8*4);
    qDebug("SARC: %08X %08X %08X %08X | %08X %08X %08X %08X",
           header[0], header[1], header[2], header[3],
           header[4], header[5], header[6], header[7]);
    file->close();
}

