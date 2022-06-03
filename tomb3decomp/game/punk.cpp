#include "../tomb3/pch.h"
#include "punk.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "box.h"

static void TriggerPunkFlame(short item_number)
{
	SPARKS* sptr;
	long dx, dz;
	uchar size;

	dx = lara_item->pos.x_pos - items[item_number].pos.x_pos;
	dz = lara_item->pos.z_pos - items[item_number].pos.z_pos;

	if (dx < -0x4000 || dx > 0x4000 || dz < -0x4000 || dz > 0x4000)
		return;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 255;
	sptr->sG = (GetRandomControl() & 0x1F) + 48;
	sptr->sB = 48;
	sptr->dR = (GetRandomControl() & 0x3F) + 192;
	sptr->dG = (GetRandomControl() & 0x3F) + 128;
	sptr->dB = 32;

	sptr->FadeToBlack = 8;
	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 12;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 7) + 24;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = 0;
	sptr->z = (GetRandomControl() & 0xF) - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = -16 - (GetRandomControl() & 0xF);
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Friction = 5;
	
	if (GetRandomControl() & 1)
	{
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->Flags = 4762;
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
	{
		sptr->Flags = 4746;
		sptr->Gravity = -16 - (GetRandomControl() & 0x1F);
		sptr->MaxYvel = -16 - (GetRandomControl() & 7);
	}

	sptr->NodeNumber = 2;
	sptr->FxObj = (uchar)item_number;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Scalar = 1;
	size = (GetRandomControl() & 0x1F) + 64;
	sptr->sWidth = size;
	sptr->Width = size;
	sptr->sHeight = size;
	sptr->Height = size;
	size >>= 2;
	sptr->dWidth = size;
	sptr->dHeight = size;
}

void InitialisePunk(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[PUNK1].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void inject_punk(bool replace)
{
	INJECT(0x0045E5E0, TriggerPunkFlame, replace);
	INJECT(0x0045DBD0, InitialisePunk, replace);
}
