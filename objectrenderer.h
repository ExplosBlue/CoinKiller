#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include "objects.h"

#include <QPainter>

class ObjectRenderer
{
public:
    ObjectRenderer();
    ObjectRenderer(Object *obj);
    virtual ~ObjectRenderer();
    virtual void render(QPainter *painter);
};

class SpriteRenderer
{
public:
    SpriteRenderer();
    SpriteRenderer(const Sprite *spr);
    inline ~SpriteRenderer();
    void render(QPainter *painter);
protected:
    const Sprite *spr;
    ObjectRenderer *ret;
};

class NormalImageRenderer: public ObjectRenderer
{
public:
    NormalImageRenderer();
    NormalImageRenderer(const Object *obj, QString filename);
    ~NormalImageRenderer();
    void render(QPainter *painter);
protected:
    const Object *obj;
    QString filename;
};

class RoundedRectRenderer: public ObjectRenderer
{
public:
    RoundedRectRenderer();
    RoundedRectRenderer(const Object *obj, QString text, QColor color);
    void render(QPainter *painter);
protected:
    const Object *obj;
    QString text;
    QColor color;
};

#endif // OBJECTRENDERER_H
