#include "../tomb3/pch.h"
#include "autogun.h"
#include "objects.h"

void InitialiseAutogun(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->anim_number = objects[ROBOT_SENTRY_GUN].anim_index + 1;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = AUTOGUN_STILL;
	item->goal_anim_state = AUTOGUN_STILL;
	item->item_flags[0] = 0;
}

void inject_autogun(bool replace)
{
	INJECT(0x00410750, InitialiseAutogun, replace);
}
