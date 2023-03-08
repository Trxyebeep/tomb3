#include "../tomb3/pch.h"
#include "swat.h"
#include "box.h"

void InitialiseSwat(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[item->object_number].anim_index + 12;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = 1;
	item->goal_anim_state = 1;
}

void inject_swat(bool replace)
{
	INJECT(0x00469F80, InitialiseSwat, replace);
}
