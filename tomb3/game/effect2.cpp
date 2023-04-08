#include "../tomb3/pch.h"
#include "effect2.h"
#include "objects.h"
#include "items.h"
#include "../specific/game.h"
#include "control.h"
#include "effects.h"
#include "lara.h"
#include "../3dsystem/phd_math.h"
#include "sound.h"
#include "fish.h"
#include "gameflow.h"
#include "../newstuff/LaraDraw.h"

static short SplashRings[8][2] =
{
	{0, -24},
	{17, -17},
	{24, 0},
	{17, 17},
	{0, 24},
	{-17, 17},
	{-24, 0},
	{-17, -17}
};

SPARKS sparks[192];
static SP_DYNAMIC spark_dynamics[32];

BAT_STRUCT bats[32];
SPLASH_STRUCT splashes[4];
RIPPLE_STRUCT ripples[16];
SPLASH_SETUP splash_setup;

long SplashCount;
long KillEverythingFlag;
long SmokeCountL;
long SmokeCountR;
long SmokeWeapon;
long SmokeWindX;
long SmokeWindZ;

DYNAMIC dynamics[64];
long number_dynamics;

long wibble;

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	if (number_dynamics != 32)
	{
		dl = &dynamics[number_dynamics];
		dl->x = x;
		dl->y = y;
		dl->z = z;
		dl->falloff = ushort(falloff << 8);
		dl->on = 1;
		dl->r = (uchar)r;
		dl->g = (uchar)g;
		dl->b = (uchar)b;
		number_dynamics++;
	}
}

void ClearDynamics()
{
	number_dynamics = 0;

	for (int i = 0; i < 32; i++)
		dynamics[i].on = 0;
}

void KillEverything()
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short item_number, nex, obj_num;

	item_number = next_item_active;

	while (item_number != NO_ITEM)
	{
		item = &items[item_number];
		nex = item->next_active;
		obj_num = item->object_number;

		if (obj_num != LARA && obj_num != FLARE && (obj_num < DOOR_TYPE1 || obj_num > DOOR_TYPE8) && item->active && !(item->flags & IFL_REVERSE))
			KillItem(item_number);

		item_number = nex;
	}

	item_number = next_fx_active;

	while (item_number != NO_ITEM)
	{
		fx = &effects[item_number];
		nex = fx->next_active;
		obj_num = fx->object_number;

		if (objects[obj_num].control && (obj_num != FLAME || fx->counter >= 0))
			KillEffect(item_number);

		item_number = nex;
	}

	for (int i = 0; i < 8; i++)
		lead_info[i].on = 0;

	KillEverythingFlag = 0;
}

void TriggerBreath(long x, long y, long z, long xv, long yv, long zv)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 32;
	sptr->dG = 32;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 32;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 37;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Friction = 0;
	sptr->Xvel = (short)xv;
	sptr->Yvel = (short)yv;
	sptr->Zvel = (short)zv;

	if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->dWidth = (GetRandomControl() & 7) + 32;
	sptr->sWidth = sptr->dWidth >> 3;
	sptr->Width = sptr->sWidth;
	sptr->sHeight = sptr->sWidth;
	sptr->Height = sptr->sWidth;
	sptr->dHeight = sptr->dWidth;
}

void TriggerAlertLight(long x, long y, long z, long r, long g, long b, short angle, short rn)
{
	GAME_VECTOR s;
	GAME_VECTOR t;

	s.x = x;
	s.y = y;
	s.z = z;
	GetFloor(x, y, z, &rn);
	s.room_number = rn;
	t.x = x + (rcossin_tbl[angle << 1] << 1);
	t.y = y;
	t.z = z + (rcossin_tbl[(angle << 1) + 1] << 1);

	if (!LOS(&s, &t))
		TriggerDynamic(t.x, t.y, t.z, 8, r, g, b);
}

void TriggerFireFlame(long x, long y, long z, long body_part, long type)
{
	SPARKS* sptr;
	long dx, dz;
	uchar size;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (type == 2)
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 48;
		sptr->sG = sptr->sR;
		sptr->sB = (GetRandomControl() & 0x3F) - 64;
	}
	else if (type == 254)
	{
		sptr->sR = 48;
		sptr->sG = 255;
		sptr->sB = (GetRandomControl() & 0x1F) + 48;
		sptr->dR = 32;
		sptr->dG = (GetRandomControl() & 0x3F) - 64;
		sptr->dB = (GetRandomControl() & 0x3F) + 128;
	}
	else
	{
		sptr->sR = 255;
		sptr->sG = (GetRandomControl() & 0x1F) + 48;
		sptr->sB = 48;
	}

	if (type != 254)
	{
		sptr->dR = (GetRandomControl() & 0x3F) - 64;
		sptr->dB = 32;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
	}

	if (body_part == -1)
	{
		if (type == 2 || type == 255 || type == 254)
		{
			sptr->FadeToBlack = 6;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 5;
			sptr->Life = (type < 254 ? 0 : 8) + (GetRandomControl() & 3) + 16;
			sptr->sLife = sptr->Life;
		}
		else
		{
			sptr->FadeToBlack = 8;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 20;
			sptr->Life = (GetRandomControl() & 7) + 40;
			sptr->sLife = sptr->Life;
		}
	}
	else
	{
		sptr->FadeToBlack = 16;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 8;
		sptr->Life = (GetRandomControl() & 3) + 28;
		sptr->sLife = sptr->Life;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;

	if (body_part != -1)
	{
		sptr->x = (GetRandomControl() & 0x1F) - 16;
		sptr->y = 0;
		sptr->z = (GetRandomControl() & 0x1F) - 16;
	}
	else
	{
		if (!type || type == 1)
		{
			sptr->x = (GetRandomControl() & 0x1F) + x - 16;
			sptr->y = y;
			sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		}
		else if (type >= 254)
		{
			sptr->x = (GetRandomControl() & 0x3F) + x - 32;
			sptr->y = y;
			sptr->z = (GetRandomControl() & 0x3F) + z - 32;
		}
		else
		{
			sptr->x = (GetRandomControl() & 0xF) + x - 8;
			sptr->y = y;
			sptr->z = (GetRandomControl() & 0xF) + z - 8;
		}
	}

	if (type == 2)
	{
		sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Yvel = -1024 - (GetRandomControl() & 0x1FF);
		sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
		sptr->Friction = 68;
	}
	else
	{
		sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
		sptr->Yvel = -16 - (GetRandomControl() & 0xF);
		sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

		if (type == 1)
			sptr->Friction = 51;
		else
			sptr->Friction = 5;
	}

	if (GetRandomControl() & 1)
	{
		if (body_part == -1)
		{
			sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
			sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		}
		else
		{
			sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;
			sptr->FxObj = (uchar)body_part;
			sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
			sptr->MaxYvel = -24 - (GetRandomControl() & 7);
		}

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (body_part == -1)
	{
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}
	else
	{
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;
		sptr->FxObj = (uchar)body_part;
		sptr->Gravity = -32 - (GetRandomControl() & 0x3F);
		sptr->MaxYvel = -24 - (GetRandomControl() & 7);
	}

	sptr->Scalar = 2;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;

	if (!type)
		size = (GetRandomControl() & 0x1F) + 128;
	else if (type == 1)
		size = (GetRandomControl() & 0x1F) + 64;
	else if (type < 254)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		size = (GetRandomControl() & 0x1F) + 32;
	}
	else
		size = (GetRandomControl() & 0xF) + 48;

	sptr->sWidth = size;
	sptr->Width = size;
	sptr->sHeight = size;
	sptr->Height = size;

	if (type == 2)
	{
		sptr->dWidth = size >> 2;
		sptr->dHeight = size >> 2;
	}
	else
	{
		sptr->dWidth = size >> 4;
		sptr->dHeight = size >> 4;
	}
}

void TriggerFireSmoke(long x, long y, long z, long body_part, long type)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 32;
	sptr->dG = 32;
	sptr->dB = 32;

	if (body_part == -1)
	{
		if (type == 255)
		{
			sptr->FadeToBlack = 8;
			sptr->ColFadeSpeed = (GetRandomControl() & 3) + 16;
			sptr->Life = (GetRandomControl() & 7) + 28;
			sptr->sLife = sptr->Life;
		}
		else
		{
			sptr->FadeToBlack = 16;
			sptr->ColFadeSpeed = (GetRandomControl() & 7) + 32;
			sptr->Life = (GetRandomControl() & 0xF) + 57;
			sptr->sLife = sptr->Life;
		}
	}
	else
	{
		sptr->FadeToBlack = 12;
		sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
		sptr->Life = (GetRandomControl() & 3) + 20;
		sptr->sLife = sptr->Life;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = y - (GetRandomControl() & 0x7F) - 256;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -16 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->dWidth = (GetRandomControl() & 0x3F) + 64;
	sptr->sWidth = sptr->dWidth >> 2;
	sptr->Width = sptr->sWidth;
	sptr->sHeight = sptr->sWidth;
	sptr->Height = sptr->sWidth;
	sptr->dHeight = sptr->dWidth;
}

void TriggerStaticFlame(long x, long y, long z, long size)
{
	SPARKS* sptr;
	long dx, dz;
	uchar s;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = (GetRandomControl() & 0x3F) + 128;
	sptr->sB = 64;
	sptr->dR = sptr->sR;
	sptr->dG = sptr->sG;
	sptr->dB = 64;
	sptr->ColFadeSpeed = 1;
	sptr->FadeToBlack = 0;
	sptr->Life = 2;
	sptr->sLife = 2;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 7) + x - 4;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 7) + z - 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Friction = 0;
	sptr->Zvel = 0;
	sptr->Yvel = 0;
	sptr->Xvel = 0;
	sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 2;
	s = (uchar)size;
	sptr->sWidth = s;
	sptr->Width = s;
	sptr->dWidth = s;
	sptr->sHeight = s;
	sptr->Height = s;
	sptr->dHeight = s;
}

void TriggerSideFlame(long x, long y, long z, long angle, long speed, long pilot)
{
	SPARKS* sptr;
	long dx, dz, rad, s, c;
	uchar size;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = (GetRandomControl() & 0x3F) - 64;
	sptr->dG = (GetRandomControl() & 0x3F) + 0x80;
	sptr->dB = 32;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 28;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (pilot)
		rad = (speed << 7) + (GetRandomControl() & 0x1F);
	else
		rad = (speed << 8) + (GetRandomControl() & 0x1FF);

	s = (rad * rcossin_tbl[angle]) >> (W2V_SHIFT - 3);
	c = (rad * rcossin_tbl[angle + 1]) >> (W2V_SHIFT - 3);
	sptr->Xvel = short((GetRandomControl() & 0x7F) + s - 64);
	sptr->Yvel = -6 - (GetRandomControl() & 7);
	sptr->Zvel = short((GetRandomControl() & 0x7F) + c - 64);
	sptr->Friction = 4;
	sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	sptr->Gravity = -8 - (GetRandomControl() & 0xF);
	sptr->MaxYvel = -8 - (GetRandomControl() & 7);
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	size = (GetRandomControl() & 0x1F) + 128;

	if (pilot)
		size >>= 2;

	sptr->dWidth = size;
	sptr->sWidth = size >> 1;
	sptr->Width = size >> 1;
	sptr->sHeight = size >> 1;
	sptr->Height = size >> 1;
	sptr->dHeight = size;
}

void TriggerRocketSmoke(long x, long y, long z, long c)
{
	SPARKS* sptr;
	uchar size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = uchar(c + 64);
	sptr->dG = uchar(c + 64);
	sptr->dB = uchar(c + 64);
	sptr->FadeToBlack = 12;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -4 - (GetRandomControl() & 3);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -4 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	size = (GetRandomControl() & 7) + 32;
	sptr->dWidth = size;
	sptr->sWidth = size >> 2;
	sptr->Width = size >> 2;
	sptr->sHeight = size >> 2;
	sptr->Height = size >> 2;
	sptr->dHeight = size;
}

void TriggerRicochetSpark(GAME_VECTOR* pos, long angle, long size)
{
	SPARKS* sptr;
	long ang;
	uchar c;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 32;
	sptr->sB = 0;
	sptr->dR = 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 96;
	sptr->dB = 0;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->Life = 24;
	sptr->sLife = 24;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->x = pos->x;
	sptr->y = pos->y;
	sptr->z = pos->z;
	ang = ((GetRandomControl() & 0x7FF) + angle - 1024) & 0xFFF;
	sptr->Xvel = -rcossin_tbl[ang << 1] >> 1;
	sptr->Yvel = 2 * (GetRandomControl() & 0x1FF) - 768;
	sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 1;
	sptr->Friction = 1;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 3;
	sptr->Gravity = short(abs(sptr->Yvel >> 6) + (GetRandomControl() & 0x1F));
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomControl() & 1) + 1;
	sptr->Height = (GetRandomControl() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomControl() & 1) + 1;
	sptr->MaxYvel = 0;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	c = uchar((GetRandomControl() & 0x3F) + 128);
	sptr->sR = c;
	sptr->sG = c;
	sptr->sB = c;
	c >>= 1;
	sptr->dR = c;
	sptr->dG = c;
	sptr->dB = c;
	sptr->TransType = 3;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 16;
	sptr->Life = 28;
	sptr->sLife = 28;
	sptr->Dynamic = -1;
	sptr->x = pos->x;
	sptr->y = pos->y;
	sptr->z = pos->z;
	ang = ((GetRandomControl() & 0x7FF) + angle - 1023) & 0xFFF;
	sptr->Xvel = -rcossin_tbl[ang << 1] >> 1;
	sptr->Yvel = (GetRandomControl() & 0x1FF) - 384;
	sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 1;
	sptr->Friction = 33;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 3;
	sptr->Gravity = (GetRandomControl() & 7) + 4;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomControl() & 1) + 1;
	sptr->Height = (GetRandomControl() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomControl() & 1) + 1;
	sptr->MaxYvel = 0;
}

void TriggerBlood(long x, long y, long z, long angle, long num)
{
	SPARKS* sptr;
	long ang;
	short rad;

	for (int i = 0; i < num; i++)
	{
		sptr = &sparks[GetFreeSpark()];
		sptr->On = 1;

		if (gameflow.language == 2)
		{
			sptr->sR = 112;
			sptr->sG = 0;
			sptr->sB = 224;
			sptr->dR = 96;
			sptr->dG = 0;
			sptr->dB = 192;
		}
		else
		{
			sptr->sR = 224;
			sptr->sG = 0;
			sptr->sB = 32;
			sptr->dR = 192;
			sptr->dG = 0;
			sptr->dB = 24;
		}

		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 1;
		sptr->Dynamic = -1;
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
		rad = GetRandomControl() & 0xF;
		ang = ((GetRandomControl() & 0x1F) + angle - 16) & 0xFFF;
		sptr->Xvel = -(rad * rcossin_tbl[ang << 1]) >> 5;
		sptr->Yvel = -128 - (GetRandomControl() & 0xFF);
		sptr->Zvel = (rad * rcossin_tbl[(ang << 1) + 1]) >> 5;
		sptr->Friction = 4;
		sptr->Flags = SF_BLOOD | SF_SCALE;
		sptr->Scalar = 3;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 31;
		sptr->Width = 2;
		sptr->sWidth = 2;
		sptr->Height = 2;
		sptr->sHeight = 2;
		sptr->dWidth = 2 - (GetRandomControl() & 1);
		sptr->dHeight = 2 - (GetRandomControl() & 1);
	}
}

void TriggerBloodD(long x, long y, long z, long angle, long num)
{
	SPARKS* sptr;
	long ang;
	short rad;

	for (int i = 0; i < num; i++)
	{
		sptr = &sparks[GetFreeSpark()];
		sptr->On = 1;

		if (gameflow.language == 2)
		{
			sptr->sR = 112;
			sptr->sG = 0;
			sptr->sB = 224;
			sptr->dR = 96;
			sptr->dG = 0;
			sptr->dB = 192;
		}
		else
		{
			sptr->sR = 224;
			sptr->sG = 0;
			sptr->sB = 32;
			sptr->dR = 192;
			sptr->dG = 0;
			sptr->dB = 24;
		}

		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 8;
		sptr->Life = 24;
		sptr->sLife = 24;
		sptr->TransType = 1;
		sptr->Dynamic = -1;
		sptr->x = (GetRandomDraw() & 0x1F) + x - 16;
		sptr->y = (GetRandomDraw() & 0x1F) + y - 16;
		sptr->z = (GetRandomDraw() & 0x1F) + z - 16;
		rad = GetRandomDraw() & 0xF;
		ang = ((GetRandomDraw() & 0x1F) + angle - 16) & 0xFFF;
		sptr->Xvel = -(rad * rcossin_tbl[ang << 1]) >> 5;
		sptr->Yvel = -128 - (GetRandomDraw() & 0xFF);
		sptr->Zvel = (rad * rcossin_tbl[(ang << 1) + 1]) >> 5;
		sptr->Friction = 4;
		sptr->Flags = SF_NONE;
		sptr->Scalar = 3;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomDraw() & 0x1F) + 31;
		sptr->Width = 2;
		sptr->sWidth = 2;
		sptr->Height = 2;
		sptr->sHeight = 2;
		sptr->dWidth = 2 - (GetRandomDraw() & 1);
		sptr->dHeight = 2 - (GetRandomDraw() & 1);
	}
}

void TriggerUnderwaterBlood(long x, long y, long z, long size)
{
	RIPPLE_STRUCT* ripple;
	long n;

	ripple = ripples;
	n = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		n++;

		if (n >= 16)
			return;
	}

	ripple->flags = 51;
	ripple->init = 1;
	ripple->life = (GetRandomControl() & 7) - 16;
	ripple->size = (uchar)size;
	ripple->x = (GetRandomControl() & 0x3F) + x - 32;
	ripple->y = y;
	ripple->z = (GetRandomControl() & 0x3F) + z - 32;
}

void TriggerUnderwaterBloodD(long x, long y, long z, long size)
{
	RIPPLE_STRUCT* ripple;
	long n;

	ripple = ripples;
	n = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		n++;

		if (n >= 16)
			return;
	}

	ripple->flags = 51;
	ripple->init = 1;
	ripple->life = (GetRandomDraw() & 7) - 16;
	ripple->size = (uchar)size;
	ripple->x = (GetRandomDraw() & 0x3F) + x - 32;
	ripple->y = y;
	ripple->z = (GetRandomDraw() & 0x3F) + z - 32;
}

void TriggerFlareSparks(long x, long y, long z, long xv, long yv, long zv, long smoke, long unused)
{
	SPARKS* sptr;
	SPARKS* smokeSpark;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 255;
	sptr->dR = 255;
	sptr->dG = (GetRandomDraw() & 0x7F) + 64;
	sptr->dB = 192 - sptr->dG;
	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 5;
	sptr->Life = 10;
	sptr->sLife = 10;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomDraw() & 7) + x - 3;
	sptr->y = (GetRandomDraw() & 7) + y - 3;
	sptr->z = (GetRandomDraw() & 7) + z - 3;
	sptr->Xvel = short((GetRandomDraw() & 0xFF) + xv - 128);
	sptr->Yvel = short((GetRandomDraw() & 0xFF) + yv - 128);
	sptr->Zvel = short((GetRandomDraw() & 0xFF) + zv - 128);
	sptr->Friction = 34;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomDraw() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = (GetRandomDraw() & 1) + 1;
	sptr->Height = (GetRandomDraw() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = (GetRandomDraw() & 1) + 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Flags = SF_SCALE;

	if (smoke)
	{
		smokeSpark = &sparks[GetFreeSpark()];
		smokeSpark->On = 1;
		smokeSpark->sR = sptr->dR >> 1;
		smokeSpark->sG = sptr->dG >> 1;
		smokeSpark->sB = sptr->dB >> 1;
		smokeSpark->dR = 32;
		smokeSpark->dG = 32;
		smokeSpark->dB = 32;
		smokeSpark->ColFadeSpeed = (GetRandomDraw() & 3) + 8;
		smokeSpark->FadeToBlack = 4;
		smokeSpark->TransType = 2;
		smokeSpark->Life = (GetRandomDraw() & 7) + 13;
		smokeSpark->sLife = smokeSpark->Life;
		smokeSpark->x = x + (xv >> 5);
		smokeSpark->y = y + (yv >> 5);
		smokeSpark->z = z + (zv >> 5);
		smokeSpark->extras = 0;
		smokeSpark->Dynamic = -1;
		smokeSpark->Xvel = short((GetRandomDraw() & 0x3F) + xv - 32);
		smokeSpark->Yvel = (short)yv;
		smokeSpark->Zvel = short((GetRandomDraw() & 0x3F) + zv - 32);
		smokeSpark->Friction = 4;

		if (GetRandomDraw() & 1)
		{
			smokeSpark->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
			smokeSpark->RotAng = GetRandomDraw() & 0xFFF;

			if (GetRandomDraw() & 1)
				smokeSpark->RotAdd = -16 - (GetRandomDraw() & 0xF);
			else
				smokeSpark->RotAdd = (GetRandomDraw() & 0xF) + 16;
		}
		else
			smokeSpark->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

		smokeSpark->Def = (uchar)objects[EXPLOSION1].mesh_index;
		smokeSpark->Scalar = 2;
		smokeSpark->Gravity = -8 - (GetRandomDraw() & 3);
		smokeSpark->MaxYvel = -4 - (GetRandomDraw() & 3);
		smokeSpark->dWidth = (GetRandomDraw() & 0xF) + 24;
		smokeSpark->sWidth = smokeSpark->dWidth >> 3;
		smokeSpark->Width = smokeSpark->dWidth >> 3;
		smokeSpark->dHeight = smokeSpark->dWidth;
		smokeSpark->sHeight = smokeSpark->dHeight >> 3;
		smokeSpark->Height = smokeSpark->dHeight >> 3;
	}
}

void TriggerGunSmoke(long x, long y, long z, long xv, long yv, long zv, long initial, long weapon, long shade)
{
	SPARKS* sptr;
	uchar size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = uchar(shade << 2);
	sptr->dG = uchar(shade << 2);
	sptr->dB = uchar(shade << 2);
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = uchar(32 - (initial << 4));
	sptr->Life = (GetRandomControl() & 3) + 40;
	sptr->sLife = sptr->Life;

	if ((weapon == LG_PISTOLS || weapon == LG_MAGNUMS || weapon == LG_UZIS) && sptr->dR > 64)
	{
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (initial)
	{
		sptr->Xvel = short((GetRandomControl() & 0x3FF) + xv - 512);
		sptr->Yvel = short((GetRandomControl() & 0x3FF) + yv - 512);
		sptr->Zvel = short((GetRandomControl() & 0x3FF) + zv - 512);
	}
	else
	{
		sptr->Xvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Yvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
		sptr->Zvel = ((GetRandomControl() & 0x1FF) - 256) >> 1;
	}

	sptr->Friction = 4;

	if (GetRandomControl() & 1)
	{
		if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
			sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (room[lara_item->room_number].flags & ROOM_NOT_INSIDE)
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;
	sptr->Gravity = -2 - (GetRandomControl() & 1);
	sptr->MaxYvel = -2 - (GetRandomControl() & 1);

	size = (GetRandomControl() & 7) - ((weapon == LG_ROCKET || weapon == LG_GRENADE) ? 0 : 12) + 24;

	if (initial)
	{
		sptr->Width = size >> 1;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = (size + 4) << 1;
	}
	else
	{
		sptr->Width = size >> 2;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = size;
	}

	if (initial)
	{
		sptr->Height = size >> 1;
		sptr->sHeight = sptr->Width;
		sptr->dHeight = (size + 4) << 1;
	}
	else
	{
		sptr->Height = size >> 2;
		sptr->sHeight = sptr->Width;
		sptr->dHeight = size;
	}
}

void TriggerExplosionSparks(long x, long y, long z, long extras, long dynamic, long uw, short room_number)
{
	SPARKS* sptr;
	SP_DYNAMIC* pDL;
	long dx, dz, i;
	uchar extras_table[4];
	uchar r, g, b;

	extras_table[0] = 0;
	extras_table[1] = 4;
	extras_table[2] = 7;
	extras_table[3] = 10;
	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;

	if (uw == 1)
	{
		sptr->sG = (GetRandomControl() & 0x3F) + 128;
		sptr->sB = 32;
		sptr->dR = 192;
		sptr->dG = (GetRandomControl() & 0x1F) + 64;
		sptr->dB = 0;
	}
	else
	{
		sptr->sG = (GetRandomControl() & 0xF) + 32;
		sptr->sB = 0;
		sptr->dR = (GetRandomControl() & 0x3F) + 192;
		sptr->dG = (GetRandomControl() & 0x3F) + 128;
		sptr->dB = 32;
	}

	if (uw == 1)
	{
		sptr->ColFadeSpeed = 7;
		sptr->FadeToBlack = 8;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 7) + 16;
		sptr->sLife = sptr->Life;
		sptr->RoomNumber = (uchar)room_number;
	}
	else
	{
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 16;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 7) + 24;
		sptr->sLife = sptr->Life;
	}

	sptr->Dynamic = (char)dynamic;
	sptr->extras = uchar(extras | ((extras_table[extras] + (GetRandomControl() & 7) - 4) << 3));

	if (dynamic == -2)
	{
		for (i = 0; i < 32; i++)
		{
			pDL = &spark_dynamics[i];

			if (!pDL->On)
			{
				pDL->On = 1;
				pDL->Falloff = 4;

				if (uw == 1)
					pDL->Flags = 2;
				else
					pDL->Flags = 1;

				sptr->Dynamic = (char)i;
				break;
			}
		}

		if (i == 32)
			sptr->Dynamic = -1;
	}

	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = (GetRandomControl() & 0xFFF) - 2048;
	sptr->Yvel = (GetRandomControl() & 0xFFF) - 2048;
	sptr->Zvel = (GetRandomControl() & 0xFFF) - 2048;

	if (dynamic != -2 || uw == 1)
	{
		sptr->x = (GetRandomControl() & 0x1F) + x - 16;
		sptr->y = (GetRandomControl() & 0x1F) + y - 16;
		sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	}
	else
	{
		sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
		sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
		sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	}

	if (uw == 1)
		sptr->Friction = 17;
	else
		sptr->Friction = 51;

	if (GetRandomControl() & 1)
	{
		if (uw == 1)
			sptr->Flags = SF_UNWATER | SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;
		sptr->RotAdd = (GetRandomControl() & 0xFF) + 128;
	}
	else if (uw == 1)
		sptr->Flags = SF_UNWATER | SF_ALTDEF | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Width = (GetRandomControl() & 0xF) + 40;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width << 1;
	sptr->Height = sptr->Width + (GetRandomControl() & 7) + 8;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height << 1;

	if (uw == 2)
	{
		r = sptr->sR;
		g = sptr->sG;
		b = sptr->sB;
		sptr->sR = b;
		sptr->sG = r;
		sptr->sB = g;
		r = sptr->dR;
		g = sptr->dG;
		b = sptr->dB;
		sptr->dR = b;
		sptr->dG = r;
		sptr->dB = g;
		sptr->Flags |= SF_GREEN;
	}
	else if (extras)
		TriggerExplosionSmoke(x, y, z, uw);
	else
		TriggerExplosionSmokeEnd(x, y, z, uw);
}

void TriggerExplosionSmoke(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 144;
	sptr->sG = 144;
	sptr->sB = 144;
	sptr->dR = 64;
	sptr->dG = 64;
	sptr->dB = 64;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 8;
	sptr->TransType = 3;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 10;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1FF) + x - 256;
	sptr->y = (GetRandomControl() & 0x1FF) + y - 256;
	sptr->z = (GetRandomControl() & 0x1FF) + z - 256;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
		sptr->Friction = 2;
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Gravity = -3 - (GetRandomControl() & 3);
	sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 3;
	sptr->sHeight = sptr->Height;
}

void TriggerExplosionSmokeEnd(long x, long y, long z, long uw)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;

	if (uw)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 208;
	}
	else
	{
		sptr->sR = 144;
		sptr->sG = 144;
		sptr->sB = 144;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 64;
	sptr->Life = (GetRandomControl() & 0x1F) + 96;
	sptr->sLife = sptr->Life;

	if (uw)
		sptr->TransType = 2;
	else
		sptr->TransType = 3;

	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = ((GetRandomControl() & 0xFFF) - 2048) >> 2;

	if (uw)
	{
		sptr->Friction = 20;
		sptr->Yvel >>= 4;
		sptr->y += 32;
	}
	else
		sptr->Friction = 6;

	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;
	if ((GetRandomControl() & 1) != 0)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 3;

	if (uw)
	{
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else
	{
		sptr->Gravity = -3 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}

	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 3;
	sptr->sHeight = sptr->Height;
}

void TriggerShotgunSparks(long x, long y, long z, long xv, long yv, long zv)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = 255;
	sptr->sB = 0;
	sptr->dR = 255;
	sptr->dG = (GetRandomControl() & 0x7F) + 64;
	sptr->dB = 0;
	sptr->ColFadeSpeed = 3;
	sptr->FadeToBlack = 5;
	sptr->Life = 10;
	sptr->sLife = 10;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 7) + x - 3;
	sptr->y = (GetRandomControl() & 7) + y - 3;
	sptr->z = (GetRandomControl() & 7) + z - 3;
	sptr->Xvel = short((GetRandomControl() & 0x1FF) + xv - 256);
	sptr->Yvel = short((GetRandomControl() & 0x1FF) + yv - 256);
	sptr->Zvel = short((GetRandomControl() & 0x1FF) + zv - 256);
	sptr->Friction = 0;
	sptr->Flags = SF_SCALE;
	sptr->Scalar = 2;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = 1;
	sptr->Height = (GetRandomControl() & 3) + 4;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = 1;
}

void TriggerRocketFlame(long x, long y, long z, long xv, long yv, long zv, long itemNum)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x1F) + 48;
	sptr->sG = sptr->sR;
	sptr->sB = (GetRandomControl() & 0x3F) + 192;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;
	sptr->FadeToBlack = 12;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 3) + 28;
	sptr->sLife = sptr->Life;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = (short)xv;
	sptr->Yvel = (short)yv;
	sptr->Zvel = (short)zv;
	sptr->Friction = 51;
	sptr->FxObj = (uchar)itemNum;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;

	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Scalar = 2;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Width = (GetRandomControl() & 7) + 32;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = 2;
}

void TriggerWaterfallMist(long x, long y, long z, long ang)
{
	SPARKS* sptr;
	long offsets[4];
	long ang2, rad;

	offsets[0] = 576;
	offsets[1] = 203;
	offsets[2] = -203;
	offsets[3] = -576;
	ang2 = (ang + 1024) & 0xFFF;

	for (int i = 0; i < 4; i++)
	{
		sptr = &sparks[GetFreeSpark()];
		sptr->On = 1;
		sptr->sR = 128;
		sptr->sG = 128;
		sptr->sB = 128;
		sptr->dR = 192;
		sptr->dG = 192;
		sptr->dB = 192;
		sptr->ColFadeSpeed = 2;
		sptr->FadeToBlack = 4;
		sptr->TransType = 2;
		sptr->Life = (GetRandomControl() & 3) + 6;
		sptr->sLife = sptr->Life;
		sptr->Dynamic = -1;
		rad = (GetRandomControl() & 0x1F) + offsets[i] - 16;
		sptr->x = ((rad * rcossin_tbl[ang2 << 1]) >> 12) + (GetRandomControl() & 0xF) + x - 8;
		sptr->y = (GetRandomControl() & 0xF) + y - 8;
		sptr->z = ((rad * rcossin_tbl[(ang2 << 1) + 1]) >> 12) + (GetRandomControl() & 0xF) + z - 8;
		sptr->Xvel = rcossin_tbl[ang << 1] >> 12;
		sptr->Yvel = 0;
		sptr->Zvel = rcossin_tbl[(ang << 1) + 1] >> 12;
		sptr->Friction = 3;

		if (GetRandomControl() & 1)
		{
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
			sptr->RotAng = GetRandomControl() & 0xFFF;

			if (GetRandomControl() & 1)
				sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
			else
				sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
		}
		else
			sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

		sptr->Scalar = 6;
		sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
		sptr->Gravity = 0;
		sptr->MaxYvel = 0;
		sptr->dWidth = (GetRandomControl() & 7) + 12;
		sptr->Width = sptr->dWidth >> 1;
		sptr->sWidth = sptr->Width;
		sptr->dHeight = sptr->dWidth;
		sptr->Height = sptr->dHeight >> 1;
		sptr->sHeight = sptr->Height;
	}
}

void TriggerDartSmoke(long x, long y, long z, long xv, long zv, long hit)
{
	SPARKS* sptr;
	long dx, dz, rnd;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 16;
	sptr->sG = 8;
	sptr->sB = 4;
	sptr->dR = 64;
	sptr->dG = 48;
	sptr->dB = 32;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 4;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 32;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = (GetRandomControl() & 0x1F) + y - 16;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;

	if (hit)
	{
		sptr->Xvel = short((GetRandomControl() & 0xFF) - xv - 128);
		sptr->Yvel = -4 - (GetRandomControl() & 3);
		sptr->Zvel = short((GetRandomControl() & 0xFF) - zv - 128);
	}
	else
	{
		if (xv)
			sptr->Xvel = (short)-xv;
		else
			sptr->Xvel = (GetRandomControl() & 0xFF) - 128;

		sptr->Yvel = -4 - (GetRandomControl() & 3);

		if (zv)
			sptr->Zvel = (short)-zv;
		else
			sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	}

	sptr->Friction = 3;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	rnd = (GetRandomControl() & 0x3F) + 72;

	if (hit)
	{
		rnd >>= 1;
		sptr->dWidth = (uchar)rnd;
		sptr->Width = sptr->dWidth >> 2;
		sptr->sWidth = sptr->Width;
		sptr->dHeight = (uchar)rnd;
		sptr->Height = sptr->dHeight >> 2;
		sptr->sHeight = sptr->Height;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
	}
	else
	{
		sptr->dWidth = (uchar)rnd;
		sptr->Width = sptr->dWidth >> 4;
		sptr->sWidth = sptr->Width;
		sptr->dHeight = (uchar)rnd;
		sptr->Height = sptr->dHeight >> 4;
		sptr->sHeight = sptr->Height;
		sptr->Gravity = -4 - (GetRandomControl() & 3);
		sptr->MaxYvel = -4 - (GetRandomControl() & 3);
	}
}

void TriggerExplosionBubble(long x, long y, long z, short room_number)
{
	SPARKS* sptr;
	PHD_3DPOS pos;
	long dx, dz;
	uchar size;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 128;
	sptr->sG = 64;
	sptr->sB = 0;
	sptr->dR = 128;
	sptr->dG = 128;
	sptr->dB = 128;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 12;
	sptr->Life = 24;
	sptr->sLife = 24;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 0;
	sptr->Flags = SF_UNWATER | SF_DEF | SF_SCALE;
	sptr->Scalar = 3;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	size = (GetRandomControl() & 7) + 63;
	sptr->Width = size >> 1;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = size << 1;
	sptr->Height = size >> 1;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = size << 1;

	for (int i = 0; i < 7; i++)
	{
		pos.x_pos = (GetRandomControl() & 0x1FF) + x - 256;
		pos.y_pos = (GetRandomControl() & 0x7F) + y - 64;
		pos.z_pos = (GetRandomControl() & 0x1FF) + z - 256;
		CreateBubble(&pos, room_number, 6, 15);
	}
}

void TriggerBubble(long x, long y, long z, long size, long sizerange, short fxNum)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - x;
	dz = lara_item->pos.z_pos - z;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 144;
	sptr->dG = 144;
	sptr->dB = 144;
	sptr->FadeToBlack = 2;
	sptr->TransType = 2;
	sptr->ColFadeSpeed = 4;
	sptr->Life = 128;
	sptr->sLife = 128;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = 0;
	sptr->y = 0;
	sptr->z = 0;
	sptr->Xvel = 0;
	sptr->Yvel = 0;
	sptr->Zvel = 0;
	sptr->Friction = 0;
	sptr->Flags = SF_ATTACHEDPOS | SF_FX | SF_DEF | SF_SCALE;
	sptr->FxObj = (uchar)fxNum;
	sptr->Def = (uchar)objects[BUBBLES1].mesh_index;
	sptr->Scalar = 0;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->Width = uchar(GetRandomControl() % sizerange + size);
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width << 3;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height << 3;
}

void ControlSmokeEmitter(short item_number)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long dx, dz;

	item = &items[item_number];

	if (!TriggerActive(item) || wibble & 0xC || (item->object_number == STEAM_EMITTER && wibble & 0x1F))
		return;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;
	sptr->dR = 32;
	sptr->dG = 32;
	sptr->dB = 32;
	sptr->FadeToBlack = 64;
	sptr->ColFadeSpeed = (GetRandomControl() & 7) + 16;
	sptr->Life = (GetRandomControl() & 0xF) + 96;
	sptr->sLife = sptr->Life;

	if (item->object_number == SMOKE_EMITTER_BLACK)
		sptr->TransType = 3;
	else
		sptr->TransType = 2;

	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + item->pos.x_pos - 8;
	sptr->y = (GetRandomControl() & 0xF) + item->pos.y_pos - 8;
	sptr->z = (GetRandomControl() & 0xF) + item->pos.z_pos - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 4;
	
	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -4 - (GetRandomControl() & 7);
		else
			sptr->RotAdd = (GetRandomControl() & 7) + 4;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_OUTSIDE | SF_DEF | SF_SCALE;

	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = -8 - (GetRandomControl() & 7);
	sptr->MaxYvel = -4 - (GetRandomControl() & 7);
	sptr->dWidth = (GetRandomControl() & 0x1F) + 128;
	sptr->Width = sptr->dWidth >> 2;
	sptr->sWidth = sptr->Width;
	sptr->dHeight = sptr->dWidth + (GetRandomControl() & 0x1F) + 32;
	sptr->Height = sptr->dHeight >> 2;
	sptr->sHeight = sptr->Height >> 2;

	if (item->object_number == STEAM_EMITTER)
	{
		sptr->Gravity >>= 1;
		sptr->Yvel >>= 1;
		sptr->MaxYvel >>= 1;
		sptr->dR = 24;
		sptr->dG = 24;
		sptr->dB = 24;
	}
}

void DetatchSpark(long num, long type)
{
	SPARKS* sptr;
	FX_INFO* fx;
	ITEM_INFO* item;

	for (int i = 0; i < 192; i++)
	{
		sptr = &sparks[i];

		if (sptr->On && sptr->Flags & type && sptr->FxObj == num)
		{
			if (type == SF_FX)
			{
				if (sptr->Flags & SF_ATTACHEDPOS)
					sptr->On = 0;
				else
				{
					fx = &effects[num];
					sptr->x += fx->pos.x_pos;
					sptr->y += fx->pos.y_pos;
					sptr->z += fx->pos.z_pos;
					sptr->Flags &= ~SF_FX;
				}
			}
			else if (type == SF_ITEM)
			{
				if (sptr->Flags & SF_ATTACHEDPOS)
					sptr->On = 0;
				else
				{
					item = &items[num];
					sptr->x += item->pos.x_pos;
					sptr->y += item->pos.y_pos;
					sptr->z += item->pos.z_pos;
					sptr->Flags &= ~SF_ITEM;
				}
			}
		}
	}
}

long GetFreeSpark()
{
	SPARKS* sptr;
	static long next_spark;
	long free, min_life;

	free = next_spark;
	sptr = &sparks[next_spark];

	for (int i = 0; i < 192; i++)
	{
		if (sptr->On)
		{
			if (free == 191)
			{
				sptr = &sparks[0];
				free = 0;
			}
			else
			{
				free++;
				sptr++;
			}
		}
		else
		{
			next_spark = (free + 1) & 0xBF;
			return free;
		}
	}

	free = 0;
	min_life = 4095;

	for (int i = 0; i < 192; i++)
	{
		sptr = &sparks[i];

		if (sptr->Life < min_life && sptr->Dynamic == -1 && (!(sptr->Flags & SF_BLOOD) || i & 1))
		{
			free = i;
			min_life = sptr->Life;
		}
	}

	next_spark = (free + 1) & 0xBF;
	return free;
}

void InitialiseSparks()
{
	for (int i = 0; i < 192; i++)
	{
		sparks[i].On = 0;
		sparks[i].Dynamic = -1;
	}

	for (int i = 0; i < 4; i++)
		splashes[i].flags = 0;

	for (int i = 0; i < 16; i++)
		ripples[i].flags = 0;

	for (int i = 0; i < 32; i++)
		bats[i].flags = 0;
}

void UpdateSparks()
{
	SPARKS* sptr;
	SP_DYNAMIC* pDL;
	long fade, uw, rnd, x, y, z, falloff, r, g, b;
	uchar def;

	def = (uchar)objects[EXPLOSION1].mesh_index;

	for (int i = 0; i < 192; i++)
	{
		sptr = &sparks[i];

		if (!sptr->On)
			continue;

		if (!(sptr->Flags & SF_ATTACHEDPOS) || sptr->Life > 16)
			sptr->Life--;

		if (!sptr->Life)
		{
			if (sptr->Dynamic != -1)
			{
				spark_dynamics[sptr->Dynamic].On = 0;
				sptr->Dynamic = -1;
			}

			sptr->On = 0;
			continue;
		}

		if (sptr->sLife - sptr->Life < sptr->ColFadeSpeed)
		{
			fade = ((sptr->sLife - sptr->Life) << 16) / sptr->ColFadeSpeed;
			sptr->R = uchar(sptr->sR + ((fade * (sptr->dR - sptr->sR)) >> 16));
			sptr->G = uchar(sptr->sG + ((fade * (sptr->dG - sptr->sG)) >> 16));
			sptr->B = uchar(sptr->sB + ((fade * (sptr->dB - sptr->sB)) >> 16));
		}
		else if (sptr->Life < sptr->FadeToBlack)
		{
			fade = ((sptr->Life - sptr->FadeToBlack) << 16) / sptr->FadeToBlack + 0x10000;
			sptr->R = uchar((sptr->dR * fade) >> 16);
			sptr->G = uchar((sptr->dG * fade) >> 16);
			sptr->B = uchar((sptr->dB * fade) >> 16);
		}
		else
		{
			sptr->R = sptr->dR;
			sptr->G = sptr->dG;
			sptr->B = sptr->dB;
		}

		if (sptr->Life == sptr->FadeToBlack && sptr->Flags & SF_UNWATER)
		{
			sptr->dWidth >>= 2;
			sptr->dHeight >>= 2;
		}

		if (sptr->Flags & SF_ROTATE)
			sptr->RotAng = (sptr->RotAng + sptr->RotAdd) & 0xFFF;

		if (sptr->Flags & SF_ALTDEF)
		{
			if (sptr->R < 16 && sptr->G < 16 && sptr->B < 16)
				sptr->Def = def + 3;
			else if (sptr->R < 64 && sptr->G < 64 && sptr->B < 64)
				sptr->Def = def + 2;
			else if (sptr->R < 96 && sptr->G < 96 && sptr->B < 96)
				sptr->Def = def + 1;
			else
				sptr->Def = def;
		}

		if (sptr->sLife - sptr->Life == sptr->extras >> 3 && sptr->extras & 7)
		{
			if (sptr->Flags & SF_UNWATER)
				uw = 1;
			else if (sptr->Flags & SF_GREEN)
				uw = 2;
			else
				uw = 0;

			for (int j = 0; j < (sptr->extras & 7); j++)
			{
				TriggerExplosionSparks(sptr->x, sptr->y, sptr->z, (sptr->extras & 7) - 1, sptr->Dynamic, uw, sptr->RoomNumber);
				sptr->Dynamic = -1;
			}

			if (sptr->Flags & SF_UNWATER)
				TriggerExplosionBubble(sptr->x, sptr->y, sptr->z, sptr->RoomNumber);

			sptr->extras = 0;
		}

		fade = ((sptr->sLife - sptr->Life) << 16) / sptr->sLife;
		sptr->Yvel += sptr->Gravity;

		if (sptr->MaxYvel)
		{
			if (sptr->Yvel < 0 && sptr->Yvel < sptr->MaxYvel << 5 || sptr->Yvel > 0 && sptr->Yvel > sptr->MaxYvel << 5)
				sptr->Yvel = sptr->MaxYvel << 5;
		}

		if (sptr->Friction & 0xF)
		{
			sptr->Xvel -= sptr->Xvel >> (sptr->Friction & 0xF);
			sptr->Zvel -= sptr->Zvel >> (sptr->Friction & 0xF);
		}

		if (sptr->Friction & 0xF0)
			sptr->Yvel -= sptr->Yvel >> (sptr->Friction >> 4);

		sptr->x += sptr->Xvel >> 5;
		sptr->y += sptr->Yvel >> 5;
		sptr->z += sptr->Zvel >> 5;

		if (sptr->Flags & SF_OUTSIDE)
		{
			sptr->x += SmokeWindX >> 1;
			sptr->z += SmokeWindZ >> 1;
		}

		sptr->Width = uchar(sptr->sWidth + ((fade * (sptr->dWidth - sptr->sWidth)) >> 16));
		sptr->Height = uchar(sptr->sHeight + ((fade * (sptr->dHeight - sptr->sHeight)) >> 16));
	}

	for (int i = 0; i < 192; i++)
	{
		sptr = &sparks[i];

		if (!sptr->On || sptr->Dynamic == -1)
			continue;

		pDL = &spark_dynamics[sptr->Dynamic];

		if (!pDL->On)
			continue;

		rnd = GetRandomControl();
		x = sptr->x + ((rnd & 0xF) << 4);
		y = sptr->y + (rnd & 0xF0);
		z = sptr->z + ((rnd >> 4) & 0xF0);
		falloff = sptr->sLife - sptr->Life - 1;

		if (falloff < 2)
		{
			if (pDL->Falloff < 28)
				pDL->Falloff += 6;

			r = 255 - (rnd & 0x1F) - (falloff << 3);
			g = 255 - (rnd & 0x1F) - (falloff << 4);
			b = 255 - (rnd & 0x1F) - (falloff << 6);
		}
		else if (falloff < 4)
		{
			if (pDL->Falloff < 28)
				pDL->Falloff += 6;

			r = 255 - (rnd & 0x1F) - (falloff << 3);
			g = 128 - (falloff << 3);
			b = (4 - falloff) << 2;

			if (b < 0)
				b = 0;
			else
				b <<= 3;
		}
		else
		{
			if (pDL->Falloff)
				pDL->Falloff--;

			r = (rnd & 0x1F) + 224;
			g = ((rnd >> 4) & 0x1F) + 128;
			b = (rnd >> 8) & 0x3F;
		}

		falloff = pDL->Falloff;

		if (falloff > 31)
			falloff = 31;

		if (sptr->Flags & SF_GREEN)
			TriggerDynamic(x, y, z, falloff, b, r, g);
		else
			TriggerDynamic(x, y, z, falloff, r, g, b);
	}
}

void TriggerGunShell(short lr, long objNum, long weapon)
{
	FX_INFO* fx;
	PHD_VECTOR pos;
	short fxNum;

	if (lr)
	{
		switch (weapon)
		{
		case LG_PISTOLS:
			pos.x = 8;
			pos.y = 48;
			pos.z = 40;
			break;

		case LG_MAGNUMS:
			pos.x = 16;
			pos.y = 40;
			pos.z = 56;
			break;

		case LG_UZIS:
			pos.x = 8;
			pos.y = 35;
			pos.z = 48;
			break;

		case LG_SHOTGUN:
			pos.x = 16;
			pos.y = 114;
			pos.z = 32;
			break;

		case LG_M16:
			pos.x = 16;
			pos.y = 2;
			pos.z = 64;
			break;
		}

		GetLaraMeshPos(&pos, LMX_HAND_R);
	}
	else
	{
		switch (weapon)
		{
		case LG_PISTOLS:
			pos.x = -12;
			pos.y = 48;
			pos.z = 40;
			break;

		case LG_MAGNUMS:
			pos.x = -16;
			pos.y = 40;
			pos.z = 56;
			break;

		case LG_UZIS:
			pos.x = -16;
			pos.y = 35;
			pos.z = 48;
			break;
		}

		GetLaraMeshPos(&pos, LMX_HAND_L);
	}

	if (weapon == LG_SHOTGUN)
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, weapon, 24);
	else
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, weapon, 16);

	fxNum = CreateEffect(lara_item->room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = pos.x;
	fx->pos.y_pos = pos.y;
	fx->pos.z_pos = pos.z;
	fx->room_number = lara_item->room_number;
	fx->pos.x_rot = 0;
	fx->pos.y_rot = 0;
	fx->pos.z_rot = (short)GetRandomControl();
	fx->speed = (GetRandomControl() & 0x1F) + 16;
	fx->object_number = (short)objNum;
	fx->frame_number = objects[fx->object_number].mesh_index;
	fx->fallspeed = -48 - (GetRandomControl() & 7);
	fx->shade = 0x4210;
	fx->counter = (GetRandomControl() & 1) + 1;

	if (lr)
	{
		if (weapon == LG_SHOTGUN)
		{
			fx->flag1 = lara.left_arm.y_rot + lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.torso_y_rot + 0x2800;
			fx->pos.y_rot += lara.torso_y_rot + lara.left_arm.y_rot + lara_item->pos.y_rot;

			if (fx->speed < 24)
				fx->speed += 24;
		}
		else if (weapon == LG_M16)
		{
			fx->flag1 = lara.left_arm.y_rot + lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.torso_y_rot + 0x4800;
			fx->pos.y_rot += lara.torso_y_rot + lara.left_arm.y_rot + lara_item->pos.y_rot;
		}
		else
			fx->flag1 = lara_item->pos.y_rot - (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot + 0x4800;
	}
	else
		fx->flag1 = lara_item->pos.y_rot + (GetRandomControl() & 0xFFF) + lara.left_arm.y_rot - 0x4800;
}

void ControlGunShell(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long ox, oy, oz, c, h;
	short room_number;

	fx = &effects[fx_number];
	ox = fx->pos.x_pos;
	oy = fx->pos.y_pos;
	oz = fx->pos.z_pos;

	fx->fallspeed += 6;
	fx->pos.x_rot += 182 * ((fx->speed >> 1) + 7);
	fx->pos.y_rot += 182 * fx->speed;
	fx->pos.z_rot += 4186;
	fx->pos.x_pos += fx->speed * phd_sin(fx->flag1) >> (W2V_SHIFT + 1);
	fx->pos.y_pos += fx->fallspeed;
	fx->pos.z_pos += fx->speed * phd_cos(fx->flag1) >> (W2V_SHIFT + 1);

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, room_number);

	if (room[room_number].flags & ROOM_UNDERWATER)
	{
		SetupRipple(fx->pos.x_pos, room[room_number].maxceiling, fx->pos.z_pos, -8 - (GetRandomControl() & 3), 1);
		KillEffect(fx_number);
		return;
	}

	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos < c)
	{
		SoundEffect(SFX_LARA_SHOTGUN_SHELL, &fx->pos, SFX_DEFAULT);
		fx->speed -= 4;
		fx->counter--;

		if (fx->counter < 0 || fx->speed < 8)
		{
			KillEffect(fx_number);
			return;
		}

		fx->fallspeed = -fx->fallspeed;
		fx->pos.y_pos = c;
	}

	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h)
	{
		SoundEffect(SFX_LARA_SHOTGUN_SHELL, &fx->pos, SFX_DEFAULT);
		fx->speed -= 8;
		fx->counter--;

		if (fx->counter < 0 || fx->speed < 8)
		{
			KillEffect(fx_number);
			return;
		}

		if (oy > h)
		{
			fx->flag1 += 0x8000;
			fx->pos.x_pos = ox;
			fx->pos.z_pos = oz;
		}
		else
			fx->fallspeed = -fx->fallspeed >> 1;

		fx->pos.y_pos = oy;
	}

	room_number = fx->room_number;
	GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, room_number);
}

RIPPLE_STRUCT* SetupRipple(long x, long y, long z, long size, long flags)
{
	RIPPLE_STRUCT* ripple;
	long num;

	ripple = ripples;
	num = 0;

	while (ripple->flags & 1)
	{
		ripple++;
		num++;

		if (num >= 16)
			return ripple;
	}

	ripple = &ripples[num];

	if (size < 0)
	{
		if (flags)
			ripple->flags = 19;
		else
			ripple->flags = 3;

		size = -size;
	}
	else
		ripple->flags = 1;

	ripple->init = 1;
	ripple->size = (uchar)size;
	ripple->life = (GetRandomControl() & 0xF) + 48;
	ripple->x = (GetRandomControl() & 0x7F) + x - 64;
	ripple->y = y;
	ripple->z = (GetRandomControl() & 0x7F) + z - 64;
	return ripple;
}

void SetupSplash(SPLASH_SETUP* setup)
{
	SPLASH_STRUCT* splash;
	SPLASH_VERTS* v;
	long n;

	splash = splashes;
	n = 0;

	while (splash->flags & 1)
	{
		splash++;
		n++;

		if (n >= 4)
		{
			SoundEffect(SFX_LARA_SPLASH, (PHD_3DPOS*)setup, SFX_DEFAULT);
			return;
		}
	}

	splash->flags = 3;

	if (setup->OuterFriction == -9)
	{
		splash->flags = 67;
		setup->OuterFriction = 9;
	}

	splash->x = setup->x;
	splash->y = setup->y;
	splash->z = setup->z;
	splash->life = 63;

	v = splash->sv;

	for (int i = 0; i < 8; i++)
	{
		v->wx = (setup->InnerXZoff * SplashRings[i][0]) << 1;
		v->wy = 0;
		v->wz = (setup->InnerXZoff * SplashRings[i][1]) << 1;
		v->xv = (setup->InnerXZvel * SplashRings[i][0]) / 12;
		v->yv = 0;
		v->zv = (setup->InnerXZvel * SplashRings[i][1]) / 12;
		v->oxv = v->xv >> 3;
		v->ozv = v->zv >> 3;
		v->gravity = 0;
		v->friction = setup->InnerFriction - 2;
		v++;
	}

	for (int i = 0; i < 8; i++)
	{
		v->wx = ((setup->InnerXZoff + setup->InnerXZsize) * SplashRings[i][0]) << 1;
		v->wy = setup->InnerYsize;
		v->wz = ((setup->InnerXZoff + setup->InnerXZsize) * SplashRings[i][1]) << 1;
		v->xv = (setup->InnerXZvel * SplashRings[i][0]) >> 3;
		v->yv = setup->InnerYvel;
		v->zv = (setup->InnerXZvel * SplashRings[i][1]) >> 3;
		v->oxv = v->xv >> 3;
		v->ozv = v->zv >> 3;
		v->gravity = (uchar)setup->InnerGravity;
		v->friction = (uchar)setup->InnerFriction;
		v++;
	}

	for (int i = 0; i < 8; i++)
	{
		v->wx = (setup->MiddleXZoff * SplashRings[i][0]) << 1;
		v->wy = 0;
		v->wz = (setup->MiddleXZoff * SplashRings[i][1]) << 1;
		v->xv = (setup->MiddleXZvel * SplashRings[i][0]) / 12;
		v->yv = 0;
		v->zv = (setup->MiddleXZvel * SplashRings[i][1]) / 12;
		v->oxv = v->xv >> 3;
		v->ozv = v->zv >> 3;
		v->gravity = 0;
		v->friction = setup->MiddleFriction - 2;
		v++;
	}

	for (int i = 0; i < 8; i++)
	{
		v->wx = ((setup->MiddleXZoff + setup->MiddleXZsize) * SplashRings[i][0]) << 1;
		v->wy = setup->MiddleYsize;
		v->wz = ((setup->MiddleXZoff + setup->MiddleXZsize) * SplashRings[i][1]) << 1;
		v->xv = (setup->MiddleXZvel * SplashRings[i][0]) >> 3;
		v->yv = setup->MiddleYvel;
		v->zv = (setup->MiddleXZvel * SplashRings[i][1]) >> 3;
		v->oxv = v->xv >> 3;
		v->ozv = v->zv >> 3;
		v->gravity = (uchar)setup->MiddleGravity;
		v->friction = (uchar)setup->MiddleFriction;
		v++;
	}

	for (int i = 0; i < 8; i++)
	{
		v->wx = (setup->OuterXZoff * SplashRings[i][0]) << 1;
		v->wy = 0;
		v->wz = (setup->OuterXZoff * SplashRings[i][1]) << 1;
		v->xv = (setup->OuterXZvel * SplashRings[i][0]) / 12;
		v->yv = 0;
		v->zv = (setup->OuterXZvel * SplashRings[i][1]) / 12;
		v->oxv = v->xv >> 3;
		v->ozv = v->zv >> 3;
		v->gravity = 0;
		v->friction = setup->OuterFriction - 2;
		v++;
	}

	for (int i = 0; i < 8; i++)
	{
		v->wx = ((setup->OuterXZoff + setup->OuterXZsize) * SplashRings[i][0]) << 1;
		v->wy = 0;
		v->wz = ((setup->OuterXZoff + setup->OuterXZsize) * SplashRings[i][1]) << 1;
		v->xv = (setup->OuterXZvel * SplashRings[i][0]) >> 3;
		v->yv = 0;
		v->zv = (setup->OuterXZvel * SplashRings[i][1]) >> 3;
		v->oxv = v->xv >> 3;
		v->ozv = v->zv >> 3;
		v->gravity = 0;
		v->friction = (uchar)setup->OuterFriction;
		v++;
	}

	SoundEffect(SFX_LARA_SPLASH, (PHD_3DPOS*)setup, SFX_DEFAULT);
}

void UpdateSplashes()
{
	SPLASH_STRUCT* splash;
	SPLASH_VERTS* v;
	RIPPLE_STRUCT* ripple;
	long set;

	for (int i = 0; i < 4; i++)
	{
		splash = &splashes[i];

		if (!(splash->flags & 1))
			continue;

		set = 0;

		for (int j = 0; j < 48; j++)
		{
			v = &splash->sv[j];
			v->wx += v->xv >> 2;
			v->wy += short(v->yv >> 6);
			v->wz += v->zv >> 2;
			v->xv -= v->xv >> v->friction;
			v->zv -= v->zv >> v->friction;

			if ((v->oxv < 0 && v->xv > v->oxv) || (v->oxv > 0 && v->xv < v->oxv))
				v->xv = v->oxv;
			else if ((v->ozv < 0 && v->zv > v->ozv) || (v->ozv > 0 && v->zv < v->ozv))
				v->zv = v->ozv;

			v->yv += v->gravity << 3;

			if (v->yv > 0x10000)
				v->yv = 0x10000;

			if (v->wy > 0)
			{
				if (j < 16)
					splash->flags |= 4;
				else if (j < 32)
					splash->flags |= 8;

				v->wy = 0;
				set = 1;
			}
		}

		if (set)
		{
			splash->life--;

			if (!splash->life)
				splash->flags = 0;
		}
	}

	for (int i = 0; i < 16; i++)
	{
		ripple = &ripples[i];

		if (!(ripple->flags & 1))
			continue;

		if (ripple->size < 254)
			ripple->size += 2;

		if (!ripple->init)
		{
			ripple->life -= 2;

			if (ripple->life > 250)
				ripple->flags = 0;
		}
		else if (ripple->init < ripple->life)
		{
			ripple->init += 4;

			if (ripple->init >= ripple->life)
				ripple->init = 0;
		}
	}
}

void ControlColouredLights(short item_number)
{
	ITEM_INFO* item;
	long objnum;
	uchar colours[5][3] =
	{
		{ 255, 0, 0 },		//RED_LIGHT
		{ 0, 255, 0 },		//GREEN_LIGHT
		{ 0, 0, 255 },		//BLUE_LIGHT
		{ 255, 192, 0 },	//AMBER_LIGHT 
		{ 224, 224, 255 }	//WHITE_LIGHT
	};

	item = &items[item_number];

	if (TriggerActive(item))
	{
		objnum = item->object_number - RED_LIGHT;
		TriggerDynamic(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 24, colours[objnum][0], colours[objnum][1], colours[objnum][2]);
	}
}

void KillAllCurrentItems(short item_number)
{
	KillEverythingFlag = 1;
}

void TriggerBats(long x, long y, long z, short ang)
{
	BAT_STRUCT* bat;

	ang = (ang - 1024) & 0xFFF;

	for (int i = 0; i < 32; i++)
	{
		bat = &bats[i];
		bat->x = (GetRandomControl() & 0x1FF) + x - 256;
		bat->y = y - (GetRandomControl() & 0xFF) + 256;
		bat->z = (GetRandomControl() & 0x1FF) + z - 256;
		bat->angle = ((GetRandomControl() & 0x7F) + ang - 64) & 0xFFF;
		bat->speed = (GetRandomControl() & 0x1F) + 64;
		bat->WingYoff = GetRandomControl() & 0x3F;
		bat->life = (GetRandomControl() & 7) + 144;
		bat->flags |= 1;
	}
}

void BatEmitterControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->active)
	{
		TriggerBats(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->pos.y_rot >> 4);
		KillItem(item_number);
	}
}

void UpdateBats()
{
	BAT_STRUCT* bat;
	PHD_3DPOS pos;

	for (int i = 0; i < 32; i++)
	{
		bat = &bats[i];

		if (!(bat->flags & 1))
			continue;

		if (!(i & 3) && !(GetRandomControl() & 7))
		{
			pos.x_pos = bat->x;
			pos.y_pos = bat->y;
			pos.z_pos = bat->z;
			SoundEffect(SFX_BATS_1, &pos, SFX_DEFAULT);
		}

		bat->x -= (bat->speed * rcossin_tbl[(bat->angle << 1) + 1]) >> W2V_SHIFT;
		bat->y -= GetRandomControl() & 3;
		bat->z += (bat->speed * rcossin_tbl[bat->angle << 1]) >> W2V_SHIFT;
		bat->WingYoff = (bat->WingYoff + 11) & 0x3F;

		if (bat->life < 128)
		{
			bat->y += -4 - (i >> 1);

			if (!(GetRandomControl() & 3))
			{
				bat->angle = (bat->angle + (GetRandomControl() & 0xFF) - 128) & 0xFFF;
				bat->speed += GetRandomControl() & 3;
			}
		}

		bat->speed += 12;

		if (bat->speed > 300)
			bat->speed = 300;

		if (bat->life && wibble & 4)
		{
			bat->life--;

			if (!bat->life)
				bat->flags = 0;
		}
	}
}
