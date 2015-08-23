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

    };
};

#endif // SARCFILESYSTEM_H
