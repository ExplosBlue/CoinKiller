#include "objectrenderer.h"
#include "objects.h"
#include "imagecache.h"

#include <QPainter>
#include <QPainterPath>

SpriteRenderer::SpriteRenderer(const Sprite *spr, Tileset *tilesets[])
{
    this->spr = spr;

    switch (spr->getid()) {
    case 1: // Water Flow For Pipe
        ret = new PipeFlowRenderer(spr);
        break;
    case 2: // Downwards Water Pull
        ret = new DownWaterPullRenderer(spr);
        break;
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
        ret = new WhompRenderer(spr);
        break;
    case 10: // Switchable Conveyor Belt - Lemmy Battle
        ret = new NormalImageRenderer(spr, "switchable_conveyor_belt_lemmy.png");
        break;
    case 16: // Amp
        ret = new NormalImageRenderer(spr, "amp.png");
        break;
    case 17: // Amp Circle
        ret = new CoinCircleRenderer(spr, tilesets[0]);
        break;
    case 18: // Tile God
        ret = new TileGodRenderer(spr, tilesets[0]);
        break;
    case 19: // Desert Crater
        ret = new NormalImageRenderer(spr, "desert_crater.png");
        break;
    case 20: // Gold Block
        ret = new GoldBlockRenderer(spr, tilesets[0]);
        break;
    case 21: // Note Block
        ret = new NoteBlockRenderer(spr);
        break;
    case 22: // Special Exit Controller
        ret = new SpecialExitControllerRenderer(spr);
        break;
    case 24: case 25: // Flying Question Block
        ret = new FlyingQuestionBlockRenderer(spr);
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
    case 34: // Sledge Bro.
        ret = new NormalImageRenderer(spr, "sledge_bro.png");
        break;
    case 35: // Lava Bubble
        ret = new NormalImageRenderer(spr, "lava_bubble.png");
        break;
    case 36: // Arc Lava Bubble
        ret = new ArcLavaBubbleRenderer(spr);
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
        ret = new FourBurnerRenderer(spr);
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
        ret = new SkewerRenderer(spr);
        break;
    case 64: // Skewer Right
        ret = new SkewerRenderer(spr);
        break;
    case 65: // Morton Pipe
        ret = new NormalImageRenderer(spr, "morton_pipe.png");
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
    case 74: // Ghost House Door - Event triggered
        ret = new NormalImageRenderer(spr, "door_ghosthouse_event.png");
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
    case 82: // Fire Snake
        ret = new NormalImageRenderer(spr, "fire_snake.png");
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
    case 91: // Coin Coffer
        ret = new NormalImageRenderer(spr, "coin_coffer.png");
        break;
    case 92: // Grinder
        ret = new NormalImageRenderer(spr, "grinder.png");
        break;
    case 93: // Scuttlebug
        ret = new NormalImageRenderer(spr, "scuttlebug.png");
        break;
    case 94: // Flipper (One way gate)
        ret = new FlipperRenderer(spr);
        break;
    case 95: // Blooper
        ret = new BlooperRenderer(spr);
        break;
    case 96: // Blooper Nanny
        ret = new BlooperRenderer(spr);
        break;
    case 97: // End of Level Flag
        ret = new GoalRenderer(spr);
        break;
    case 99: // Wiggler
        ret = new NormalImageRenderer(spr, "wiggler.png");
        break;
    case 100: // Rotation Controlled ? Block
        ret = new ItemBlockRenderer(spr, tilesets[0]);
        break;
    case 101: // Movement Controlled ? Block
        ret = new ItemBlockRenderer(spr, tilesets[0]);
        break;
    case 102: // Iggy
        ret = new NormalImageRenderer(spr, "iggy.png");
        break;
    case 104: // Lakitu
        ret = new NormalImageRenderer(spr, "lakitu.png");
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
    case 113: // Flame Chomp
        ret = new NormalImageRenderer(spr, "flame_chomp.png");
        break;
    case 114: // Floating Box
        ret = new FloatingBoxRenderer(spr);
        break;
    case 115: // Bullet Bill Launcher
        ret = new BulletBillLauncherRenderer(spr);
        break;
    case 116: // Bullet Bill Turret
        ret = new BulletBillTurretRenderer(spr);
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
        ret = new UpDownMushroomRenderer(spr);
        break;
    case 121: // Expanding Mushroom
        ret = new ExpandMushroomRenderer(spr);
        break;
    case 122: // Synchronized Expanding Mushroom
        ret = new ExpandMushroomRenderer(spr);
        break;
    case 123: // Bouncy Mushroom
        ret = new BouncyMushroomRenderer(spr);
        break;
    case 124: // Mushroom Platform
        ret = new MushroomPlatformRenderer(spr);
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
    case 131: // Bowser Block
        ret = new BowserBlockRenderer(spr, tilesets[0]);
        break;
    case 132: // Bowser Battle Switch Controller
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
    case 141: // Larry
        ret = new NormalImageRenderer(spr, "larry.png");
        break;
    case 142: // Lemmy
        ret = new NormalImageRenderer(spr, "lemmy.png");
        break;
    case 143: // Conveyor Belt Switch
        ret = new NormalImageRenderer(spr, "conveyor_belt_switch.png");
        break;
    case 144: // Horizontal Lift
        ret = new LiftRenderer(spr);
        break;
    case 145: // Vertical Lift
        ret = new LiftRenderer(spr);
        break;
    case 146: // Track Controlled Lift
        ret = new TrackLiftRenderer(spr);
        break;
    case 147: // 3 Plat rickshaw
        ret = new ThreePlatRickRenderer(spr);
        break;
    case 148: // Move While On Lift
        ret = new NormalImageRenderer(spr, "move_while_on_lift.png");
        break;
    case 149: // Changeable Direction Platform
        ret = new NormalImageRenderer(spr, "change_direction_platofrm.png");
        break;
    case 150: // Seesaw Lift
        ret = new NormalImageRenderer(spr, "seesaw_lift.png");
        break;
    case 151: // Scale Lift
        ret = new ScalePlatformRenderer(spr);
        break;
    case 152: // Path Controlled Lift With Peepa
        ret = new PeepaLiftRenderer(spr);
        break;
    case 154: // 3 Plat rickshaw
        ret = new FourPlatRickRenderer(spr);
        break;
    case 155: // Ludwig
        ret = new NormalImageRenderer(spr, "ludwig.png");
        break;
    case 158: // Buzzy Beetle
        ret = new BuzzyBeetleRenderer(spr);
        break;
    case 159: // Spike Top
        ret = new SpikeTopRenderer(spr);
        break;
    case 162: // Morton
        ret = new NormalImageRenderer(spr, "morton.png");
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
        ret = new PlantRenderer(spr, "piranha_plant");
        break;
    case 176: // Big Grounded Pirahna Plant
        ret = new PlantRenderer(spr, "big_piranha_plant");
        break;
    case 177: // Grounded Fire Piranha Plant
        ret = new PlantRenderer(spr, "fire_piranha_plant");
        break;
    case 178: // Big Grounded Fire Pirahna Plant
        ret = new PlantRenderer(spr, "big_fire_piranha_plant");
        break;
    case 179: // Grounded Bone Piranha Plant
        ret = new PlantRenderer(spr, "bone_piranha_plant");
        break;
    case 180: // Big Grounded Bone Pirahna Plant
        ret = new PlantRenderer(spr, "big_bone_piranha_plant");
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
    case 186: // Paratroopa Circle
        ret = new CoinCircleRenderer(spr, tilesets[0]);
        break;
    case 187: // Path Controlled Rectangle Lift
        ret = new PathRecLiftRenderer(spr, tilesets[0]);
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
        ret = new CheepCheepRenderer(spr);
        break;
    case 195: // Big Cheep Cheep
        ret = new BigCheepRenderer(spr);
        break;
    case 196: // Coin Cheep
        ret = new NormalImageRenderer(spr, "coin_cheep.png");
        break;
    case 197: // Jumping Cheep Cheep
        ret = new JumpingCheepRenderer(spr);
        break;
    case 199: // Cheep Cheep Circle
        ret = new CoinCircleRenderer(spr, tilesets[0]);
        break;
    case 200: // Spiny Cheep Cheep
        ret = new SpinyCheepRenderer(spr);
        break;
    case 203: // Rotation Controlled Brick Block
        ret = new ItemBlockRenderer(spr, tilesets[0]);
        break;
    case 204: // Movement Controlled Brick Block
        ret = new ItemBlockRenderer(spr, tilesets[0]);
        break;
    case 205: // Red Ring
        ret = new NormalImageRenderer(spr, "red_ring.png");
        break;
    case 206: // Gold Ring
        ret = new NormalImageRenderer(spr, "gold_ring.png");
        break;
    case 207: // Underwater Rock
        ret = new NormalImageRenderer(spr, "underwater_rock.png");
        break;
    case 208: // Big Underwater Rock
        ret = new NormalImageRenderer(spr, "big_underwater_rock.png");
        break;
    case 209: // Swinging Rope
        ret = new SwingingRopeRenderer(spr);
        break;
    case 210: // Tightrope
        ret = new TightropeRenderer(spr);
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
        ret = new BrickBlockSwitchRenderer(spr, "exclamation_switch.png", tilesets[0]);
        break;
    case 223: // ? Switch
        ret = new SwitchRenderer(spr, "question_switch.png");
        break;
    case 224: // Brick Block Contaning ? Switch
        ret = new BrickBlockSwitchRenderer(spr, "question_switch.png", tilesets[0]);
        break;
    case 225: // P Switch
        ret = new SwitchRenderer(spr, "p_switch.png");
        break;
    case 226: // Brick Block Contaning P Switch
        ret = new BrickBlockSwitchRenderer(spr, "p_switch.png", tilesets[0]);
        break;
    case 227: // Floating Barrel
        ret = new NormalImageRenderer(spr, "barrel.png");
        break;
    case 228: // Boo
        ret = new NormalImageRenderer(spr, "boo.png");
        break;
    case 229: // Big Boo
        ret = new BigBooRenderer(spr);
        break;
    case 230: // Peepa Circle
        ret = new CoinCircleRenderer(spr, tilesets[0]);
        break;
    case 231: // Boohemoth
        ret = new BoohemothRenderer(spr);
        break;
    case 232: // Spiny
        ret = new SpinyRenderer(spr);
        break;
    case 233: // Celing Spiny
        ret = new CeilingSpinyRenderer(spr);
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
    case 239: // Swinging Vine
        ret = new SwingingVineRenderer(spr);
        break;
    case 240: // Urchin
        ret = new UrchinRenderer(spr);
        break;
    case 242: // Whirlpool
        ret = new NormalImageRenderer(spr, "whirlpool.png");
        break;
    case 243: // Ghost Wall
        ret = new GhostWallRenderer(spr);
        break;
    case 244: // Chain Chomp
        ret = new ChainChompRenderer(spr);
        break;
    case 245: // Wendy
        ret = new NormalImageRenderer(spr, "wendy.png");
        break;
    case 247: // Invisible Bouncy Block
        ret = new InvisibleBouncyBlockRenderer(spr);
        break;
    case 248: // Move Once On Ghost House Platform
        ret = new MoveOnGhostPlatRenderer(spr);
        break;
    case 249: // Rotation Controlled Rectangle Lift - Tower
        ret = new RotRecLiftRenderer(spr, "tower_rectangle_lift/");
        break;
    case 250: // Rotation Controlled Rectangle Lift - GhostHouse (checkered)
        ret = new RotRecLiftRenderer(spr, "checkered_rectangle_lift/");
        break;
    case 251: // Treasure Chest
        ret = new NormalImageRenderer(spr, "treasure_chest.png");
        break;
    case 252: // Item
        ret = new ItemRenderer(spr);
        break;
    case 253: // Larry Battle Platform
        ret = new LarryPlatformRenderer(spr, tilesets[0]);
        break;
    case 255: // Bowser Head Statue
        ret = new NormalImageRenderer(spr, "bowser_head_statue.png");
        break;
    case 257: // Movement Controlled Bone Platform
        ret = new BonePlatformRenderer(spr);
        break;
    case 259: // Rotation Controlled Hard Block
        ret = new HardBlockRenderer(spr, tilesets[0]);
        break;
    case 267: // Long Question Block
        ret = new ItemBlockRenderer(spr, tilesets[0]);
        break;
    case 268: // Rectangle Lift - Lava
        ret = new LavaRectLiftRenderer(spr);
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
        ret = new CoinRouletteRenderer(spr);
        break;
    case 274: // Flying Gold Block Spawn Point
        ret = new NormalImageRenderer(spr, "flying_gold_block.png");
        break;
    case 275: // Long Question Block - Underground
        ret = new ItemBlockRenderer(spr, tilesets[0]);
        break;
    case 276: // Long Question Block - Lava
        ret = new ItemBlockRenderer(spr, tilesets[0]);
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
    case 282: // Rotation Controlled Rectangle Lift - GhostHouse
        ret = new RecLiftRenderer(spr, "ghost_rectangle_lift/");
        break;
    case 283: // Rectangle Lift Larry Battle
        ret = new EventRecLiftRenderer(spr, "rect_lift_larry");
        break;
    case 284: // Chandelier Lift - Small
        ret = new NormalImageRenderer(spr, "chandelier_small.png");
        break;
    case 285: // Chandelier Lift - Medium
        ret = new NormalImageRenderer(spr, "chandelier_medium.png");
        break;
    case 286: // Chandelier Lift - Big
        ret = new NormalImageRenderer(spr, "chandelier_big.png");
        break;
    case 287: // Toad House Door
        ret = new NormalImageRenderer(spr, "door_toadhouse.png");
        break;
    case 289: // Bouncy Mushroom Platform - Castle
        ret = new BounceMushCastleRenderer(spr);
        break;
    case 290: // Path Controlled Fence(Small)
        ret = new NormalImageRenderer(spr, "small_path_fence.png");
        break;
    case 291: // Path Controlled Fence(Big)
        ret = new NormalImageRenderer(spr, "big_path_fence.png");
        break;
    case 292: // Warp Cannon Signboard
        ret = new NormalImageRenderer(spr, "sign_big.png");
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
    case 299: // Rectangle Lift Lemmy Battle
        ret = new NormalImageRenderer(spr, "rect_lift_lemmy.png");
        break;
    case 300: // Applause Coin
        ret = new NormalImageRenderer(spr, "coin.png");
        break;
    case 302: // Moon Coin
        ret = new NormalImageRenderer(spr, "moon_coin.png");
        break;
    case 303: // Rotation Controlled Moon Coin
        ret = new RotationSpriteRenderer(spr, "moon_coin.png");
        break;
    case 305: // Path Controlled Ice Lift
        ret = new IceLiftRenderer(spr);
        break;
    case 307: // Event Activated Rectangle Lift - Ruins
        ret = new EventRecLiftRenderer(spr, "rect_lift_ruins");
        break;
    case 308: // Event Activated Rectangle Lift - Sand
        ret = new EventRecLiftRenderer(spr, "rect_lift_sand");
        break;
    case 311: // Coin Meteor
        ret = new CoinMeteorRenderer(spr);
        break;
    case 313: // Underwater Rectangle Lift
        ret = new UnderwaterRecLiftRenderer(spr);
        break;
    case 314: // Ruins Rickshaw
        ret = new RuinsRickRenderer(spr);
        break;
    case 315: // Peach
        ret = new NormalImageRenderer(spr, "peach.png");
        break;
    case 318: // Event Controlled Rectangle Lift
        ret = new EventRecLiftRenderer(spr, "event_rect_lift_tower");
        break;
    case 320: // Path Controlled Lift
        ret = new PathLiftRenderer(spr);
        break;
    case 321: // Snowy Mushroom Platform
        ret = new MushroomPlatformRenderer(spr);
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

    painter->setPen(Qt::NoPen);
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

    painter->setPen(Qt::NoPen);
}

MovIndicatorRenderer::MovIndicatorRenderer(int x, int y, int distX, int distY, bool vertical, QColor color)
{
    this->x = x;
    this->y = y;
    this->distX = distX;
    this->distY = distY;
    this->vertical = vertical;
    this->color = color;
}

void MovIndicatorRenderer::render(QPainter *painter)
{
    QPen outline(QColor(0,0,0,150), 8, Qt::SolidLine);
    QPen fill(color, 4, Qt::SolidLine);

    painter->setPen(outline);

    if (vertical && distY == y)
        return;

    else if (!vertical && distX == x)
        return;

    for (int i = 0;  i <= 1; i++)
    {

        if ( i == 1)
            painter->setPen(fill);
        else
            painter->setPen(outline);

        if (vertical)
        {
            if (distY < y) // Go Up
            {
                painter->drawLine(x, y+20, x, distY+18);
                painter->drawEllipse(x-5, distY+5, 10, 10);
            }
            else // Go Down
            {
                painter->drawLine(x, y-20, x, distY-18);
                painter->drawEllipse(x-5, distY-15, 10, 10);
            }
        }
        else
        {
            if (distX < x) // Go Left
            {
                painter->drawLine(x+20, y, distX+18, y);
                painter->drawEllipse(distX+5, y-5, 10, 10);
            }
            else // Go Right
            {
                painter->drawLine(x-20, y, distX-18, y);
                painter->drawEllipse(distX-15, y-5, 10, 10);
            }
        }
    }

    painter->setPen(Qt::NoPen);
}

// Rotate a pixmap around an arbitrary point
QPixmap rotateAroundPoint(QPixmap image, QPointF curPos, QPointF anchor, qreal angle, QPointF &newPos)
{
    qreal angleRads = qDegreesToRadians(angle);
    QPointF center(image.width() / 2, image.height() / 2);
    qreal dist = QLineF(anchor, center).length();
    qreal a = qAtan2(anchor.y() - center.y(), anchor.x() - center.x());
    QPointF rotAnchor(qCos(angleRads + a) * dist, qSin(angleRads + a) * dist);
    rotAnchor += center;

    QPixmap rotImage = image.transformed(QTransform().rotateRadians(angleRads));

    QPointF rotCenter(rotImage.width() / 2, rotImage.height() / 2);
    QPointF offset = rotCenter - center;

    newPos = curPos - (rotAnchor + offset);

    return rotImage;
}

// Sprite Renderers

// Sprite 1: Water Flow For Pipe
PipeFlowRenderer::PipeFlowRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void PipeFlowRenderer::render(QPainter *painter, QRect *)
{
    QPixmap air = ImageCache::getInstance()->get(SpriteImg, "air.png");
    QPixmap bubbles = ImageCache::getInstance()->get(SpriteImg, "bubbles.png");

    switch (spr->getNybble(11))
    {
        case 1:
            air = air.transformed(QTransform().rotate(180));
            bubbles = bubbles.transformed(QTransform().rotate(180));
            break;
        case 2:
            air = air.transformed(QTransform().rotate(90));
            bubbles = bubbles.transformed(QTransform().rotate(90));
            break;
        case 3:
            air = air.transformed(QTransform().rotate(-90));
            bubbles = bubbles.transformed(QTransform().rotate(-90));
            break;
        default:
            break;
    }

    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    if (spr->getNybble(4) != 1)
        painter->drawPixmap(x, y, spr->getwidth(), spr->getheight(), air);
    painter->drawPixmap(x, y, spr->getwidth(), spr->getheight(), bubbles);
}

// Sprite 2: Downwards Water Pull
DownWaterPullRenderer::DownWaterPullRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void DownWaterPullRenderer::render(QPainter *painter, QRect *)
{
    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    if (spr->getNybble(4) == 1)
        painter->drawPixmap(x, y, spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "down_water_pull.png"));
    else
        painter->drawPixmap(x, y, spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "down_water_pull_big.png"));
}

// Sprite 4/5/6/7: Burner Right/Down/Left/Up
BurnerRenderer::BurnerRenderer(const Sprite *spr, QString name)
{
    this->spr = spr;
    this->name = name;
}

void BurnerRenderer::render(QPainter *painter, QRect *)
{
    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    if (spr->getNybble(10) == 1)
        painter->drawPixmap(x, y, spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, name + "_big.png"));
    else
        painter->drawPixmap(x, y, spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, name + ".png"));
}

// Sprite 9: Whomp
WhompRenderer::WhompRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(11) == 1)
        filename  = "whomp_big.png";
    else
        filename = "whomp.png";

    img = new NormalImageRenderer(spr, filename);
}

void WhompRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 18: Tile God
TileGodRenderer::TileGodRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void TileGodRenderer::render(QPainter *painter, QRect *)
{
    QRect sprrect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight());

    if ((spr->getNybble(9) % 2) == 0)
    {
        QBrush b(Qt::BDiagPattern);
        b.setColor(QColor(25, 25, 25));

        painter->fillRect(sprrect, b);

        painter->setPen(QPen(Qt::black));
    }

    else
    {
        if (tileset == nullptr)
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

            if (spr->getNybble(8) < 12)
                tileId = tileIds[spr->getNybble(8)];


            for (int x = 0; x < spr->getwidth()/20; x++)
                for (int y = 0; y < spr->getheight()/20; y++)
                {
                    if (spr->getNybble(7) == 1)
                    {
                        if (x % 2 != 0) // row odd
                        {
                            if (y % 2 != 0 ) // column odd
                            {
                                tileset->drawTile(tempPainter, tileGrid, tileId, x, y, 1, 0);
                            }
                        }
                        else // row even
                        {
                            if (y % 2 == 0) // column even
                            {
                                tileset->drawTile(tempPainter, tileGrid, tileId, x, y, 1, 0);
                            }
                        }
                    }
                    else if (spr->getNybble(7) == 2)
                    {
                        if (x % 2 != 0) // row odd
                        {
                            if (y % 2 == 0 ) // column even
                            {
                                tileset->drawTile(tempPainter, tileGrid, tileId, x, y, 1, 0);
                            }
                        }
                        else // row even
                        {
                            if (y % 2 != 0) // column odd
                            {
                                tileset->drawTile(tempPainter, tileGrid, tileId, x, y, 1, 0);
                            }
                        }
                    }
                    else
                    {
                        tileset->drawTile(tempPainter, tileGrid, tileId, x, y, 1, 0);
                    }
                }


            painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), pix);
        }
    }

    QPainterPath path;
    path.addRoundedRect(sprrect, 2.0, 2.0);

    painter->setPen(QPen(Qt::black));
    painter->drawPath(path);

    painter->drawRect(sprrect);

    painter->setPen(Qt::NoPen);
}

// Sprite 20: Gold Block
GoldBlockRenderer::GoldBlockRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void GoldBlockRenderer::render(QPainter *painter, QRect *)
{
    if (tileset == nullptr)
    {
        painter->drawPixmap(spr->getx(), spr->gety(), 20, 20, ImageCache::getInstance()->get(TileOverride, "error.png"));
        return;
    }

    QPixmap tile(20, 20);
    tile.fill(QColor(0,0,0,0));
    QPainter tempPainter(&tile);

    TileGrid tileGrid;
    tileGrid[0xFFFFFFFF] = 1;

        tileset->drawTile(tempPainter, tileGrid, 34, 0, 0, 1, 0);

    painter->drawPixmap(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight(), tile);
}

// Sprite 21: Note Block
NoteBlockRenderer::NoteBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void NoteBlockRenderer::render(QPainter *painter, QRect *)
{
    QString overlay;

    switch (spr->getNybble(19))
    {
        case 0: break;
        case 1: overlay = "coin.png"; break;
        case 2: overlay = "fire_flower.png"; break;
        case 4: overlay = "super_leaf.png"; break;
        case 5: overlay = "gold_flower.png"; break;
        case 6: overlay = "mini_mushroom.png"; break;
        case 7: overlay = "super_star.png"; break;
        case 8: overlay = "coin_star.png"; break;
        case 9: overlay = "mega_mushroom.png"; break;
        case 10: overlay = "10_coins.png"; break;
        case 11: overlay = "1up_mushroom.png"; break;
        case 13: overlay = "trampoline.png"; break;
        case 14: overlay = "coin_super_mushroom.png"; break;
        default: return;
    }

    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    painter->drawPixmap(x, y, 20, 20, ImageCache::getInstance()->get(SpriteImg, "note_block.png"));
    painter->drawPixmap(x+2, y+1, 18, 18, ImageCache::getInstance()->get(TileOverlay, overlay));
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

    painter->setPen(Qt::NoPen);
}

// Sprite 24: Flying Question Block
FlyingQuestionBlockRenderer::FlyingQuestionBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void FlyingQuestionBlockRenderer::render(QPainter *painter, QRect *)
{
    QString type;
    QString overlay;

    if (spr->getid() != 25)
    {
        switch (spr->getNybble(11))
        {
            case 1: type = "_underground.png"; break;
            case 2: type = "_snow.png"; break;
            case 3: type = "_lava.png"; break;
            default: type = ".png"; break;
        }

        switch (spr->getNybble(19))
        {
            case 0: break;
            case 1: overlay = "coin.png"; break;
            case 2: overlay = "fire_flower.png"; break;
            case 4: overlay = "super_leaf.png"; break;
            case 5: overlay = "gold_flower.png"; break;
            case 6: overlay = "mini_mushroom.png"; break;
            case 7: overlay = "super_star.png"; break;
            case 8: overlay = "coin_star.png"; break;
            case 9: overlay = "mega_mushroom.png"; break;
            case 10: overlay = "10_coins.png"; break;
            case 11: overlay = "1up_mushroom.png"; break;
            case 13: overlay = "trampoline.png"; break;
            case 14: overlay = "coin_super_mushroom.png"; break;
            default: return;
        }
    }
    else
    {
        switch (spr->getNybble(9))
        {
            case 1: type = "_underground.png"; break;
            case 2: type = "_snow.png"; break;
            case 3: type = "_lava.png"; break;
            default: type = ".png"; break;
        }

        overlay = "1up_mushroom.png";
    }

    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    painter->drawPixmap(x, y, 28, 22, ImageCache::getInstance()->get(SpriteImg, "flying_question_block" + type));
    painter->drawPixmap(x+2, y+1, 18, 18, ImageCache::getInstance()->get(TileOverlay, overlay));
}

// Sprite 26: Roulette Block
RouletteBlockRenderer::RouletteBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void RouletteBlockRenderer::render(QPainter *painter, QRect *)
{
    switch (spr->getNybble(19))
    {
        case 1: contents = "gold_leaf.png"; break;
        case 2: contents = "leaf_star.png"; break;
        case 3: contents = "star.png"; break;
        case 4: contents = "flower_gold.png"; break;
        default: contents = "flower_star.png"; break;
    }

    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    painter->drawPixmap(x, y, 30, 30, ImageCache::getInstance()->get(SpriteImg, "roulette_block/roulette_block.png"));
    painter->drawPixmap(x+5, y+5, 20, 20, ImageCache::getInstance()->get(SpriteImg, "roulette_block/" + contents));
}

// Sprite 28/60/101/204: Movement Controlled Sprites
MovementSpriteRenderer::MovementSpriteRenderer(const Sprite *spr, QString fileName)
{
    this->spr = spr;
    img = new NormalImageRenderer(spr, fileName);
}

void MovementSpriteRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, ImageCache::getInstance()->get(TileOverlay, "movement.png"));
}

// Sprite 36: Arc Lava Bubble

ArcLavaBubbleRenderer::ArcLavaBubbleRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ArcLavaBubbleRenderer::render(QPainter *painter, QRect *)
{
    QPixmap arcPixmap(300,300);
    arcPixmap.fill(Qt::transparent);

    QPainter arcPainter(&arcPixmap);
    arcPainter.setRenderHint(QPainter::Antialiasing);

    QPen arcPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap);
    arcPainter.setPen(arcPen);
    arcPainter.setOpacity(0.6);

    QPainterPath arcPath;
    arcPath.moveTo(290, 100);
    arcPath.quadTo(180, -140, 20, 220);

    arcPainter.drawPath(arcPath);

    if (spr->getNybble(7) == 1)
    {
        painter->drawPixmap(spr->getx(), spr->gety()-80, 300, 300, arcPixmap.transformed(QTransform().scale(-1,1)));
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), (ImageCache::getInstance()->get(SpriteImg, "arc_lavabubble.png")).transformed(QTransform().scale(-1,1)));
    }
    else
    {
        painter->drawPixmap(spr->getx()-280, spr->gety()-80, 300, 300, arcPixmap);
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "arc_lavabubble.png"));
    }

    painter->setPen(Qt::NoPen);
}

// Sprite 38: Reznor Wheel
ReznorWheelRenderer::ReznorWheelRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ReznorWheelRenderer::render(QPainter *painter, QRect *)
{
    int x = spr->getx()+spr->getOffsetX();
    int y = spr->gety()+spr->getOffsetY();

    if(spr->getNybble(8) == 1)
        painter->drawPixmap(x, y, 349, 353, ImageCache::getInstance()->get(SpriteImg, "reznor_wheel_big.png"));
    else
    {   //Wheel
        painter->drawPixmap(x, y, 201, 177, ImageCache::getInstance()->get(SpriteImg, "reznor_wheel.png"));
        //Top Right
        if((spr->getNybble(11) == 1)||(spr->getNybble(11) == 3)||(spr->getNybble(11) == 5)||(spr->getNybble(11) == 7)||(spr->getNybble(11) == 9)||(spr->getNybble(11) == 11)||(spr->getNybble(11) == 13)||(spr->getNybble(11) == 15))
            painter->drawPixmap(x+145, y-10, 48, 55, ImageCache::getInstance()->get(SpriteImg, "reznor.png"));
        //Top Left
        if((spr->getNybble(11) == 2)||(spr->getNybble(11) == 3)||(spr->getNybble(11) == 6)||(spr->getNybble(11) == 7)||(spr->getNybble(11) == 10)||(spr->getNybble(11) == 11)||(spr->getNybble(11) == 14)||(spr->getNybble(11) == 15))
            painter->drawPixmap(x+35, y-40, 48, 55, ImageCache::getInstance()->get(SpriteImg, "reznor.png"));
        //Bottom Left
        if((spr->getNybble(11) == 4)||(spr->getNybble(11) == 5)||(spr->getNybble(11) == 6)||(spr->getNybble(11) == 7)||(spr->getNybble(11) == 12)||(spr->getNybble(11) == 13)||(spr->getNybble(11) == 14)||(spr->getNybble(11) == 15))
            painter->drawPixmap(x+5, y+71, 48, 55, ImageCache::getInstance()->get(SpriteImg, "reznor.png"));
        //Bottom Right
        if((spr->getNybble(11) == 8)||(spr->getNybble(11) == 9)||(spr->getNybble(11) == 10)||(spr->getNybble(11) == 11)||(spr->getNybble(11) == 12)||(spr->getNybble(11) == 13)||(spr->getNybble(11) == 14)||(spr->getNybble(11) == 15))
            painter->drawPixmap(x+115, y+100, 48, 55, ImageCache::getInstance()->get(SpriteImg, "reznor.png"));
    }
}

// Sprite 39: Reznor Battle Disapearing Blocks
ReznorBlockRenderer::ReznorBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ReznorBlockRenderer::render(QPainter *painter, QRect *)
{
    if(spr->getNybble(10) == 0)
        painter->drawPixmap(spr->getx(), spr->gety(), 20, 20, ImageCache::getInstance()->get(SpriteImg, + "reznor_platform.png"));
    for (int i = 0; i < spr->getNybble(10); i++)
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+i*20, spr->gety(), 20, 20, ImageCache::getInstance()->get(SpriteImg, + "reznor_platform.png"));
}

// Sprite 44: Track-controlled Burner - Four Directions
FourBurnerRenderer::FourBurnerRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString name = "four_track_controlled_burner/";
    int size;

    if(spr->getNybble(10) % 2 == 0)
        size = 60;
    else
    {
        name.append("big_");
        size = 120;
    }

    up = new NormalImageRenderer(QRect(spr->getx(),spr->gety()-(size), 22, size), name + "up.png");
    down = new NormalImageRenderer(QRect(spr->getx(),spr->gety()+21, 22, size), name + "down.png");
    left = new NormalImageRenderer(QRect(spr->getx()-(size),spr->gety(), size, 22), name + "left.png");
    right = new NormalImageRenderer(QRect(spr->getx()+21,spr->gety(), size, 22), name + "right.png");
    center = new NormalImageRenderer(QRect(spr->getx(),spr->gety(), 21, 21), "four_track_controlled_burner/center.png");

}
void FourBurnerRenderer::render(QPainter *painter, QRect *drawrect)
{
    center->render(painter, drawrect);
    if((spr->getNybble(6) == 4)||(spr->getNybble(6) == 5)||(spr->getNybble(6) == 6)||(spr->getNybble(6) == 7)||(spr->getNybble(6) == 12)||(spr->getNybble(6) == 13)||(spr->getNybble(6) == 14)||(spr->getNybble(6) == 15))
        up->render(painter,drawrect);
    if((spr->getNybble(6) == 8)||(spr->getNybble(6) == 9)||(spr->getNybble(6) == 10)||(spr->getNybble(6) == 11)||(spr->getNybble(6) == 12)||(spr->getNybble(6) == 13)||(spr->getNybble(6) == 14)||(spr->getNybble(6) == 15))
        down->render(painter,drawrect);
    if((spr->getNybble(6) == 2)||(spr->getNybble(6) == 3)||(spr->getNybble(6) == 6)||(spr->getNybble(6) == 7)||(spr->getNybble(6) == 10)||(spr->getNybble(6) == 11)||(spr->getNybble(6) == 14)||(spr->getNybble(6) == 15))
        left->render(painter,drawrect);
    if((spr->getNybble(6) == 1)||(spr->getNybble(6) == 3)||(spr->getNybble(6) == 5)||(spr->getNybble(6) == 7)||(spr->getNybble(6) == 9)||(spr->getNybble(6) == 11)||(spr->getNybble(6) == 13)||(spr->getNybble(6) == 15))
        right->render(painter,drawrect);
}

// Sprite 51: Fuzzy
FuzzyRenderer::FuzzyRenderer(const Sprite *spr)
{
    if ((spr->getNybble(9) % 2)== 1) img = new NormalImageRenderer(spr, "fuzzy_big.png");
    else img = new NormalImageRenderer(spr, "fuzzy.png");
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

    if((spr->getwidth() == 20) && (spr->getheight() == 20))
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, ImageCache::getInstance()->get(TileOverlay, "rotation.png"));
    else
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+10, spr->gety()+spr->getOffsetY()+10, 20, 20, ImageCache::getInstance()->get(TileOverlay, "rotation.png"));
}

// Sprite 61: RedCoin
RedCoinRenderer::RedCoinRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;
    if (spr->getNybble(7) >= 1) filename.insert(filename.size()-4, "_falling");

    img = new NormalImageRenderer(spr, filename);
}

void RedCoinRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 63/64: Skewer
SkewerRenderer::SkewerRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void SkewerRenderer::render(QPainter *painter, QRect *)
{
    int initialXOffset = 0;
    int initialYOffset = 0;
    int endXOffset = 0;
    int endYOffset = 0;
    int imgXOffset = 0;
    int imgYOffset = 0;
    QString img;
    if (spr->getid() == 63 || spr->getid() == 64)
    {
        img = "skewer_left.png";
        initialYOffset = spr->getheight()/2;
        endYOffset = spr->getheight()/2;
        switch (spr->getNybble(6))
        {
            case 1:
                endXOffset = -140;
                break;
            case 2:
                endXOffset = -280;
                break;
            case 3:
                endXOffset = -200;
                break;
            default:
                endXOffset = -320;
                break;
        }
    }
    if (spr->getid() == 64)
    {
        img = "skewer_right.png";
        endXOffset = abs(endXOffset) + 20;
        imgXOffset = -spr->getwidth() + 20;
    }
    MovIndicatorRenderer track(spr->getx()+initialXOffset, spr->gety()+initialYOffset, spr->getx()+endXOffset, spr->gety()+endYOffset, false, QColor(244, 250, 255));
    QPixmap image(ImageCache::getInstance()->get(SpriteImg, img));
    track.render(painter);
    painter->drawPixmap(QRect(spr->getx() + imgXOffset, spr->gety() + imgYOffset, spr->getwidth(), spr->getheight()), image);
}

// Sprite 78: Firebar
FireBarRenderer::FireBarRenderer(const Sprite *spr)
{
    this->spr = spr;

    int rad = (spr->getNybble(11)*15);
    int diameter = rad*2;

    int posoff = rad-10;

    radius = new CircleRenderer(spr->getx()-posoff, spr->gety()-posoff, diameter, diameter, "", QColor(0,0,0));
}

void FireBarRenderer::render(QPainter *painter, QRect *drawrect)
{
    // Draw Center tile first
    if (spr->getNybble(6)%2)
        painter->drawPixmap(QRect(spr->getx()-10, spr->gety(), 40, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_center_wide.png"));
    else
        painter->drawPixmap(QRect(spr->getx(), spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "firebar_center.png"));

    // Draw Firebar radius
    if(spr->getNybble(11) > 0)
        radius->render(painter, drawrect);

    int gap = 0;
    qreal angle = 0;
    int x = 0;
    int y = 0;

    int rad = spr->getNybble(11);
    int barCount = spr->getNybble(10)+1;

    if (barCount > 4)
        barCount = 4;

    int rads = 0;

    // Calc Positions on a circle
    for (int i = 0; i < barCount; i++)
    {
        gap = float(0.75 - (1 / barCount));
        angle = -360 * i / (barCount + gap);
        angle = qDegreesToRadians(angle+90);

        // Draw Firebar flame
        while (rads <= rad)
        {
            // prevent middle flame from being drawn for every bar
            if (rads == 0)
            {
                rads++;
                continue;
            }

            x = float(qSin(angle) * ((rads * 15)));
            y = float(-(qCos(angle) * ((rads * 15))));

            painter->drawPixmap(spr->getx()+x, spr->gety()+y, 20, 20, ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));

            rads++;
        }
        rads = 0;
    }

    painter->drawPixmap(spr->getx(), spr->gety(), 20, 20, ImageCache::getInstance()->get(SpriteImg, "firebar_fire.png"));
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

    painter->setPen(QColor(0,0,0));

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

    painter->setPen(Qt::NoPen);
}

// Sprite 93: Scuttlebug
ScuttlebugRenderer::ScuttlebugRenderer(const Sprite *spr, const Zone *zone)
{
    this->spr = spr;
    this->zone = zone;
}

void ScuttlebugRenderer::render(QPainter *painter, QRect *)
{
    QPen stringpen;
    stringpen.setColor(QColor(255,255,255,125));
    stringpen.setWidth(2);
    painter->setPen(stringpen);
    painter->drawLine(spr->getx()+10, spr->gety(), spr->getx()+10, zone->gety());
    painter->setPen(Qt::NoPen);
}

// Sprite 94: Flipper (One Way Gate)
FlipperRenderer::FlipperRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void FlipperRenderer::render(QPainter *painter, QRect *)
{
    QString type = "";
    if (spr->getNybble(10) != 0)
        type = "small_";

    QPixmap img = ImageCache::getInstance()->get(SpriteImg, type + "flipper.png");

    switch (spr->getNybble(11))
    {
    case 1: // Left - Swing down
        img  = img.transformed(QTransform().scale(-1,1));
        break;
    case 2: // Right - Swing up
        img  = img.transformed(QTransform().scale(1,-1));
        break;
    case 3: // Left - Swing up
        img  = img.transformed(QTransform().scale(-1,-1));
        break;
    case 4: // Up - Swing left
        img  = img.transformed(QTransform().rotate(90).scale(-1,1));
        break;
    case 5: // Down - Swing left
        img  = img.transformed(QTransform().rotate(90));
        break;
    case 6: // Up - Swing right
        img  = img.transformed(QTransform().rotate(90).scale(-1,-1));
        break;
    case 7: // Down - Swing right
        img  = img.transformed(QTransform().rotate(90).scale(1,-1));
        break;
    default:
        break;
    }

    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), img);
}

// Sprite 95/96: Blooper/Blooper Nanny
BlooperRenderer::BlooperRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(4) == 0 || spr->getNybble(4) > 12)
    {
        if (spr->getid() == 96)
            img = new NormalImageRenderer(spr, "blooper_nanny.png");
        else
            img = new NormalImageRenderer(spr, "blooper.png");
    }
    else
        img = new NormalImageRenderer(spr, QString("blooper_pipe_%1.png").arg(spr->getNybble(4)));
}

void BlooperRenderer::render(QPainter *painter, QRect *drawrect)
{
       img->render(painter, drawrect);
}

// Sprite 97: End of Level Flag
GoalRenderer::GoalRenderer(const Sprite *spr)
{
    if (spr->getNybble(4) == 1)
    {
        pole = new NormalImageRenderer(QRect(spr->getx()-22, spr->gety(), 62, 200), "flagpole_secret.png");
        fort = new NormalImageRenderer(QRect(spr->getx()+200, spr->gety()+80, 120, 120), "castle_secret.png");
    }
    else
    {
        pole = new NormalImageRenderer(QRect(spr->getx()-22, spr->gety(), 62, 200), "flagpole.png");
        fort = new NormalImageRenderer(QRect(spr->getx()+200, spr->gety()+80, 120, 120), "castle.png");
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
    this->rect = new RoundedRectRenderer(spr, QString("%1").arg(spr->getid()), QColor(0,90,150,150));
}

void PathContFenceRenderer::render(QPainter *painter, QRect *drawrect)
{
    if (spr->getNybbleData(16, 19) == 0)
        rect->render(painter, drawrect);

    for (int i = 0; i < 4; i++)
    {
        // Row 1
        if ((spr->getNybble(16) & (1 << i)) != 0)
            painter->drawPixmap(spr->getx()-((i-1)*60), spr->gety()+spr->getOffsetY(), 60, 60, ImageCache::getInstance()->get(SpriteImg, "multi_peice_fence.png"));

        // Row 2
        if ((spr->getNybble(17) & (1 << i)) != 0)
            painter->drawPixmap(spr->getx()-((i-1)*60), spr->gety()+spr->getOffsetY()+60, 60, 60, ImageCache::getInstance()->get(SpriteImg, "multi_peice_fence.png"));

        // Row 3
        if ((spr->getNybble(18) & (1 << i)) != 0)
            painter->drawPixmap(spr->getx()-((i-1)*60), spr->gety()+spr->getOffsetY()+120, 60, 60, ImageCache::getInstance()->get(SpriteImg, "multi_peice_fence.png"));

        // Row 4
        if ((spr->getNybble(19) & (1 << i)) != 0)
            painter->drawPixmap(spr->getx()-((i-1)*60), spr->gety()+spr->getOffsetY()+180, 60, 60, ImageCache::getInstance()->get(SpriteImg, "multi_peice_fence.png"));
    }
}

// Sprite 109: Signboard
SignboardRenderer::SignboardRenderer(const Sprite *spr)
{
    QString signboardPath("sign_%1.png");

    if (spr->getNybble(11) < 10) img = new NormalImageRenderer(spr, signboardPath.arg(spr->getNybble(11)));
    else img = new NormalImageRenderer(spr, signboardPath.arg(7));
}

void SignboardRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 114: Floating Box
FloatingBoxRenderer::FloatingBoxRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "floating_box_big.png");
    else img = new NormalImageRenderer(spr, "floating_box_small.png");
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
    if (spr->getNybble(4) == 1)
    {
        painter->drawPixmap(spr->getx(), spr->gety()+spr->getheight()-40, 20, 40, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_launcher.png").transformed(QTransform().scale(1,-1)));
        for (int i = 0; i < spr->getNybble(10); i++) painter->drawPixmap(spr->getx(), spr->gety()+i*20, 20, 20, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_launcher_middle.png"));
    }
    else
    {
        painter->drawPixmap(spr->getx(), spr->gety()+spr->getOffsetY(), 20, 40, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_launcher.png"));
        for (int i = 0; i < spr->getNybble(10); i++) painter->drawPixmap(spr->getx(), spr->gety()+spr->getOffsetY()+40+i*20, 20, 20, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_launcher_middle.png"));
    }
}

// Sprite 116: Bullet Bill Turret
BulletBillTurretRenderer::BulletBillTurretRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BulletBillTurretRenderer::render(QPainter *painter, QRect *)
{
    bool faceRight = false;
    for (int i = 0; i < spr->getNybble(7)+1; i++)
    {
        if (i > 7) break;

        if ((spr->getNybbleData(10, 11) & (1 << i)) == 0)
            faceRight = true;
        else
            faceRight = false;

        if ((spr->getNybbleData(8, 9) & (1 << i)) == 0)
        {
            if (faceRight)
                painter->drawPixmap(spr->getx(), spr->gety()-(i*20), 26, 20, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_turret_head.png").transformed(QTransform().scale(-1,1)));
            else
                painter->drawPixmap(spr->getx()-6, spr->gety()-(i*20), 26, 20, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_turret_head.png"));
        }
        else
            painter->drawPixmap(spr->getx(), spr->gety()-(i*20), 20, 20, ImageCache::getInstance()->get(SpriteImg, "bullet_bill_turret_base.png"));
    }
}

// Sprite 118: Bullet Bill
BulletBillRenderer::BulletBillRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "bullet_bill_left.png");
    else if (spr->getNybble(11) == 2) img = new NormalImageRenderer(spr, "bullet_bill_down.png");
    else if (spr->getNybble(11) == 3) img = new NormalImageRenderer(spr, "bullet_bill_up.png");
    else if (spr->getNybble(11) == 4) img = new NormalImageRenderer(spr, "bullet_bill_down_left.png");
    else if (spr->getNybble(11) == 5) img = new NormalImageRenderer(spr, "bullet_bill_down_right.png");
    else if (spr->getNybble(11) == 6) img = new NormalImageRenderer(spr, "bullet_bill_up_right.png");
    else if (spr->getNybble(11) == 7) img = new NormalImageRenderer(spr, "bullet_bill_up_left.png");
    else img = new NormalImageRenderer(spr, "bullet_bill_right.png");
}

void BulletBillRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 119: Banzai Bill
BanzaiBillRenderer::BanzaiBillRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "banzai_bill_left.png");
    else img = new NormalImageRenderer(spr, "banzai_bill_right.png");
}

void BanzaiBillRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 120: Up-Down Mushroom
UpDownMushroomRenderer::UpDownMushroomRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void UpDownMushroomRenderer::render(QPainter *painter, QRect *)
{
    QString color;
    int offset;
    if((spr->getNybble(9)) < (spr->getNybble(10)))
    {
        color = "blue";
        offset = (spr->getNybble(10)-spr->getNybble(9))*20;
    }
    else
    {
        color = "pink";
        offset = (spr->getNybble(9)-spr->getNybble(10))*20;

    }

    // Final Position
    if (spr->getNybble(9) < spr->getNybble(10))
    {
        painter->setOpacity(0.3);
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+offset, 30, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_l.png"));
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-10, spr->gety()+offset, 30, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_r.png"));
        for (int i = 30; i < spr->getwidth()-30; i += 20)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety()+offset, 20, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_m.png"));
        painter->setOpacity(1);
    }
    else
    {
        painter->setOpacity(0.3);
        for (int i = 0; i < spr->getNybble(9); i++)
            painter->drawPixmap(QRect(spr->getx(), spr->gety()-offset-30+((i+2)*20), 20, 20), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/stem.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()-offset, 30, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_l.png"));
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-10, spr->gety()-offset, 30, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_r.png"));
        for (int i = 30; i < spr->getwidth()-30; i += 20)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety()-offset, 20, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_m.png"));
        painter->setOpacity(1);
    }
    // Inital Position
    for (int i = 0; i < spr->getNybble(10); i++)
        painter->drawPixmap(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+30+i*20, 20, 20), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/stem.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 30, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_l.png"));
    painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-10, spr->gety(), 30, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_r.png"));
    for (int i = 30; i < spr->getwidth()-30; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety(), 20, 30), ImageCache::getInstance()->get(SpriteImg, "up_down_mushroom/" + color + "_m.png"));
}

// Sprite 121/122: Expanding Mushroom Platforms
ExpandMushroomRenderer::ExpandMushroomRenderer(const Sprite *spr)
{
    this-> spr = spr;
}

void ExpandMushroomRenderer::render(QPainter *painter, QRect *)
{
    // Stem
    QString color;
    if (spr->getNybble(8) %2 == 0)
        color = "";
    else
        color = "grey_";

    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+spr->getwidth()/2-10, spr->gety()+20, 20, 40), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/" + color + "stem_top.png"));
    for (int i = 0; i < spr->getNybble(11); i++)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+spr->getwidth()/2-10, spr->gety()+60+i*20, 20, 20), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/" + color + "stem.png"));

    // Platform
    if (spr->getNybble(10) %2 == 0)
    {
        // Start Contracted
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()-20, spr->gety(), 40, 20), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/contracted.png"));

        //Transparent Guides
        if (spr->getNybble(9) %2 == 0)
        {
            // 10 Block Expantion
            painter->setOpacity(0.3);
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-80, spr->gety(), 200, 20), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/big.png"));
            painter->setOpacity(1);
        }
        else
        {
            // 5 Block Expansion
            painter->setOpacity(0.3);
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-40, spr->gety(), 120, 20), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/small.png"));
            painter->setOpacity(1);
        }
    }
    else
    {
        // Start Expanded
        if (spr->getNybble(9) %2 == 0)
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 200, 20), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/big.png"));
        else
            painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 120, 20), ImageCache::getInstance()->get(SpriteImg, "expand_mushroom/small.png"));
    }
}

// Sprite 123: Bouncy Mushroom
BouncyMushroomRenderer::BouncyMushroomRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(17) == 1)
        img = new NormalImageRenderer(spr, "bouncy_mushroom_big.png");
    else
        img = new NormalImageRenderer(spr, "bouncy_mushroom.png");
}

void BouncyMushroomRenderer::render(QPainter* painter, QRect* drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 124/321: Mushroom Platform Renderer
MushroomPlatformRenderer::MushroomPlatformRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void MushroomPlatformRenderer::render(QPainter *painter, QRect *)
{
    QString basepath = "mushroom_platform";

    if (spr->getid() == 321) basepath = "snow_mushroom_platform";

    QPixmap platform(spr->getwidth(), 20);
    platform.fill(Qt::transparent);
    QPainter platformPainter(&platform);

    int extendLength = 0;
    if (spr->getNybble(6)%3 == 0)
    {
        extendLength = spr->getBits(24,26) * 20;
    }

    // Draw the platform into a pixmap for use later
    platformPainter.drawPixmap(0, 0, 24, 20, ImageCache::getInstance()->get(SpriteImg, basepath + "/l.png"));
    for (int i = 20; i < spr->getwidth()-44; i += 20)
        platformPainter.drawPixmap(4+i, 0, 20, 20, ImageCache::getInstance()->get(SpriteImg, basepath + "/m.png"));
    platformPainter.drawPixmap(spr->getwidth()-24, 0, 24, 20, ImageCache::getInstance()->get(SpriteImg, basepath + "/r.png"));

//    // No guides if angle is 0
//    if (spr->getBits(32,34) != 0)
//    {
//        // Draw guides
//        painter->setOpacity(0.3);
//        QPixmap guideL = platform.transformed(QTransform().rotate(-11.25*(spr->getBits(32,34))));
//        QPixmap guideR = platform.transformed(QTransform().rotate(11.25*(spr->getBits(32,34))));

//        // Extended Guide
//        if (spr->getNybble(6)%3 == 0)
//        {
//            int offsetx = 9 * spr->getBits(24,26);
//            int offsety = 3 * spr->getBits(24,26);

//            switch (spr->getBits(32,34))
//            {
//            case 2: // 22.5
//                offsety += 4;
//                offsetx += 15;
//                break;
//            case 3: // 33.75
//                offsety += 8;
//                offsetx += 30;
//                break;
//            case 4: // 45
//                offsety += 18;
//                offsetx += 50;
//                break;
//            case 5: // 56.25
//                offsety += 24;
//                offsetx += 60;
//                break;
//            case 6: // 67.5
//                offsety += 34;
//                offsetx += 75;
//                break;
//            case 7: // 78.75
//                offsety += 45;
//                offsetx += 84;
//                break;
//            }

//            painter->drawPixmap(spr->getx()-(guideR.width()/2)-18 + offsetx, spr->gety()-(guideR.height()/2)-4 + offsety, guideR.width(), guideR.height(), guideR);
//            painter->drawPixmap(spr->getx()-(guideL.width()/2)+36 - offsetx, spr->gety()-(guideL.height()/2)-4 + offsety, guideL.width(), guideL.height(), guideL);
//        }
//        painter->setOpacity(1.0);
//    }

    // Draw the stem top
    for (int i = 0; i < extendLength; i+=20)
        painter->drawPixmap(QRect(spr->getx(), spr->gety() + 20 + i, 20, 20), ImageCache::getInstance()->get(SpriteImg, basepath + "/stem_top.png"));

    // Draw the stem center
    painter->drawPixmap(QRect(spr->getx(), spr->gety()+extendLength+20, 20, 80), ImageCache::getInstance()->get(SpriteImg, basepath + "/stem.png"));

    // Draw the stem bottom
    for (int i = 0; i < spr->getNybble(4)*20; i+=20)
        painter->drawPixmap(QRect(spr->getx(), spr->gety() + 100 + extendLength + i, 20, 20), ImageCache::getInstance()->get(SpriteImg, basepath + "/stem_bottom.png"));

    // Draw the platform
    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety(), platform.width(), platform.height(), platform);



//    // Stretch - 7 tile
//    painter->drawPixmap(spr->getx()-(guideR.width()/2)+73, spr->gety()-(guideR.height()/2)-39, guideR.width(), guideR.height(), guideR);

    // todo: draw stem indicator
}


// Sprite 127: Bowser Flame
BowserFlameRenderer::BowserFlameRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "dry_bowser_flame.png");
    else img = new NormalImageRenderer(spr, "bowser_flame.png");
}

void BowserFlameRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 135: Goomba
GoombaRenderer::GoombaRenderer(const Sprite *spr)
{
    this->spr = spr;
    if (spr->getNybble(16) % 2 == 0) img = new NormalImageRenderer(spr, "goomba.png");
    else img = new NormalImageRenderer(spr, "goomba_blue.png");
}

void GoombaRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 136: Bone Goomba
BoneGoombaRenderer::BoneGoombaRenderer(const Sprite *spr)
{
    this->spr = spr;
    if (spr->getNybble(16) % 2 == 0) img = new NormalImageRenderer(spr, "bone_goomba.png");
    else img = new NormalImageRenderer(spr, "bone_goomba_alt.png");
}

void BoneGoombaRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}


// Sprite 139: Goomba Tower
GoombaTowerRenderer::GoombaTowerRenderer(const Sprite *spr)
{
    top = new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY(), spr->getwidth(), 25), "goomba_tower_top.png");
    bottom = new NormalImageRenderer(QRect(spr->getx(), spr->gety(), spr->getwidth(), 21), "goomba_tower_bottom.png");

    if (spr->getNybble(11) < 3) return;
    if (spr->getNybble(11) == 0)
        middle.append(new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+46, spr->getwidth(), 21), "goomba_tower_middle.png"));
    else
    for (int i = 0; i < spr->getNybble(11)-2; i++) middle.append(new NormalImageRenderer(QRect(spr->getx(), spr->gety()+spr->getOffsetY()+25+i*21, spr->getwidth(), 21), "goomba_tower_middle.png"));
}

void GoombaTowerRenderer::render(QPainter *painter, QRect *drawrect)
{
    top->render(painter, drawrect);
    for(int i = 0; i < middle.size(); i++) middle[i]->render(painter, drawrect);
    bottom->render(painter, drawrect);
}


// Sprite 144/145: Horizontal/Vertical Moving Lift
LiftRenderer::LiftRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void LiftRenderer::render(QPainter *painter, QRect *)
{
    int distance = spr->getNybble(9)*20;

        if (spr->getNybble(7) == 1  && distance > 0)
        {
            if (spr->getid() == 145)
            {
                MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+13, spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+distance, true, QColor(244,250,255));
                track.render(painter);
            }
            else
            {
                MovIndicatorRenderer track(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-distance, spr->gety()+spr->getheight()/2, false, QColor(244,250,255));
                track.render(painter);
            }
        }
        else if (distance > 0)
        {
            if (spr->getid() == 145)
            {
                MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()-27, spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()-20-distance, true, QColor(244,250,255));
                track.render(painter);
            }
            else
            {
                MovIndicatorRenderer track(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+distance, spr->gety()+spr->getheight()/2, false, QColor(244,250,255));
                track.render(painter);
            }
        }


    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 20, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/l.png"));
    if(spr->getNybble(11) == 0)
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()+(spr->getNybble(11))*20+20, spr->gety(), 22, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/r.png"));
    else
        painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()+(spr->getNybble(11)-1)*20+20, spr->gety(), 22, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/r.png"));
    for (int i = 20; i < spr->getwidth()-20; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety(), 20, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/m.png"));
}


// Sprite 146: Track Controlled Lift
TrackLiftRenderer::TrackLiftRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void TrackLiftRenderer::render(QPainter *painter, QRect *)
{
    QString name = "track_controlled_lift/";
    if((spr->getNybble(8) != 1))
        name.append("wood");
    else
        name.append("stone");

    if((spr->getNybble(9) == 1))
        name.append("_rounded");

    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 22, 22), ImageCache::getInstance()->get(SpriteImg, name + "_l.png"));
    painter->drawPixmap(QRect(spr->getx()-spr->getOffsetX()+58, spr->gety()+spr->getOffsetY(), 22, 22), ImageCache::getInstance()->get(SpriteImg, name + "_r.png"));
    for (int i = 22; i < spr->getwidth()-22; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i, spr->gety()+spr->getOffsetY(), 20, 22), ImageCache::getInstance()->get(SpriteImg, name + "_m.png"));
}

// Sprite 147: 3 Plat Rickshaw
ThreePlatRickRenderer::ThreePlatRickRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "3Plat_Rickshaw_Big.png");
    else img = new NormalImageRenderer(spr, "3Plat_Rickshaw.png");
}

void ThreePlatRickRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 151: Scale Platform Renderer
ScalePlatformRenderer::ScalePlatformRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ScalePlatformRenderer::render(QPainter *painter, QRect *)
{
    //used to keep platforms centered
    int poffset;
    if (spr->getNybble(17) == 0)
        poffset = 32;
    else
        poffset = 32 + ((spr->getNybble(17))*10);

    //Rope
    painter->drawPixmap(QRect(spr->getx()-4, spr->gety()-13, 17, 16), ImageCache::getInstance()->get(SpriteImg, "scale_lift/left.png"));
    for (int i = 0; i < spr->getNybble(11); i++)
        painter->drawPixmap(QRect(spr->getx()+13, spr->gety()-12, 20*i, 4), ImageCache::getInstance()->get(SpriteImg, "scale_lift/top.png"));
    if (spr->getNybble(11) == 0)
        painter->drawPixmap(QRect(spr->getx()+6, spr->gety()-13, 17, 16), ImageCache::getInstance()->get(SpriteImg, "scale_lift/right.png"));
    else
        painter->drawPixmap(QRect(spr->getx()-4+(spr->getNybble(11)*20-10), spr->gety()-13, 17, 16), ImageCache::getInstance()->get(SpriteImg, "scale_lift/right.png"));
    for (int i = 0; i < spr->getNybble(9); i++)
        painter->drawPixmap(QRect(spr->getx()-2, spr->gety()+3, 4, (20*i)+7), ImageCache::getInstance()->get(SpriteImg, "scale_lift/down.png"));
    for (int i = 0; i < spr->getNybble(10); i++)
        if (spr->getNybble(11) == 0)
            painter->drawPixmap(QRect(spr->getx()+18, spr->gety()+3, 4, (20*i)+7), ImageCache::getInstance()->get(SpriteImg, "scale_lift/down.png"));
        else
            painter->drawPixmap(QRect(spr->getx()-4+(spr->getNybble(11)*20+2), spr->gety()+3, 4, (20*i)+7), ImageCache::getInstance()->get(SpriteImg, "scale_lift/down.png"));
    //Platforms
    if ((spr->getNybble(17) == 0))
    {
        //Left Platform
        painter->drawPixmap(QRect(spr->getx()-poffset, spr->gety()-13+(20*spr->getNybble(9))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_l.png"));
        painter->drawPixmap(QRect(spr->getx()-poffset+22, spr->gety()-13+(20*spr->getNybble(9))+3, 20, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_m.png"));
        painter->drawPixmap(QRect(spr->getx()-poffset+42, spr->gety()-13+(20*spr->getNybble(9))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_r.png"));
        //Right Platform
        painter->drawPixmap(QRect(spr->getx()-poffset+(spr->getNybble(11)*20), spr->gety()-13+(20*spr->getNybble(10))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_l.png"));
        painter->drawPixmap(QRect(spr->getx()-poffset+22+(spr->getNybble(11)*20), spr->gety()-13+(20*spr->getNybble(10))+3, 20, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_m.png"));
        painter->drawPixmap(QRect(spr->getx()-poffset+42+(spr->getNybble(11)*20), spr->gety()-13+(20*spr->getNybble(10))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_r.png"));
    }
    else
    {
        //Left Platform
        painter->drawPixmap(QRect(spr->getx()-poffset, spr->gety()-13+(20*spr->getNybble(9))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_l.png"));
        for (int i = 0; i < spr->getNybble(17)+1; i++)
        {
            painter->drawPixmap(QRect(spr->getx()-poffset+22+(20*i), spr->gety()-13+(20*spr->getNybble(9))+3, 20, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_m.png"));
        }

        painter->drawPixmap(QRect(spr->getx()-poffset+22+(20*(spr->getNybble(17)+1)), spr->gety()-13+(20*spr->getNybble(9))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_r.png"));

        //Right Platform
        painter->drawPixmap(QRect(spr->getx()-poffset+(spr->getNybble(11)*20), spr->gety()-13+(20*spr->getNybble(10))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_l.png"));
        for (int i = 0; i < spr->getNybble(17)+1; i++)
        {
            painter->drawPixmap(QRect(spr->getx()-poffset+22+(20*i)+(spr->getNybble(11)*20), spr->gety()-13+(20*spr->getNybble(10))+3, 20, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_m.png"));
        }

        painter->drawPixmap(QRect(spr->getx()-poffset+22+(20*(spr->getNybble(17)+1))+(spr->getNybble(11)*20), spr->gety()-13+(20*spr->getNybble(10))+3, 22, 22), ImageCache::getInstance()->get(SpriteImg, "scale_lift/platform_r.png"));
    }
}

// Sprite 152: Path Controlled Lift With Peepa
PeepaLiftRenderer::PeepaLiftRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void PeepaLiftRenderer::render(QPainter *painter, QRect *)
{
    //Draw Peepa glow
    if (spr->getNybble(17) == 0)
    {
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-17, spr->gety(), 40, 40), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/glow.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getNybble(11))*20+23, spr->gety(), 40, 40), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/glow.png"));
    }
    else
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getwidth()/2)-17, spr->gety(), 40, 40), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/glow.png"));

    // Draw Platform
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 23, 22), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/l.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getNybble(11))*20+23, spr->gety(), 23, 22), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/r.png"));
    for (int i = 20; i < spr->getwidth()-43; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i+3, spr->gety(), 20, 22), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/m.png"));

    //Draw Peepa
    if (spr->getNybble(17) == 0)
    {
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()-17, spr->gety(), 40, 40), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/peepa.png"));
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getNybble(11))*20+23, spr->gety(), 40, 40), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/peepa.png"));
    }
    else
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getwidth()/2)-17, spr->gety(), 40, 40), ImageCache::getInstance()->get(SpriteImg, "peepa_lift_platform/peepa.png"));

}

// Sprite 154: 4 Plat Rickshaw
FourPlatRickRenderer::FourPlatRickRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "4Plat_Rickshaw.png");
    else img = new NormalImageRenderer(spr, "4Plat_Rickshaw_Big.png");
}

void FourPlatRickRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 158: Buzzy Beetle
BuzzyBeetleRenderer::BuzzyBeetleRenderer(const Sprite *spr)
{
    this->spr = spr;
    QString imgName = "buzzy_beetle_l";

    if (spr->getNybble(9)%4 == 1)
        imgName = "buzzy_beetle_r";

    switch (spr->getNybble(11) %4) {
    case 1:
        imgName.append("_ceiling");
        break;
    case 2:
        imgName.append("_shell");
        break;
    case 3:
        imgName.append("_shell_upsidedown");
        break;
    default:
        break;
    }

    img = new NormalImageRenderer(spr, QString("%1.png").arg(imgName));
}

void BuzzyBeetleRenderer::render(QPainter *painter, QRect *drawrect)
{

    img->render(painter, drawrect);
}

// Sprite 159: Spike Top
SpikeTopRenderer::SpikeTopRenderer(const Sprite *spr)
{
    QString flipped;
    if (spr->getNybble(11) == 1)
        flipped = "flipped_";

    if (spr->getNybble(10) == 1) img = new NormalImageRenderer(spr, flipped + "spiketop_left.png");
    else if (spr->getNybble(10) == 2) img = new NormalImageRenderer(spr, flipped +"spiketop_ceiling.png");
    else if (spr->getNybble(10) == 3) img = new NormalImageRenderer(spr, flipped + "spiketop_right.png");
    else img = new NormalImageRenderer(spr, flipped + "spiketop.png");
}

void SpikeTopRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 163: Climbing Koopa - Horizontal
ClimbKoopaHorRenderer::ClimbKoopaHorRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "climbing_koopa_red.png");
    else img = new NormalImageRenderer(spr, "climbing_koopa.png");
}

void ClimbKoopaHorRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 164: Climbing Koopa - vertical
ClimbKoopaVertRenderer::ClimbKoopaVertRenderer(const Sprite *spr)
{
    QString behind;
    if(spr->getNybble(10) == 1)
         behind = "behind_";

    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, behind + "climbing_koopa_red.png");
    else img = new NormalImageRenderer(spr, behind + "climbing_koopa.png");
}

void ClimbKoopaVertRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 165: Koopa Troopa
KoopaTroopaRenderer::KoopaTroopaRenderer(const Sprite *spr)
{
    QString filename = "koopa_troopa";

    if (spr->getNybble(10)%2)
        filename = "koopa_shell";

    if (spr->getNybble(11) == 1)
        img = new NormalImageRenderer(spr, QString("%1_red.png").arg(filename));
    else
        img = new NormalImageRenderer(spr, QString("%1_green.png").arg(filename));
}

void KoopaTroopaRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 175/176/177/178/179/180: Grounded Piranha plants
PlantRenderer::PlantRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;

    if (spr->getNybble(11) == 1)
        filename.append("_flipped");

    img = new NormalImageRenderer(spr, QString("%1.png").arg(filename));
}

void PlantRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 185: Koopa Paratroopa
KoopaParatroopaRenderer::KoopaParatroopaRenderer(const Sprite *spr)
{
    this->spr = spr;
    QString filename = "koopa_paratroopa";

    if (spr->getNybble(8)%2 && spr->getNybble(10) != 2)
        filename.append("_r");

    if (spr->getNybble(11) == 1)
        img = new NormalImageRenderer(spr, QString("%1_red.png").arg(filename));
    else
        img = new NormalImageRenderer(spr, QString("%1_green.png").arg(filename));
}

void KoopaParatroopaRenderer::render(QPainter *painter, QRect *drawrect)
{
    // Distance indicators
    if (spr->getNybble(10) == 1) // fly horizontally
    {
        int distance = 77;
        int y = spr->gety()+spr->getOffsetY()+spr->getheight()/2;
        int x = spr->getx()+spr->getOffsetX();

        if (spr->getNybble(9) == 1) // start at edge
        {
            distance = 153;

            if (spr->getNybble(8) == 1) // right
            {
                MovIndicatorRenderer indicator(x+spr->getwidth(), y, x+spr->getwidth()+distance, y, false, QColor(244,250,255));
                indicator.render(painter);
            }
            else // left
            {
                MovIndicatorRenderer indicator(x, y, x-distance, y, false, QColor(244,250,255));
                indicator.render(painter);
            }
        }
        else // from center
        {
            int offsetx = 0;
            if (spr->getNybble(8)%2)
                offsetx = -2;

                MovIndicatorRenderer rightIndicator(x+spr->getwidth()+offsetx, y, x+spr->getwidth()+distance+offsetx, y, false, QColor(244,250,255));
                rightIndicator.render(painter);

                MovIndicatorRenderer leftIndicator(x+offsetx, y, x-distance+offsetx, y, false, QColor(244,250,255));
                leftIndicator.render(painter);
        }
    }
    else if (spr->getNybble(10) == 2) // fly vertically
    {
        int distance = 80;
        int y = spr->gety()+spr->getOffsetY();
        int x = spr->getx()+spr->getOffsetX()+spr->getwidth()/2;

        if (spr->getNybble(9) == 1) // start at edge
        {
            distance = 160;

            if (spr->getNybble(8) == 1) // bottom
            {
                MovIndicatorRenderer indicator(x, y, x, y-distance+10, true, QColor(244,250,255));
                indicator.render(painter);
            }
            else // top
            {
                MovIndicatorRenderer indicator(x, y+spr->getheight(), x, y+spr->getheight()+distance, true, QColor(244,250,255));
                indicator.render(painter);
            }
        }
        else // from center
        {
            int offsetx = 0;
            if (spr->getNybble(8)%2)
                offsetx = -2;

                MovIndicatorRenderer topIndicator(x, y, x, y-distance+10, true, QColor(244,250,255));
                topIndicator.render(painter);

                MovIndicatorRenderer bottomIndicator(x, y+spr->getheight(), x, y+spr->getheight()+distance, true, QColor(244,250,255));
                bottomIndicator.render(painter);
        }
    }

    img->render(painter, drawrect);
}

// Sprite 194: Cheep Cheep
CheepCheepRenderer::CheepCheepRenderer(const Sprite *spr)
{
    this->spr = spr;
    QString filename = "cheep_cheep";

    int type = spr->getNybble(11);

    // Set Type
    switch(type)
    {
    case 1:
        filename = "deep_cheep";
        break;
    case 3:
        if (spr->getNybble(4)%3 == 0)
            filename = "cheep_cheep_r";
        break;
    case 8:
        filename = "eep_cheep";
        break;
    case 9:
        filename = "cheep_cheep_tl";
        break;
    case 10:
        filename = "cheep_cheep_bl";
        break;
    default:
        break;
    }

    // Set Direction
    if (spr->getNybble(4)%3 == 2 && type != 9 && type != 10)
    {
        if (type == 3)
            filename = "ranged_cheep_r";
        else
            filename.append("_r");
    }


    img = new NormalImageRenderer(spr, filename + ".png");
}

void CheepCheepRenderer::render(QPainter *painter, QRect *drawrect)
{
    if (spr->getNybble(11) == 3) // draw range indicators
    {
        int distance = spr->getNybble(7)*20 +30;

        int y = spr->gety()+spr->getheight()/2;

        MovIndicatorRenderer rightIndicator(spr->getx()+spr->getwidth()+10, y, spr->getx()+spr->getwidth()+distance, y, false, QColor(244,250,255));
        rightIndicator.render(painter);

        MovIndicatorRenderer leftIndicator(spr->getx()-10, y, spr->getx()-distance, y, false, QColor(244,250,255));
        leftIndicator.render(painter);
    }

    img->render(painter, drawrect);
}

// Sprite 195: Big Cheep Cheep
BigCheepRenderer::BigCheepRenderer(const Sprite *spr)
{
    this->spr = spr;
    QString filename = "cheep_cheep";

    if (spr->getNybble(11) == 1)
        filename = "deep_cheep";

    // set direction
    if (spr->getNybble(4)%3 == 2)
        filename.append("_r");

    if (spr->getNybble(5) == 0)
        img = new NormalImageRenderer(spr, QString("big_%1.png").arg(filename));
    else
        img = new NormalImageRenderer(spr, QString("huge_%1.png").arg(filename));
}

void BigCheepRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 197: Jumping Cheep Cheep
JumpingCheepRenderer::JumpingCheepRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(4) == 1 || spr->getNybble(4) == 2)
        img = new NormalImageRenderer(spr, "jumping_cheep_cheep.png");
    else
        img = new NormalImageRenderer(spr, "jumping_cheep_cheep_r.png");
}

void JumpingCheepRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 200: Spiny Cheep Cheep
SpinyCheepRenderer::SpinyCheepRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(4)%3 == 2)
        img = new NormalImageRenderer(spr, "spiny_cheep_cheep_r.png");
    else
        img = new NormalImageRenderer(spr, "spiny_cheep_cheep.png");
}

void SpinyCheepRenderer::render(QPainter *painter, QRect *drawrect)
{
    int distance = spr->getNybble(7)*20 +30;

    int y = spr->gety()+spr->getOffsetY()+spr->getheight()/2;
    int x = spr->getx()+spr->getOffsetX();

    int offset = 0;

    if (spr->getNybble(4)%3 == 2)
        offset = 2;

    MovIndicatorRenderer rightIndicator(x+spr->getwidth()+10, y, x+spr->getwidth()+distance + offset, y, false, QColor(244,250,255));
    rightIndicator.render(painter);

    MovIndicatorRenderer leftIndicator(x-10 - offset, y, x-distance, y, false, QColor(244,250,255));
    leftIndicator.render(painter);

    img->render(painter, drawrect);
}

// Sprite 209: Swinging Rope
SwingingRopeRenderer::SwingingRopeRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void SwingingRopeRenderer::render(QPainter *painter, QRect *)
{
    int length = (spr->getNybble(11)+3)*20;

    for(int i = 0; i < length; i+=20)
    {
        painter->drawPixmap(spr->getx(), spr->gety()+i, 20, 20, ImageCache::getInstance()->get(SpriteImg, "rope_segment.png"));
    }
    painter->drawPixmap(spr->getx(), spr->gety()+length, 20, 20, ImageCache::getInstance()->get(SpriteImg, "rope_end.png"));
}

// Sprite 210: Tightrope
TightropeRenderer::TightropeRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void TightropeRenderer::render(QPainter *painter, QRect *)
{
    int ropelength = 20*(spr->getNybble(4));
    int heightoffset = 0;

    QPen rope;
    rope.setWidth(6);
    rope.setColor(QColor(184,172,120));
    painter->setPen(rope);

    if (spr->getNybble(5) <=7)
    {
        for(heightoffset = 0; heightoffset < spr->getNybble(5); heightoffset++);
        painter->drawLine(spr->getx(), spr->gety(), spr->getx()+ropelength-4, spr->gety()-(heightoffset*20));
        painter->drawPixmap(QRect(spr->getx()-8+ropelength,spr->gety()-10-(heightoffset*20), 16, 16), ImageCache::getInstance()->get(SpriteImg, "rope_joint.png"));
    }
    else
    {
        heightoffset = 160;
        for (int i = 8; i != spr->getNybble(5); i++) heightoffset -= 20;
        painter->drawLine(spr->getx(), spr->gety(), spr->getx()+ropelength-4, spr->gety()+heightoffset);
        painter->drawPixmap(QRect(spr->getx()-8+ropelength,spr->gety()-10+heightoffset, 16, 16), ImageCache::getInstance()->get(SpriteImg, "rope_joint.png"));
    }

    painter->drawPixmap(QRect(spr->getx()-8,spr->gety()-10, 16, 16), ImageCache::getInstance()->get(SpriteImg, "rope_joint.png"));

    painter->setPen(Qt::NoPen);
}

// Sprite 213: Pokey
PokeyRenderer::PokeyRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void PokeyRenderer::render(QPainter *painter, QRect *)
{
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY(), 33, 33), ImageCache::getInstance()->get(SpriteImg, "pokey_top.png"));
    for (int i = 0; i < spr->getNybble(11)+1; i++)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY()+32+(i*22), 33, 22), ImageCache::getInstance()->get(SpriteImg, "pokey_mid.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(),spr->gety()+spr->getOffsetY()+54+(spr->getNybble(11)*22), 33, 24), ImageCache::getInstance()->get(SpriteImg, "pokey_bot.png"));
}

// Sprite 217/218: SnakeBlock
SnakeBlockRenderer::SnakeBlockRenderer(const Sprite *spr, const Path *path)
{
    this->spr = spr;
    this->path = path;
}

void SnakeBlockRenderer::render(QPainter *painter, QRect *)
{
    int startnode = spr->getNybble(8);
    int blocksToDraw = spr->getNybble(11)+2;

    int curNode = startnode;
    int nextNode = curNode+1;

    int curX = 0;
    int curY = 0;

    QString img = "snakeblock.png";
    if (spr->getNybble(15) == 1)
        img = "ice_snakeblock.png";

    if (!(nextNode >= path->getNumberOfNodes()))
    {
        curX = path->getNode(curNode)->getx();
        curY = path->getNode(curNode)->gety();
        painter->drawPixmap(QRect(curX, curY, 20, 20), ImageCache::getInstance()->get(SpriteImg, img));
    }

    spr->getSelectionRects()->clear();

    spr->getSelectionRects()->append(QRect(0, 0, 20, 20));

    if (startnode < path->getNumberOfNodes())
    {
        painter->drawPixmap(QRect(path->getNode(startnode)->getx(), path->getNode(startnode)->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, img));
        spr->getSelectionRects()->append(QRect(curX - spr->getx(), curY - spr->gety(), 20, 20));

        while (blocksToDraw > 0)
        {
            if (nextNode >= path->getNumberOfNodes())
            {
                blocksToDraw = 0;
                break;
            }

            int x = 0;
            int y = 0;

            if (path->getNode(curNode)->getx() > path->getNode(nextNode)->getx() && path->getNode(curNode)->gety() == path->getNode(nextNode)->gety())
                x = -20;
            if (path->getNode(curNode)->getx() < path->getNode(nextNode)->getx() && path->getNode(curNode)->gety() == path->getNode(nextNode)->gety())
                x = +20;
            if (path->getNode(curNode)->gety() > path->getNode(nextNode)->gety() && path->getNode(curNode)->getx() == path->getNode(nextNode)->getx())
                y = -20;
            if (path->getNode(curNode)->gety() < path->getNode(nextNode)->gety() && path->getNode(curNode)->getx() == path->getNode(nextNode)->getx())
                y = +20;

            curX = curX+x;
            curY = curY+y;

            painter->drawPixmap(QRect(curX, curY, 20, 20), ImageCache::getInstance()->get(SpriteImg, img));

            spr->getSelectionRects()->append(QRect(curX - spr->getx(), curY - spr->gety(), 20, 20));

            // Check if the current position is a node
            if (curX == path->getNode(nextNode)->getx() && curY == path->getNode(nextNode)->gety())
            {
                curNode++;
                nextNode++;

                curX = path->getNode(curNode)->getx();
                curY = path->getNode(curNode)->gety();
            }
            blocksToDraw--;
        }
    }
}

// Sprite 221/223/225: Switches
SwitchRenderer::SwitchRenderer(const Sprite *spr, QString filename)
{
    this->filename = filename;
    if (spr->getNybble(11) == 1)
        filename.insert(filename.size()-4, "_flipped");

    img = new NormalImageRenderer(spr, filename);
}

void SwitchRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 222/224/226: Blocks With Switches
BrickBlockSwitchRenderer::BrickBlockSwitchRenderer(const Sprite *spr, QString filename, Tileset *tileset)
{
    this->spr = spr;
    this->filename = filename;
    this->tileset = tileset;

    if (spr->getNybble(11) == 1)
        this->filename.insert(filename.size()-4, "_flipped");
}

void BrickBlockSwitchRenderer::render(QPainter *painter, QRect *)
{
    if (tileset == nullptr)
    {
        painter->drawPixmap(spr->getx(), spr->gety(), 20, 20, ImageCache::getInstance()->get(TileOverride, "error.png"));
    }
    else
    {
        QPixmap tile(20, 20);
        tile.fill(QColor(0,0,0,0));
        QPainter tempPainter(&tile);

        TileGrid tileGrid;
        tileGrid[0xFFFFFFFF] = 1;

            tileset->drawTile(tempPainter, tileGrid, 20, 0, 0, 1, 0);

        painter->drawPixmap(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight(), tile);
    }

    painter->drawPixmap(spr->getx()+1, spr->gety(), 18, 20, ImageCache::getInstance()->get(SpriteImg, filename));
}

// Sprite 187: Path Controlled Rect Lift
PathRecLiftRenderer::PathRecLiftRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void PathRecLiftRenderer::render(QPainter *painter, QRect *)
{
    QString path = "tower_rectangle_lift/";

    int blockWidth = spr->getNybble(17)*20;
    int blockHeight = spr->getNybble(19)*20;

    if (blockHeight == 0 || blockWidth == 0)
    {
        if (tileset == nullptr)
        {
            painter->drawPixmap(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(TileOverride, "error.png"));
        }
        else
        {
            QPixmap block(20, 20);
            block.fill(QColor(0,0,0,0));
            QPainter tempPainter(&block);

            TileGrid tileGrid;
            tileGrid[0xFFFFFFFF] = 1;

            tileset->drawTile(tempPainter, tileGrid, 22, 0, 0, 1, 0);
            painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20+blockWidth, 20+blockHeight, block);
        }
    }
    else
    {
        painter->drawPixmap(QRect(spr->getx(), spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "tl.png"));
        painter->drawPixmap(QRect(spr->getx(), spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "bl.png"));
        painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "tr.png"));
        painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "br.png"));

        for (int i = 0; i < spr->getNybble(17)-1; i++)
        {
            painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "t.png"));
            painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "b.png"));
        }
        for (int i = 0; i < spr->getNybble(19)-1; i++)
        {
            painter->drawPixmap(QRect(spr->getx(), spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "l.png"));
            painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "r.png"));
        }

        for (int x = 20; x < blockWidth; x+=20)
            for (int y = 20; y < blockHeight; y+=20)
                painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "c.png"));
    }
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
    int hOffX = 0;
    int hOffY = 0;
    int vOffX = 0;
    int vOffY = 0;

    switch (spr->getNybble(7)) {
    case 1:
        hOffY = -10;
        vOffY = -20;
        break;
    case 2:
        hOffY = -10;
        break;
    case 3:
        hOffY = -20;
        vOffY = -20;
        break;
    case 4:
        hOffX = -20;
        vOffX = -10;
        break;
    case 5:
        vOffX = -10;
        break;
    case 6:
        hOffX = -20;
        vOffX = -20;
        break;
    default:
        hOffX = 0;
        hOffY = 0;
    }

    // Render Movement Indicators
    int distance = spr->getNybble(14)*20;
    int eventDistance = spr->getNybbleData(10, 11)*20;


    switch (spr->getNybble(5)) {
    case 1: case 5: case 9: case 13: // Left
    {
        MovIndicatorRenderer track(spr->getx()+hOffX, spr->gety()+hOffY+spr->getheight()/2, spr->getx()+hOffX-distance, spr->gety()+hOffY+spr->getheight()/2, false, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+hOffX, spr->gety()+hOffY+spr->getheight()/2, spr->getx()+hOffX-eventDistance, spr->gety()+hOffY+spr->getheight()/2, false, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    case 2: case 6: case 10: case 14: // Up
    {
        MovIndicatorRenderer track(spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY, spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY-distance, true, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY, spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY-eventDistance, true, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    case 3: case 7: case 11: case 15: // Down
    {
        MovIndicatorRenderer track(spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY+spr->getheight(), spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY+spr->getheight()+distance, true, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY+spr->getheight(), spr->getx()+vOffX-sideOffset+spr->getwidth()/2, spr->gety()+vOffY+spr->getheight()+eventDistance, true, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    default: // Right
    {
        MovIndicatorRenderer track(spr->getx()+hOffX+spr->getwidth(), spr->gety()+hOffY+spr->getheight()/2, spr->getx()+hOffX+spr->getwidth()+distance, spr->gety()+hOffY+spr->getheight()/2, false, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+hOffX+spr->getwidth(), spr->gety()+hOffY+spr->getheight()/2, spr->getx()+hOffX+spr->getwidth()+eventDistance, spr->gety()+hOffY+spr->getheight()/2, false, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    }

    int blockWidth = spr->getNybble(17) > 0 ? spr->getNybble(17)*20 : 20;
    int blockHeight = spr->getNybble(19) > 0 ? spr->getNybble(19)*20 : 20;

    painter->drawPixmap(QRect(spr->getx()-sideOffset, spr->gety(), 20+sideOffset, 20), ImageCache::getInstance()->get(SpriteImg, path + "tl.png"));
    painter->drawPixmap(QRect(spr->getx()-sideOffset, spr->gety()+blockHeight, 20+sideOffset, 20), ImageCache::getInstance()->get(SpriteImg, path + "bl.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety(), 20+sideOffset, 20), ImageCache::getInstance()->get(SpriteImg, path + "tr.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety()+blockHeight, 20+sideOffset, 20), ImageCache::getInstance()->get(SpriteImg, path + "br.png"));

    for (int i = 0; i < spr->getNybble(17)-1; i++)
    {
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "t.png"));
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "b.png"));
    }
    for (int i = 0; i < spr->getNybble(19)-1; i++)
    {
        painter->drawPixmap(QRect(spr->getx(), spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "l.png"));
        painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "r.png"));
    }

    if (spr->getid() == 282)
    {
        int yTile = 1;
        int xTile = 1;

        for (int x = 20; x < blockWidth; x+=20)
        {
            yTile = 1;

            for (int y = 20; y < blockHeight; y+=20)
            {
                painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "c" +  QString::number(yTile) + QString::number(xTile) + ".png"));
                yTile++;
                if (yTile > 2) yTile = 1;
            }
            xTile++;
            if (xTile > 4) xTile = 1;
        }
    }
    else
    {
        for (int x = 20; x < blockWidth; x+=20)
            for (int y = 20; y < blockHeight; y+=20)
                painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "c.png"));
    }

    if (spr->getNybble(7) == 1 || spr->getNybble(7) == 3) for (int x = 0; x < blockWidth+20; x+=20) painter->drawPixmap(QRect(spr->getx()+x, spr->gety()-20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "s_t.png"));
    if (spr->getNybble(7) == 2 || spr->getNybble(7) == 3) for (int x = 0; x < blockWidth+20; x+=20) painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+blockHeight+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "s_b.png"));
    if (spr->getNybble(7) == 4 || spr->getNybble(7) == 6) for (int y = 0; y < blockHeight+20; y+=20) painter->drawPixmap(QRect(spr->getx()-20, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "s_l.png"));
    if (spr->getNybble(7) == 5 || spr->getNybble(7) == 6) for (int y = 0; y < blockHeight+20; y+=20) painter->drawPixmap(QRect(spr->getx()+blockWidth+20, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "s_r.png"));

}


// Sprite 215: Bob-omb Cannon
BobOmbCannonRenderer::BobOmbCannonRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BobOmbCannonRenderer::render(QPainter *painter, QRect *)
{

    //if (spr->getNybble(11) != 1)
    QPixmap img = ImageCache::getInstance()->get(SpriteImg, "cannon.png");

    switch (spr->getNybble(10))
    {
    case 1:
        img = img.transformed(QTransform().scale(1,-1));
        break;
    case 2:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon_angle.png");
        break;
    case 3: case 4:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon_angle.png");
        img = img.transformed(QTransform().rotate(-90));
        break;
    case 5:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon_angle.png");
        img = img.transformed(QTransform().rotate(-90).scale(1,-1));
        break;
    case 6: case 10: case 11:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon_straight.png");
        img = img.transformed(QTransform().rotate(-90));
        break;
    case 7: case 12: case 13:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon_straight.png");
        img = img.transformed(QTransform().rotate(-90).scale(1,-1));
        break;
    case 8:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon.png");
        img = img.transformed(QTransform().rotate(-90));
        break;
    case 9:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon.png");
        img = img.transformed(QTransform().rotate(-90).scale(1,-1));
        break;
    case 14: case 15:
        img = img = ImageCache::getInstance()->get(SpriteImg, "cannon_straight.png");
        break;
    default:
        break;
    }

    if (spr->getNybble(11) == 1)
    {
        switch (spr->getNybble(10))
        {
        case 0: case 1:
            img = img.transformed(QTransform().scale(-1,1));
            break;
        case 2:
            img = img.transformed(QTransform().scale(-1,1).rotate(-90));
            break;
        case 3:
            img = img.transformed(QTransform().scale(-1,1).rotate(90));
            break;
        case 4: case 5: case 14: case 15:
            break;
        default:
            img = img.transformed(QTransform().scale(1,-1));
            break;
        }
    }

    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), img);
}

// Sprite 229: Big Boo
BigBooRenderer::BigBooRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void BigBooRenderer::render(QPainter* painter, QRect*)
{
    if (spr->getNybble(9)%2 == 0)
        painter->drawPixmap(spr->getx()+spr->getOffsetX() -27, spr->gety()+spr->getOffsetY() -26, 132, 132, ImageCache::getInstance()->get(SpriteImg, "big_boo_glow.png"));
    else
        painter->drawPixmap(spr->getx()+spr->getOffsetX() -14, spr->gety()+spr->getOffsetY() -13, 106, 106, ImageCache::getInstance()->get(SpriteImg, "big_boo.png"));
}

// Sprite 17/186/199/230: Amp/Paratroopa/CheepCheep/Peepa Circle
CoinCircleRenderer::CoinCircleRenderer(const Sprite *spr, Tileset* tileset)
{
    this->spr = spr;
    this->tileset = tileset;
    circle = new CircleRenderer(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), "", QColor(0,0,0));
}

void CoinCircleRenderer::render(QPainter *painter, QRect *drawrect)
{
    circle->render(painter, drawrect);

    QPixmap img;

    if (spr->getid() == 17)
        img = ImageCache::getInstance()->get(SpriteImg, "amp_circle.png");
    else if (spr->getid() == 186)
        img = ImageCache::getInstance()->get(SpriteImg, "paratroopa_circle.png");
    else if (spr->getid() == 199)
        img = ImageCache::getInstance()->get(SpriteImg, "cheepcheep_circle.png");
    else
        img = ImageCache::getInstance()->get(SpriteImg, "peepa.png");

    QPixmap coin(40, 40);
    coin.fill(QColor(0,0,0,0));
    QPainter coinPainter(&coin);

    TileGrid tileGrid;
    tileGrid[0xFFFFFFFF] = 1;

    if (tileset == nullptr)
    {
        coinPainter.drawPixmap(10, 10, 20, 20, ImageCache::getInstance()->get(TileOverride, "error.png"));
    }
    else
    {
        QPixmap tempCoin(20, 20);
        tempCoin.fill(QColor(0,0,0,0));
        QPainter tempPainter(&tempCoin);

        if (spr->getNybble(8)%2 == 1)
            tileset->drawTile(tempPainter, tileGrid, 16, 0, 0, 1, 0);
        else
            tileset->drawTile(tempPainter, tileGrid, 15, 0, 0, 1, 0);

        coinPainter.drawPixmap(10, 10, 20, 20, tempCoin);
    }

    if (spr->getNybble(15) == 2)
        img = coin;

    int missingImgWeight = 0;
    qreal angle = 0;
    int x = 0;
    int y = 0;

    int radius = spr->getNybble(6)+1;
    int imgCount = 1+spr->getNybble(7);

    for (int i = 0; i < imgCount; i++)
    {
        missingImgWeight = float(0.75 - (1 / imgCount));
        angle = -360 * i / (imgCount + missingImgWeight);
        angle = qDegreesToRadians(angle+90);

        x = float(qSin(angle) * ((radius * 20)) - 10);
        y = float(-(qCos(angle) * ((radius * 20))) - 10);

        // Peepa circles are -14 pixels higher than other circles
        if (spr->getid() == 230)
            y -= 14;

        // Koopas appear -16 pixels higher on the circle than coins
        int imgOffsetY = 0;
        if (spr->getid() == 186 && spr->getNybble(15) != 2)
            imgOffsetY = -16;

        if ((spr->getNybbleData(16, 19) & (1 << i)) == (1 << i) && spr->getNybble(15) == 1)
            painter->drawPixmap(spr->getx()+x, spr->gety()+y, 40, 40, coin);

        else if ((spr->getNybbleData(16, 19) & (1 << i)) != (1 << i))
            painter->drawPixmap(spr->getx()+x, spr->gety()+y+imgOffsetY, 40, 40, img);
    }
}

// Sprite 231: Boohemoth
BoohemothRenderer::BoohemothRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString imgName = "boohemoth_r.png";

    if (spr->getNybble(10) %2)
        imgName = "boohemoth_l.png";

    img = new NormalImageRenderer(spr, imgName);
}

void BoohemothRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 232: Spiny
SpinyRenderer::SpinyRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString imgName = "spiny";

    switch (spr->getNybble(11) %4) {
    case 1:
        imgName.append("_egg");
        break;
    case 2:
        imgName.append("_shell");
        break;
    case 3:
        imgName.append("_shell_upsidedown");
        break;
    default:
        break;
    }

    img = new NormalImageRenderer(spr, QString("%1.png").arg(imgName));
}

void SpinyRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 233: Ceiling Spiny
CeilingSpinyRenderer::CeilingSpinyRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString imgName = "spiny_ceiling";

    if (spr->getNybble(8) %2)
        imgName = "spiny_falling";
    img = new NormalImageRenderer(spr, QString("%1.png").arg(imgName));
}

void CeilingSpinyRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 239: Swinging Vine
SwingingVineRenderer::SwingingVineRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void SwingingVineRenderer::render(QPainter *painter, QRect *)
{
    int length = (spr->getNybble(11) + 3) * 20;

    for (int i = 0; i < length; i+=20)
    {
        if ((i/20)%2 == 1)
            painter->drawPixmap(spr->getx(), spr->gety()+i, 18, 20, ImageCache::getInstance()->get(SpriteImg, "vine_b.png"));
        else
            painter->drawPixmap(spr->getx(), spr->gety()+i, 18, 20, ImageCache::getInstance()->get(SpriteImg, "vine_a.png"));
    }
    painter->drawPixmap(spr->getx(), spr->gety()+length, 18, 20, ImageCache::getInstance()->get(SpriteImg, "vine_c.png"));
}

// Sprite 240: Urchin
UrchinRenderer::UrchinRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(11) != 1)
        img = new NormalImageRenderer(spr, "urchin.png");
    else
        img = new NormalImageRenderer(spr, "urchin_big.png");
}

void UrchinRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 243: Ghost Wall
GhostWallRenderer::GhostWallRenderer(const Sprite *spr)
{
    this->spr = spr;

    QString imgName = "ghost_wall_right";

    if (spr->getNybble(8)%4 == 2)
        imgName = "ghost_wall_left";

    if (spr->getNybble(6) &1)
        imgName.append("_boos");

    img = new NormalImageRenderer(spr, QString("%1.png").arg(imgName));
}

void GhostWallRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 244: Chain Chomp
ChainChompRenderer::ChainChompRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(11)%2 == 0)
        img = new NormalImageRenderer(spr, "chain_chomp_3.png");
    else
        img = new NormalImageRenderer(spr, "chain_chomp_1.png");
}

void ChainChompRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 247: Invisible Bouncy Blocks
InvisibleBouncyBlockRenderer::InvisibleBouncyBlockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void InvisibleBouncyBlockRenderer::render(QPainter *painter, QRect *)
{
    // value + 2 i think
    if(spr->getNybble(4) == 0)
        painter->drawPixmap(spr->getx(), spr->gety(), 20, 20, ImageCache::getInstance()->get(SpriteImg, + "invisible_bounce.png"));
    for (int i = 0; i < spr->getNybble(4) + 1; i++)
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+i*20, spr->gety(), 20, 20, ImageCache::getInstance()->get(SpriteImg, + "invisible_bounce.png"));
}

// Sprite 248: Move Once On Ghost House Platform
MoveOnGhostPlatRenderer::MoveOnGhostPlatRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void MoveOnGhostPlatRenderer::render(QPainter *painter, QRect *)
{
    int blockWidth = spr->getNybble(11)*20 +20;
    int blockHeight = spr->getNybble(8)*20 +20;

    painter->drawPixmap(QRect(spr->getx(), spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/tl.png"));
    painter->drawPixmap(QRect(spr->getx(), spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/bl.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/tr.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/br.png"));

    for (int i = 0; i < spr->getNybble(11); i++)
    {
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/t.png"));
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/b.png"));
    }
    for (int i = 0; i < spr->getNybble(8); i++)
    {
        painter->drawPixmap(QRect(spr->getx(), spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/l.png"));
        painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/r.png"));
    }

    int yTile = 1;
    int xTile = 1;

    for (int x = 20; x < blockWidth; x+=20)
    {
        yTile = 1;

        for (int y = 20; y < blockHeight; y+=20)
        {
            painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, "ghost_rectangle_lift/c" +  QString::number(yTile) + QString::number(xTile) + ".png"));
            yTile++;
            if (yTile > 2) yTile = 1;
        }
        xTile++;
        if (xTile > 4) xTile = 1;
    }
}

// Sprite 249: Rotation Controlled Rectangle Lift
RotRecLiftRenderer::RotRecLiftRenderer(const Sprite *spr, QString path)
{
    this->spr = spr;
    this->path = path;
}

void RotRecLiftRenderer::render(QPainter *painter, QRect *)
{
    int blockWidth = spr->getNybble(17)*20 + 40;
    int blockHeight = spr->getNybble(19)*20 + 40;

    painter->drawPixmap(QRect(spr->getx(), spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "tl.png"));
    painter->drawPixmap(QRect(spr->getx(), spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "bl.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "tr.png"));
    painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "br.png"));

    for (int i = 0; i < spr->getNybble(17)+1; i++)
    {
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety(), 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "t.png"));
        painter->drawPixmap(QRect(spr->getx() + i*20+20, spr->gety()+blockHeight, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "b.png"));
    }
    for (int i = 0; i < spr->getNybble(19)+1; i++)
    {
        painter->drawPixmap(QRect(spr->getx(), spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "l.png"));
        painter->drawPixmap(QRect(spr->getx()+blockWidth, spr->gety() + i*20+20, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "r.png"));
    }

    for (int x = 20; x < blockWidth; x+=20)
        for (int y = 20; y < blockHeight; y+=20)
            painter->drawPixmap(QRect(spr->getx()+x, spr->gety()+y, 20, 20), ImageCache::getInstance()->get(SpriteImg, path + "c.png"));

    painter->drawPixmap(spr->getx()+spr->getwidth()/2-10, spr->gety()+spr->getheight()/2-10, 20, 20, ImageCache::getInstance()->get(TileOverlay, "rotation.png"));
}

// Sprites 100/101/203/204/267/275/276: Item Blocks
ItemBlockRenderer::ItemBlockRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void ItemBlockRenderer::render(QPainter *painter, QRect *)
{
    bool invalid = false;
    if (tileset == nullptr)
    {
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(TileOverride, "error.png"));
        invalid = true;
    }
    else
    {    if (spr->getid() != 267 || spr->getid() != 275 || spr->getid() != 276)
        {
            int tileId = 0;
            switch (spr->getid())
            {
            case 100: case 101: tileId = 21; break;
            case 203: case 204: tileId = 20; break;
            }

            QPixmap block(spr->getwidth(), spr->getheight());
            block.fill(QColor(0,0,0,0));
            QPainter tempPainter(&block);

            TileGrid tileGrid;
            tileGrid[0xFFFFFFFF] = 1;

            tileset->drawTile(tempPainter, tileGrid, tileId, 0, 0, 1, 0);
            painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, block);
        }
    }

    if ( spr->getid() == 267 || spr->getid() == 275 || spr->getid() == 276 )
    {
        QString filename;
        switch (spr->getid())
        {
            case 267: filename = "long_question_block.png"; break;
            case 275: filename = "long_question_block_underground.png"; break;
            case 276: filename = "long_question_block_lava.png"; break;
            default: return;
        }
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, filename));
    }

    QString overlay;

    switch (spr->getNybble(19))
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
        default: return;
    }

    if (((spr->getheight() == 20) && (spr->getwidth() == 20)) && !invalid)
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, ImageCache::getInstance()->get(TileOverlay, overlay));
    else
        painter->drawPixmap(spr->getx()+spr->getOffsetX()+20, spr->gety()+spr->getOffsetY(), 20, 20, ImageCache::getInstance()->get(TileOverlay, overlay));

    if (((spr->getid() == 100) || (spr->getid() == 203)) && !invalid)
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, ImageCache::getInstance()->get(TileOverlay, "rotation.png"));
    if (((spr->getid() == 101) || (spr->getid() == 204)) && !invalid)
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, ImageCache::getInstance()->get(TileOverlay, "movement.png"));
}

// Sprite 252: Item Renderer
ItemRenderer::ItemRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ItemRenderer::render(QPainter *painter, QRect *)
{
    QString item;
    switch (spr->getNybble(4))
    {
    case 2:
        item  = "fire_flower.png";
        break;
    case 4:
        item  = "super_leaf.png";
        break;
    case 5:
        item  = "gold_flower.png";
        break;
    case 6:
        item  = "mini_mushroom.png";
        break;
    case 7:
        item  = "super_star.png";
        break;
    case 9:
        item = "mega_mushroom.png";
        break;
    case 11:
        item = "1up_mushroom.png";
        break;
    default:
        item = "super_mushroom.png";
        break;
    }

    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "item/" + item));

}

// Sprite 253: Larry Battle Platform
LarryPlatformRenderer::LarryPlatformRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void LarryPlatformRenderer::render(QPainter *painter, QRect *)
{
    if (tileset == nullptr)
    {
        for (int x = 0; x < spr->getwidth(); x += 20)
            painter->drawPixmap(spr->getx() + x, spr->gety(), 20, 20, ImageCache::getInstance()->get(TileOverride, "error.png"));
        return;
    }

    QPixmap tile(spr->getwidth(), 20);
    tile.fill(QColor(0,0,0,0));
    QPainter tempPainter(&tile);

    TileGrid tileGrid;
    tileGrid[0xFFFFFFFF] = 1;

    for (int x = 0; x < spr->getwidth()/20; x++)
        tileset->drawTile(tempPainter, tileGrid, 29, x, 0, 1, 0);

    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), tile);
}

// Sprite 257: Movement Controlled Bone Platform
BonePlatformRenderer::BonePlatformRenderer(const Sprite* spr)
{
    this->spr = spr;
}

void BonePlatformRenderer::render(QPainter* painter, QRect*)
{
    QString img_name;
    switch (spr->getNybble(19))
    {
    case 1:
        img_name = "coaster_long";
        break;

    case 2:
        img_name = "coaster_end";
        break;

    case 3:
        img_name = "coaster_short";
        break;

    default:
        img_name = "coaster_head";
        break;
    }

    painter->drawPixmap(QRect(spr->getx(), spr->gety(), spr->getwidth(), spr->getheight()), ImageCache::getInstance()->get(SpriteImg, img_name + ".png"));

}

// Sprite 259: Rotation Controlled hard Block
HardBlockRenderer::HardBlockRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void HardBlockRenderer::render(QPainter *painter, QRect *)
{
    if (tileset == nullptr)
    {
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(TileOverride, "error.png"));
    }
    else
    {
        int tileId = 0;
        switch (spr->getNybble(18))
        {
            case 1: tileId = 33; break;
            case 2: tileId = 22; break;
            case 3: tileId = 20; break;
            default: tileId = 24; break;
        }

        QPixmap block(spr->getwidth(), spr->getheight());
        block.fill(QColor(0,0,0,0));
        QPainter tempPainter(&block);

        TileGrid tileGrid;
        tileGrid[0xFFFFFFFF] = 1;

        tileset->drawTile(tempPainter, tileGrid, tileId, 0, 0, 1, 0);
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20, block);
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 20, 20,ImageCache::getInstance()->get(TileOverlay, "rotation.png"));
    }
}

// Sprite 260/261/262/263/264/256: Boss Cutscene Controllers
BossControllerRenderer::BossControllerRenderer(const Sprite *spr, const Zone *zone)
{
    this->spr = spr;
    this->zone = zone;
}

void BossControllerRenderer::render(QPainter *painter, QRect *)
{
    int x = zone->getx()+zone->getwidth()-80;
    int y = zone->gety();

    QString bossName;
    switch (spr->getid())
    {
        case 260: bossName = "roy"; break;
        case 261: bossName = "iggy"; break;
        case 262: bossName = "wendy"; break;
        case 263: bossName = "morton"; break;
        case 264: bossName = "ludwig"; break;
        default: bossName = "tower"; break;
    }

    painter->drawPixmap(x, y, 70, 140, ImageCache::getInstance()->get(SpriteImg, bossName + "_cutscene.png"));
}

// Sprite 268: Rectangle Lift - Lava
LavaRectLiftRenderer::LavaRectLiftRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void LavaRectLiftRenderer::render(QPainter *painter, QRect *)
{
    // Render Movement Indicators
    int distance = spr->getNybbleData(17, 14)*20;
    int eventDistance = spr->getNybbleData(10, 11)*20;


    switch (spr->getNybble(5)) {
    case 1: case 5: case 9: case 13: // Left
    {
        MovIndicatorRenderer track(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-distance, spr->gety()+spr->getheight()/2, false, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-eventDistance, spr->gety()+spr->getheight()/2, false, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    case 2: case 6: case 10: case 14: // Up
    {
        MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety(), spr->getx()+spr->getwidth()/2, spr->gety()-distance, true, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+spr->getwidth()/2, spr->gety(), spr->getx()+spr->getwidth()/2, spr->gety()-eventDistance, true, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    case 3: case 7: case 11: case 15: // Down
    {
        MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+distance, true, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+eventDistance, true, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    default: // Right
    {
        MovIndicatorRenderer track(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+distance, spr->gety()+spr->getheight()/2, false, QColor(244,250,255));
        track.render(painter);

        MovIndicatorRenderer eventTrack(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+eventDistance, spr->gety()+spr->getheight()/2, false, QColor(243,156,18));
        eventTrack.render(painter);
    }
        break;
    }

    if (spr->getNybble(19) == 1)
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 100, 603,ImageCache::getInstance()->get(SpriteImg, "rect_lift_lava_long.png"));
    else
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), 250, 80,ImageCache::getInstance()->get(SpriteImg, "rect_lift_lava.png"));
}

// Sprite 275: Coin Roulette Block
CoinRouletteRenderer::CoinRouletteRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void CoinRouletteRenderer::render(QPainter *painter, QRect *)
{
    QString img = "coin_roulette_block.png";

    switch (spr->getNybble(19))
    {
    case 1:
        img = "coin_roulette_block_10.png";
        break;
    case 2:
        img = "coin_roulette_block_10_big.png";
        break;
    case 3:
        img = "coin_roulette_block_50.png";
        break;
    default:
        break;
    }

    painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, img));
}

// Sprite 280: + Clock
ClockRenderer::ClockRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void ClockRenderer::render(QPainter *painter, QRect *)
{
    if (spr->getNybble(11) != 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, + "clock_50.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, + "clock_10.png"));
}

// Sprites 283/307/308/318: Event Activated Rectangle Lifts
EventRecLiftRenderer::EventRecLiftRenderer(const Sprite *spr, QString path)
{
    this->spr = spr;

    QString type = ".png";

    if (spr->getid() == 318)
    {
        switch (spr->getNybble(19))
        {
        case 1:
            type = "_small.png";
            break;
        case 2:
            type = "_big.png";
            break;
        default:
            type = ".png";
            break;
        }
    }

    this->img = new NormalImageRenderer(spr, path + type);
}

void EventRecLiftRenderer::render(QPainter *painter, QRect *drawrect)
{
    // Render Movement Indicators

    int distance = 0;

    if (spr->getid() == 283)
        distance = spr->getNybble(14)*20;
    else
        distance = spr->getNybbleData(4,5)*20;

    switch (spr->getNybble(5)) {
    case 1: case 5: case 9: case 13: // Left
    {
        MovIndicatorRenderer track(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-distance, spr->gety()+spr->getheight()/2, false, QColor(243,156,18));
        track.render(painter);
    }
        break;
    case 2: case 6: case 10: case 14: // Up
    {
        MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety(), spr->getx()+spr->getwidth()/2, spr->gety()-distance, true, QColor(243,156,18));
        track.render(painter);
    }
        break;
    case 3: case 7: case 11: case 15: // Down
    {
        MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+distance, true, QColor(243,156,18));
        track.render(painter);
    }
        break;
    default: // Right
    {
        MovIndicatorRenderer track(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+distance, spr->gety()+spr->getheight()/2, false, QColor(243,156,18));
        track.render(painter);
    }
        break;
    }

    img->render(painter, drawrect);
}

// Sprite 297: Horizontal Rail Controlled Fence
RailContFenceRenderer::RailContFenceRenderer(const Sprite *spr)
{
    this->spr = spr;
}
void RailContFenceRenderer::render(QPainter *painter, QRect *)
{
    if (spr->getNybble(11) == 1) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "rail_fence_3x6.png"));
    else if (spr->getNybble(11) == 2) painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "rail_fence_6x3.png"));
    else painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(SpriteImg, "rail_fence_3x3.png"));
}

// Sprite 298: Bouncy Mushroom - Castle
BounceMushCastleRenderer::BounceMushCastleRenderer(const Sprite *spr)
{
    this->spr = spr;
    if (spr->getNybble(17) == 1)
        img = new NormalImageRenderer(spr, "bouncy_mushroom_castle_small.png");
    else
        img = new NormalImageRenderer(spr, "bouncy_mushroom_castle.png");
}
void BounceMushCastleRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 131: Bowser Block
BowserBlockRenderer::BowserBlockRenderer(const Sprite *spr, Tileset *tileset)
{
    this->spr = spr;
    this->tileset = tileset;
}

void BowserBlockRenderer::render(QPainter *painter, QRect *)
{
    if (tileset == nullptr)
        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), ImageCache::getInstance()->get(TileOverride, "error.png"));
    else
    {
        QPixmap pix(spr->getwidth(), spr->getheight());
        pix.fill(QColor(0,0,0,0));
        QPainter tempPainter(&pix);

        TileGrid tileGrid;
        tileGrid[0xFFFFFFFF] = 1;

        for (int x = 0; x < spr->getwidth()/20; x++)
            for (int y = 0; y < spr->getheight()/20; y++)
                tileset->drawTile(tempPainter, tileGrid, 24, x, y, 1, 0);

        painter->drawPixmap(spr->getx()+spr->getOffsetX(), spr->gety()+spr->getOffsetY(), spr->getwidth(), spr->getheight(), pix);
    }
}

// Sprite 134: 3 Plat Rickshaw ruins
RuinsRickRenderer::RuinsRickRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "3Plat_Rickshaw_Ruins_Big.png");
    else img = new NormalImageRenderer(spr, "3Plat_Rickshaw_Ruins.png");
}

void RuinsRickRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 305: Path Controlled Ice Lift
IceLiftRenderer::IceLiftRenderer(const Sprite *spr)
{
    switch (spr->getNybble(11))
    {
        case 1: img = new NormalImageRenderer(spr, "ice_lift_two.png"); break;
        case 2: img = new NormalImageRenderer(spr, "ice_lift_tiered.png"); break;
        default: img = new NormalImageRenderer(spr, "ice_lift.png"); break;
    }
}

void IceLiftRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 311: Coin Meteor
CoinMeteorRenderer::CoinMeteorRenderer(const Sprite *spr)
{
    if (spr->getNybble(11) == 1) img = new NormalImageRenderer(spr, "coin_meteor_big.png");
    else img = new NormalImageRenderer(spr, "coin_meteor.png");
}

void CoinMeteorRenderer::render(QPainter *painter, QRect *drawrect)
{
    img->render(painter, drawrect);
}

// Sprite 313: Underwater Rectangle Lift
UnderwaterRecLiftRenderer::UnderwaterRecLiftRenderer(const Sprite *spr)
{
    this->spr = spr;

    if (spr->getNybble(19) == 1)
        img = new NormalImageRenderer(spr, "underwater_rect_lift_big.png");
    else
        img = new NormalImageRenderer(spr, "underwater_rect_lift.png");
}

void UnderwaterRecLiftRenderer::render(QPainter *painter, QRect *drawrect)
{
    int distance = spr->getNybbleData(10, 11)*20;

    switch (spr->getNybble(5)) {
    case 1: case 5: case 9: case 13: // Left
        {
            MovIndicatorRenderer track(spr->getx(), spr->gety()+spr->getheight()/2, spr->getx()-distance, spr->gety()+spr->getheight()/2, false, QColor(243,156,18));
            track.render(painter);
        }
        break;
    case 2: case 6: case 10: case 14: // Up
        {
            MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety(), spr->getx()+spr->getwidth()/2, spr->gety()-distance, true, QColor(243,156,18));
            track.render(painter);
        }
        break;
    case 3: case 7: case 11: case 15: // Down
        {
            MovIndicatorRenderer track(spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight(), spr->getx()+spr->getwidth()/2, spr->gety()+spr->getheight()+distance, true, QColor(243,156,18));
            track.render(painter);
        }
        break;
    default: // Right
        {
            MovIndicatorRenderer track(spr->getx()+spr->getwidth(), spr->gety()+spr->getheight()/2, spr->getx()+spr->getwidth()+distance, spr->gety()+spr->getheight()/2, false, QColor(243,156,18));
            track.render(painter);
        }
        break;
    }

    img->render(painter, drawrect);
}

// Sprite 320: Path Controlled Lift
PathLiftRenderer::PathLiftRenderer(const Sprite *spr)
{
    this->spr = spr;
}

void PathLiftRenderer::render(QPainter *painter, QRect *)
{
    // Draw Platform
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX(), spr->gety(), 23, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/l.png"));
    painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+(spr->getNybble(11))*20+23, spr->gety(), 23, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/r.png"));
    for (int i = 20; i < spr->getwidth()-43; i += 20)
        painter->drawPixmap(QRect(spr->getx()+spr->getOffsetX()+i+3, spr->gety(), 20, 22), ImageCache::getInstance()->get(SpriteImg, "lift_platform/m.png"));
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

    QRect textRect(entr->getx()+entr->getOffsetX()+2, entr->gety()+entr->getOffsetY()+1, entr->getwidth()-4, entr->getheight());
    QRect imgRect(entr->getx()+entr->getOffsetX(), entr->gety()+entr->getOffsetY(), 20, 20);

    painter->setPen(QColor(0,0,0));

    switch (entr->getEntrType())
    {
    case 0: case 11: case 12: case 13: case 14:
        painter->drawText(textRect, "N", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "normal.png"));
        break;
    case 1:
        painter->drawText(textRect, "C", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "normal.png"));
        break;
    case 2:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "door_exit.png"));
        break;
    case 3: case 16:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "pipe_up.png"));
        break;
    case 4: case 17:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "pipe_down.png"));
        break;
    case 5: case 18:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "pipe_left.png"));
        break;
    case 6: case 19:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "pipe_right.png"));
        break;
    case 7:
        painter->drawText(textRect, "F", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "down.png"));
        break;
    case 8:
        painter->drawText(textRect, "G", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "down.png"));
        break;
    case 9:
        painter->drawText(textRect, "S", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "normal.png"));
        break;
    case 10:
        painter->drawText(textRect, "S", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "swimming.png"));
        break;
    case 15:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "boss_door_entrance.png"));
        break;
    case 20:
        painter->drawText(textRect, "J", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "up.png"));
        break;
    case 21:
        painter->drawText(textRect, "V", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "up.png"));
        break;
    case 23: case 25: case 26:
        painter->drawText(textRect, "B", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "normal.png"));
        break;
    case 24:
        painter->drawText(textRect, "J", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "left.png"));
        break;
    case 27:
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "door_entrance.png"));
        break;
    case 30:
        painter->drawText(textRect, "W", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "normal.png"));
        break;
    case 31:
        painter->drawText(textRect, "W", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "left.png"));
        break;
    default:painter->drawText(textRect, "?", Qt::AlignLeft | Qt::AlignBottom);
        painter->drawPixmap(imgRect, ImageCache::getInstance()->get(EntranceImg, "unknown.png"));
        break;
    }

    painter->setFont(QFont("Arial", 7, QFont::Normal));
    painter->drawText(textRect, QString("%1").arg(entr->getid()), Qt::AlignLeft | Qt::AlignTop);
    painter->setPen(Qt::NoPen);
}

// Vertical Camera Limit Renderer
VCameraLimitRenderer::VCameraLimitRenderer(int leftX, int rightX, int yPos, int yRenderOffset, bool permiable, bool isBelow)
{
    this->leftX = leftX;
    this->rightX = rightX;
    this->yPos = yPos;
    this->yRenderOffset = yRenderOffset;
    this->permiable = permiable;
    this->isBelow = isBelow;
}

void VCameraLimitRenderer::render(QPainter *painter, QRect *)
{
    QColor dark(100,0,0,200);
    QColor light(220,0,0,150);

    QPen normalPen(light, 2, Qt::SolidLine, Qt::FlatCap);
    QPen permiablePen(light, 2, Qt::DashDotLine, Qt::FlatCap);

    QPixmap arrow = ImageCache::getInstance()->get(SpriteImg, "camlimit.png");

    if (!isBelow)
        yPos = yPos+20;

    // Draw Center Line
    if(permiable)
    {
        if (yRenderOffset == 0)
            permiablePen.setColor(dark);

        painter->setPen(permiablePen);
    }
    else
    {
        if (yRenderOffset == 0)
            normalPen.setColor(dark);

        painter->setPen(normalPen);
    }

    painter->drawLine(leftX+20, yPos, rightX, yPos);

    // Draw Offset Line
    if (yRenderOffset != 0)
    {
        normalPen.setColor(dark);
        painter->setPen(normalPen);

        if (isBelow)
            painter->drawLine(leftX, yPos-yRenderOffset, rightX+20, yPos-yRenderOffset);
        else
            painter->drawLine(leftX, yPos+yRenderOffset, rightX+20, yPos+yRenderOffset);
    }

    // Draw Arrows
    QRect imgRect;

    int arrowOffset = 0;

    if (yRenderOffset == 0)
        arrowOffset = 20;

    int lineDistance = (rightX - arrowOffset) - (leftX + arrowOffset);
    float emptySpaces = fmodf(lineDistance, 60);
    int offset = int(arrowOffset + emptySpaces/2);

    if (isBelow)
    {
        // Draw Down-facing Arrow
        arrow = arrow.transformed(QTransform().rotate(180));

        for (int i = leftX + offset; i <= rightX - offset; i+=60)
        {
            imgRect = QRect(i, yPos-yRenderOffset, 20, 20);
            painter->drawPixmap(imgRect, arrow);
        }
    }
    else
    {
        // Draw Up-Facing Arrows
        for (int i = leftX + offset; i <= rightX - offset; i+=60)
        {
            imgRect = QRect(i, yPos+yRenderOffset-20, 20, 20);
            painter->drawPixmap(imgRect, arrow);
        }
    }

    painter->setPen(Qt::NoPen);
}

// Horizontal Camera Limit Renderer
HCameraLimitRenderer::HCameraLimitRenderer(int topY, int bottomY, int xPos, int xRenderOffset, bool permiable, bool isRight)
{
    this->topY = topY;
    this->bottomY = bottomY;
    this->xPos = xPos;
    this->xRenderOffset = xRenderOffset;
    this->permiable = permiable;
    this->isRight = isRight;
}

void HCameraLimitRenderer::render(QPainter *painter, QRect *)
{
    QColor dark(100,0,0,200);
    QColor light(220,0,0,150);

    QPen normalPen(light, 2, Qt::SolidLine, Qt::FlatCap);
    QPen permiablePen(light, 2, Qt::DashDotLine, Qt::FlatCap);

    QPixmap arrow = ImageCache::getInstance()->get(SpriteImg, "camlimit.png");

    if (!isRight)
        xPos = xPos+20;

    // Draw Center Line
    if(permiable)
    {
        if (xRenderOffset == 0)
            permiablePen.setColor(dark);

        painter->setPen(permiablePen);
    }
    else
    {
        if (xRenderOffset == 0)
            normalPen.setColor(dark);

        painter->setPen(normalPen);
    }

    painter->drawLine(xPos, bottomY, xPos, topY+20);

    // Draw Offset Line
    if (xRenderOffset != 0)
    {
        normalPen.setColor(dark);
        painter->setPen(normalPen);

        if (isRight)
            painter->drawLine(xPos-xRenderOffset, bottomY+20, xPos-xRenderOffset, topY);
        else
            painter->drawLine(xPos+xRenderOffset, bottomY+20, xPos+xRenderOffset, topY);
    }

    // Draw Direction Arrows
    QRect imgRect;

    int arrowOffset = 0;

    if (xRenderOffset == 0)
        arrowOffset = 20;

    int lineDistance = (bottomY - arrowOffset) - (topY + arrowOffset);
    float emptySpaces = fmodf(lineDistance, 60);
    int offset = int(arrowOffset + emptySpaces/2);

    if (isRight)
    {
        // Draw Right-facing Arrows
        arrow = arrow.transformed(QTransform().rotate(90));

        for (int i = topY + offset; i <= bottomY - offset; i+=60)
        {
            imgRect = QRect(xPos-xRenderOffset, i, 20, 20);
            painter->drawPixmap(imgRect, arrow);
        }
    }
    else
    {
        // Draw Left-Facing Arrows
        arrow = arrow.transformed(QTransform().rotate(-90));

        for (int i = topY + offset; i <= bottomY - offset; i+=60)
        {
            imgRect = QRect(xPos+xRenderOffset-20, i, 20, 20);
            painter->drawPixmap(imgRect, arrow);
        }
    }

    painter->setPen(Qt::NoPen);
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
    else if (liquid->getid() == 14)
        filename = "cloud";
    else
        filename = "water";
}

void LiquidRenderer::render(QPainter *painter, QRect *drawrect)
{
    if (liquid->getid() != 14)
    {
        QPixmap top = ImageCache::getInstance()->get(SpriteImg, filename + "_top.png");
        QPixmap base = ImageCache::getInstance()->get(SpriteImg, filename + ".png");

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
    else
    {
        QPixmap cloud = ImageCache::getInstance()->get(SpriteImg, filename + ".png");

        int currY = liquid->gety();

        for (int x = zone->getx(); x < zone->getx() + zone->getwidth(); x += cloud.width())
        {
            QRect rect = QRect(x, currY, qMin(zone->getx() + zone->getwidth() - x, cloud.width()), qMin(zone->gety() + zone->getheight() - currY, cloud.height()));

            if (!drawrect->intersects(rect))
                continue;

            painter->drawPixmap(rect, cloud, QRect(0, 0, rect.right()-rect.left(), rect.bottom()-rect.top()));
        }
    }
}

