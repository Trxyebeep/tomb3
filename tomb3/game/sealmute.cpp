#include "../tomb3/pch.h"
#include "sealmute.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"

static void TriggerSealmuteGas(long x, long y, long z, long xv, long yv, long zv, long FxObj)
{
	SPARKS* sptr;
	long size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) + 128;
	sptr->sG = (GetRandomControl() & 0x3F) + 128;
	sptr->sB = 32;
	sptr->dR = (GetRandomControl() & 0xF) + 32;
	sptr->dG = (GetRandomControl() & 0xF) + 32;
	sptr->dB = 0;

	if (xv || yv || zv)
	{
		sptr->ColFadeSpeed = 6;
		sptr->FadeToBlack = 2;
		sptr->Life = (GetRandomControl() & 1) + 16;
	}
	else
	{
		sptr->ColFadeSpeed = 8;
		sptr->FadeToBlack = 16;
		sptr->Life = (GetRandomControl() & 3) + 28;
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

void inject_sealmute(bool replace)
{
	INJECT(0x00463700, TriggerSealmuteGas, replace);
}
