#include "../tomb3/pch.h"
#include "cleaner.h"
#include "effect2.h"
#include "../specific/game.h"

static void TriggerElectricSparks(PHD_VECTOR* pos, short item_number, short Node)
{
	SPARKS* sptr;
	long dx, dz;

	dx = lara_item->pos.x_pos - pos->x;
	dz = lara_item->pos.z_pos - pos->z;

	if (dx < -0x5000 || dx > 0x5000 || dz < -0x5000 || dz > 0x5000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = (GetRandomControl() & 0x3F) + 192;
	sptr->sG = sptr->sR;
	sptr->sB = sptr->sR;
	sptr->dR = sptr->sB >> 2;
	sptr->dG = sptr->sB >> 1;
	sptr->dB = (GetRandomControl() & 0x3F) + 192;
	sptr->ColFadeSpeed = 8;
	sptr->FadeToBlack = 8;
	sptr->TransType = 2;
	sptr->Dynamic = -1;
	sptr->Life = (GetRandomControl() & 7) + 20;
	sptr->sLife = sptr->Life;
	sptr->x = (GetRandomControl() & 0x1F) - 16;
	sptr->y = (GetRandomControl() & 0x1F) - 16;
	sptr->z = (GetRandomControl() & 0x1F) - 16;
	sptr->Xvel = ((GetRandomControl() & 0xFF) << 2) - 512;
	sptr->Yvel = (GetRandomControl() & 7) - 4;
	sptr->Zvel = ((GetRandomControl() & 0xFF) << 2) - 512;
	sptr->Friction = 4;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_SCALE;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = (uchar)Node;
	sptr->Scalar = 1;
	sptr->Width = (GetRandomControl() & 3) + 4;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 1;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 1;
	sptr->MaxYvel = 0;
	sptr->Gravity = (GetRandomControl() & 3) + 4;
}

void inject_cleaner(bool replace)
{
	INJECT(0x0041D0B0, TriggerElectricSparks, replace);
}
