#include "../tomb3/pch.h"
#include "51rocket.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"

static void Trigger51BlastFire(long x, long y, long z, long smoke, long end)
{
	SPARKS* sptr;
	long size;

	if (end < 0)
		sptr = &sparks[GetFreeSpark()];
	else
		sptr = &sparks[end];

	sptr->On = 1;

	if (smoke)
	{
		sptr->sR = 0;
		sptr->sG = 0;
		sptr->sB = 0;
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}
	else
	{
		sptr->sR = (GetRandomControl() & 0x1F) + 128;
		sptr->sG = (GetRandomControl() & 0x1F) + 64;
		sptr->sB = 32;
		sptr->dR = (GetRandomControl() & 0x1F) + 224;
		sptr->dG = (GetRandomControl() & 0x1F) + 160;
		sptr->dB = 32;
	}

	sptr->ColFadeSpeed = 16;

	if (end)
	{
		sptr->FadeToBlack = (GetRandomControl() & 0x1F) + 32;
		sptr->Life = uchar((end >> 1) + 72);
	}
	else
	{
		sptr->FadeToBlack = smoke != 0 ? 32 : 8;
		sptr->Life = (!smoke ? 0 : 32) + (GetRandomControl() & 7) + 32;
	}
	
	sptr->sLife = sptr->Life;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = y;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -(GetRandomControl() & 7);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;

	sptr->Friction = 4;
	sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
	sptr->RotAng = GetRandomControl() & 0xFFF;

	if (GetRandomControl() & 1)
		sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
	else
		sptr->RotAdd = (GetRandomControl() & 0xF) + 16;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 4;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;

	size = (GetRandomControl() & 0x3F) + 64;

	if (end)
	{
		sptr->Width = (uchar)size;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = sptr->Width;
		
		sptr->Height = sptr->Width;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->Height;
	}
	else
	{
		sptr->dWidth = (uchar)size;
		sptr->Width = sptr->dWidth >> 1;
		sptr->sWidth = sptr->Width;
		
		sptr->dHeight = sptr->dWidth;
		sptr->Height = sptr->dHeight >> 1;
		sptr->sHeight = sptr->Height;
	}
}

void inject_51rocket(bool replace)
{
	INJECT(0x0040FCC0, Trigger51BlastFire, replace);
}
