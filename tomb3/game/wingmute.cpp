#include "../tomb3/pch.h"
#include "wingmute.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"
#include "effect2.h"

static void TriggerWingMuteParticles(short item_number)
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
	sptr->sG = (GetRandomControl() & 0x3F) + 32;
	sptr->sB = sptr->sG >> 1;
	sptr->sR = sptr->sG >> 2;
	sptr->dG = (GetRandomControl() & 0x1F) + 224;
	sptr->dB = sptr->dG >> 1;
	sptr->dR = sptr->dG >> 2;
	sptr->Life = 8;
	sptr->sLife = 8;
	sptr->ColFadeSpeed = 4;
	sptr->FadeToBlack = 2;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) - 8;
	sptr->y = (GetRandomControl() & 0xF) - 8;
	sptr->z = (GetRandomControl() & 0x7F) - 64;
	sptr->Xvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Yvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Zvel = (GetRandomControl() & 0x1F) - 16;
	sptr->Flags = SF_ATTACHEDNODE | SF_ITEM | SF_DEF | SF_SCALE;
	sptr->Friction = 34;
	sptr->MaxYvel = 0;
	sptr->Gravity = 0;
	sptr->FxObj = (uchar)item_number;
	sptr->NodeNumber = 1;
	sptr->Scalar = 3;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Width = (GetRandomControl() & 3) + 3;
	sptr->sWidth = sptr->Width;
	sptr->dWidth = sptr->Width >> 1;
	sptr->Height = sptr->Width;
	sptr->sHeight = sptr->Height;
	sptr->dHeight = sptr->Height >> 1;
}

void InitialiseWingmute(short item_number)
{
	ITEM_INFO* item;

	InitialiseCreature(item_number);
	item = &items[item_number];
	item->anim_number = objects[MUTANT1].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = WING_WAIT;
	item->goal_anim_state = WING_WAIT;
	item->item_flags[1] = GetRandomControl() & 0x7F;
}

void inject_wingmute(bool replace)
{
	INJECT(0x00473BA0, TriggerWingMuteParticles, replace);
	INJECT(0x00473780, InitialiseWingmute, replace);
}
