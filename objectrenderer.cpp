#include "objectrenderer.h"
#include "objects.h"

#include <QPainter>

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
    case 20: // Gold Block
        ret = new NormalImageRenderer(spr, basePath + "gold_block.png");
        break;
    case 21: // Note Block
        ret = new NormalImageRenderer(spr, basePath + "note_block.png");
        break;
    case 22: // Special Exit Controller
        ret = new SpecialExitControllerRenderer(spr);
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
    case 67: // Pipe Cannon - Ludwig Bossfight
        ret = new NormalImageRenderer(spr, basePath + "pipe_cannon_ludwig.png");
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
    case 76: // Big Thwomp
        ret = new NormalImageRenderer(spr, basePath + "big_thwomp.png");
        break;
    case 77: // Thwomp
        ret = new NormalImageRenderer(spr, basePath + "thwomp.png");
        break;
    case 81: // Fireball Pipe - ! Junction
        ret = new NormalImageRenderer(spr, basePath + "fireballpipe_junction.png");
        break;
    case 83: // Fish Bone
        ret = new NormalImageRenderer(spr, basePath + "fish_bone.png");
        break;
    case 92: // Grinder
        ret = new NormalImageRenderer(spr, basePath + "grinder.png");
        break;
    case 94: // Flipper (One way gate)
        ret = new FlipperRenderer(spr);
        break;
    case 93: // Scuttlebug
        ret = new NormalImageRenderer(spr, basePath + "scuttlebug.png");
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
    case 115: // Floating Box
        ret = new BulletBillLauncherRenderer(spr);
        break;
    case 117: // Banzi Bill Launcher
        ret = new NormalImageRenderer(spr, basePath + "banzai_bill_launcher.png");
        break;
    case 124: // Mushroom Platform
        ret = new MushroomPlatformRenderer(spr, basePath + "mushroom_platform/");
        break;
    case 125: // Bowser
        ret = new NormalImageRenderer(spr, basePath + "bowser.png");
        break;
    case 135: // Goomba
        ret = new GoombaRenderer(spr);
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
    case 143: // Conveyor Belt Switch
        ret = new NormalImageRenderer(spr, basePath + "conveyor_belt_switch.png");
        break;
    case 148: //Move While On Lift
        ret = new NormalImageRenderer(spr, basePath + "move_while_on_lift.png");
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
    case 167: // Pipe Piranha Plant - Down
        ret = new NormalImageRenderer(spr, basePath + "piranha_pipe_down.png");
        break;
    case 175: // Grounded Piranha Plant
        ret = new NormalImageRenderer(spr, basePath + "piranha_plant.png");
        break;
    case 181: // Pipe Piranha Plant - Left
        ret = new NormalImageRenderer(spr, basePath + "piranha_pipe_left.png");
        break;
    case 182: // Pipe Piranha Plant - Right
        ret = new NormalImageRenderer(spr, basePath + "piranha_pipe_right.png");
        break;
    case 183: // Pipe Piranha Plant - Up
        ret = new NormalImageRenderer(spr, basePath + "piranha_pipe_up.png");
        break;
    case 184: // Parabomb
        ret = new NormalImageRenderer(spr, basePath + "parabomb.png");
        break;
    case 189: // Rectangle Lift - Tower
        ret = new RecLiftRenderer(spr, basePath + "tower_rectangle_lift/");
        break;
    case 190: // Rectangle Lift - Ghosthouse (Checkered)
        ret = new RecLiftRenderer(spr, basePath + "checkered_rectangle_lift/");
        break;
    case 191: // Rectangle Lift - Desert
        ret = new RecLiftRenderer(spr, basePath + "desert_rectangle_lift/");
        break;
    case 192: // Rectangle Lift - Castle
        ret = new RecLiftRenderer(spr, basePath + "castle_rectangle_lift/");
        break;
    case 193: // Rectangle Lift - Underwater
        ret = new RecLiftRenderer(spr, basePath + "underwater_rectangle_lift/");
        break;
    case 194: // Cheep Cheep
        ret = new NormalImageRenderer(spr, basePath + "cheep_cheep.png");
        break;
    case 195: // Big Cheep Cheep
        ret = new NormalImageRenderer(spr, basePath + "big_cheep_cheep.png");
        break;
    case 200: // Spiny Cheep Cheep
        ret = new NormalImageRenderer(spr, basePath + "spiny_cheep_cheep.png");
        break;
    case 205: // Red Ring
        ret = new NormalImageRenderer(spr, basePath + "red_ring.png");
        break;
    case 206: // Gold Ring
        ret = new NormalImageRenderer(spr, basePath + "gold_ring.png");
        break;
    case 211: // Roy Koopa
        ret = new NormalImageRenderer(spr, basePath + "boss_roy.png");
        break;
    case 215: // Bob-omb Cannon
        ret = new BobOmbCannonRenderer(spr);
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
    case 228: // Boo
        ret = new NormalImageRenderer(spr, basePath + "boo.png");
        break;
    case 229: // Big Boo
        ret = new NormalImageRenderer(spr, basePath + "big_boo.png");
        break
    case 234: // Spiked Ball
        ret = new NormalImageRenderer(spr, basePath + "spiked_ball.png");
        break;
    case 235: // Big Spiked Ball
        ret = new NormalImageRenderer(spr, basePath + "big_spiked_ball.png");
        break;
    case 236: // Mega Spiked Ball
        ret = new NormalImageRenderer(spr, basePath + "mega_spiked_ball.png");
        break;
    case 240: // Urchin
        ret = new UrchinRenderer(spr);
        break;
    case 244: // Chain Chomp
        ret = new ChainChompRenderer(spr);
        break;
    case 251: // Treasure Chest
        ret = new NormalImageRenderer(spr, basePath + "treasure_chest.png");
        break;
    case 255: // Bowser Head Statue
        ret = new NormalImageRenderer(spr, basePath + "bowser_head_statue.png");
        break;
    case 267: // Long Question Block
        ret = new LongQBlockRenderer(spr, basePath + "long_question_block.png");
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
    case 275: // Long Question Block - Underground
        ret = new LongQBlockRenderer(spr, basePath + "long_question_block_underground.png");
        break;
    case 276: // Long Question Block - Lava
        ret = new LongQBlockRenderer(spr, basePath + "long_question_block_lava.png");
        break;
    case 277: // Switchable Conveyor Belt
        ret = new NormalImageRenderer(spr, basePath + "switchable_conveyor_belt.png");
        break;
    case 278: // Assist Block
        ret = new NormalImageRenderer(spr, basePath + "assist_block.png");
        break;
    case 279: // Lemmy Ball
        ret = new NormalImageRenderer(spr, basePath + "lemmy_ball.png");
        break;
    case 280: // + Clock
        ret = new ClockRenderer(spr);
        break;
    case 287: // Toad House Door
        ret = new NormalImageRenderer(spr, basePath + "door_toadhouse.png");
        break;
    case 289: // Bouncy Mushroom Platform - Castle
        ret = new NormalImageRenderer(spr, basePath + "bouncy_mushroom_castle.png");
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
    case 302: // Moon Coin
        ret = new NormalImageRenderer(spr, basePath + "moon_coin.png");
        break;
    case 322: // Big Grinder
        ret = new NormalImageRenderer(spr, basePath + "big_grinder.png");
        break;
    default:
        ret = new RoundedRectRenderer(spr, QString("%1").arg(spr->getid()), QColor(0,90,150,150));
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


RoundedRectRenderer::RoundedRectRenderer(const Object *obj, QString text, QColor color, QTextOption align)
{
    this->obj = obj;
    this->text = text;
    this->color = color;
    this->align = align;
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
    painter->drawText(rect, text, align);
}

// Sprite 22: Special Exit Controller
SpecialExitControllerRenderer::SpecialExitControllerRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void SpecialExitControllerRenderer::render(QPainter *painter)
{
    QRect sprRect(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight());
    painter->fillRect(sprRect, QColor(0,255,0,30));
    painter->setPen(Qt::green);
    painter->drawRect(sprRect);
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 8, QFont::Normal));
    painter->drawText(sprRect.adjusted(2,2,15,0), "Warp");
}

// Sprite 94: Flipper (One Way Gate)
FlipperRenderer::FlipperRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "Flipper_Left_Down.png");
    else if (spr->getNybble(5) == 2) img = new NormalImageRenderer(spr, basePath + "Flipper_Right_Up.png");
    else if (spr->getNybble(5) == 3) img = new NormalImageRenderer(spr, basePath + "Flipper_Left_Up.png");
    else if (spr->getNybble(5) == 4) img = new NormalImageRenderer(spr, basePath + "Flipper_Up_Left.png");
    else if (spr->getNybble(5) == 5) img = new NormalImageRenderer(spr, basePath + "Flipper_Down_Left.png");
    else if (spr->getNybble(5) == 6) img = new NormalImageRenderer(spr, basePath + "Flipper_Up_Right.png");
    else if (spr->getNybble(5) == 7) img = new NormalImageRenderer(spr, basePath + "Flipper_Down_Right.png");
    else img = new NormalImageRenderer(spr, basePath + "Flipper_Right_Down.png");
}

void FlipperRenderer::render(QPainter *painter)
{
    img->render(painter);
}

// Sprite 97: End of Level Flag
GoalRenderer::GoalRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(10) == 1)
    {
        pole = new NormalImageRenderer(spr, basePath + "flagpole_secret.png");
        fort = new NormalImageRenderer(QRect(spr->getx()+200, spr->gety()+80, 120, 120), basePath + "castle_secret.png");
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


// Sprite 115: Bullet Bill Launcher
BulletBillLauncherRenderer::BulletBillLauncherRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BulletBillLauncherRenderer::render(QPainter *painter)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(10) == 1)
    {
        painter->drawPixmap(spr->getx(), spr->gety()+spr->getheight()-40, 20, 40, QPixmap(basePath + "bullet_bill_launcher_flipped.png"));
        for (int i = 0; i < spr->getNybble(4); i++) painter->drawPixmap(spr->getx(), spr->gety()+i*20, 20, 20, QPixmap(basePath + "bullet_bill_launcher_middle.png"));
    }
    else
    {
        painter->drawPixmap(spr->getx(), spr->gety()+spr->getOffsetY(), 20, 40, QPixmap(basePath + "bullet_bill_launcher.png"));
        for (int i = 0; i < spr->getNybble(4); i++) painter->drawPixmap(spr->getx(), spr->gety()+spr->getOffsetY()+40+i*20, 20, 20, QPixmap(basePath + "bullet_bill_launcher_middle.png"));
    }
}


// Sprite 135: Goomba
GoombaRenderer::GoombaRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void GoombaRenderer::render(QPainter *painter)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(14) % 2 == 0) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "goomba.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "goomba_blue.png"));
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


// RecLiftRenderer
RecLiftRenderer::RecLiftRenderer(const Sprite *spr, QString path)
{
    this->spr = spr;
    this->path = path;
    if (spr->getid() == 192) sideOffset = 3;
}

void RecLiftRenderer::render(QPainter *painter)
{
    int blockWidth = spr->getNybble(15) > 0 ? spr->getNybble(15)*20 : 20;
    int blockHeight = spr->getNybble(13) > 0 ? spr->getNybble(13)*20 : 20;

    painter->drawPixmap(QRect(spr->getx()-sideOffset, spr->gety(), 20+sideOffset, 20), QPixmap(path + "tl.png"));
    painter->drawPixmap(QRect(spr->getx()-sideOffset, spr->gety()+blockHeight, 20+sideOffset, 20), QPixmap(path + "bl.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety(), 20+sideOffset, 20), QPixmap(path + "tr.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety()+blockHeight, 20+sideOffset, 20), QPixmap(path + "br.png"));

    for (int i = 0; i < spr->getNybble(15)-1; i++)
    {
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety(), 20, 20), QPixmap(path + "t.png"));
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety()+blockHeight, 20, 20), QPixmap(path + "b.png"));
    }
    for (int i = 0; i < spr->getNybble(13)-1; i++)
    {
        painter->drawPixmap(QRect(spr->getx(), spr->gety() + i*20+20, 20, 20), QPixmap(path + "l.png"));
        painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety() + i*20+20, 20, 20), QPixmap(path + "r.png"));
    }
    for (int x = 20; x < blockWidth; x+=20)
        for (int y = 20; y < blockHeight; y+=20)
            painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+y, 20, 20), QPixmap(path + "c.png"));
    if (spr->getNybble(9) == 1 || spr->getNybble(9) == 3) for (int x = 0; x < blockWidth+20; x+=20) painter->drawPixmap(QRect(spr->getx()+x, spr->gety()-20, 20, 20), QPixmap(path + "s_t.png"));
    if (spr->getNybble(9) == 2 || spr->getNybble(9) == 3) for (int x = 0; x < blockWidth+20; x+=20) painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+blockHeight+20, 20, 20), QPixmap(path + "s_b.png"));
    if (spr->getNybble(9) == 4 || spr->getNybble(9) == 6) for (int y = 0; y < blockHeight+20; y+=20) painter->drawPixmap(QRect(spr->getx()-20, spr->gety()+y, 20, 20), QPixmap(path + "s_l.png"));
    if (spr->getNybble(9) == 5 || spr->getNybble(9) == 6) for (int y = 0; y < blockHeight+20; y+=20) painter->drawPixmap(QRect(spr->getx()+blockWidth+20, spr->gety()+y, 20, 20), QPixmap(path + "s_r.png"));
}

// Sprite 124: Mushroom Platform Renderer
MushroomPlatformRenderer::MushroomPlatformRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void MushroomPlatformRenderer::render(QPainter *painter)
{
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 24, 20), QPixmap(basePath + "l.png"));
    painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-24, spr->gety(), 24, 20), QPixmap(basePath + "r.png"));
    for (int i = 24; i < spr->getwidth()-24; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety(), 20, 20), QPixmap(basePath + "m.png"));
}


// Sprite 215: Bob-omb Cannon
BobOmbCannonRenderer::BobOmbCannonRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BobOmbCannonRenderer::render(QPainter *painter)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) != 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "cannon_right.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "cannon_left.png"));
}


// Sprite 240: Urchin
UrchinRenderer::UrchinRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void UrchinRenderer::render(QPainter *painter)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) != 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "urchin.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "urchin_big.png"));
}


// Sprite 244: Chain Chomp
ChainChompRenderer::ChainChompRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ChainChompRenderer::render(QPainter *painter)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5)%2 == 0) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "chain_chomp_3.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "chain_chomp_1.png"));
}

// Sprites 267/275/276: Long ? Blocks
LongQBlockRenderer::LongQBlockRenderer(const Sprite *spr, QString filename)
{
    this->spr = spr;
    block = new NormalImageRenderer(spr, filename);
}

void LongQBlockRenderer::render(QPainter *painter)
{
    block->render(painter);

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/tileoverlays/");
    QString overlay;

    switch (spr->getNybble(13))
    {
        case 1: case 8: overlay = "coin.png"; break;
        case 2: overlay = "fire_flower.png"; break;
        case 4: overlay = "super_leaf.png"; break;
        case 5: overlay = "gold_flower.png"; break;
        case 6: overlay = "mini_mushroom.png"; break;
        case 7: overlay = "super_star.png"; break;
        case 10: overlay = "10_coins.png"; break;
        case 11: overlay = "1up_mushroom.png"; break;
        case 13: overlay = "trampoline.png"; break;
        case 14: overlay = "coin_super_mushroom.png"; break;
        default: return; break;
    }

    painter->drawPixmap(spr->getx()+spr->getOffsetX()+20, spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + overlay));
}


// Sprite 280: + Clock
ClockRenderer::ClockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ClockRenderer::render(QPainter *painter)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) != 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "clock_50.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "clock_10.png"));
}


// Entrance Renderer
EntranceRenderer::EntranceRenderer(const Entrance *entrance)
{
    this->entr = entrance;

    this->rect = new RoundedRectRenderer(entrance, "", QColor(255,0,0,150));
}

void EntranceRenderer::render(QPainter *painter)
{
    rect->render(painter);
    QRect textRect(entr->getx()+2, entr->gety()+1, 16, 20);
    QRect imgRect(entr->getx(), entr->gety(), 20, 20);

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/entrances/");

    switch (entr->getEntrType())
    {
    case 0: case 1:
        painter->drawText(textRect, "N", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "normal.png"));
        break;
    case 2:
        painter->drawPixmap(imgRect, QPixmap(basePath + "door_exit.png"));
        break;
    case 3:
        painter->drawPixmap(imgRect, QPixmap(basePath + "pipe_up.png"));
        break;
    case 4:
        painter->drawPixmap(imgRect, QPixmap(basePath + "pipe_down.png"));
        break;
    case 5:
        painter->drawPixmap(imgRect, QPixmap(basePath + "pipe_left.png"));
        break;
    case 6:
        painter->drawPixmap(imgRect, QPixmap(basePath + "pipe_right.png"));
        break;
    case 7:
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "down.png"));
        break;
    case 8:
        painter->drawText(textRect, "G", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "down.png"));
        break;
    case 9:
        painter->drawText(textRect, "S", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "normal.png"));
        break;
    case 10:
        painter->drawText(textRect, "S", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "swimming.png"));
        break;
    case 20:
        painter->drawText(textRect, "J", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "up.png"));
        break;
    case 21:
        painter->drawText(textRect, "V", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "up.png"));
        break;
    case 23: case 25: case 26:
        painter->drawText(textRect, "B", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "normal.png"));
        break;
    case 24:
        painter->drawText(textRect, "J", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "left.png"));
        break;
    case 27:
        painter->drawPixmap(imgRect, QPixmap(basePath + "door_entrance.png"));
        break;
    default:painter->drawText(textRect, "?", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, QPixmap(basePath + "unknown.png"));
        break;
    }

    painter->setFont(QFont("Arial", 7, QFont::Normal));
    painter->drawText(textRect, QString("%1").arg(entr->getid()), Qt::AlignLeft | Qt::AlignTop);
}
