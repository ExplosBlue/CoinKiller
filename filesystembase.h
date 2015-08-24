#ifndef FILESYSTEMBASE_H
#define FILESYSTEMBASE_H


class FilesystemBase
{
public:

    virtual FileBase* openFile(QString path)=0;
    virtual bool save(FileBase* file)=0;
};


#endif // FILESYSTEMBASE_H

