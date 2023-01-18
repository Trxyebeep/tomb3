#include "../tomb3/pch.h"
#include "wingmute.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"

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
	INJECT(0x00473780, InitialiseWingmute, replace);
}
