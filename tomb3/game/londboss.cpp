#include "../tomb3/pch.h"
#include "londboss.h"
#include "items.h"
#include "../specific/game.h"
#include "objects.h"
#include "../3dsystem/phd_math.h"
#include "effect2.h"
#include "lara.h"

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

void inject_londboss(bool replace)
{
	INJECT(0x00451DE0, TriggerPlasmaBall, replace);
	INJECT(0x00452240, KnockBackCollision, replace);
	INJECT(0x00452090, TriggerPlasmaBallFlame, replace);
}
