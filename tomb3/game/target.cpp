#include "../tomb3/pch.h"
#include "target.h"
#include "items.h"

void InitialiseTarget(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->active)
		RemoveActiveItem(item_number);

	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = anims[item->anim_number].current_anim_state;
	item->goal_anim_state = item->current_anim_state;
	item->required_anim_state = 0;
	item->active = 0;
	item->status = ITEM_INACTIVE;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->item_flags[2] = 0;
	item->timer = 0;
	item->flags = 0;
	item->hit_points = objects[item->object_number].hit_points;
	item->data = 0;
}

void inject_target(bool replace)
{
	INJECT(0x0046A8B0, InitialiseTarget, replace);
}
