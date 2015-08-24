#ifndef EXTERNALFILESYSTEM_H
#define EXTERNALFILESYSTEM_H


class ExternalFilesystem : public FilesystemBase
{
public:
    ExternalFilesystem(QString base);

    FileBase* openFile(QString path);
    bool save(FileBase* file);


private:
    QString basepath;
};

#endif // EXTERNALFILESYSTEM_H
