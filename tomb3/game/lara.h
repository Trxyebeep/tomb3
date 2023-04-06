#pragma once
#include "../global/types.h"

void LaraAboveWater(ITEM_INFO* item, COLL_INFO* coll);
void lara_void_func(ITEM_INFO* item, COLL_INFO* coll);
void extra_void_func(ITEM_INFO* item, COLL_INFO* coll);
long LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll);
long TestLaraSlide(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestClimbStance(ITEM_INFO* item, COLL_INFO* coll);
long TestLaraVault(ITEM_INFO* item, COLL_INFO* coll);
void lara_slide_slope(ITEM_INFO* item, COLL_INFO* coll);
void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll);
long LaraHitCeiling(ITEM_INFO* item, COLL_INFO* coll);
long LaraDeflectEdge(ITEM_INFO* item, COLL_INFO* coll);
void LaraDeflectEdgeJump(ITEM_INFO* item, COLL_INFO* coll);
void LaraSlideEdgeJump(ITEM_INFO* item, COLL_INFO* coll);
long TestHangSwingIn(ITEM_INFO* item, short angle);
long LaraFallen(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_duck(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_duck(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_all4s(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_all4s(ITEM_INFO* item, COLL_INFO* coll);
short LaraFloorFront(ITEM_INFO* item, short ang, long dist);
short LaraCeilingFront(ITEM_INFO* item, short ang, long dist);
void lara_as_crawl(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_crawl(ITEM_INFO* item, COLL_INFO* coll);
long LaraDeflectEdgeDuck(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_all4turnl(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_all4turnl(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_all4turnr(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_crawlb(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_crawlb(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_crawl2hang(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestEdgeCatch(ITEM_INFO* item, COLL_INFO* coll, long* edge);
long LaraTestHangOnClimbWall(ITEM_INFO* item, COLL_INFO* coll);
long TestWall(ITEM_INFO* item, long front, long right, long down);
void LaraCollideStop(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_dash(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_dash(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_dashdive(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_dashdive(ITEM_INFO* item, COLL_INFO* coll);
void MonkeySwingFall(ITEM_INFO* item);
void MonkeySwingSnap(ITEM_INFO* item, COLL_INFO* coll);
short GetDirOctant(long rot);
short TestMonkeyLeft(ITEM_INFO* item, COLL_INFO* coll);
short TestMonkeyRight(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_hang2(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_hang2(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_monkeyswing(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_monkeyswing(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_monkeyl(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_monkeyl(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_monkeyr(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_monkeyr(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_hangturnl(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_hangturnr(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_hangturnlr(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_monkey180(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_monkey180(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_stop(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_stop(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_upjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_upjump(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestHangJumpUp(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_forwardjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_walk(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_run(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_fastback(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_turn_r(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_turn_l(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_death(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_fastfall(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_hang(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_reach(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_splat(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_compress(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_back(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_fastturn(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_stepright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_stepleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_slide(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_backjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_rightjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_leftjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_fallback(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_hangleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_hangright(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_slideback(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_pushblock(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_ppready(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_pickup(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_pickupflare(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_switchon(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_usekey(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_special(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_swandive(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_fastdive(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_gymnast(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_waterout(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_wade(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_deathslide(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_breath(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_sharkkill(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_airlock(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_gongbong(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_dinokill(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_startanim(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_trainkill(ITEM_INFO* item, COLL_INFO* coll);
void extra_as_rapidsdrown(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_walk(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_run(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_forwardjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_fastback(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_turn_r(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_turn_l(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_death(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_fastfall(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_hang(ITEM_INFO* item, COLL_INFO* coll);
long LaraTestHangJump(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_reach(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_splat(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_land(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_compress(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_back(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_stepright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_stepleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_slide(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_backjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_rightjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_leftjump(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_fallback(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_hangleft(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_hangright(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_slideback(ITEM_INFO* item, COLL_INFO* coll);
void lara_default_col(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_pushblock(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_roll(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_roll2(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_swandive(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_fastdive(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_wade(ITEM_INFO* item, COLL_INFO* coll);
void LaraHangTest(ITEM_INFO* item, COLL_INFO* coll);
void lara_as_duckroll(ITEM_INFO* item, COLL_INFO* coll);
void lara_col_duckroll(ITEM_INFO* item, COLL_INFO* coll);
void LookUpDown();
void LookLeftRight();
void ResetLook();
long GetStaticObjects(ITEM_INFO* item, short ang, long hite, long rad, long dist);
void ControlDeathSlide(short item_number);
void DeathSlideCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

enum lara_extras
{
	EXTRA_BREATH,
	EXTRA_PLUNGER,
	EXTRA_YETIKILL,
	EXTRA_SHARKKILL,
	EXTRA_AIRLOCK,
	EXTRA_GONGBONG,
	EXTRA_DINOKILL,
	EXTRA_PULLDAGGER,
	EXTRA_STARTANIM,
	EXTRA_STARTHOUSE,
	EXTRA_FINALANIM,
	EXTRA_TRAINKILL,
	EXTRA_RAPIDSDROWN
};

enum lara_states
{
	AS_WALK = 0,
	AS_RUN = 1,
	AS_STOP = 2,
	AS_FORWARDJUMP = 3,
	AS_POSE = 4,
	AS_FASTBACK = 5,
	AS_TURN_R = 6,
	AS_TURN_L = 7,
	AS_DEATH = 8,
	AS_FASTFALL = 9,
	AS_HANG = 10,
	AS_REACH = 11,
	AS_SPLAT = 12,
	AS_TREAD = 13,
	AS_LAND = 14,
	AS_COMPRESS = 15,
	AS_BACK = 16,
	AS_SWIM = 17,
	AS_GLIDE = 18,
	AS_NULL = 19,
	AS_FASTTURN = 20,
	AS_STEPRIGHT = 21,
	AS_STEPLEFT = 22,
	AS_HIT = 23,
	AS_SLIDE = 24,
	AS_BACKJUMP = 25,
	AS_RIGHTJUMP = 26,
	AS_LEFTJUMP = 27,
	AS_UPJUMP = 28,
	AS_FALLBACK = 29,
	AS_HANGLEFT = 30,
	AS_HANGRIGHT = 31,
	AS_SLIDEBACK = 32,
	AS_SURFTREAD = 33,
	AS_SURFSWIM = 34,
	AS_DIVE = 35,
	AS_PUSHBLOCK = 36,
	AS_PULLBLOCK = 37,
	AS_PPREADY = 38,
	AS_PICKUP = 39,
	AS_SWITCHON = 40,
	AS_SWITCHOFF = 41,
	AS_USEKEY = 42,
	AS_USEPUZZLE = 43,
	AS_UWDEATH = 44,
	AS_ROLL = 45,
	AS_SPECIAL = 46,
	AS_SURFBACK = 47,
	AS_SURFLEFT = 48,
	AS_SURFRIGHT = 49,
	AS_USEMIDAS = 50,
	AS_DIEMIDAS = 51,
	AS_SWANDIVE = 52,
	AS_FASTDIVE = 53,
	AS_GYMNAST = 54,
	AS_WATEROUT = 55,
	AS_CLIMBSTNC = 56,
	AS_CLIMBING = 57,
	AS_CLIMBLEFT = 58,
	AS_CLIMBEND = 59,
	AS_CLIMBRIGHT = 60,
	AS_CLIMBDOWN = 61,
	AS_LARATEST1 = 62,
	AS_LARATEST2 = 63,
	AS_LARATEST3 = 64,
	AS_WADE = 65,
	AS_WATERROLL = 66,
	AS_FLAREPICKUP = 67,
	AS_TWIST = 68,
	AS_KICK = 69,
	AS_DEATHSLIDE = 70,
	AS_DUCK = 71,
	AS_DUCKROLL = 72,
	AS_DASH = 73,
	AS_DASHDIVE = 74,
	AS_HANG2 = 75,
	AS_MONKEYSWING = 76,
	AS_MONKEYL = 77,
	AS_MONKEYR = 78,
	AS_MONKEY180 = 79,
	AS_ALL4S = 80,
	AS_CRAWL = 81,
	AS_HANGTURNL = 82,
	AS_HANGTURNR = 83,
	AS_ALL4TURNL = 84,
	AS_ALL4TURNR = 85,
	AS_CRAWLBACK = 86,
	AS_HANG2DUCK = 87,
	AS_CRAWL2HANG = 88,
	AS_LAST
};

enum lara_anims
{
	ANIM_RUN = 0,
	ANIM_WALK_STOP_LEFT = 2,
	ANIM_WALK_STOP_RIGHT = 3,
	ANIM_STARTRUN = 6,
	ANIM_STOP = 11,
	ANIM_FASTFALL = 23,
	ANIM_LANDFAR = 24,
	ANIM_STOPHANG = 28,
	ANIM_LAND = 31,
	ANIM_FASTSPLAT = 32,
	ANIM_FALLDOWN = 34,
	ANIM_VAULT3 = 42,
	ANIM_VAULT2 = 50,
	ANIM_HITWALLLEFT = 53,
	ANIM_HITWALLRIGHT = 54,
	ANIM_RUNSTEPUP_RIGHT = 55,
	ANIM_RUNSTEPUP_LEFT = 56,
	ANIM_WALKSTEPUP_LEFT = 57,
	ANIM_WALKSTEPUP_RIGHT = 58,
	ANIM_WALKSTEPD_RIGHT = 59,
	ANIM_WALKSTEPD_LEFT = 60,
	ANIM_BACKSTEPD_LEFT = 61,
	ANIM_BACKSTEPD_RIGHT = 62,
	ANIM_SWITCHOFF = 63,
	ANIM_SWITCHON = 64,
	ANIM_SLIDE = 70,
	ANIM_SWIMCHEAT = 87,
	ANIM_FALLBACK = 93,
	ANIM_GRABLOOP = 95,
	ANIM_GRABLEDGE = 96,
	ANIM_BREATH = 103,
	ANIM_SLIDEBACK = 104,
	ANIM_TREAD = 108,
	ANIM_SURFCLIMB = 111,
	ANIM_SURFTREAD = 114,
	ANIM_SURFDIVE = 119,
	ANIM_PPREADY = 120,
	ANIM_PULL = 122,
	ANIM_PUSH = 123,
	ANIM_UWDEATH = 124,
	ANIM_SPAZ_FORWARD = 125,
	ANIM_SPAZ_BACK = 126,
	ANIM_SPAZ_RIGHT = 127,
	ANIM_SPAZ_LEFT = 128,
	ANIM_PICKUP_UW = 130,
	ANIM_USEKEY = 131,
	ANIM_USEPUZZLE = 134,
	ANIM_PICKUP = 135,
	ANIM_RBALL_DEATH = 139,
	ANIM_ROLL = 146,
	ANIM_SPIKED = 149,
	ANIM_GRABLEDGEIN = 150,
	ANIM_CLIMBING = 161,
	ANIM_CLIMBSTNC = 164,
	ANIM_CLIMBDOWN = 168,
	ANIM_WADE = 177,
	ANIM_HANGUP = 187,
	ANIM_HANGDOWN = 188,
	ANIM_THROWFLARE = 189,
	ANIM_SURF2WADE1 = 190,
	ANIM_SURF2STND = 191,
	ANIM_SWIM2QSTND = 192,
	ANIM_SURF2QSTND = 193,
	ANIM_GENERATORSW_OFF = 195,
	ANIM_GENERATORSW_ON = 196,
	ANIM_ONEHANDPUSHSW = 197,
	ANIM_GRABRIGHT = 201,
	ANIM_GRABLEFT = 202,
	ANIM_WATERROLL = 203,
	ANIM_PICKUPF = 204,
	ANIM_PICKUPF_UW = 206,
	ANIM_DUCKBREATHE = 222,
	ANIM_UPJUMPGRAB = 233,
	ANIM_MONKEYHANG = 234,
	ANIM_DUCK_TO_ALL4S = 258,
	ANIM_ALL4S = 263,
	ANIM_ALL4S2 = 264,
	ANIM_ALL4TURNL = 269,
	ANIM_ALL4TURNR = 270,
	ANIM_DUCKPICKUP = 291,
	ANIM_SPAZ_DUCKB = 293,
	ANIM_SPAZ_DUCKF = 294,
	ANIM_SPAZ_DUCKR = 295,
	ANIM_SPAZ_DUCKL = 296,
	ANIM_CRAWL_TO_HANG = 302,
	ANIM_DUCKPICKUPF = 312
};

extern LARA_INFO lara;
extern ITEM_INFO* lara_item;
extern short DashTimer;
extern short ExposureMeter;
extern uchar LaraOnPad;
