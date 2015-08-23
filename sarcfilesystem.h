#ifndef SARCFILESYSTEM_H
#define SARCFILESYSTEM_H

#include "filesystembase.h"


class SarcFilesystem : public FilesystemBase
{
public:
    SarcFilesystem(FileBase* file);


private:
    struct InternalSarcFile
    {
        QString name;
        quint32 offset, size;

        quint32 nameOffset;
        quint32 nameHash;
    };
};

#endif // SARCFILESYSTEM_H
