#include "filesystem.h"

ExternalFilesystem::ExternalFilesystem(QString base)
{
    basepath = base;
}


// path be like: /game/course/level.bin
// (completely fictive filename invented for the sake of this example)
FileBase* ExternalFilesystem::openFile(QString path)
{
    //QFile file(path);
    //if (!file.exists()) throw new std::runtime_exception("File doesn't exist");
    return new ExternalFile(this, basepath + path);
}

bool ExternalFilesystem::save(FileBase *file)
{
    // no real need to do shit here!
    return true;
}

