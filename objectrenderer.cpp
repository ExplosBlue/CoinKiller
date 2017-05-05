#include "objectrenderer.h"
#include "objects.h"
#include "imagecache.h"

#include <QPainter>

SpriteRenderer::SpriteRenderer(const Sprite *spr, Tileset *tilesets[])
{
    this->spr = spr;

    switch (spr->getid()) {
    case 3: // Cheep Chomp
        ret = new NormalImageRenderer(spr, "cheep_chomp.png");
        break;
    case 4: // Burner Right
        ret = new BurnerRenderer(spr, "burner_right");
        break;
    case 5: // Burner Down
        ret = new BurnerRenderer(spr, "burner_down");
        break;
    case 6: // Burner Left
        ret = new BurnerRenderer(spr, "burner_left");
        break;
    case 7: // Burner Up
        ret = new BurnerRenderer(spr, "burner_up");
        break;
    case 8: // Swoop
        ret = new NormalImageRenderer(spr, "swoop.png");
        break;
    case 9: // Whomp
        ret = new NormalImageRenderer(spr, "whomp.png");
        break;
    case 18: // Tile God
        ret = new TileGodRenderer(spr, tilesets[0]);
        break;
    case 19: // Desert Crater
        ret = new NormalImageRenderer(spr, "desert_crater.png");
        break;
    case 20: // Gold Block
        ret = new NormalImageRenderer(spr, "gold_block.png");
        break;
    case 21: // Note Block
        ret = new NormalImageRenderer(spr, "note_block.png");
        break;
    case 22: // Special Exit Controller
        ret = new SpecialExitControllerRenderer(spr);
        break;
    case 26: // Roulette Block
        ret = new RouletteBlockRenderer(spr);
        break;
    case 27: // POW Block
        ret = new NormalImageRenderer(spr, "pow_block.png");
        break;
    case 28: // Movement Controlled POW Block
        ret = new MovementSpriteRenderer(spr, "pow_block.png");
        break;
    case 29: // Bob-omb
        ret = new NormalImageRenderer(spr, "bob-omb.png");
        break;
    case 31: // Boomerang Bro.
        ret = new NormalImageRenderer(spr, "boomerang_bro.png");
        break;
    case 32: // Fire Bro.
        ret = new NormalImageRenderer(spr, "fire_bro.png");
        break;
    case 33: // Hammer Bro.
        ret = new NormalImageRenderer(spr, "hammer_bro.png");
        break;
    case 35: // Lava Bubble
        ret = new NormalImageRenderer(spr, "lava_bubble.png");
        break;
    case 38: // Reznor Wheel
        ret = new ReznorWheelRenderer(spr);
        break;
    case 39: // Reznor Battle Dissapearing Blocks
        ret = new ReznorBlockRenderer(spr);
        break;
    case 40: // ! Burner Right
        ret = new BurnerRenderer(spr, "!burner_right");
        break;
    case 41: // ! Burner Down
        ret = new BurnerRenderer(spr, "!burner_down");
        break;
    case 42: // ! Burner Left
        ret = new BurnerRenderer(spr, "!burner_left");
        break;
    case 43: // ! Burner Up
        ret = new BurnerRenderer(spr, "!burner_up");
        break;
    case 44: // Track-controlled Burner - Four Directions
        ret = new FourBurnerRenderer(spr, "four_track_controlled_burner/");
        break;
    case 45: // Track Controlled Burner Right
        ret = new BurnerRenderer(spr, "track_burner_right");
        break;
    case 46: // Track Controlled Burner Down
        ret = new BurnerRenderer(spr, "track_burner_down");
        break;
    case 47: // Track Controlled Burner Left
        ret = new BurnerRenderer(spr, "track_burner_left");
        break;
    case 48: // Track Controlled Burner Up
        ret = new BurnerRenderer(spr, "track_burner_up");
        break;
    case 51: // Fuzzy
        ret = new FuzzyRenderer(spr);
        break;
    case 52: // Checkpoint Flag
        ret = new NormalImageRenderer(spr, "checkpoint_flag.png");
        break;
    case 55: // Coin
        ret = new NormalImageRenderer(spr, "coin.png");
        break;
    case 56: // Coin Bubble
        ret = new NormalImageRenderer(spr, "coin_bubble.png");
        break;
    case 59: // Rotation Controlled Coin
        ret = new RotationSpriteRenderer(spr, "coin.png");
        break;
    case 60: // Movement Controlled Coin
        ret = new MovementSpriteRenderer(spr, "coin.png");
        break;
    case 61: // Red Coin
        ret = new RedCoinRenderer(spr, "red_coin.png");
        break;
    case 62: // Rotation Controlled Red Coin
        ret = new RotationSpriteRenderer(spr, "red_coin.png");
        break;
    case 63: // Skewer Left
        ret = new NormalImageRenderer(spr, "skewer_left.png");
        break;
    case 64: // Skewer Right
        ret = new NormalImageRenderer(spr, "skewer_right.png");
        break;
    case 66: // Pipe Cannon
        ret = new NormalImageRenderer(spr, "pipe_cannon.png");
        break;
    case 67: // Pipe Cannon - Ludwig Bossfight
        ret = new NormalImageRenderer(spr, "pipe_cannon_ludwig.png");
        break;
    case 69: // Door
        ret = new NormalImageRenderer(spr, "door.png");
        break;
    case 70: // Castle Boss Door
        ret = new NormalImageRenderer(spr, "door_castle.png");
        break;
    case 71: // Closed Door
        ret = new NormalImageRenderer(spr, "door_closed.png");
        break;
    case 72: // Final Boss Door
        ret = new NormalImageRenderer(spr, "door_final_boss.png");
        break;
    case 73: // Ghost House Door
        ret = new NormalImageRenderer(spr, "door_ghosthouse.png");
        break;
    case 75: // Tower Boss Door
        ret = new NormalImageRenderer(spr, "door_tower.png");
        break;
    case 76: // Big Thwomp
        ret = new NormalImageRenderer(spr, "big_thwomp.png");
        break;
    case 77: // Thwomp
        ret = new NormalImageRenderer(spr, "thwomp.png");
        break;
    case 78: // Fire Bar
        ret = new FireBarRenderer(spr);
        break;
    case 81: // Fireball Pipe - ! Junction
        ret = new NormalImageRenderer(spr, "fireballpipe_junction.png");
        break;
    case 83: // Fish Bone
        ret = new NormalImageRenderer(spr, "fish_bone.png");
        break;
    case 84: // Flag Controller - Chainer
        ret = new FlagRenderer(spr);
        break;
    case 85: // Flag Controller - IF
        ret = new FlagRenderer(spr);
        break;
    case 86: // Flag Controller - Zone enter
        ret = new FlagRenderer(spr);
        break;
    case 87: // Flag Controller - Multi Chainer
        ret = new FlagRenderer(spr);
        break;
    case 88: // Flag Controller - On screen
        ret = new FlagRenderer(spr);
        break;
    case 89: // porcupuffer
        ret = new NormalImageRenderer(spr, "porcupuffer.png");
        break;
    case 92: // Grinder
        ret = new NormalImageRenderer(spr, "grinder.png");
        break;
    case 94: // Flipper (One way gate)
        ret = new FlipperRenderer(spr);
        break;
    case 93: // Scuttlebug
        ret = new NormalImageRenderer(spr, "scuttlebug.png");
        break;
    case 95: // Blooper
        ret = new NormalImageRenderer(spr, "blooper.png");
        break;
    case 97: // End of Level Flag
        ret = new GoalRenderer(spr);
        break;
    case 99: // Wiggler
        ret = new NormalImageRenderer(spr, "wiggler.png");
        break;
    case 100: // Rotation Controlled ? Block
        ret = new ItemBlockRenderer(spr, "question_block");
        break;
    case 101: // Movement Controlled ? Block
        ret = new ItemBlockRenderer(spr, "question_block");
        break;
    case 106: // Trampoline
        ret = new NormalImageRenderer(spr, "trampoline.png");
        break;
    case 107: // Path Controlled Climbable Fence
        ret = new PathContFenceRenderer(spr);
        break;
    case 108: // Spider Web
        ret = new NormalImageRenderer(spr, "spider_web.png");
        break;
    case 109: // Signboard
        ret = new SignboardRenderer(spr);
        break;
    case 110: // Dry Bones
        ret = new NormalImageRenderer(spr, "dry_bones.png");
        break;
    case 111: // Giant Dry Bones
        ret = new NormalImageRenderer(spr, "giant_dry_bones.png");
        break;
    case 114: // Floating Box
        ret = new FloatingBoxRenderer(spr);
        break;
    case 115: // Floating Box
        ret = new BulletBillLauncherRenderer(spr);
        break;
    case 117: // Banzai Bill Launcher
        ret = new NormalImageRenderer(spr, "banzai_bill_launcher.png");
        break;
    case 118: // Bullet Bill
        ret = new BulletBillRenderer(spr);
        break;
    case 119: // Banzi Bill
        ret = new BanzaiBillRenderer(spr);
        break;
    case 120: // Up Down Mushroom
        ret = new UpDownMushroomRenderer(spr, "up_down_mushroom/");
        break;
    case 123: // Bouncy Mushroom
        ret = new BouncyMushroomRenderer(spr);
        break;
    case 124: // Mushroom Platform
        ret = new MushroomPlatformRenderer(spr, "mushroom_platform/");
        break;
    case 125: // Bowser
        ret = new NormalImageRenderer(spr, "bowser.png");
        break;
    case 127: // Bowser Flame
        ret = new BowserFlameRenderer(spr);
        break;
    case 128: // Dry Bowser
        ret = new NormalImageRenderer(spr, "dry_bowser.png");
        break;
    case 132: // Bowser Battle Lift and Switch Controller
        ret = new NormalImageRenderer(spr, "bowser_switch.png");
        break;
    case 133: // Bowser Shutter
        ret = new NormalImageRenderer(spr, "bowser_shutter.png");
        break;
    case 134: // Bowser Bridge
        ret = new NormalImageRenderer(spr, "bowser_bridge.png");
        break;
    case 135: // Goomba
        ret = new GoombaRenderer(spr);
        break;
    case 136: // Bone Goomba
        ret = new BoneGoombaRenderer(spr);
        break;
    case 137: // Micro Goomba
        ret = new NormalImageRenderer(spr, "micro_goomba.png");
        break;
    case 138: // Paragoomba
        ret = new NormalImageRenderer(spr, "paragoomba.png");
        break;
    case 139: // Goomba Tower
        ret = new GoombaTowerRenderer(spr);
        break;
    case 140: // Crowber
        ret = new NormalImageRenderer(spr, "crowber.png");
        break;
    case 143: // Conveyor Belt Switch
        ret = new NormalImageRenderer(spr, "conveyor_belt_switch.png");
        break;
    case 144: // Horizontal Lift
        ret = new LiftRenderer(spr, "lift_platform/");
        break;
    case 145: // Vertical Lift
        ret = new LiftRenderer(spr, "lift_platform/");
        break;
    case 146: // Track Controlled Lift
        ret = new TrackLiftRenderer(spr, "track_controlled_lift/");
        break;
    case 147: // 3 Plat rickshaw
        ret = new ThreePlatRickRenderer(spr);
        break;
    case 148: //Move While On Lift
        ret = new NormalImageRenderer(spr, "move_while_on_lift.png");
        break;
    case 149: // Changeable Direction Platform
        ret = new NormalImageRenderer(spr, "change_direction_platofrm.png");
        break;
    case 150: // Seesaw Lift
        ret = new NormalImageRenderer(spr, "seesaw_lift.png");
        break;
    case 151: // Scale Lift
        ret = new ScalePlatformRenderer(spr, "scale_lift/");
        break;
    case 154: // 3 Plat rickshaw
        ret = new FourPlatRickRenderer(spr);
        break;
    case 158: // Buzzy Beetle
        ret = new SpinyRenderer(spr, "buzzy_beetle");
        break;
    case 159: // Spike Top
        ret = new SpikeTopRenderer(spr);
        break;
    case 163: // Climing Koopa - Horizontal
        ret = new ClimbKoopaHorRenderer(spr);
        break;
    case 164: // Climing Koopa - Vertical
        ret = new ClimbKoopaVertRenderer(spr);
        break;
    case 165: // Koopa Troopa
        ret = new KoopaTroopaRenderer(spr);
        break;
    case 167: // Pipe Piranha Plant - Down
        ret = new NormalImageRenderer(spr, "piranha_pipe_down.png");
        break;
    case 168: // Fire Pipe Piranha Plant - Down
        ret = new NormalImageRenderer(spr, "piranha_fire_pipe_down.png");
        break;
    case 169: // Pipe Piranha Plant - Left
        ret = new NormalImageRenderer(spr, "piranha_fire_pipe_left.png");
        break;
    case 170: // Pipe Piranha Plant - Right
        ret = new NormalImageRenderer(spr, "piranha_fire_pipe_right.png");
        break;
    case 171: // Pipe Piranha Plant - Up
        ret = new NormalImageRenderer(spr, "piranha_fire_pipe_up.png");
        break;
    case 172: // Pipe bone Piranha Plant - Up
        ret = new NormalImageRenderer(spr, "bone_piranha_pipe_up.png");
        break;
    case 173: // Pipe Bone Piranha Plant - Left
        ret = new NormalImageRenderer(spr, "bone_piranha_pipe_left.png");
        break;
    case 174: // Pipe Bone Piranha Plant - Right
        ret = new NormalImageRenderer(spr, "bone_piranha_pipe_right.png");
        break;
    case 175: // Grounded Piranha Plant
        ret = new PlantRenderer(spr, "piranha_plant.png");
        break;
    case 176: // Big Grounded Pirahna Plant
        ret = new PlantRenderer(spr, "big_piranha_plant.png");
        break;
    case 177: // Grounded Fire Piranha Plant
        ret = new PlantRenderer(spr, "fire_piranha_plant.png");
        break;
    case 178: // Big Grounded Fire Pirahna Plant
        ret = new PlantRenderer(spr, "big_fire_piranha_plant.png");
        break;
    case 179: // Grounded Bone Piranha Plant
        ret = new PlantRenderer(spr, "bone_piranha_plant.png");
        break;
    case 180: // Big Grounded Bone Pirahna Plant
        ret = new PlantRenderer(spr, "big_bone_piranha_plant.png");
        break;
    case 181: // Pipe Piranha Plant - Left
        ret = new NormalImageRenderer(spr, "piranha_pipe_left.png");
        break;
    case 182: // Pipe Piranha Plant - Right
        ret = new NormalImageRenderer(spr, "piranha_pipe_right.png");
        break;
    case 183: // Pipe Piranha Plant - Up
        ret = new NormalImageRenderer(spr, "piranha_pipe_up.png");
        break;
    case 184: // Parabomb
        ret = new NormalImageRenderer(spr, "parabomb.png");
        break;
    case 185: // Koopa Paratroopa
        ret = new KoopaParatroopaRenderer(spr);
        break;
    case 189: // Rectangle Lift - Tower
        ret = new RecLiftRenderer(spr, "tower_rectangle_lift/");
        break;
    case 190: // Rectangle Lift - Ghosthouse (Checkered)
        ret = new RecLiftRenderer(spr, "checkered_rectangle_lift/");
        break;
    case 191: // Rectangle Lift - Desert
        ret = new RecLiftRenderer(spr, "desert_rectangle_lift/");
        break;
    case 192: // Rectangle Lift - Castle
        ret = new RecLiftRenderer(spr, "castle_rectangle_lift/");
        break;
    case 193: // Rectangle Lift - Underwater
        ret = new RecLiftRenderer(spr, "underwater_rectangle_lift/");
        break;
    case 194: // Cheep Cheep
        ret = new CheepCheepRenderer(spr, "cheep_cheep");
        break;
    case 195: // Big Cheep Cheep
        ret = new BigCheepRenderer(spr);
        break;
    case 200: // Spiny Cheep Cheep
        ret = new NormalImageRenderer(spr, "spiny_cheep_cheep.png");
        break;
    case 203: // Rotation Controlled Brick Block
        ret = new ItemBlockRenderer(spr, "brick_block");
        break;
    case 204: // Movement Controlled Brick Block
        ret = new ItemBlockRenderer(spr, "brick_block");
        break;
    case 205: // Red Ring
        ret = new NormalImageRenderer(spr, "red_ring.png");
        break;
    case 206: // Gold Ring
        ret = new NormalImageRenderer(spr, "gold_ring.png");
        break;
    case 211: // Roy Koopa
        ret = new NormalImageRenderer(spr, "boss_roy.png");
        break;
    case 212: // Roy Battle Walls
        ret = new NormalImageRenderer(spr, "roy_wall.png");
        break;
    case 213: // Pokey
        ret = new PokeyRenderer(spr);
        break;
    case 215: // Bob-omb Cannon
        ret = new BobOmbCannonRenderer(spr);
        break;
    case 216: // Boss Shutter
        ret = new NormalImageRenderer(spr, "boss_shutter.png");
        break;
    case 219: // Star Coin
        ret = new NormalImageRenderer(spr, "star_coin.png");
        break;
    case 220: // Rotation Controlled Star Coin
        ret = new RotationSpriteRenderer(spr, "star_coin.png");
        break;
    case 221: // ! Switch
        ret = new SwitchRenderer(spr, "exclamation_switch.png");
        break;
    case 222: // Brick Block Contaning ! Switch
        ret = new NormalImageRenderer(spr, "brick_block_exclamation_switch.png");
        break;
    case 223: // ? Switch
        ret = new SwitchRenderer(spr, "question_switch.png");
        break;
    case 224: // Brick Block Contaning ? Switch
        ret = new NormalImageRenderer(spr, "brick_block_question_switch.png");
        break;
    case 225: // P Switch
        ret = new SwitchRenderer(spr, "p_switch.png");
        break;
    case 226: // Brick Block Contaning P Switch
        ret = new NormalImageRenderer(spr, "brick_block_p_switch.png");
        break;
    case 227: // Floating Barrel
        ret = new NormalImageRenderer(spr, "barrel.png");
        break;
    case 228: // Boo
        ret = new NormalImageRenderer(spr, "boo.png");
        break;
    case 229: // Big Boo
        ret = new NormalImageRenderer(spr, "big_boo.png");
        break;
    case 232: // Spiny
        ret = new SpinyRenderer(spr, "spiny");
        break;
    case 233: // Celing Spiny
        ret = new NormalImageRenderer(spr, "spiny_ceiling.png");
        break;
    case 234: // Spiked Ball
        ret = new NormalImageRenderer(spr, "spiked_ball.png");
        break;
    case 235: // Big Spiked Ball
        ret = new NormalImageRenderer(spr, "big_spiked_ball.png");
        break;
    case 236: // Mega Spiked Ball
        ret = new NormalImageRenderer(spr, "mega_spiked_ball.png");
        break;
    case 240: // Urchin
        ret = new UrchinRenderer(spr);
        break;
    case 242: // Whirlpool
        ret = new NormalImageRenderer(spr, "whirlpool.png");
        break;
    case 244: // Chain Chomp
        ret = new ChainChompRenderer(spr);
        break;
    case 251: // Treasure Chest
        ret = new NormalImageRenderer(spr, "treasure_chest.png");
        break;
    case 253: // Larry Battle Platform
        ret = new LarryPlatformRenderer(spr);
        break;
    case 255: // Bowser Head Statue
        ret = new NormalImageRenderer(spr, "bowser_head_statue.png");
        break;
    case 259: // Rotation Controlled Hard Block
        ret = new HardBlockRenderer(spr);
        break;
    case 260: // Tower Cutscene Roy
        ret = new NormalImageRenderer(spr, "roy_cutscene.png");
        break;
    case 261: // Tower Cutscene Iggy
        ret = new NormalImageRenderer(spr, "iggy_cutscene.png");
        break;
    case 262: // Tower Cutscene Wendy
        ret = new NormalImageRenderer(spr, "wendy_cutscene.png");
        break;
    case 263: // Tower Cutscene Morton
        ret = new NormalImageRenderer(spr, "morton_cutscene.png");
        break;
    case 264: // Tower Cutscene Ludwig
        ret = new NormalImageRenderer(spr, "ludwig_cutscene.png");
        break;
    case 265: // Tower Cutscene No koopa
        ret = new NormalImageRenderer(spr, "tower_cutscene.png");
        break;
    case 267: // Long Question Block
        ret = new ItemBlockRenderer(spr, "long_question_block.png");
        break;
    case 269: // Gold Ship
        ret = new NormalImageRenderer(spr, "gold_ship.png");
        break;
    case 270: // Icy Spiked Ball
        ret = new NormalImageRenderer(spr, "icy_spiked_ball.png");
        break;
    case 272: // Peach Cage
        ret = new NormalImageRenderer(spr, "peach_cage.png");
        break;
    case 273: // Coin Roulette Block
        ret = new NormalImageRenderer(spr, "coin_roulette_block.png");
        break;
    case 274: // Flying Gold Block Spawn Point
        ret = new NormalImageRenderer(spr, "flying_gold_block.png");
        break;
    case 275: // Long Question Block - Underground
        ret = new ItemBlockRenderer(spr, "long_question_block_underground.png");
        break;
    case 276: // Long Question Block - Lava
        ret = new ItemBlockRenderer(spr, "long_question_block_lava.png");
        break;
    case 277: // Switchable Conveyor Belt
        ret = new NormalImageRenderer(spr, "switchable_conveyor_belt.png");
        break;
    case 278: // Assist Block
        ret = new NormalImageRenderer(spr, "assist_block.png");
        break;
    case 279: // Lemmy Ball
        ret = new NormalImageRenderer(spr, "lemmy_ball.png");
        break;
    case 280: // + Clock
        ret = new ClockRenderer(spr);
        break;
    case 281: // Rectangle Ice Lift
        ret = new NormalImageRenderer(spr, "ice_lift_rectangle.png");
        break;
    case 287: // Toad House Door
        ret = new NormalImageRenderer(spr, "door_toadhouse.png");
        break;
    case 289: // Bouncy Mushroom Platform - Castle
        ret = new NormalImageRenderer(spr, "bouncy_mushroom_castle.png");
        break;
    case 293: // Punching Glove
        ret = new NormalImageRenderer(spr, "punching_glove.png");
        break;
    case 294: // Warp Cannon
        ret = new NormalImageRenderer(spr, "warp_cannon.png");
        break;
    case 296: // Toad
        ret = new NormalImageRenderer(spr, "toad.png");
        break;
    case 297: // Horizontal Rail Controlled Fence
        ret = new RailContFenceRenderer(spr);
        break;
    case 302: // Moon Coin
        ret = new NormalImageRenderer(spr, "moon_coin.png");
        break;
    case 303: // Rotation Controlled Moon Coin
        ret = new RotationSpriteRenderer(spr, "moon_coin.png");
        break;
    case 305: // Path Controlled Ice Lift
        ret = new NormalImageRenderer(spr, "ice_lift.png");
        break;
    case 311: // Coin Meteor
        ret = new CoinMeteorRenderer(spr);
        break;
    case 314: // Ruins Rickshaw
        ret = new RuinsRickRenderer(spr);
        break;
    case 322: // Big Grinder
        ret = new NormalImageRenderer(spr, "big_grinder.png");
        break;
    default:
        ret = new RoundedRectRenderer(spr, QString("%1").arg(spr->getid()), QColor(0,90,150,150));
        break;

    }
}

void SpriteRenderer::render(QPainter *painter, QRect *drawrect)
{
    ret->render(painter, drawrect);
}

NormalImageRenderer::NormalImageRenderer(const Object *obj, QString filename)
{
    rect = QRect(obj->getx()+obj->getOffsetX(), obj->gety()+obj->getOffsetY(), obj->getwidth(), obj->getheight());
    this->name = filename;
}

NormalImageRenderer::NormalImageRenderer(QRect rect, QString filename)
{
    this->rect = rect;
    this->name = filename;
}

void NormalImageRenderer::render(QPainter* painter, QRect *)
{
    painter->drawPixmap(rect, ImageCache::getInstance()->get(SpriteImg, name));
}


RoundedRectRenderer::RoundedRectRenderer(const Object *obj, QString text, QColor color, QTextOption align)
{
    this->obj = obj;
    this->text = text;
    this->color = color;
    this->align = align;
}

void RoundedRectRenderer::render(QPainter *painter, QRect *)
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

CircleRenderer::CircleRenderer(int offsetx, int offsety, int width, int height, QString text, QColor color, QTextOption align)
{
    this->offsetx = offsetx;
    this->offsety = offsety;
    this->width = width;
    this->height = height;
    this->text = text;
    this->color = color;
    this->align = align;
}

void CircleRenderer::render(QPainter *painter, QRect *)
{
    QRect ellipse(offsetx, offsety, width, height);

    painter->setPen(color);
    painter->drawEllipse(ellipse);

    painter->setFont(QFont("Arial", 7, QFont::Normal));
    painter->drawText(ellipse, text, align);
}


// Sprite Renderers

// Sprite 4/5/6/7: Burner Right/Down/Left/Up
BurnerRenderer::BurnerRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void BurnerRenderer::render(QPainter *painter, QRect *)
{
    if (spr->getNybble(4) == 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_big.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + ".png"));
}


// Sprite 18: Tile God
TileGodRenderer::TileGodRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void TileGodRenderer::render(QPainter *painter, QRect *)
{
    QRect sprrect(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight());

    if ((spr->getNybble(7) % 2) == 0)
    {
        QBrush b(Qt::BDiagPattern);
        b.setColor(QColor(25, 25, 25));

        painter->fillRect(sprrect, b);

        painter->setPen(QPen(Qt::black));
    }

    else
    {
        if (tileset == NULL)
            painter->fillRect(sprrect, QColor(200, 0, 0, 120));
        else
        {
            QPixmap pix(spr->getwidth(), spr->getheight());
            pix.fill(QColor(0,0,0,0));
            QPainter tempPainter(&pix);

            TileGrid tileGrid;
            tileGrid[0xFFFFFFFF] = 1;

            tempPainter.setOpacity(0.5);

            int tileId = tileIds[0];

            if (spr->getNybble(6) < 12)
                tileId = tileIds[spr->getNybble(6)];


            for (int x = 0; x < spr->getwidth()/20; x++)
                for (int y = 0; y < spr->getheight()/20; y++)
                    tileset->drawTile(tempPainter, tileGrid, tileId, x, y, 1, 0);

            painter->drawPixmap(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight(), pix);
        }
    }

    painter->drawRect(sprrect);
}

// Sprite 22: Special Exit Controller
SpecialExitControllerRenderer::SpecialExitControllerRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void SpecialExitControllerRenderer::render(QPainter *painter, QRect *)
{
    QRect sprRect(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight());
    painter->fillRect(sprRect, QColor(0,255,0,30));
    painter->setPen(Qt::green);
    painter->drawRect(sprRect);
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 8, QFont::Normal));
    painter->drawText(sprRect.adjusted(2,2,15,0), "Warp");
}

// Sprite 26: Roulette Block
RouletteBlockRenderer::RouletteBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void RouletteBlockRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/roulette_block/");

    switch (spr->getNybble(13))
    {
        case 1: contents = "gold_leaf.png"; break;
        case 2: contents = "leaf_star.png"; break;
        case 3: contents = "star.png"; break;
        case 4: contents = "flower_gold.png"; break;
        default: contents = "flower_star.png"; break;
    }

    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 30, 30, QPixmap(basePath + "roulette_block.png"));
    painter->drawPixmap(spr->getx()+spr->getOffsetX()+5, spr->gety()+spr->getOffsetY()+5, 20, 20, QPixmap(basePath + contents));

}

// Sprite 28/60/101/204/: Movement Controlled Sprites
MovementSpriteRenderer::MovementSpriteRenderer(const Sprite *spr, QString fileName)
{
    this->spr = spr;
    img = new NormalImageRenderer(spr, fileName);
}

void MovementSpriteRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter,drawrect);

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/tileoverlays/");
    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + "movement.png"));
}

// Sprite 38: Reznor Wheel
ReznorWheelRenderer::ReznorWheelRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ReznorWheelRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if(spr->getNybble(6) == 1)
        painter->drawPixmap(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY(),349, 353, QPixmap(basePath + "reznor_wheel_big.png"));
    else
    {   //Wheel
        painter->drawPixmap(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY(),201, 177, QPixmap(basePath + "reznor_wheel.png"));        
        //Top Right
        if((spr->getNybble(5) == 1)||(spr->getNybble(5) == 3)||(spr->getNybble(5) == 5)||(spr->getNybble(5) == 7)||(spr->getNybble(5) == 9)||(spr->getNybble(5) == 11)||(spr->getNybble(5) == 13)||(spr->getNybble(5) == 15))
            painter->drawPixmap(spr->getx()+spr->getOffsetX()+145,spr->gety()+spr->getOffsetY()-10,48, 55, QPixmap(basePath + "reznor.png"));
        //Top Left
        if((spr->getNybble(5) == 2)||(spr->getNybble(5) == 3)||(spr->getNybble(5) == 6)||(spr->getNybble(5) == 7)||(spr->getNybble(5) == 10)||(spr->getNybble(5) == 11)||(spr->getNybble(5) == 14)||(spr->getNybble(5) == 15))
            painter->drawPixmap(spr->getx()+spr->getOffsetX()+35,spr->gety()+spr->getOffsetY()-40,48, 55, QPixmap(basePath + "reznor.png"));
        //Bottom Left
        if((spr->getNybble(5) == 4)||(spr->getNybble(5) == 5)||(spr->getNybble(5) == 6)||(spr->getNybble(5) == 7)||(spr->getNybble(5) == 12)||(spr->getNybble(5) == 13)||(spr->getNybble(5) == 14)||(spr->getNybble(5) == 15))
            painter->drawPixmap(spr->getx()+spr->getOffsetX()+5,spr->gety()+spr->getOffsetY()+71,48, 55, QPixmap(basePath + "reznor.png"));
        //Bottom Right
        if((spr->getNybble(5) == 8)||(spr->getNybble(5) == 9)||(spr->getNybble(5) == 10)||(spr->getNybble(5) == 11)||(spr->getNybble(5) == 12)||(spr->getNybble(5) == 13)||(spr->getNybble(5) == 14)||(spr->getNybble(5) == 15))
            painter->drawPixmap(spr->getx()+spr->getOffsetX()+115,spr->gety()+spr->getOffsetY()+100,48, 55, QPixmap(basePath + "reznor.png"));
    }
}

// Sprite 39: Reznor Battle Disapearing Blocks
ReznorBlockRenderer::ReznorBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ReznorBlockRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if(spr->getNybble(4) == 0)
        painter->drawPixmap(spr->getx(), spr->gety(), 20, 20, QPixmap(basePath + "reznor_platform.png"));
    for (int i = 0; i < spr->getNybble(4); i++)
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+i*20, spr->gety(), 20, 20, QPixmap(basePath + "reznor_platform.png"));
}

// Sprite 44: Track-controlled Burner - Four Directions
FourBurnerRenderer::FourBurnerRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;

    if(spr->getNybble(4) == 0)
    {
        big = "";
        size = 60;
    }
    else
    {
        big = "big_";
        size = 120;
    }

    up = new NormalImageRenderer(QRect(spr->getx(),spr->gety()-(size), 22, size), basePath + big + "up.png");
    down = new NormalImageRenderer(QRect(spr->getx(),spr->gety()+21, 22, size), basePath + big + "down.png");
    left = new NormalImageRenderer(QRect(spr->getx()-(size),spr->gety(), size, 22), basePath + big + "left.png");
    right = new NormalImageRenderer(QRect(spr->getx()+21,spr->gety(), size, 22), basePath + big + "right.png");
    center = new NormalImageRenderer(QRect(spr->getx(),spr->gety(), 21, 21), basePath + "center.png");

}
void FourBurnerRenderer::render(QPainter *painter, QRect *drawrect)
{
    center->render(painter, drawrect);
    if((spr->getNybble(8) == 4)||(spr->getNybble(8) == 5)||(spr->getNybble(8) == 6)||(spr->getNybble(8) == 7)||(spr->getNybble(8) == 12)||(spr->getNybble(8) == 13)||(spr->getNybble(8) == 14)||(spr->getNybble(8) == 15))
        up->render(painter,drawrect);
    if((spr->getNybble(8) == 8)||(spr->getNybble(8) == 9)||(spr->getNybble(8) == 10)||(spr->getNybble(8) == 11)||(spr->getNybble(8) == 12)||(spr->getNybble(8) == 13)||(spr->getNybble(8) == 14)||(spr->getNybble(8) == 15))
        down->render(painter,drawrect);
    if((spr->getNybble(8) == 2)||(spr->getNybble(8) == 3)||(spr->getNybble(8) == 6)||(spr->getNybble(8) == 7)||(spr->getNybble(8) == 10)||(spr->getNybble(8) == 11)||(spr->getNybble(8) == 14)||(spr->getNybble(8) == 15))
        left->render(painter,drawrect);
    if((spr->getNybble(8) == 1)||(spr->getNybble(8) == 3)||(spr->getNybble(8) == 5)||(spr->getNybble(8) == 7)||(spr->getNybble(8) == 9)||(spr->getNybble(8) == 11)||(spr->getNybble(8) == 13)||(spr->getNybble(8) == 15))
        right->render(painter,drawrect);
}

// Sprite 51: Fuzzy
FuzzyRenderer::FuzzyRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(7) == 1) img = new NormalImageRenderer(spr, basePath + "fuzzy_big.png");
    else img = new NormalImageRenderer(spr, basePath + "fuzzy.png");
}

void FuzzyRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 59/62/100/203/220/259/303: Rotation Controlled Sprites
RotationSpriteRenderer::RotationSpriteRenderer(const Sprite *spr, QString fileName)
{
    this->spr = spr;
    img = new NormalImageRenderer(spr, fileName);
}

void RotationSpriteRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter,drawrect);

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/tileoverlays/");
    if((spr->getwidth() == 20) && (spr->getheight() == 20))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + "rotation.png"));
    else
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+10, spr->gety()+spr->getOffsetY()+10, 20, 20, QPixmap(basePath + "rotation.png"));
}

// Sprite 61: RedCoin
RedCoinRenderer::RedCoinRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;
    if (spr->getNybble(9) >= 1) filename.insert(filename.size()-4, "_falling");

    img = new NormalImageRenderer(spr, filename);
}

void RedCoinRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 78: Firebar
FireBarRenderer::FireBarRenderer(const Sprite *spr)
{
    this->spr = spr;

    size = (spr->getNybble(5)*40) + 20;
    posoff = (spr->getNybble(5)*20);

    center = new NormalImageRenderer(spr, "firebar_center.png");
    radius = new CircleRenderer(spr->getx()-posoff, spr->gety()-posoff, size, size, "", QColor(0,0,0));

}

void FireBarRenderer::render(QPainter *painter, QRect *drawrect)
{
    center->render(painter, drawrect);
    if(spr->getNybble(5) > 0)
        radius->render(painter, drawrect);

    //Center flame
    painter->drawPixmap(QRect(spr->getx(), spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));

    //Right side bar
    if ((spr->getNybble(4) == 0) || (spr->getNybble(4) == 1) || (spr->getNybble(4) == 3))
    {
        for (int i = 0; i < spr->getNybble(5); i++)
            painter->drawPixmap(QRect(spr->getx()+20+i*20, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
    }
    //Left side bar
    if ((spr->getNybble(4) == 1) ||(spr->getNybble(4) == 3))
    {
        for (int i = 0; i < spr->getNybble(5); i++)
            painter->drawPixmap(QRect(spr->getx()-20-i*20, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
    }
    //Top bar
    if ((spr->getNybble(4) == 2) || (spr->getNybble(4) == 3))
    {
        for (int i = 0; i < spr->getNybble(5); i++)
            painter->drawPixmap(QRect(spr->getx(), spr->gety()-20-i*20, 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
    }
    //Bottom bar
    if ((spr->getNybble(4) == 3))
    {
        for (int i = 0; i < spr->getNybble(5); i++)
            painter->drawPixmap(QRect(spr->getx(), spr->gety()+20+i*20, 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
    }
    //Bottom Left Bar
    if ((spr->getNybble(4) == 2))
    {
        for (int i = 0; i < spr->getNybble(5); i++)
            painter->drawPixmap(QRect(spr->getx()-15-i*15, spr->gety()+15+i*15, 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
    }
    //Bottom Right Bar
    if ((spr->getNybble(4) == 2))
    {
        for (int i = 0; i < spr->getNybble(5); i++)
            painter->drawPixmap(QRect(spr->getx()+15+i*15, spr->gety()+15+i*15, 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
    }
    if(spr->getNybble(4) >= 4)
    {
        painter->setFont(QFont("Arial", 8, QFont::Bold));
        painter->drawText(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20), QString::number(spr->getNybble(4) + 1), Qt::AlignHCenter | Qt::AlignVCenter);
    }

}

// Sprite 84/85/86/87/88: Flags
FlagRenderer::FlagRenderer(const Sprite *spr)
{
    this->spr = spr;

    this->rect = new RoundedRectRenderer(spr, "", QColor(243,156,18,150));
}
void FlagRenderer::render(QPainter *painter, QRect *drawrect)
{
    rect->render(painter, drawrect);
    QRect textRect(spr->getx()+2, spr->gety()+1, 16, 20);

    if(spr->getid() == 84) // Chainer
    {
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignTop);
        painter->setFont(QFont("Arial", 7, QFont::Thin));
        painter->drawText(textRect, QString("Cha"), Qt::AlignLeft | Qt::AlignBottom);
    }
    else if(spr->getid() == 85) // Location
    {
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignTop);
        painter->setFont(QFont("Arial", 7, QFont::Normal));
        painter->drawText(textRect, QString("Loc"), Qt::AlignLeft | Qt::AlignBottom);
    }
    else if(spr->getid() == 86) // Zone Enter
    {
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignTop);
        painter->setFont(QFont("Arial", 7, QFont::Normal));
        painter->drawText(textRect, QString("Zon"), Qt::AlignLeft | Qt::AlignBottom);
    }
    else if(spr->getid() == 87) // Multi Chaner
    {
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignTop);
        painter->setFont(QFont("Arial", 7, QFont::Normal));
        painter->drawText(textRect, QString("Mch"), Qt::AlignLeft | Qt::AlignBottom);
    }
    else // Onscreen
    {
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignTop);
        painter->setFont(QFont("Arial", 7, QFont::Normal));
        painter->drawText(textRect, QString("Ons"), Qt::AlignLeft | Qt::AlignBottom);
    }
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

void FlipperRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
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

void GoalRenderer::render(QPainter *painter, QRect *drawrect)
{
    pole->render(painter, drawrect);
    fort->render(painter, drawrect);
}


// Sprite 107: Path-Controlled Climbable fence
PathContFenceRenderer::PathContFenceRenderer(const Sprite *spr)
{
    this->spr = spr;
    basePath = (QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

}
void PathContFenceRenderer::render(QPainter *painter, QRect *)
{
    //Row 1
    if((spr->getNybble(14) % 2))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+180, spr->gety(), 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(14) == 2)||(spr->getNybble(14) == 3)||(spr->getNybble(14) == 6)||(spr->getNybble(14) == 7)||(spr->getNybble(14) == 10)||(spr->getNybble(14) == 11)||(spr->getNybble(14) >= 14))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+120, spr->gety(), 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(14) == 4)||(spr->getNybble(14) == 5)||(spr->getNybble(14) == 6)||(spr->getNybble(14) == 7)||(spr->getNybble(14) >= 12))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+60, spr->gety(), 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(14) >= 8))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety(), 60, 60, QPixmap(basePath + "path_fence.png"));
    //Row 2
    if((spr->getNybble(15) % 2))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+180, spr->gety()+spr->getOffsetY()+60, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(15) == 2)||(spr->getNybble(15) == 3)||(spr->getNybble(15) == 6)||(spr->getNybble(15) == 7)||(spr->getNybble(15) == 10)||(spr->getNybble(15) == 11)||(spr->getNybble(15) >= 14))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+120, spr->gety()+spr->getOffsetY()+60, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(15) == 4)||(spr->getNybble(15) == 5)||(spr->getNybble(15) == 6)||(spr->getNybble(15) == 7)||(spr->getNybble(15) >= 12))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+60, spr->gety()+spr->getOffsetY()+60, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(15) >= 8))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY()+60, 60, 60, QPixmap(basePath + "path_fence.png"));
    //Row 3
    if((spr->getNybble(12) % 2))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+180, spr->gety()+spr->getOffsetY()+120, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(12) == 2)||(spr->getNybble(12) == 3)||(spr->getNybble(12) == 6)||(spr->getNybble(12) == 7)||(spr->getNybble(12) == 10)||(spr->getNybble(12) == 11)||(spr->getNybble(12) >= 14))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+120, spr->gety()+spr->getOffsetY()+120, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(12) == 4)||(spr->getNybble(12) == 5)||(spr->getNybble(12) == 6)||(spr->getNybble(12) == 7)||(spr->getNybble(12) >= 12))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+60, spr->gety()+spr->getOffsetY()+120, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(12) >= 8))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY()+120, 60, 60, QPixmap(basePath + "path_fence.png"));
    //Row 4
    if((spr->getNybble(13) % 2))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+180, spr->gety()+spr->getOffsetY()+180, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(13) == 2)||(spr->getNybble(13) == 3)||(spr->getNybble(13) == 6)||(spr->getNybble(13) == 7)||(spr->getNybble(13) == 10)||(spr->getNybble(13) == 11)||(spr->getNybble(13) >= 14))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+120, spr->gety()+spr->getOffsetY()+180, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(13) == 4)||(spr->getNybble(13) == 5)||(spr->getNybble(13) == 6)||(spr->getNybble(13) == 7)||(spr->getNybble(13) >= 12))
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+60, spr->gety()+spr->getOffsetY()+180, 60, 60, QPixmap(basePath + "path_fence.png"));
    if((spr->getNybble(13) >= 8))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY()+180, 60, 60, QPixmap(basePath + "path_fence.png"));
}

// Sprite 109: Signboard
SignboardRenderer::SignboardRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");
    QString signboardPath("sign_%1");

    if (spr->getNybble(5) < 10) img = new NormalImageRenderer(spr, basePath + signboardPath.arg(spr->getNybble(5)));
    else img = new NormalImageRenderer(spr, basePath + signboardPath.arg(7));
}

void SignboardRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 114: Floating Box
FloatingBoxRenderer::FloatingBoxRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "floating_box_big.png");
    else img = new NormalImageRenderer(spr, basePath + "floating_box_small.png");
}

void FloatingBoxRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 115: Bullet Bill Launcher
BulletBillLauncherRenderer::BulletBillLauncherRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BulletBillLauncherRenderer::render(QPainter *painter, QRect *)
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

// Sprite 118: Bullet Bill
BulletBillRenderer::BulletBillRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "bullet_bill_left.png");
    else if (spr->getNybble(5) == 2) img = new NormalImageRenderer(spr, basePath + "bullet_bill_down.png");
    else if (spr->getNybble(5) == 3) img = new NormalImageRenderer(spr, basePath + "bullet_bill_up.png");
    else if (spr->getNybble(5) == 4) img = new NormalImageRenderer(spr, basePath + "bullet_bill_down_left.png");
    else if (spr->getNybble(5) == 5) img = new NormalImageRenderer(spr, basePath + "bullet_bill_down_right.png");
    else if (spr->getNybble(5) == 6) img = new NormalImageRenderer(spr, basePath + "bullet_bill_up_right.png");
    else if (spr->getNybble(5) == 7) img = new NormalImageRenderer(spr, basePath + "bullet_bill_up_left.png");
    else img = new NormalImageRenderer(spr, basePath + "bullet_bill_right.png");
}

void BulletBillRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 119: Banzai Bill
BanzaiBillRenderer::BanzaiBillRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "banzai_bill_left.png");
    else img = new NormalImageRenderer(spr, basePath + "banzai_bill_right.png");
}

void BanzaiBillRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 120: Up-Down Mushroom
UpDownMushroomRenderer::UpDownMushroomRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void UpDownMushroomRenderer::render(QPainter *painter, QRect *)
{
    QString color;
    if((spr->getNybble(7)) < (spr->getNybble(4)))
    {
        color = "blue";
        offset = (spr->getNybble(4)-spr->getNybble(7))*20;
    }
    else
    {
        color = "pink";
        offset = (spr->getNybble(7)-spr->getNybble(4))*20;

    }

    // Inital Position
    for (int i = 0; i < spr->getNybble(4); i++)
        painter->drawPixmap(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+20+i*20, 20, 30), QPixmap(basePath + "stem.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 30, 30), QPixmap(basePath + color + "_l.png"));
    painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-10, spr->gety(), 30, 30), QPixmap(basePath + color + "_r.png"));
    for (int i = 30; i < spr->getwidth()-30; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety(), 20, 30), QPixmap(basePath + color + "_m.png"));

    // Final Position
    if (spr->getNybble(7) < spr->getNybble(4))
    {
        painter->setOpacity(0.3);
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+offset, 30, 30), QPixmap(basePath + color + "_l.png"));
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-10, spr->gety()+offset, 30, 30), QPixmap(basePath + color + "_r.png"));
        for (int i = 30; i < spr->getwidth()-30; i += 20)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety()+offset, 20, 30), QPixmap(basePath + color + "_m.png"));
        painter->setOpacity(1);
    }
    else
    {
        painter->setOpacity(0.3);
        for (int i = 0; i < spr->getNybble(7); i++)
            painter->drawPixmap(QRect(spr->getx(), spr->gety()-offset-20+((i+2)*20), 20, 30), QPixmap(basePath + "stem.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()-offset, 30, 30), QPixmap(basePath + color + "_l.png"));
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-10, spr->gety()-offset, 30, 30), QPixmap(basePath + color + "_r.png"));
        for (int i = 30; i < spr->getwidth()-30; i += 20)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety()-offset, 20, 30), QPixmap(basePath + color + "_m.png"));
        painter->setOpacity(1);
    }
}

// Sprite 123: Bouncy Mushroom
BouncyMushroomRenderer::BouncyMushroomRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BouncyMushroomRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(15) == 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "bouncy_mushroom_big.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "bouncy_mushroom.png"));
}


// Sprite 124: Mushroom Platform Renderer
MushroomPlatformRenderer::MushroomPlatformRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void MushroomPlatformRenderer::render(QPainter *painter, QRect *)
{
    painter->drawPixmap(QRect(spr->getx()-10, spr->gety()+spr->getOffsetY()+20, 20, 80), QPixmap(basePath + "stem_top.png"));
    for (int i = 0; i < spr->getNybble(10); i++)
        painter->drawPixmap(QRect(spr->getx()-10, spr->gety()+spr->getOffsetY()+100+i*20, 20, 20), QPixmap(basePath + "stem.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 24, 20), QPixmap(basePath + "l.png"));
    painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-24, spr->gety(), 24, 20), QPixmap(basePath + "r.png"));
    for (int i = 24; i < spr->getwidth()-24; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety(), 20, 20), QPixmap(basePath + "m.png"));

}


// Sprite 135: Goomba
GoombaRenderer::GoombaRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void GoombaRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(14) % 2 == 0) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "goomba.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "goomba_blue.png"));
}


// Sprite 136: Bone Goomba
BoneGoombaRenderer::BoneGoombaRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BoneGoombaRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(14) % 2 == 0) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "bone_goomba.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "bone_goomba_alt.png"));
}


// Sprite 127: Bowser Flame
BowserFlameRenderer::BowserFlameRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "dry_bowser_flame.png");
    else img = new NormalImageRenderer(spr, basePath + "bowser_flame.png");
}

void BowserFlameRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 139: Goomba Tower
GoombaTowerRenderer::GoombaTowerRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    top = new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY(), spr->getwidth(), 25), basePath + "goomba_tower_top.png");
    bottom = new NormalImageRenderer(QRect(spr->getx(), spr->gety(), spr->getwidth(), 21), basePath + "goomba_tower_bottom.png");

    if (spr->getNybble(5) < 3) return;
    if (spr->getNybble(5) == 0)
        middle.append(new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+46, spr->getwidth(), 21), basePath + "goomba_tower_middle.png"));
    else
    for (int i = 0; i < spr->getNybble(5)-2; i++) middle.append(new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+25+i*21, spr->getwidth(), 21), basePath + "goomba_tower_middle.png"));
}

void GoombaTowerRenderer::render(QPainter *painter, QRect *drawrect)
{
    top->render(painter, drawrect);
    for(int i = 0; i < middle.size(); i++) middle[i]->render(painter, drawrect);
    bottom->render(painter, drawrect);
}


// Sprite 144/145: Horizontal/Vertical Moving Lift
LiftRenderer::LiftRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void LiftRenderer::render(QPainter *painter, QRect *)
{
    QPen distLine;
    distLine.setColor(Qt::black);
    distLine.setWidth(2);

    painter->setPen(distLine);
    if (spr->getNybble(7)!= 0)
    {
        if (spr->getNybble(9) == 1)
        {
            if (spr->getid() == 145)
                painter->drawLine(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+(spr->getNybble(7)*20));
            else
                painter->drawLine(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-(spr->getNybble(7)*20), spr->gety()+spr->getheight()/2);
        }
        else
        {
            if (spr->getid() == 145)
                painter->drawLine(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()-20-(spr->getNybble(7)*20));
            else
                painter->drawLine(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+(spr->getNybble(7)*20), spr->gety()+spr->getheight()/2);
        }
    }

    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 20, 22), QPixmap(basePath + "l.png"));
    if(spr->getNybble(5) == 0)
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()+(spr->getNybble(5))*20+20, spr->gety(), 22, 22), QPixmap(basePath + "r.png"));
    else
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()+(spr->getNybble(5)-1)*20+20, spr->gety(), 22, 22), QPixmap(basePath + "r.png"));
    for (int i = 20; i < spr->getwidth()-20; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety(), 20, 22), QPixmap(basePath + "m.png"));
}


// Sprite 146: Track Controlled Lift
TrackLiftRenderer::TrackLiftRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void TrackLiftRenderer::render(QPainter *painter, QRect *)
{
    QString type;
    if((spr->getNybble(6) != 1))
        type = "wood";
    else
        type = "stone";

    QString rounded;
    if((spr->getNybble(7) != 1))
        rounded = "";
    else
        rounded = "_rounded";

    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 22, 22), QPixmap(basePath + type + rounded + "_l.png"));
    painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()+58, spr->gety()+spr->getOffsetY(), 22, 22), QPixmap(basePath + type + rounded + "_r.png"));
    for (int i = 22; i < spr->getwidth()-22; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety()+spr->getOffsetY(), 20, 22), QPixmap(basePath + type + rounded + "_m.png"));
}

// Sprite 147: 3 Plat Rickshaw
ThreePlatRickRenderer::ThreePlatRickRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "3Plat_Rickshaw_Big.png");
    else img = new NormalImageRenderer(spr, basePath + "3Plat_Rickshaw.png");
}

void ThreePlatRickRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 151: Scale Platform Renderer
ScalePlatformRenderer::ScalePlatformRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;

    //used to keep platforms centered
    if (spr->getNybble(15) == 0)
        poffset = 28;
    else
        poffset = 28 + ((spr->getNybble(15)-1)*10);
}

void ScalePlatformRenderer::render(QPainter *painter, QRect *)
{
    //Rope
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 17, 16), QPixmap(basePath + "left.png"));
    for (int i = 0; i < spr->getNybble(5); i++)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+17, spr->gety()+spr->getOffsetY()+1, 20*i, 4), QPixmap(basePath + "top.png"));
    if (spr->getNybble(5) == 0)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+10, spr->gety()+spr->getOffsetY(), 17, 16), QPixmap(basePath + "right.png"));
    else
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getNybble(5)*20-10), spr->gety()+spr->getOffsetY(), 17, 16), QPixmap(basePath + "right.png"));
    for (int i = 0; i < spr->getNybble(7); i++)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+2, spr->gety()+spr->getOffsetY()+16, 4, (20*i)+7), QPixmap(basePath + "down.png"));
    for (int i = 0; i < spr->getNybble(4); i++)
        if (spr->getNybble(5) == 0)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+22, spr->gety()+spr->getOffsetY()+16, 4, (20*i)+7), QPixmap(basePath + "down.png"));
        else
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getNybble(5)*20+2), spr->gety()+spr->getOffsetY()+16, 4, (20*i)+7), QPixmap(basePath + "down.png"));
    //Platforms
    if ((spr->getNybble(15) == 0))
    {
        //Left Platform
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset, spr->gety()+spr->getOffsetY()+(20*spr->getNybble(7))+3, 22, 22), QPixmap(basePath + "platform_l.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+22, spr->gety()+spr->getOffsetY()+(20*spr->getNybble(7))+3, 20, 22), QPixmap(basePath + "platform_m.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+42, spr->gety()+spr->getOffsetY()+(20*spr->getNybble(7))+3, 22, 22), QPixmap(basePath + "platform_r.png"));
        //Right Platform
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+(spr->getNybble(5)*20), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(4))+3, 22, 22), QPixmap(basePath + "platform_l.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+22+(spr->getNybble(5)*20), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(4))+3, 20, 22), QPixmap(basePath + "platform_m.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+42+(spr->getNybble(5)*20), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(4))+3, 22, 22), QPixmap(basePath + "platform_r.png"));
    }
    else
    {
        //Left Platform
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset, spr->gety()+spr->getOffsetY()+(20*spr->getNybble(7))+3, 22, 22), QPixmap(basePath + "platform_l.png"));
        for (int i = 0; i < spr->getNybble(15); i++)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+22+(20*i), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(7))+3, 20, 22), QPixmap(basePath + "platform_m.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+22+(20*(spr->getNybble(15))), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(7))+3, 22, 22), QPixmap(basePath + "platform_r.png"));
        //Right Platform
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+(spr->getNybble(5)*20), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(4))+3, 22, 22), QPixmap(basePath + "platform_l.png"));
        for (int i = 0; i < spr->getNybble(15); i++)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+22+(20*i)+(spr->getNybble(5)*20), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(4))+3, 20, 22), QPixmap(basePath + "platform_m.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-poffset+22+(20*(spr->getNybble(15)))+(spr->getNybble(5)*20), spr->gety()+spr->getOffsetY()+(20*spr->getNybble(4))+3, 22, 22), QPixmap(basePath + "platform_r.png"));
    }
}

// Sprite 154: 4 Plat Rickshaw
FourPlatRickRenderer::FourPlatRickRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "4Plat_Rickshaw.png");
    else img = new NormalImageRenderer(spr, basePath + "4Plat_Rickshaw_Big.png");
}

void FourPlatRickRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 159: Spike Top
SpikeTopRenderer::SpikeTopRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1)
        flipped = "flipped_";
    else
        flipped = "";

    if (spr->getNybble(4) == 1) img = new NormalImageRenderer(spr, basePath + flipped + "spiketop_left.png");
    else if (spr->getNybble(4) == 2) img = new NormalImageRenderer(spr, basePath + flipped +"spiketop_ceiling.png");
    else if (spr->getNybble(4) == 3) img = new NormalImageRenderer(spr, basePath + flipped + "spiketop_right.png");
    else img = new NormalImageRenderer(spr, basePath + flipped + "spiketop.png");
}

void SpikeTopRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 163: Climbing Koopa - Horizontal
ClimbKoopaHorRenderer::ClimbKoopaHorRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "climbing_koopa_red.png");
    else img = new NormalImageRenderer(spr, basePath + "climbing_koopa.png");
}

void ClimbKoopaHorRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 164: Climbing Koopa - vertical
ClimbKoopaVertRenderer::ClimbKoopaVertRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if(spr->getNybble(4) == 1)
         behind = "behind_";
    else
         behind = "";

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + behind + "climbing_koopa_red.png");
    else img = new NormalImageRenderer(spr, basePath + behind + "climbing_koopa.png");
}

void ClimbKoopaVertRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 165: Koopa Troopa
KoopaTroopaRenderer::KoopaTroopaRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "koopa_troopa_red.png");
    else img = new NormalImageRenderer(spr, basePath + "koopa_troopa_green.png");
}

void KoopaTroopaRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 175/176/177/178/179/180: Grounded Piranha plants
PlantRenderer::PlantRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;
    if (spr->getNybble(5) == 1) filename.insert(filename.size()-4, "_flipped");

    img = new NormalImageRenderer(spr, filename);
}

void PlantRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 185: Koopa Paratroopa
KoopaParatroopaRenderer::KoopaParatroopaRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "koopa_paratroopa_red.png");
    else img = new NormalImageRenderer(spr, basePath + "koopa_paratroopa_green.png");
}

void KoopaParatroopaRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 194: Cheep Cheep
CheepCheepRenderer::CheepCheepRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath =basePath;

    switch (spr->getNybble(5))
    {
        case 1: filename = "_green"; break;
        case 3: case 4: case 6: case 7: filename = "_right"; break;
        case 8: filename = "_orange"; break;
        default: filename = ""; break;
    }

    img = new NormalImageRenderer(spr, basePath + filename + ".png");
}
void CheepCheepRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 195: Big Cheep Cheep
BigCheepRenderer::BigCheepRenderer(const Sprite *spr)
{
    if (spr->getNybble(5) == 1)
        img = new NormalImageRenderer(spr, "big_cheep_cheep_green.png");
    else
        img = new NormalImageRenderer(spr, "big_cheep_cheep.png");
}

void BigCheepRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 213: Pokey
PokeyRenderer::PokeyRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void PokeyRenderer::render(QPainter *painter, QRect *)
{
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY(), 33, 33), ImageCache::getInstance()->get(SpriteImg, "pokey_top.png"));
    for(int i = 0; i < spr->getNybble(5)+1; i++)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY()+32+(i*22), 33, 22), ImageCache::getInstance()->get(SpriteImg, "pokey_mid.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY()+54+(spr->getNybble(5)*22), 33, 24), ImageCache::getInstance()->get(SpriteImg, "pokey_bot.png"));
}

// Sprite 221/223/225: Switches
SwitchRenderer::SwitchRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;
    if (spr->getNybble(5) == 1) filename.insert(filename.size()-4, "_flipped");

    img = new NormalImageRenderer(spr, filename);
}

void SwitchRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// RecLiftRenderer
RecLiftRenderer::RecLiftRenderer(const Sprite *spr, QString path)
{
    this->spr = spr;
    this->path = path;
    if (spr->getid() == 192) sideOffset = 3;
}
void RecLiftRenderer::render(QPainter *painter, QRect *)
{
    QPen distLine;
    distLine.setColor(Qt::black);
    distLine.setWidth(2);

    if (spr->getNybble(16) != 0)
    {
        painter->setPen(distLine);
        if (spr->getNybble(11) %4 == 0) painter->drawLine(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+(spr->getNybble(16)*20), spr->gety()+spr->getheight()/2);
        if (spr->getNybble(11) %4 == 1) painter->drawLine(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-(spr->getNybble(16)*20), spr->gety()+spr->getheight()/2);

        if (spr->getNybble(11) %4 == 2) painter->drawLine(spr->getx()+spr->getwidth()/2, spr->gety(), spr->getx()+spr->getwidth()/2, spr->gety()-(spr->getNybble(16)*20));
        if (spr->getNybble(11) %4 == 3) painter->drawLine(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+(spr->getNybble(16)*20));
    }

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


// Sprite 215: Bob-omb Cannon
BobOmbCannonRenderer::BobOmbCannonRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BobOmbCannonRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) != 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "cannon_right.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "cannon_left.png"));
}


// Sprite 232: Spiny
SpinyRenderer::SpinyRenderer(const Sprite *spr, QString basePath)
{
    this->spr = spr;
    this->basePath = basePath;
}

void SpinyRenderer::render(QPainter *painter, QRect *)
{
    if (spr->getNybble(5) == 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_ceiling.png"));
    else if (spr->getNybble(5) == 2) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_shell.png"));
    else if (spr->getNybble(5) == 3) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_shell_ceiling.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + ".png"));
}


// Sprite 240: Urchin
UrchinRenderer::UrchinRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void UrchinRenderer::render(QPainter *painter, QRect *)
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

void ChainChompRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5)%2 == 0) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "chain_chomp_3.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "chain_chomp_1.png"));
}

// Sprites 100/101/203/204/267/275/276: Item Blocks
ItemBlockRenderer::ItemBlockRenderer(const Sprite *spr, QString filename)
{
    this->spr = spr;
    block = new NormalImageRenderer(spr, filename);
}

void ItemBlockRenderer::render(QPainter *painter, QRect *drawrect)
{
    block->render(painter, drawrect);

    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/tileoverlays/");
    QString overlay;

    switch (spr->getNybble(13))
    {
        case 0: break;
        case 1: overlay = "coin.png"; break;
        case 2: overlay = "fire_flower.png"; break;
        case 4: overlay = "super_leaf.png"; break;
        case 5: overlay = "gold_flower.png"; break;
        case 6: overlay = "mini_mushroom.png"; break;
        case 7: overlay = "super_star.png"; break;
        case 8: overlay = "coin_star.png"; break;
        case 10: overlay = "10_coins.png"; break;
        case 11: overlay = "1up_mushroom.png"; break;
        case 13: overlay = "trampoline.png"; break;
        case 14: overlay = "coin_super_mushroom.png"; break;
        default: return; break;
    }

    if ((spr->getheight() == 20) && (spr->getwidth() == 20))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + overlay));
    else
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+20, spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + overlay));

    if((spr->getid() == 100) || (spr->getid() == 203))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + "rotation.png"));
    if((spr->getid() == 101) || (spr->getid() == 204))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(basePath + "movement.png"));
}

// Sprite 253: Larry Battle Platform
LarryPlatformRenderer::LarryPlatformRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void LarryPlatformRenderer::render(QPainter *painter, QRect *)
{
    int b_width = spr->getNybble(10);
    if (b_width < 1)
        b_width = 1;

    for (int i=0; i < b_width; i++)
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+i*20, spr->gety()+spr->getOffsetY(), 20, 10, ImageCache::getInstance()->get(SpriteImg, "larry_platform.png"));
}

// Sprite 259: Rotation Controlled hard Block
HardBlockRenderer::HardBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void HardBlockRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(12) == 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "pivot_block.png"));
    else if (spr->getNybble(12) == 2)painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "used_block.png"));
    else if (spr->getNybble(12) >= 3)painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "brick_block.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "stone_block.png"));
    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, QPixmap(QCoreApplication::applicationDirPath() + "/coinkiller_data/tileoverlays/rotation.png"));

}

// Sprite 280: + Clock
ClockRenderer::ClockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ClockRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) != 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "clock_50.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "clock_10.png"));
}

// Sprite 297: Horizontal Rail Controlled Fence
RailContFenceRenderer::RailContFenceRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void RailContFenceRenderer::render(QPainter *painter, QRect *)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/rail_fence");

    if (spr->getNybble(5) == 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_3x6.png"));
    else if (spr->getNybble(5) == 2) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_6x3.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), QPixmap(basePath + "_3x3.png"));
}

// Sprite 134: 3 Plat Rickshaw ruins
RuinsRickRenderer::RuinsRickRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "3Plat_Rickshaw_Ruins_Big.png");
    else img = new NormalImageRenderer(spr, basePath + "3Plat_Rickshaw_Ruins.png");
}

void RuinsRickRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 311: Coin Meteor
CoinMeteorRenderer::CoinMeteorRenderer(const Sprite *spr)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    if (spr->getNybble(5) == 1) img = new NormalImageRenderer(spr, basePath + "coin_meteor_big.png");
    else img = new NormalImageRenderer(spr, basePath + "coin_meteor.png");
}

void CoinMeteorRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Entrance Renderer
EntranceRenderer::EntranceRenderer(const Entrance *entrance)
{
    this->entr = entrance;

    this->rect = new RoundedRectRenderer(entrance, "", QColor(255,0,0,150));
}

void EntranceRenderer::render(QPainter *painter, QRect *drawrect)
{
    rect->render(painter, drawrect);
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


// Liquid Renderer

LiquidRenderer::LiquidRenderer(const Sprite *liquid, const Zone *zone)
{
    this->liquid = liquid;
    this->zone = zone;

    if (liquid->getid() == 12)
        filename = "lava";
    else if (liquid->getid() == 13)
        filename = "poison";
    else
        filename = "water";
}

void LiquidRenderer::render(QPainter *painter, QRect *drawrect)
{
    QString basePath(QCoreApplication::applicationDirPath() + "/coinkiller_data/sprites/");

    QPixmap top = QPixmap(basePath + filename + "_top.png");
    QPixmap base = QPixmap(basePath + filename + ".png");

    int currY = liquid->gety() - 20;

    for (int x = zone->getx(); x < zone->getx() + zone->getwidth(); x += top.width())
    {
        QRect rect = QRect(x, currY, qMin(zone->getx() + zone->getwidth() - x, top.width()), qMin(zone->gety() + zone->getheight() - currY, top.height()));

        if (!drawrect->intersects(rect))
            continue;

        painter->drawPixmap(rect, top, QRect(0, 0, rect.right()-rect.left(), rect.bottom()-rect.top()));
    }

    currY += top.height();

    for (; currY < zone->gety() + zone->getheight(); currY += base.height())
    {
        for (int x = zone->getx(); x < zone->getx() + zone->getwidth(); x += base.width())
        {
            QRect rect = QRect(x, currY, qMin(zone->getx() + zone->getwidth() - x, base.width()), qMin(zone->gety() + zone->getheight() - currY, base.height()));

            if (!drawrect->intersects(rect))
                continue;

            painter->drawPixmap(rect, base, QRect(0, 0, rect.right()-rect.left(), rect.bottom()-rect.top()));
        }
    }
}
