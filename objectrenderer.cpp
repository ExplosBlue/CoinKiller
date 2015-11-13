#include "objectrenderer.h"
#include "objects.h"

#include <QPainter>

ObjectRenderer::ObjectRenderer() { }

ObjectRenderer::ObjectRenderer(Object *obj)
{

}

ObjectRenderer::~ObjectRenderer() { }

void ObjectRenderer::render(QPainter *painter) { }


SpriteRenderer::SpriteRenderer() { }

SpriteRenderer::SpriteRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    switch (spr->getid()) {
    case 3: // Cheep Chomp
        ret = new NormalImageRenderer(spr, basePath + "cheep_chomp.png");
        break;
    case 8: // Swoop
        ret = new NormalImageRenderer(spr, basePath + "swoop.png");
        break;
    case 9: // Whomp
        ret = new NormalImageRenderer(spr, basePath + "whomp.png");
        break;
    case 19: // Desert Crater
        ret = new NormalImageRenderer(spr, basePath + "desert_crater.png");
        break;
    case 27: // POW Block
        ret = new NormalImageRenderer(spr, basePath + "pow_block.png");
        break;
    case 29: // Bob-omb
        ret = new NormalImageRenderer(spr, basePath + "bob-omb.png");
        break;
    case 31: // Boomerang Bro.
        ret = new NormalImageRenderer(spr, basePath + "boomerang_bro.png");
        break;
    case 32: // Fire Bro.
        ret = new NormalImageRenderer(spr, basePath + "fire_bro.png");
        break;
    case 33: // Hammer Bro.
        ret = new NormalImageRenderer(spr, basePath + "hammer_bro.png");
        break;
    case 35: // Lava Bubble
        ret = new NormalImageRenderer(spr, basePath + "lava_bubble.png");
        break;
    case 52: // Checkpoint Flag
        ret = new NormalImageRenderer(spr, basePath + "checkpoint_flag.png");
        break;
    case 55: // Coin
        ret = new NormalImageRenderer(spr, basePath + "coin.png");
        break;
    case 66: // Pipe Cannon
        ret = new NormalImageRenderer(spr, basePath + "pipe_cannon.png");
        break;
    case 69: // Door
        ret = new NormalImageRenderer(spr, basePath + "door.png");
        break;
    case 70: // Castle Boss Door
        ret = new NormalImageRenderer(spr, basePath + "door_castle_boss.png");
        break;
    case 71: // Closed Door
        ret = new NormalImageRenderer(spr, basePath + "door_closed.png");
        break;
    case 72: // Final Boss Door
        ret = new NormalImageRenderer(spr, basePath + "door_final_boss.png");
        break;
    case 73: // Ghost House Door
        ret = new NormalImageRenderer(spr, basePath + "door_ghosthouse.png");
        break;
    case 75: // Tower Boss Door
        ret = new NormalImageRenderer(spr, basePath + "door_tower.png");
        break;
    case 77: // Thwomp
        ret = new NormalImageRenderer(spr, basePath + "thwomp.png");
        break;
    case 83: // Fish Bone
        ret = new NormalImageRenderer(spr, basePath + "fish_bone.png");
        break;
    case 92: // Grinder
        ret = new NormalImageRenderer(spr, basePath + "grinder.png");
        break;
    case 95: // Blooper
        ret = new NormalImageRenderer(spr, basePath + "blooper.png");
        break;
    case 97: // End of Level Flag
        ret = new GoalRenderer(spr);
        break;
    case 99: // Wiggler
        ret = new NormalImageRenderer(spr, basePath + "wiggler.png");
        break;
    case 106: // Trampoline
        ret = new NormalImageRenderer(spr, basePath + "trampoline.png");
        break;
    case 108: // Spider Web
        ret = new NormalImageRenderer(spr, basePath + "spider_web.png");
        break;
    case 109: // Signboard
        ret = new SignboardRenderer(spr);
        break;
    case 110: // Dry Bones
        ret = new NormalImageRenderer(spr, basePath + "dry_bones.png");
        break;
    case 111: // Giant Dry Bones
        ret = new NormalImageRenderer(spr, basePath + "sgiant_dry_bones.png");
        break;
    case 114: // Floating Box
        ret = new FloatingBoxRenderer(spr);
        break;
    case 135: // Goomba
        ret = new NormalImageRenderer(spr, basePath + "goomba.png");
        break;
    case 136: // Bone Goomba
        ret = new NormalImageRenderer(spr, basePath + "bone_goomba.png");
        break;
    case 137: // Micro Goomba
        ret = new NormalImageRenderer(spr, basePath + "micro_goomba.png");
        break;
    case 138: // Paragoomba
        ret = new NormalImageRenderer(spr, basePath + "paragoomba.png");
        break;
    case 139: // Goomba Tower
        ret = new GoombaTowerRenderer(spr);
        break;
    case 140: // Crowber
        ret = new NormalImageRenderer(spr, basePath + "crowber.png");
        break;
    case 150: // Seesaw Lift
        ret = new NormalImageRenderer(spr, basePath + "seesaw_lift.png");
        break;
    case 158: // Buzzy Beetle
        ret = new NormalImageRenderer(spr, basePath + "buzzy_beetle.png");
        break;
    case 165: // Koopa Troopa
        ret = new KoopaTroopaRenderer(spr);
        break;
    case 175: // Grounded Piranha Plant
        ret = new NormalImageRenderer(spr, basePath + "piranha_plant.png");
        break;
    case 184: // Parabomb
        ret = new NormalImageRenderer(spr, basePath + "parabomb.png");
        break;
    case 205: // Red Ring
        ret = new NormalImageRenderer(spr, basePath + "red_ring.png");
        break;
    case 206: // Gold Ring
        ret = new NormalImageRenderer(spr, basePath + "gold_ring.png");
        break;
    case 219: // Star Coin
        ret = new NormalImageRenderer(spr, basePath + "star_coin.png");
        break;
    case 221: // ! Switch
        ret = new SwitchRenderer(spr, basePath + "exclamation_switch.png");
        break;
    case 223: // ? Switch
        ret = new SwitchRenderer(spr, basePath + "question_switch.png");
        break;
    case 225: // P Switch
        ret = new SwitchRenderer(spr, basePath + "p_switch.png");
        break;
    case 234: // Spiked Ball
        ret = new NormalImageRenderer(spr, basePath + "spiked_ball.png");
        break;
    case 255: // Bowser Head Statue
        ret = new NormalImageRenderer(spr, basePath + "bowser_head_statue.png");
        break;
    case 267: // Long Question Block
        ret = new NormalImageRenderer(spr, basePath + "long_question_block.png");
        break;
    case 269: // Gold Ship
        ret = new NormalImageRenderer(spr, basePath + "gold_ship.png");
        break;
    case 270: // Icy Spiked Ball
        ret = new NormalImageRenderer(spr, basePath + "icy_spiked_ball.png");
        break;
    case 273: // Coin Roulette Block
        ret = new NormalImageRenderer(spr, basePath + "coin_roulette_block.png");
        break;
    case 274: // Flying Gold Block Spawn Point
        ret = new NormalImageRenderer(spr, basePath + "flying_gold_block.png");
        break;
    case 278: // Assist Block
        ret = new NormalImageRenderer(spr, basePath + "assist_block.png");
        break;
    case 279: // Lemmy Ball
        ret = new NormalImageRenderer(spr, basePath + "lemmy_ball.png");
        break;
    case 287: // Toad House Door
        ret = new NormalImageRenderer(spr, basePath + "door_toadhouse.png");
        break;
    case 293: // Punching Glove
        ret = new NormalImageRenderer(spr, basePath + "punching_glove.png");
        break;
    case 294: // Warp Cannon
        ret = new NormalImageRenderer(spr, basePath + "warp_cannon.png");
        break;
    case 296: // Toad
        ret = new NormalImageRenderer(spr, basePath + "toad.png");
        break;
    case 322: // Big Grinder
        ret = new NormalImageRenderer(spr, basePath + "big_grinder.png");
        break;
    default:
        ret = new RoundedRectRenderer(spr, QString("%1").arg(spr->getid()), QColor(0,90,150,200));
        break;
    }
}

void SpriteRenderer::render(QPainter *painter)
{
    ret->render(painter);
}


NormalImageRenderer::NormalImageRenderer(const Object *obj, QString filename)
{
    rect = QRect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());
    this->filename = filename;
}

NormalImageRenderer::NormalImageRenderer(QRect rect, QString filename)
{
    this->rect = rect;
    this->filename = filename;
}

void NormalImageRenderer::render(QPainter *painter)
{
    painter->drawPixmap(rect, QPixmap(filename));
}


RoundedRectRenderer::RoundedRectRenderer(const Object *obj, QString text, QColor color)
{
    this->obj = obj;
    this->text = text;
    this->color = color;
}

void RoundedRectRenderer::render(QPainter *painter)
{
    QRect rect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());

    painter->setPen(QColor(0,0,0));

    QPainterPath path;
    path.addRoundedRect(rect, 2.0, 2.0);
    painter->fillPath(path, color);
    painter->drawPath(path);

    painter->setFont(QFont("Arial", 7, QFont::Normal));
    painter->drawText(rect, text, Qt::AlignHCenter | Qt::AlignVCenter);
}


// Sprite 97: End of Level Flag
GoalRenderer::GoalRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1)
    {
        pole = new NormalImageRenderer(spr, basePath + "flagpole_secret.png");
        fort = new NormalImageRenderer(QRect(spr->getx()+200, spr->gety()+80, 120, 120), basePath + "secret_castle.png");
    }
    else
    {
        pole = new NormalImageRenderer(spr, basePath + "flagpole.png");
        fort = new NormalImageRenderer(QRect(spr->getx()+200, spr->gety()+80, 120, 120), basePath + "castle.png");
    }
}

void GoalRenderer::render(QPainter *painter)
{
    pole->render(painter);
    fort->render(painter);
}


// Sprite 109: Signboard
SignboardRenderer::SignboardRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");
    QString signboardPath("sign_%1");

    if (spr->getNybble(5) < 10) img = new NormalImageRenderer(spr, basePath + signboardPath.arg(spr->getNybble(5)));
    else img = new NormalImageRenderer(spr, basePath + signboardPath.arg(7));
}

void SignboardRenderer::render(QPainter *painter)
{
    img->render(painter);
}


// Sprite 114: Floating Box
FloatingBoxRenderer::FloatingBoxRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "floating_box_big.png");
    else img = new NormalImageRenderer(spr, basePath + "floating_box_small.png");
}

void FloatingBoxRenderer::render(QPainter *painter)
{
    img->render(painter);
}


// Sprite 139: Goomba Tower
GoombaTowerRenderer::GoombaTowerRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    top = new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY(), spr->getwidth(), 25), basePath + "goomba_tower_top.png");
    bottom = new NormalImageRenderer(QRect(spr->getx(), spr->gety(), spr->getwidth(), 21), basePath + "goomba_tower_bottom.png");

    if (spr->getNybble(5) < 3) return;
    for (int i = 0; i < spr->getNybble(5)-2; i++) middle.append(new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+25+i*21, spr->getwidth(), 21), basePath + "goomba_tower_middle.png"));
}

void GoombaTowerRenderer::render(QPainter *painter)
{
    top->render(painter);
    for(int i = 0; i < middle.size(); i++) middle[i]->render(painter);
    bottom->render(painter);
}


// Sprite 165: Koopa Troopa
KoopaTroopaRenderer::KoopaTroopaRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "koopa_troopa_red.png");
    else img = new NormalImageRenderer(spr, basePath + "koopa_troopa_green.png");
}

void KoopaTroopaRenderer::render(QPainter *painter)
{
    img->render(painter);
}


// Sprite 221/223/225: Switches
SwitchRenderer::SwitchRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;
    if (spr->getNybble(5) == 1) filename.insert(filename.size()-4, "_flipped");

    img = new NormalImageRenderer(spr, filename);
}

void SwitchRenderer::render(QPainter *painter)
{
    img->render(painter);
}
