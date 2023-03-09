#include "../tomb3/pch.h"
#include "monkey.h"
#include "box.h"
#include "objects.h"

void InitialiseMonkey(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[MONKEY].anim_index + 2;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = MONKEY_SIT;
	item->goal_anim_state = MONKEY_SIT;
}

void inject_monkey(bool replace)
{
	INJECT(0x004559B0, InitialiseMonkey, replace);
}
