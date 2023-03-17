#include "../tomb3/pch.h"
#include "clawmute.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "items.h"

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
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 255;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
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
	sptr->Xvel = short(xv + (GetRandomControl() & 0xFF) - 128);
	sptr->Yvel = (short)yv;
	sptr->Zvel = short(zv + (GetRandomControl() & 0xFF) - 128);
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
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;

	if (!type)
	{
		sptr->Scalar = 2;
		sptr->Xvel <<= 2;
		sptr->Yvel = (GetRandomControl() & 0x1FF) - 256;
		sptr->Zvel <<= 2;
		sptr->Friction = 85;
		sptr->dWidth >>= 1;
		sptr->dHeight >>= 1;
	}

	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
}

static void TriggerPlasmaBall(ITEM_INFO* item, long type, PHD_VECTOR* pos, short room_number, short angle)
{
	FX_INFO* fx;
	PHD_VECTOR p;
	short angles[2];
	short fxNum, speed;

	if (type)
	{
		p.x = pos->x;
		p.y = pos->y;
		p.z = pos->z;
		angles[0] = short(GetRandomControl() << 1);
		angles[1] = 0x2000;
		speed = (GetRandomControl() & 0xF) + 16;
	}
	else
	{
		p.x = -32;
		p.y = -16;
		p.z = -192;
		GetJointAbsPosition(item, &p, 13);
		phd_GetVectorAngles(lara_item->pos.x_pos - p.x, lara_item->pos.y_pos - p.y - 256, lara_item->pos.z_pos - p.z, angles);
		angles[0] = item->pos.y_rot;
		speed = (GetRandomControl() & 0xF) + 16;
	}

	fxNum = CreateEffect(room_number);

	if (fxNum == NO_ITEM)
		return;

	fx = &effects[fxNum];
	fx->pos.x_pos = p.x;
	fx->pos.y_pos = p.y;
	fx->pos.z_pos = p.z;
	fx->pos.x_rot = angles[1];
	fx->pos.y_rot = angles[0];
	fx->object_number = EXTRAFX1;
	fx->speed = speed;
	fx->fallspeed = 0;
	fx->flag1 = (short)type;
}

static void TriggerPlasma(short item_number)
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
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 255;
	sptr->dR = 32;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
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
	sptr->Yvel = (GetRandomControl() & 0xF) + 16;
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

	sptr->Gravity = (GetRandomControl() & 0x1F) + 16;
	sptr->NodeNumber = 6;
	sptr->MaxYvel = (GetRandomControl() & 7) + 16;
	sptr->FxObj = (uchar)item_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 2;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 2;
}

void inject_clawmute(bool replace)
{
	INJECT(0x0041C1F0, TriggerPlasmaBallFlame, replace);
	INJECT(0x0041BDA0, TriggerPlasmaBall, replace);
	INJECT(0x0041BBE0, TriggerPlasma, replace);
}
