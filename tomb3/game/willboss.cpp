#include "../tomb3/pch.h"
#include "willboss.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"

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

void inject_willboss(bool replace)
{
	INJECT(0x00473570, TriggerPlasmaBallFlame, replace);
}
