#ifndef FILESYSTEMBASE_H
#define FILESYSTEMBASE_H

#include "filebase.h"


class FilesystemBase
{
public:

    virtual FileBase* openFile(char* path)=0;
    virtual bool save()=0;
};


#endif // FILESYSTEMBASE_H

