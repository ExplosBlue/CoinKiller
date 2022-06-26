#ifndef CRC32_H
#define CRC32_H

#include <QtCore>

struct crc32
{
    static void generate_table(quint32(&table)[256])
    {
        quint32 polynomial = 0xEDB88320;
        for (quint32 i = 0; i < 256; i++)
        {
            quint32 c = i;
            for (size_t j = 0; j < 8; j++)
            {
                if (c & 1) {
                    c = polynomial ^ (c >> 1);
                }
                else {
                    c >>= 1;
                }
            }
            table[i] = c;
        }
    }

    static quint32 update(quint32 (&table)[256], quint32 initial, const void* buf, qlonglong len)
    {
        quint32 c = initial ^ 0xFFFFFFFF;
        const quint8* u = static_cast<const quint8*>(buf);
        for (qlonglong i = 0; i < len; ++i)
        {
            c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
        }
        return c ^ 0xFFFFFFFF;
    }
};

#endif // CRC32_H
