#ifndef CTPK_H
#define CTPK_H

#include "filesystem.h"

#include <QImage>


class Ctpk
{
public:
    Ctpk(FileBase* file);

    QImage* getTexture(quint32 num);


private:
    qint32 clampColor(qint32 val)
    {
        if (val > 255) return 255;
        if (val < 0) return 0;
        return val;
    }

    FileBase* file;

    quint16 numTextures;
    quint32 texSectionOffset;
    quint32 texSectionSize;
    quint32 hashSectionOffset;
    quint32 infoSectionOffset;
};

#endif // CTPK_H
