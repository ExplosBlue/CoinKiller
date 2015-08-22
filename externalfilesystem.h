#ifndef EXTERNALFILESYSTEM_H
#define EXTERNALFILESYSTEM_H

#include "filesystembase.h"


class ExternalFilesystem : public FilesystemBase
{
public:
    ExternalFilesystem(QString base);

    FileBase* openFile(QString path);
    bool save();


private:
    QString basepath;
};

#endif // EXTERNALFILESYSTEM_H
