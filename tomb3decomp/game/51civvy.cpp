#include "../tomb3/pch.h"
#include "51civvy.h"
#include "effect2.h"
#include "../specific/function_stubs.h"

static void TriggerFenceSparks(long x, long y, long z, long kill)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) - 64;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sG;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 16;
	sptr->dB = (GetRandomControl() & 0x3F) - 64;
	sptr->dR = sptr->sB >> 2;
	sptr->dG = sptr->sB >> 1;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 32;
	sptr->sLife = sptr->Life;
	sptr->x = x;
	sptr->y = y;
	sptr->z = z;
	sptr->Xvel = ((GetRandomControl() & 0xFF) - 128) << 1;
	sptr->Yvel = short((GetRandomControl() & 0xF) - (kill << 5) - 8);
	sptr->Zvel = ((GetRandomControl() & 0xFF) - 128) << 1;
	sptr->Friction = 4;
	sptr->Flags = 2;
	sptr->Scalar = uchar(kill + 1);
	sptr->sWidth = 1;
	sptr->dWidth = 1;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 0xF) + 16;
}

void inject_civvy(bool replace)
{
	INJECT(0x0040F080, TriggerFenceSparks, replace);
}
