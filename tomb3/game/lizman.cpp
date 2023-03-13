#include "../tomb3/pch.h"
#include "lizman.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "items.h"
#include "sphere.h"
#include "../3dsystem/3d_gen.h"
#include "../3dsystem/phd_math.h"

static void TriggerLizmanGas(long x, long y, long z, long xv, long yv, long zv, long FxObj)
{
	SPARKS* sptr;
	long size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = (GetRandomControl() & 0x3F) + 128;
	sptr->sB = 32;
	sptr->dR = 0;
	sptr->dG = (GetRandomControl() & 0xF) + 32;
	sptr->dB = 0;

	if (xv || yv || zv)
	{
		sptr->ColFadeSpeed = 6;
		sptr->FadeToBlack = 2;
		sptr->Life = (GetRandomControl() & 1) + 12;
	}
	else
	{
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 16;
		sptr->Life = (GetRandomControl() & 3) + 20;
	}

	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0x1F) + x - 16;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0x1F) + z - 16;
	sptr->Xvel = short((GetRandomControl() & 0xF) + xv - 16);
	sptr->Yvel = (short)yv;
	sptr->Zvel = short((GetRandomControl() & 0xF) + zv - 16);
	sptr->Friction = 0;

	if (GetRandomControl() & 1)
	{
		if (FxObj < 0)
			sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		else
			sptr->Flags = SF_ALTDEF | SF_FX | SF_ROTATE | SF_DEF | SF_SCALE;

		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else if (FxObj < 0)
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;
	else
		sptr->Flags = SF_ALTDEF | SF_FX | SF_DEF | SF_SCALE;

	sptr->MaxYvel = 0;
	sptr->FxObj = (uchar)FxObj;
	sptr->Gravity = 0;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	size = (GetRandomControl() & 0x1F) + 48;

	if (xv || yv || zv)
	{
		sptr->Width = uchar(size >> 5);
		sptr->sWidth = sptr->Width;
		sptr->dWidth = uchar(size >> 1);
		sptr->Height = sptr->Width;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->dWidth;

		if (FxObj == -2)
			sptr->Scalar = 2;
		else
			sptr->Scalar = 3;
	}
	else
	{
		sptr->Scalar = 4;
		sptr->Width = uchar(size >> 4);
		sptr->sWidth = sptr->Width;
		sptr->dWidth = uchar(size >> 1);
		sptr->Height = sptr->Width;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->dWidth;
	}
}

static short TriggerLizmanGasThrower(ITEM_INFO* item, BITE_INFO* bite, short speed)
{
	FX_INFO* fx;
	PHD_VECTOR pos;
	PHD_VECTOR pos1;
	long s, r, xv, yv, zv, lp;
	short fxNum;
	short angles[2];

	fxNum = CreateEffect(item->room_number);

	if (fxNum == NO_ITEM)
		return NO_ITEM;

	fx = &effects[fxNum];

	pos.x = bite->x;
	pos.y = bite->y;
	pos.z = bite->z;
	GetJointAbsPosition(item, &pos, bite->mesh_num);

	pos1.x = bite->x;
	pos1.y = bite->y << 3;
	pos1.z = bite->z << 2;
	GetJointAbsPosition(item, &pos1, bite->mesh_num);

	phd_GetVectorAngles(pos1.x - pos.x, pos1.y - pos.y, pos1.z - pos.z, angles);

	fx->pos.x_pos = pos.x;
	fx->pos.y_pos = pos.y;
	fx->pos.z_pos = pos.z;
	fx->room_number = item->room_number;
	fx->pos.x_rot = angles[1];
	fx->pos.z_rot = 0;
	fx->pos.y_rot = angles[0];
	fx->speed = speed << 2;
	fx->object_number = DRAGON_FIRE;
	fx->counter = 20;
	fx->flag1 = 1;
	TriggerLizmanGas(0, 0, 0, 0, 0, 0, fxNum);

	for (lp = 0; lp < 2; lp++)
	{
		s = GetRandomControl() % (speed << 2) + 32;
		r = (s * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
		xv = (r * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
		yv = -((s * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
		zv = (r * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
		TriggerLizmanGas(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -1);
	}

	r = ((speed << 1) * phd_cos(fx->pos.x_rot)) >> W2V_SHIFT;
	xv = (r * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	yv = -(((speed << 1) * phd_sin(fx->pos.x_rot)) >> W2V_SHIFT);
	zv = (r * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
	TriggerLizmanGas(fx->pos.x_pos, fx->pos.y_pos, fx->pos.z_pos, xv << 5, yv << 5, zv << 5, -2);

	return fxNum;
}

void inject_lizman(bool replace)
{
	INJECT(0x00450620, TriggerLizmanGas, replace);
	INJECT(0x004503E0, TriggerLizmanGasThrower, replace);
}
