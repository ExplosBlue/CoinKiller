#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include "objects.h"
#include "tileset.h"

#include <QPainter>

class ObjectRenderer
{
public:
    ObjectRenderer() {}
    ObjectRenderer(Object *) {}
    virtual ~ObjectRenderer() {}
    virtual void render(QPainter *, QRect *drawrect) {}
};

class SpriteRenderer: public ObjectRenderer
{
public:
    SpriteRenderer() {}
    SpriteRenderer(const Sprite *spr, Tileset *tilesets[]);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
    ObjectRenderer *ret;
};

class LiquidRenderer: public ObjectRenderer
{
public:
    LiquidRenderer() {}
    LiquidRenderer(const Sprite *liquid, const Zone *zone);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *liquid;
    const Zone *zone;
    QString filename;
    ObjectRenderer *ret;
};

class NormalImageRenderer: public ObjectRenderer
{
public:
    NormalImageRenderer(const Object *obj, QString filename);
    NormalImageRenderer(QRect rect, QString filename);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QRect rect;
    QString filename;
};

class RoundedRectRenderer: public ObjectRenderer
{
public:
    RoundedRectRenderer();
    RoundedRectRenderer(const Object *obj, QString text, QColor color, QTextOption align = Qt::AlignHCenter | Qt::AlignVCenter);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    QString text;
    QColor color;
    QTextOption align;
};


// Special Sprite Renderers

// Sprite 18: Tile God
class TileGodRenderer: public SpriteRenderer
{
public:
    TileGodRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
    Tileset *tileset;
private:
    int tileIds [12] = { 27, 20, 15, 24, 23, 23, 19, 41, 101, 94, 105, 131 };
};

// Sprite 22: Special Exit Controller
class SpecialExitControllerRenderer: public SpriteRenderer
{
public:
    SpecialExitControllerRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprite 38: Reznor Wheel
class ReznorWheelRenderer: public ObjectRenderer
{
public:
    ReznorWheelRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite* spr;
    QString basePath;
};

// Sprite 39: Reznor Battle Disapearing Blocks
class ReznorBlockRenderer: public ObjectRenderer
{
public:
    ReznorBlockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite* spr;
    QString basePath;
};

// Sprites 84/85/86/87/88: Flags
class FlagRenderer: public ObjectRenderer
{
public:
    FlagRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
private:
    RoundedRectRenderer *rect;
    const Sprite* spr;
};

// Sprite 94: Flipper (One way gate)
class FlipperRenderer: public SpriteRenderer
{
public:
    FlipperRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 97: End of Level Flag
class GoalRenderer: public ObjectRenderer
{
public:
    GoalRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
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
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 114: Floating Box
class FloatingBoxRenderer: public ObjectRenderer
{
public:
    FloatingBoxRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 115: Bullet Bill Launcher
class BulletBillLauncherRenderer: public SpriteRenderer
{
public:
    BulletBillLauncherRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprite 120: Up-Down Mushroom
class UpDownMushroomRenderer: public ObjectRenderer
{
public:
    UpDownMushroomRenderer(const Sprite *spr, QString basePath);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
    QString basePath;
};

// Sprite 123: Boucny Mushroom
class BouncyMushroomRenderer: public SpriteRenderer
{
public:
    BouncyMushroomRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};


// Sprite 124: Mushroom Platform
class MushroomPlatformRenderer: public ObjectRenderer
{
public:
    MushroomPlatformRenderer(const Sprite *spr, QString basePath);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
    QString basePath;
};

// Sprite 134: Ruins Plat Rickshaw
class RuinsRickRenderer: public ObjectRenderer
{
public:
    RuinsRickRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 135: Goomba
class GoombaRenderer: public SpriteRenderer
{
public:
    GoombaRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprite 136: Bone Goomba
class BoneGoombaRenderer: public SpriteRenderer
{
public:
    BoneGoombaRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprite 139: Goomba Tower
class GoombaTowerRenderer: public ObjectRenderer
{
public:
    GoombaTowerRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *top;
    QList<NormalImageRenderer*> middle;
    NormalImageRenderer *bottom;
};

// Sprite 147: 3 Plat Rickshaw
class ThreePlatRickRenderer: public ObjectRenderer
{
public:
    ThreePlatRickRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 154: 4 Plat Rickshaw
class FourPlatRickRenderer: public ObjectRenderer
{
public:
    FourPlatRickRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 165: Koopa Troopa
class KoopaTroopaRenderer: public ObjectRenderer
{
public:
    KoopaTroopaRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 185: Koopa Paratroopa
class KoopaParatroopaRenderer: public ObjectRenderer
{
public:
    KoopaParatroopaRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    NormalImageRenderer *img;
};

// Sprite 189: RecLiftRenderer
class RecLiftRenderer: public SpriteRenderer
{
public:
    RecLiftRenderer(const Sprite *spr, QString path);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
    QString path;
    int sideOffset = 0;
};

// Sprite 215: Bob-omb Cannon
class BobOmbCannonRenderer: public SpriteRenderer
{
public:
    BobOmbCannonRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprite 221/223/225: Switches
class SwitchRenderer: public ObjectRenderer
{
public:
    SwitchRenderer(const Sprite *spr, QString filename);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Object *obj;
    QString filename;
    NormalImageRenderer *img;
};

// Sprite 240: Urchin
class UrchinRenderer: public SpriteRenderer
{
public:
    UrchinRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprite 244: Chain Chomp
class ChainChompRenderer: public SpriteRenderer
{
public:
    ChainChompRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

// Sprites 100/101/203/204/267/275/276: Item Blocks
class ItemBlockRenderer: public SpriteRenderer
{
public:
    ItemBlockRenderer(const Sprite *spr, QString filename);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
    NormalImageRenderer* block;
};

// Sprite 280: + Clock
class ClockRenderer: public SpriteRenderer
{
public:
    ClockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Sprite *spr;
};

class EntranceRenderer : public ObjectRenderer
{
public:
    EntranceRenderer(const Entrance *entrance);
    void render(QPainter *painter, QRect *drawrect);
private:
    RoundedRectRenderer *rect;
    const Entrance* entr;
};

#endif // OBJECTRENDERER_H
