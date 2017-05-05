#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QHash>
#include <QPixmap>

enum CacheImgType
{
    SpriteImg
};

class ImageCache
{
public:
    static ImageCache* init();
    static ImageCache* getInstance();
   ~ImageCache();

    QPixmap get(CacheImgType type, QString name);

protected:
    ImageCache();

private:
    static ImageCache* instance;

    QHash<CacheImgType, QHash<QString, QPixmap>*> cache;

    void loadTypeFromFolder(CacheImgType type, QString folder);
};

#endif // IMAGECACHE_H
