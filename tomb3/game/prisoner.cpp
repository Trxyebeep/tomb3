#include "../tomb3/pch.h"
#include "prisoner.h"
#include "box.h"
#include "objects.h"

void InitialisePrisoner(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[BOB].anim_index + 6;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = BOB_STOP;
	item->goal_anim_state = BOB_STOP;
}

void inject_prisoner(bool replace)
{
	INJECT(0x0045CFF0, InitialisePrisoner, replace);
}
