#include "../tomb3/pch.h"
#include "londboss.h"
#include "items.h"
#include "../specific/game.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "effect2.h"
#include "lara.h"
#include "../3dsystem/3d_gen.h"
#include "sound.h"
#include "lot.h"
#include "control.h"
#include "effects.h"

static long heights[5] = { -1536, -1280, -832, -384, 0 };
static long radii[5] = { 200, 400, 500, 500, 475 };
static long dradii[5] = { 1600, 5600, 6400, 5600, 1600 };
static long dheights1[5] = { -7680, -4224, -768, 2688, 6144 };
static long dheights2[5] = { -1536, -1152, -768, -384, 0 };
static long death_radii[5];
static long death_heights[5];

static void TriggerPlasmaBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle)
{
	FX_INFO* fx;
	short fx_num;

	fx_num = CreateEffect(room_number);

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		fx->speed = (GetRandomControl() & 0x1F) + 64;
		fx->pos.x_pos = pos->x;
		fx->pos.y_pos = pos->y;
		fx->pos.z_pos = pos->z;
		fx->pos.x_rot = 0x2000;
		fx->pos.y_rot = short(angle + GetRandomControl() + 0x4000);
		fx->object_number = EXTRAFX5;
		fx->fallspeed = 0;
		fx->flag1 = 1;
		fx->flag2 = type == 2;
	}
}

static void TriggerPlasmaBallFlame(short fx_number, long type, long xv, long yv, long zv)
{
	FX_INFO* fx;
	SPARKS* sptr;
	long dx, dz;

	fx = &effects[fx_number];
	dx = lara_item->pos.x_pos - fx->pos.x_pos;
	dz = lara_item->pos.z_pos - fx->pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 48;
	sptr->sG = 255;
	sptr->sB = (GetRandomControl() & 0x1F) + 48;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 192;
	sptr->dB = (GetRandomControl() & 0x3F) + 128;
	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = short((GetRandomControl() & 0xFF) + xv - 128);
	sptr->Yvel = (short)yv;
	sptr->Zvel = short((GetRandomControl() & 0xFF) + zv - 128);
	sptr->Friction = 5;
	
	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;

	sptr->FxObj = (uchar)fx_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

static void TriggerLaserBolt(PHD_VECTOR* pos, ITEM_INFO* item, long type, short ang)
{
	ITEM_INFO* bolt;
	short item_number;
	short angles[2];

	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	bolt = &items[item_number];
	bolt->object_number = EXTRAFX4;
	bolt->room_number = item->room_number;
	bolt->pos.x_pos = pos->x;
	bolt->pos.y_pos = pos->y;
	bolt->pos.z_pos = pos->z;
	InitialiseItem(item_number);

	if (type == 2)
	{
		bolt->pos.y_pos += item->pos.y_pos - 384;
		bolt->pos.x_rot = short(-pos->y << 5);
		bolt->pos.y_rot = short(GetRandomControl() << 1);
	}
	else
	{
		phd_GetVectorAngles(lara_item->pos.x_pos - pos->x, lara_item->pos.y_pos - pos->y - 256, lara_item->pos.z_pos - pos->z, angles);
		bolt->pos.x_rot = angles[1];
		bolt->pos.y_rot = ang;
		bolt->pos.z_rot = 0;
	}

	if (type == 1)
	{
		bolt->speed = 24;
		bolt->item_flags[0] = 31;
		bolt->item_flags[1] = 16;
	}
	else
	{
		bolt->speed = 16;
		bolt->item_flags[0] = -24;
		bolt->item_flags[1] = 4;

		if (type == 2)
			bolt->item_flags[2] = 1;
	}

	AddActiveItem(item_number);
}

static void ExplodeLondonBoss(ITEM_INFO* item)
{
	SHIELD_POINTS* p;
	long x, y, z, lp, lp2, rad, angle, r, g, b;

	if (bossdata.explode_count == 1 || bossdata.explode_count == 15 || bossdata.explode_count == 25 ||
		bossdata.explode_count == 35 || bossdata.explode_count == 45 || bossdata.explode_count == 55)
	{
		x = (GetRandomDraw() & 0x3FF) + item->pos.x_pos - 512;
		y = item->pos.y_pos - (GetRandomDraw() & 0x3FF) - 256;
		z = (GetRandomDraw() & 0x3FF) + item->pos.z_pos - 512;
		ExpRings[bossdata.ring_count].x = x;
		ExpRings[bossdata.ring_count].y = y;
		ExpRings[bossdata.ring_count].z = z;
		ExpRings[bossdata.ring_count].on = 1;
		bossdata.ring_count++;
		TriggerExplosionSparks(x, y, z, 3, -2, 2, 0);

		for (lp = 0; lp < 2; lp++)
			TriggerExplosionSparks(x, y, z, 3, -1, 2, 0);

		SoundEffect(SFX_BLAST_CIRCLE, &item->pos, 0x800000 | SFX_SETPITCH);
	}

	for (lp = 0; lp < 5; lp++)
	{
		if (bossdata.explode_count < 128)
		{
			death_radii[lp] = (dradii[lp] >> 4) + ((bossdata.explode_count * dradii[lp]) >> 7);
			death_heights[lp] = dheights2[lp] + ((bossdata.explode_count * (dheights1[lp] - dheights2[lp])) >> 7);
		}
	}

	p = LondonBossShield;

	for (lp = 0; lp < 5; lp++)
	{
		y = death_heights[lp];
		rad = death_radii[lp];
		angle = (wibble & 0x3F) << 3;

		for (lp2 = 0; lp2 < 8; lp2++, p++)
		{
			p->x = short((rad * rcossin_tbl[angle << 1]) >> 11);
			p->y = (short)y;
			p->z = short((rad * rcossin_tbl[(angle << 1) + 1]) >> 11);

			if (!lp || lp == 16 || bossdata.explode_count >= 64)
				p->rgb = 0;
			else
			{
				r = GetRandomDraw() & 0x3F;
				g = (GetRandomDraw() & 0x1F) + 224;
				b = (g >> 2) + (GetRandomDraw() & 0x3F);
				r = ((64 - bossdata.explode_count) * r) >> 6;
				g = ((64 - bossdata.explode_count) * g) >> 6;
				b = ((64 - bossdata.explode_count) * b) >> 6;
				p->rgb = (b << 16) | (g << 8) | r;
			}

			angle = (angle + 512) & 0xFFF;
		}
	}
}

static void LondonBossDie(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->hit_points = DONT_TARGET;
	item->collidable = 0;
	KillItem(item_number);
	DisableBaddieAI(item_number);
	item->flags |= IFL_INVISIBLE;
}

long KnockBackCollision(EXPLOSION_RING* ring)
{
	long dx, dz, dist;
	short ang, dy;

	dx = lara_item->pos.x_pos - ring->x;
	dz = lara_item->pos.z_pos - ring->z;

	if (dx > 16000 || dx < -16000 || dz > 16000 || dz < -16000)
		dist = 0x7FFF;
	else
		dist = SQUARE(dx) + SQUARE(dz);

	if (dist >= SQUARE(ring->radius))
		return 0;

	lara_item->hit_points -= 200;
	lara_item->hit_status = 1;
	ang = (short)phd_atan(dz, dx);
	dy = lara_item->pos.y_rot - ang;

	if (abs(dy) >= 0x4000)
	{
		lara_item->pos.y_rot = ang + 0x8000;
		lara_item->speed = -75;
	}
	else
	{
		lara_item->pos.y_rot = ang;
		lara_item->speed = 75;
	}

	lara_item->gravity_status = 1;
	lara_item->fallspeed = -50;
	lara_item->pos.x_rot = 0;
	lara_item->pos.z_rot = 0;
	lara_item->anim_number = ANIM_FALLDOWN;
	lara_item->frame_number = anims[ANIM_FALLDOWN].frame_base;
	lara_item->current_anim_state = AS_FORWARDJUMP;
	lara_item->goal_anim_state = AS_FORWARDJUMP;
	TriggerExplosionSparks(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, 3, -2, 2, lara_item->room_number);

	for (int i = 0; i < 3; i++)
		TriggerPlasmaBall(lara_item, 2, (PHD_VECTOR*)&lara_item->pos, lara_item->room_number, short(GetRandomControl() << 1));

	return 1;
}

void ControlLondBossPlasmaBall(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	long oldY, speed, h, c, r, g, b;
	short room_number;
	uchar falloffs[2];

	falloffs[0] = 13;
	falloffs[1] = 7;
	fx = &effects[fx_number];
	fx->fallspeed++;
	oldY = fx->pos.y_pos;

	if (fx->speed > 8)
		fx->speed -= 2;

	if (fx->pos.x_rot > -15360)
		fx->pos.x_rot -= 256;

	speed = (fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.x_pos += (speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += fx->fallspeed - ((fx->speed * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	fx->pos.z_pos += (speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

	if (!(wibble & 0xF))
		TriggerPlasmaBallFlame(fx_number, 0, 0, abs(oldY - fx->pos.y_pos) << 3, 0);

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos < c || room[room_number].flags & ROOM_UNDERWATER)
	{
		KillEffect(fx_number);
		return;
	}

	if (!fx->flag2 && ItemNearLara(&fx->pos, 200))
	{
		lara_item->hit_points -= 25;
		lara_item->hit_status = 1;
		KillEffect(fx_number);
		return;
	}

	if (fx->room_number != room_number)
		EffectNewRoom(fx_number, room_number);

	c = GetRandomControl();
	r = c & 7;
	g = 31 - ((c >> 4) & 3);
	b = 24 - ((c >> 6) & 3);
	TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, falloffs[fx->flag1], r, g, b);
}

void inject_londboss(bool replace)
{
	INJECT(0x00451DE0, TriggerPlasmaBall, replace);
	INJECT(0x00452090, TriggerPlasmaBallFlame, replace);
	INJECT(0x00451980, TriggerLaserBolt, replace);
	INJECT(0x00451730, ExplodeLondonBoss, replace);
	INJECT(0x00452240, KnockBackCollision, replace);
	INJECT(0x00451E80, ControlLondBossPlasmaBall, replace);
}
