#include "../tomb3/pch.h"
#include "51baton.h"
#include "box.h"
#include "objects.h"

void InitialiseBaton(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[MP1].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = BATON_STOP;
	item->goal_anim_state = BATON_STOP;
}

void inject_51baton(bool replace)
{
	INJECT(0x0040D5F0, InitialiseBaton, replace);
}
