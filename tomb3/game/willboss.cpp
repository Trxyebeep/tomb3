#include "../tomb3/pch.h"
#include "willboss.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "effects.h"
#include "sphere.h"
#include "traps.h"

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
	sptr->Friction = 85;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = short(2 * (xv + (GetRandomControl() & 0xFF)) - 256);
	sptr->Yvel = (GetRandomControl() & 0x1FF) - 256;
	sptr->Zvel = short(2 * (zv + (GetRandomControl() & 0xFF)) - 256);
	
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
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	if (type < 0)
	{
		if (type >= -2)
			sptr->Scalar = 2;
		else
			sptr->Scalar = 4;

		sptr->Width = (GetRandomControl() & 0xF) + 16;
		sptr->Friction = 5;
		sptr->Xvel = short((GetRandomControl() & 0xFF) + xv - 128);
		sptr->Yvel = (short)yv;
		sptr->Zvel = short((GetRandomControl() & 0xFF) + zv - 128);
	}
	else
	{
		sptr->Scalar = 3;
		sptr->Width = (uchar)type;
	}

	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 3;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 3;
}

static void TriggerPlasmaBall(PHD_VECTOR* pos, short room_number, short angle, short type)
{
	FX_INFO* fx;
	short fxNum;

	fxNum = CreateEffect(room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = pos->x;
	fx->pos.y_pos = pos->y;
	fx->pos.z_pos = pos->z;
	fx->pos.x_rot = 0;
	fx->pos.y_rot = angle;
	fx->object_number = EXTRAFX2;

	if (type != -16)
		fx->speed = (GetRandomControl() & 0x1F) + 16;
	else
		fx->speed = 0;

	fx->fallspeed = -16 * type;
	fx->flag1 = type;
}

static void TriggerPlasma(short item_number, long node, long size)
{
	ITEM_INFO* item;
	SPARKS* sptr;
	long dx, dz;

	item = &items[item_number];
	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

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
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->Friction = 3;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Yvel = (GetRandomControl() & 7) + 8;
	sptr->Zvel = (GetRandomControl() & 0x1F) - 16;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ATTACHEDNODE | SF_ALTDEF | SF_ITEM | SF_DEF | SF_SCALE;

	sptr->Gravity = (GetRandomControl() & 7) + 8;
	sptr->NodeNumber = (uchar)node;
	sptr->MaxYvel = (GetRandomControl() & 7) + 16;
	sptr->FxObj = (uchar)item_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	size += GetRandomControl() & 0xF;
	sptr->Width = (uchar)size;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

void ControlWillbossPlasmaBall(short fx_number)
{
	FX_INFO* fx;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR oldPos;
	long speed, h, c;
	short room_number;
	short ang[2];
	char falloffs[5];

	falloffs[0] = 13;
	falloffs[1] = 7;
	falloffs[2] = 7;
	falloffs[3] = 7;
	falloffs[4] = 7;
	fx = &effects[fx_number];
	oldPos.x = fx->pos.x_pos;
	oldPos.y = fx->pos.y_pos;
	oldPos.z = fx->pos.z_pos;

	if (fx->flag1)
	{
		fx->fallspeed += (fx->flag1 != 1) + 1;

		if (!(wibble & 0xC))
		{
			if (fx->speed)
				fx->speed--;

			TriggerPlasmaBallFlame(fx_number, -1 - fx->flag1, 0, -(GetRandomControl() & 0x1F), 0);
		}
	}
	else
	{
		phd_GetVectorAngles(lara_item->pos.x_pos - oldPos.x, lara_item->pos.y_pos - oldPos.y - 256, lara_item->pos.z_pos - oldPos.z, ang);
		fx->pos.x_rot = ang[1];
		fx->pos.y_rot = ang[0];

		if (fx->speed < 512)
			fx->speed += (fx->speed >> 4) + 4;

		if (wibble & 4)
			TriggerPlasmaBallFlame(fx_number, fx->speed >> 1, 0, 0, 0);
	}

	speed = (fx->speed * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	fx->pos.x_pos += (speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.y_pos += fx->fallspeed - ((fx->speed * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	fx->pos.z_pos += (speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;

	room_number = fx->room_number;
	floor = GetFloor(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, &room_number);
	h = GetHeight(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);
	c = GetCeiling(floor, fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos);

	if (fx->pos.y_pos >= h || fx->pos.y_pos < c)
	{
		if (!fx->flag1)
		{
			pos.x = oldPos.x;
			pos.y = oldPos.y;
			pos.z = oldPos.z;
			h = (GetRandomControl() & 3) + 2;

			for (int i = 0; i < h; i++)
				TriggerPlasmaBall(&pos, fx->room_number, fx->pos.y_rot + (GetRandomControl() & 0x3FFF) + 0x6000, 1);
		}

		KillEffect(fx_number);
	}
	else if (ItemNearLara(&fx->pos, 200) && !fx->flag1)
	{
		for (int i = 14; i >= 0; i -= 2)
		{
			pos.x = 0;
			pos.y = 0;
			pos.z = 0;
			GetJointAbsPosition(lara_item, &pos, i);
			TriggerPlasmaBall(&pos, fx->room_number, short(GetRandomControl() << 1), 1);
		}

		LaraBurn();
		lara_item->hit_points = -1;
		lara.BurnGreen = 1;
		KillEffect(fx_number);
	}
	else
	{
		if (fx->room_number != room_number)
			EffectNewRoom(fx_number, lara_item->room_number);

		if (falloffs[fx->flag1])
		{
			c = GetRandomControl();
			TriggerDynamic(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, falloffs[fx->flag1], c & 7, 31 - ((c >> 4) & 3), 24 - ((c >> 6) & 3));
		}
	}
}

void inject_willboss(bool replace)
{
	INJECT(0x00473570, TriggerPlasmaBallFlame, replace);
	INJECT(0x004731B0, TriggerPlasmaBall, replace);
	INJECT(0x00472FE0, TriggerPlasma, replace);
	INJECT(0x00473230, ControlWillbossPlasmaBall, replace);
}
