#ifndef SARCFILESYSTEM_H
#define SARCFILESYSTEM_H

#include "filesystembase.h"


class SarcFilesystem : public FilesystemBase
{
public:
    SarcFilesystem(FileBase* file);

    // temp.
    FileBase* openFile(QString path) { return NULL; }
    bool save() {}


private:
    struct InternalSarcFile
    {
        QString name;
        quint32 offset, size;

        quint32 nameOffset;
        quint32 nameHash;
    };

    quint32 numFiles;
    quint32 hashMult;

    quint32 sfatOffset;
    quint32 sfntOffset;
    quint32 dataOffset;

    QMap<QString,InternalSarcFile> files;


    quint32 filenameHash(QString &name);
};

#endif // SARCFILESYSTEM_H
