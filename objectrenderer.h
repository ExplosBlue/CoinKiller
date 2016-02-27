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
    void render(QPainter *painter);
protected:
    const Sprite *spr;
    ObjectRenderer *ret;
};

class NormalImageRenderer: public ObjectRenderer
{
public:
    NormalImageRenderer(const Object *obj, QString filename);
    NormalImageRenderer(QRect rect, QString filename);
    void render(QPainter *painter);
protected:
    QRect rect;
    QString filename;
};

class RoundedRectRenderer: public ObjectRenderer
{
public:
    RoundedRectRenderer();
    RoundedRectRenderer(const Object *obj, QString text, QColor color, QTextOption align = Qt::AlignHCenter | Qt::AlignVCenter);
    void render(QPainter *painter);
protected:
    const Object *obj;
    QString text;
    QColor color;
    QTextOption align;
};


// Special Sprite Renderers

// Sprite 97: End of Level Flag
class GoalRenderer: public ObjectRenderer
{
public:
    GoalRenderer(const Sprite *spr);
    void render(QPainter *painter);
protected:
    const Object *obj;
    NormalImageRenderer *pole;
    NormalImageRenderer *fort;
};

// Sprite 109: Signboard
class SignboardRenderer: public ObjectRenderer
{
public:
    SignboardRenderer(const Sprite *spr);
    void render(QPainter *painter);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 114: Floating Box
class FloatingBoxRenderer: public ObjectRenderer
{
public:
    FloatingBoxRenderer(const Sprite *spr);
    void render(QPainter *painter);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 139: Goomba Tower
class GoombaTowerRenderer: public ObjectRenderer
{
public:
    GoombaTowerRenderer(const Sprite *spr);
    void render(QPainter *painter);
protected:
    const Object *obj;
    NormalImageRenderer *top;
    QList<NormalImageRenderer*> middle;
    NormalImageRenderer *bottom;
};

// Sprite 165: Koopa Troopa
class KoopaTroopaRenderer: public ObjectRenderer
{
public:
    KoopaTroopaRenderer(const Sprite *spr);
    void render(QPainter *painter);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 221/223/225: Switches
class SwitchRenderer: public ObjectRenderer
{
public:
    SwitchRenderer(const Sprite *spr, QString filename);
    void render(QPainter *painter);
protected:
    const Object *obj;
    QString filename;
    NormalImageRenderer *img;
};

class EntranceRenderer : public ObjectRenderer
{
public:
    EntranceRenderer(const Entrance *entrance);
    void render(QPainter *painter);
private:
    RoundedRectRenderer *rect;
    const Entrance* entr;
};


#endif // OBJECTRENDERER_H
