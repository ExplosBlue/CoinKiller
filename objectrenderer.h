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
    virtual void render(QPainter *, QRect *) {}
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
    void renderTranslucent(QPainter *painter, QRect *drawrect);
    void drawLiquid(bool transparent, int yOffset, bool topless);
protected:
    const Sprite *liquid;
    const Zone *zone;
    QString filename;
    QPainter *painter;
    QRect *drawrect;
};

class NormalImageRenderer: public ObjectRenderer
{
public:
    NormalImageRenderer(const Object *obj, QString name);
    NormalImageRenderer(QRect rect, QString name);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QRect rect;
    QString name;
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

class CircleRenderer: public ObjectRenderer
{
public:
    CircleRenderer();
    CircleRenderer(int offsetx, int offsety, int width, int height, QString text, QColor color, QTextOption align = Qt::AlignHCenter | Qt::AlignVCenter);
    void render(QPainter *painter, QRect *drawellipse);
protected:
    QString text;
    QTextOption align;
    QColor color;
    int width;
    int height;
    int offsetx;
    int offsety;
};

class MovIndicatorRenderer: public ObjectRenderer
{
public:
    MovIndicatorRenderer();
    MovIndicatorRenderer(int x, int y, int endX, int endY, bool vertical, QColor color);
    using ObjectRenderer::render;
    void render(QPainter *painter);
protected:
    int x;
    int y;
    int endX;
    int endY;
    bool vertical;
    QColor color;
};


// Special Sprite Renderers

// Sprite 1: Water Flow For Pipe
class PipeFlowRenderer: public SpriteRenderer
{
public:
    PipeFlowRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 2: Downwards Water Pull
class DownWaterPullRenderer: public SpriteRenderer
{
public:
    DownWaterPullRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 4/5/6/7: Burner Right/Down/Left/Up
class BurnerRenderer: public SpriteRenderer
{
public:
    BurnerRenderer(const Sprite *spr, QString name);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString name;
};

// Sprite 9: Whomp
class WhompRenderer: public SpriteRenderer
{
public:
    WhompRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 18: Tile God
class TileGodRenderer: public SpriteRenderer
{
public:
    TileGodRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
private:
    int tileIds [12] = { 27, 20, 15, 24, 23, 23, 19, 41, 101, 94, 105, 131 };
};

// Sprite 20: Gold Block
class GoldBlockRenderer: public SpriteRenderer
{
public:
    GoldBlockRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
};

// Sprite 21: Note Block
class NoteBlockRenderer: public SpriteRenderer
{
public:
    NoteBlockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 22: Special Exit Controller
class SpecialExitControllerRenderer: public SpriteRenderer
{
public:
    SpecialExitControllerRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 24: Flying Question Block
class FlyingQuestionBlockRenderer: public SpriteRenderer
{
public:
    FlyingQuestionBlockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};


// Sprite 26: Roulette Block
class RouletteBlockRenderer: public SpriteRenderer
{
public:
    RouletteBlockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString contents;
};

// Sprite 36: Arc Lava Bubble
class ArcLavaBubbleRenderer: public SpriteRenderer
{
public:
    ArcLavaBubbleRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};


// Sprite 38: Reznor Wheel
class ReznorWheelRenderer: public SpriteRenderer
{
public:
    ReznorWheelRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 28/60/101/204: Movement Controlled Sprites
class MovementSpriteRenderer: public SpriteRenderer
{
public:
    MovementSpriteRenderer(const Sprite *spr, QString fileName);
    ~MovementSpriteRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 39: Reznor Battle Disapearing Blocks
class ReznorBlockRenderer: public SpriteRenderer
{
public:
    ReznorBlockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 44: Track-controlled Burner - Four Directions
class FourBurnerRenderer: public SpriteRenderer
{
public:
    FourBurnerRenderer(const Sprite *spr);
    ~FourBurnerRenderer() { delete up; delete down; delete left; delete right; delete center; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *up;
    NormalImageRenderer *down;
    NormalImageRenderer *left;
    NormalImageRenderer *right;
    NormalImageRenderer *center;
};

// Sprite 51: Fuzzy
class FuzzyRenderer: public SpriteRenderer
{
public:
    FuzzyRenderer(const Sprite *spr);
    ~FuzzyRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 59/62/100/203/220/259/303: Rotation Controlled Sprites
class RotationSpriteRenderer: public SpriteRenderer
{
public:
    RotationSpriteRenderer(const Sprite *spr, QString fileName);
    ~RotationSpriteRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 61: RedCoin
class RedCoinRenderer: public SpriteRenderer
{
public:
    RedCoinRenderer(const Sprite *spr, QString filename);
    ~RedCoinRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString filename;
    NormalImageRenderer *img;
};

// Sprite 63/64: Skewer
class SkewerRenderer: public SpriteRenderer
{
public:
    SkewerRenderer(const Sprite* spr);
    void render(QPainter* painter, QRect*);
protected:
    const Sprite* spr;
};

// Sprite 78: Firebar
class FireBarRenderer: public SpriteRenderer
{
public:
    FireBarRenderer(const Sprite *spr);
    ~FireBarRenderer() { delete radius; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    CircleRenderer *radius;
};

// Sprites 84/85/86/87/88: Flags
class FlagRenderer: public SpriteRenderer
{
public:
    FlagRenderer(const Sprite *spr);
    ~FlagRenderer() { delete rect; }
    void render(QPainter *painter, QRect *drawrect);
private:
    RoundedRectRenderer *rect;
};

// Sprite 93: Scuttlebug
class ScuttlebugRenderer: public SpriteRenderer
{
public:
    ScuttlebugRenderer(const Sprite *spr, const Zone *zone);
    void render(QPainter *painter, QRect *);
protected:
    const Zone *zone;
};

// Sprite 94: Flipper (One way gate)
class FlipperRenderer: public SpriteRenderer
{
public:
    FlipperRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 95: Blooper
class BlooperRenderer: public SpriteRenderer
{
public:
    BlooperRenderer(const Sprite *spr);
    ~BlooperRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 97: End of Level Flag
class GoalRenderer: public SpriteRenderer
{
public:
    GoalRenderer(const Sprite *spr);
    ~GoalRenderer() { delete pole; delete fort; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *pole;
    NormalImageRenderer *fort;
};

// Sprite 107: Path-Controlled Climbable Fence
class PathContFenceRenderer: public SpriteRenderer
{
public:
    PathContFenceRenderer(const Sprite *spr);
    ~PathContFenceRenderer() { delete rect; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    RoundedRectRenderer *rect;
};

// Sprite 109: Signboard
class SignboardRenderer: public SpriteRenderer
{
public:
    SignboardRenderer(const Sprite *spr);
    ~SignboardRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 114: Floating Box
class FloatingBoxRenderer: public SpriteRenderer
{
public:
    FloatingBoxRenderer(const Sprite *spr);
    ~FloatingBoxRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 115: Bullet Bill Launcher
class BulletBillLauncherRenderer: public SpriteRenderer
{
public:
    BulletBillLauncherRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 115: Bullet Bill Turret
class BulletBillTurretRenderer: public SpriteRenderer
{
public:
    BulletBillTurretRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 118: Bullet Bill
class BulletBillRenderer: public SpriteRenderer
{
public:
    BulletBillRenderer(const Sprite *spr);
    ~BulletBillRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 119: Banzai Bill
class BanzaiBillRenderer: public SpriteRenderer
{
public:
    BanzaiBillRenderer(const Sprite *spr);
    ~BanzaiBillRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 120: Up-Down Mushroom
class UpDownMushroomRenderer: public SpriteRenderer
{
public:
    UpDownMushroomRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 121: Expanding Mushroom
class ExpandMushroomRenderer: public SpriteRenderer
{
public:
    ExpandMushroomRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 123: Boucny Mushroom
class BouncyMushroomRenderer: public SpriteRenderer
{
public:
    BouncyMushroomRenderer(const Sprite *spr);
    ~BouncyMushroomRenderer() { delete img; }
    void render(QPainter* painter, QRect* drawrect);
protected:
    NormalImageRenderer *img;
};


// Sprite 124: Mushroom Platform
class MushroomPlatformRenderer: public SpriteRenderer
{
public:
    MushroomPlatformRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);

};

// Sprite 127: Bowser Flame
class BowserFlameRenderer: public SpriteRenderer
{
public:
    BowserFlameRenderer(const Sprite *spr);
    ~BowserFlameRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 131: Bowser Block
class BowserBlockRenderer: public SpriteRenderer
{
public:
    BowserBlockRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
};


// Sprite 134: Ruins Plat Rickshaw
class RuinsRickRenderer: public SpriteRenderer
{
public:
    RuinsRickRenderer(const Sprite *spr);
    ~RuinsRickRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 135: Goomba
class GoombaRenderer: public SpriteRenderer
{
public:
    GoombaRenderer(const Sprite *spr);
    ~GoombaRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 136: Bone Goomba
class BoneGoombaRenderer: public SpriteRenderer
{
public:
    BoneGoombaRenderer(const Sprite *spr);
    ~BoneGoombaRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 139: Goomba Tower
class GoombaTowerRenderer: public SpriteRenderer
{
public:
    GoombaTowerRenderer(const Sprite *spr);
    ~GoombaTowerRenderer() { delete top; qDeleteAll(middle); delete bottom; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *top;
    QList<NormalImageRenderer*> middle;
    NormalImageRenderer *bottom;
};

// Sprite 144/145: Horizontal/Vertical Lift
class LiftRenderer: public SpriteRenderer
{
public:
    LiftRenderer(const Sprite *spr, QString dirname);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString dirname;
};

// Sprite 146: Track Controlled Lift
class TrackLiftRenderer: public SpriteRenderer
{
public:
    TrackLiftRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 147: 3 Plat Rickshaw
class ThreePlatRickRenderer: public SpriteRenderer
{
public:
    ThreePlatRickRenderer(const Sprite *spr);
    ~ThreePlatRickRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 151: Scale Platform
class ScalePlatformRenderer: public SpriteRenderer
{
public:
    ScalePlatformRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 152: Path Controlled Lift With Peepa
class PeepaLiftRenderer: public SpriteRenderer
{
public:
    PeepaLiftRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 154: 4 Plat Rickshaw
class FourPlatRickRenderer: public SpriteRenderer
{
public:
    FourPlatRickRenderer(const Sprite *spr);
    ~FourPlatRickRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 156/157: Vertical Camera Limits
class VCameraLimitRenderer: public SpriteRenderer
{
public:
    VCameraLimitRenderer(int leftX, int rightX, int yPos, int yRenderOffset, bool permiable, bool isBelow);
    void render(QPainter *painter, QRect *drawrect);
protected:
    int leftX;
    int rightX;
    int yPos;
    int yRenderOffset;
    bool permiable;
    bool isBelow;
};

// Sprite 158: Buzzy Beetle
class BuzzyBeetleRenderer: public SpriteRenderer
{
public:
    BuzzyBeetleRenderer(const Sprite *spr);
    ~BuzzyBeetleRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 159: Spike Top
class SpikeTopRenderer: public SpriteRenderer
{
public:
    SpikeTopRenderer(const Sprite *spr);
    ~SpikeTopRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 160/161: Horizontal Camera Limits
class HCameraLimitRenderer: public SpriteRenderer
{
public:
    HCameraLimitRenderer(int topY, int bottomY, int xPos, int xRenderOffset, bool permiable, bool isRight);
    void render(QPainter *painter, QRect *drawrect);
protected:
    int topY;
    int bottomY;
    int xPos;
    int xRenderOffset;
    bool permiable;
    bool isRight;
};

// Sprite 163: Climbing Koopa - Horizontal
class ClimbKoopaHorRenderer: public SpriteRenderer
{
public:
    ClimbKoopaHorRenderer(const Sprite *spr);
    ~ClimbKoopaHorRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 164: Climbing Koopa - Vertical
class ClimbKoopaVertRenderer: public SpriteRenderer
{
public:
    ClimbKoopaVertRenderer(const Sprite *spr);
    ~ClimbKoopaVertRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 165: Koopa Troopa
class KoopaTroopaRenderer: public SpriteRenderer
{
public:
    KoopaTroopaRenderer(const Sprite *spr);
    ~KoopaTroopaRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 175/176/177/178/179/180: Grounded Piranha Plants
class PlantRenderer: public SpriteRenderer
{
public:
    PlantRenderer(const Sprite *spr, QString filename);
    ~PlantRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString filename;
    NormalImageRenderer *img;
};

// Sprite 185: Koopa Paratroopa
class KoopaParatroopaRenderer: public SpriteRenderer
{
public:
    KoopaParatroopaRenderer(const Sprite *spr);
    ~KoopaParatroopaRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 187: Path Controlled Rect Lift
class PathRecLiftRenderer: public SpriteRenderer
{
public:
    PathRecLiftRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
};

// Sprite 189: RecLiftRenderer
class RecLiftRenderer: public SpriteRenderer
{
public:
    RecLiftRenderer(const Sprite *spr, QString path);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString path;
    int sideOffset = 0;
};

// Sprite 194: Cheep Cheep
class CheepCheepRenderer: public SpriteRenderer
{
public:
    CheepCheepRenderer(const Sprite *spr);
    ~CheepCheepRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 195: Big Cheep Cheep
class BigCheepRenderer: public SpriteRenderer
{
public:
    BigCheepRenderer(const Sprite *spr);
    ~BigCheepRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 197: Jumping Cheep Cheep
class JumpingCheepRenderer: public SpriteRenderer
{
public:
    JumpingCheepRenderer(const Sprite *spr);
    ~JumpingCheepRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 200: Spiny Cheep Cheep
class SpinyCheepRenderer: public SpriteRenderer
{
public:
    SpinyCheepRenderer(const Sprite *spr);
    ~SpinyCheepRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 209: Swinging Rope
class SwingingRopeRenderer: public SpriteRenderer
{
public:
    SwingingRopeRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 210: Tightrope
class TightropeRenderer: public SpriteRenderer
{
public:
    TightropeRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 213: Pokey
class PokeyRenderer: public SpriteRenderer
{
public:
    PokeyRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 217/218: SnakeBlock
class SnakeBlockRenderer: public SpriteRenderer
{
public:
    SnakeBlockRenderer(const Sprite *spr, const Path *path);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Path *path;
};

// Sprite 215: Bob-omb Cannon
class BobOmbCannonRenderer: public SpriteRenderer
{
public:
    BobOmbCannonRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 221/223/225: Switches
class SwitchRenderer: public SpriteRenderer
{
public:
    SwitchRenderer(const Sprite *spr, QString filename);
    ~SwitchRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString filename;
    NormalImageRenderer *img;
};

// Sprite 222/224/226: Bricks With Switches
class BrickBlockSwitchRenderer: public SpriteRenderer
{
public:
    BrickBlockSwitchRenderer(const Sprite *spr, QString filename, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString filename;
    Tileset *tileset;
};

// Sprite 230: Big Boo
class BigBooRenderer: public SpriteRenderer
{
public:
    BigBooRenderer(const Sprite *spr);
    ~BigBooRenderer() {}
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 186/199/230: Paratroopa/CheepCheep/Peepa Circle
class CoinCircleRenderer: public SpriteRenderer
{
public:
    CoinCircleRenderer(const Sprite *spr, Tileset* tileset);
    ~CoinCircleRenderer() { delete circle; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    CircleRenderer *circle;
    Tileset *tileset;
};

// Sprite 231: Boohemoth
class BoohemothRenderer: public SpriteRenderer
{
public:
    BoohemothRenderer(const Sprite *spr);
    ~BoohemothRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 232: Spiny
class SpinyRenderer: public SpriteRenderer
{
public:
    SpinyRenderer(const Sprite *spr);
    ~SpinyRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 233: Ceiling Spiny
class CeilingSpinyRenderer: public SpriteRenderer
{
public:
    CeilingSpinyRenderer(const Sprite *spr);
    ~CeilingSpinyRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 239: Swinging Vine
class SwingingVineRenderer: public SpriteRenderer
{
public:
    SwingingVineRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 240: Urchin
class UrchinRenderer: public SpriteRenderer
{
public:
    UrchinRenderer(const Sprite *spr);
    ~UrchinRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 241: Rotating Urchin
class RotatingUrchinRenderer: public SpriteRenderer
{
public:
    RotatingUrchinRenderer(const Sprite *spr);
    ~RotatingUrchinRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 243: Ghost Wall
class GhostWallRenderer: public SpriteRenderer
{
public:
    GhostWallRenderer(const Sprite *spr);
    ~GhostWallRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 244: Chain Chomp
class ChainChompRenderer: public SpriteRenderer
{
public:
    ChainChompRenderer(const Sprite *spr);
    ~ChainChompRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 247: Invisible Bouncy Block
class InvisibleBouncyBlockRenderer: public SpriteRenderer
{
public:
    InvisibleBouncyBlockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 248: Move Once On Ghost House Platform
class MoveOnGhostPlatRenderer: public SpriteRenderer
{
public:
    MoveOnGhostPlatRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 249: Rotation Controlled Rectangle Lift
class RotRecLiftRenderer: public SpriteRenderer
{
public:
    RotRecLiftRenderer(const Sprite *spr, QString path);
    void render(QPainter *painter, QRect *drawrect);
protected:
    QString path;
};

// Sprites 100/101/203/204/267/275/276: Item Blocks
class ItemBlockRenderer: public SpriteRenderer
{
public:
    ItemBlockRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
};

// Sprite 252: Item
class ItemRenderer: public SpriteRenderer
{
public:
    ItemRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 253: Larry Battle Platform
class LarryPlatformRenderer: public SpriteRenderer
{
public:
    LarryPlatformRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
};

// Sprite 257: Movement-Controlled Bone Platform
class BonePlatformRenderer: public SpriteRenderer
{
public:
    BonePlatformRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect* drawrect);
};

// Sprite 259: Rotation Controlled Hard Block
class HardBlockRenderer: public SpriteRenderer
{
public:
    HardBlockRenderer(const Sprite *spr, Tileset *tileset);
    void render(QPainter *painter, QRect *drawrect);
protected:
    Tileset *tileset;
};

// Sprite 260/261/262/263/264/256: Boss Cutscene Controllers
class BossControllerRenderer: public SpriteRenderer
{
public:
    BossControllerRenderer(const Sprite *spr, const Zone *zone);
    void render(QPainter *painter, QRect *drawrect);
protected:
    const Zone *zone;
};

// Sprite 268: Rectangle Lift - Lava
class LavaRectLiftRenderer: public SpriteRenderer
{
public:
    LavaRectLiftRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 273: Coin Roulette Block
class CoinRouletteRenderer: public SpriteRenderer
{
public:
    CoinRouletteRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 280: + Clock
class ClockRenderer: public SpriteRenderer
{
public:
    ClockRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};


// Sprites 283/307/308: Event Activated Rectangle Lifts
class EventRecLiftRenderer: public SpriteRenderer
{
public:
    EventRecLiftRenderer(const Sprite *spr, QString path);
    ~EventRecLiftRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 297: Horizontal Rail Controlled Fence
class RailContFenceRenderer: public SpriteRenderer
{
public:
    RailContFenceRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

// Sprite 298: Bouncy Mushroom - Castle
class BounceMushCastleRenderer: public SpriteRenderer
{
public:
    BounceMushCastleRenderer(const Sprite *spr);
    ~BounceMushCastleRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 305: Path Controlled Ice Lift
class IceLiftRenderer: public SpriteRenderer
{
public:
    IceLiftRenderer(const Sprite *spr);
    ~IceLiftRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer* img;
};

// Sprite 311: Coin Meteor
class CoinMeteorRenderer: public SpriteRenderer
{
public:
    CoinMeteorRenderer(const Sprite *spr);
    ~CoinMeteorRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 313: Underwater Rectangle Lift
class UnderwaterRecLiftRenderer: public SpriteRenderer
{
public:
    UnderwaterRecLiftRenderer(const Sprite *spr);
    ~UnderwaterRecLiftRenderer() { delete img; }
    void render(QPainter *painter, QRect *drawrect);
protected:
    NormalImageRenderer *img;
};

// Sprite 320: Path Controlled Lift
class PathLiftRenderer: public SpriteRenderer
{
public:
    PathLiftRenderer(const Sprite *spr);
    void render(QPainter *painter, QRect *drawrect);
};

class EntranceRenderer : public ObjectRenderer
{
public:
    EntranceRenderer(const Entrance *entrance);
    ~EntranceRenderer() { delete rect; }
    void render(QPainter *painter, QRect *drawrect);
private:
    RoundedRectRenderer *rect;
    const Entrance* entr;
};

#endif // OBJECTRENDERER_H
