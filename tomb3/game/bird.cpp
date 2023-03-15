#include "../tomb3/pch.h"
#include "bird.h"
#include "box.h"
#include "objects.h"

void InitialiseVulture(short item_number)
{
	ITEM_INFO* item;

	InitialiseCreature(item_number);
	item = &items[item_number];

	if (item->object_number == CROW)
	{
		item->anim_number = objects[CROW].anim_index + 14;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = BIRD_EAT;
		item->goal_anim_state = BIRD_EAT;
	}
	else
	{
		item->anim_number = objects[VULTURE].anim_index + 5;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = BIRD_STOP;
		item->goal_anim_state = BIRD_STOP;
	}
}

void inject_bird(bool replace)
{
	INJECT(0x00411450, InitialiseVulture, replace);
}
