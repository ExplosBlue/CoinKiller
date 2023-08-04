#ifndef LAYERMASK_H
#define LAYERMASK_H

#include <QtGlobal>

enum LAYER_MASK : quint8
{
    NONE        = 0x00,
    LAYER_ONE   = 0x01,
    LAYER_TWO   = 0x02,
    LAYER_THREE = 0x04,
    ALL         = 0x07
};

#endif // LAYERMASK_H
