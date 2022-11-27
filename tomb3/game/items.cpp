#include "../tomb3/pch.h"
#include "items.h"
#include "effect2.h"

void InitialiseItemArray(short num_items)
{
	ITEM_INFO* item;

	item = &items[level_items];
	next_item_free = (short)level_items;
	body_bag = NO_ITEM;
	next_item_active = NO_ITEM;

	for (int i = level_items + 1; i < num_items; i++)
	{
		item->next_item = i;
		item->active = 0;
		item->il.init = 0;
		item++;
	}

	item->next_item = NO_ITEM;
}

void KillItem(short item_num)
{
	ITEM_INFO* item;
	short linknum;

	DetatchSpark(item_num, 128);
	item = &items[item_num];
	item->active = 0;
	item->really_active = 0;

	if (next_item_active == item_num)
		next_item_active = item->next_active;
	else
	{
		for (linknum = next_item_active; linknum != NO_ITEM; linknum = items[linknum].next_active)
		{
			if (items[linknum].next_active == item_num)
			{
				items[linknum].next_active = item->next_active;
				break;
			}
		}
	}

	if (item->room_number != 255)
	{
		linknum = room[item->room_number].item_number;

		if (linknum == item_num)
			room[item->room_number].item_number = item->next_item;
		else
		{
			for (; linknum != NO_ITEM; linknum = items[linknum].next_item)
			{
				if (items[linknum].next_item == item_num)
				{
					items[linknum].next_item = item->next_item;
					break;
				}
			}
		}
	}

	if (item == lara.target)
		lara.target = 0;

	if (item_num < level_items)
		item->flags |= IFL_CLEARBODY;
	else
	{
		item->next_item = next_item_free;
		next_item_free = item_num;
	}
}

short CreateItem()
{
	short item_number;

	item_number = next_item_free;

	if (item_number != NO_ITEM)
	{
		items[item_number].flags = 0;
		items[item_number].il.init = 0;
		next_item_free = items[item_number].next_item;
	}

	return item_number;
}

void InitialiseItem(short item_num)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	FLOOR_INFO* floor;

	item = &items[item_num];
	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = anims[item->anim_number].current_anim_state;
	item->goal_anim_state = anims[item->anim_number].current_anim_state;
	item->required_anim_state = 0;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->fallspeed = 0;
	item->speed = 0;
	item->item_flags[3] = 0;
	item->item_flags[2] = 0;
	item->item_flags[1] = 0;
	item->item_flags[0] = 0;
	item->hit_points = objects[item->object_number].hit_points;
	item->collidable = 1;
	item->clear_body = 0;
	item->timer = 0;
	item->mesh_bits = -1;
	item->touch_bits = 0;
	item->after_death = 0;
	item->il.init = 0;
	item->fired_weapon = 0;
	item->data = 0;

	if (item->flags & IFL_INVISIBLE)
	{
		item->status = ITEM_INVISIBLE;
		item->flags -= IFL_INVISIBLE;
	}
	else if (objects[item->object_number].intelligent)
		item->status = ITEM_INVISIBLE;

	if (item->flags & IFL_CLEARBODY)
	{
		item->clear_body = 1;
		item->flags -= IFL_CLEARBODY;
	}

	if ((item->flags & IFL_CODEBITS) == IFL_CODEBITS)
	{
		item->flags -= IFL_CODEBITS;
		item->flags |= IFL_REVERSE;
		AddActiveItem(item_num);
		item->status = ITEM_ACTIVE;
	}

	r = &room[item->room_number];
	item->next_item = r->item_number;
	r->item_number = item_num;
	floor = &r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)];
	item->floor = floor->floor << 8;
	item->box_number = floor->box;

	if (savegame.bonus_flag && !DemoPlay)
		item->hit_points *= 2;

	if (objects[item->object_number].initialise)
		objects[item->object_number].initialise(item_num);
}

void RemoveActiveItem(short item_num)
{
	short linknum;

	if (!items[item_num].active)
		return;

	items[item_num].active = 0;

	if (next_item_active == item_num)
		next_item_active = items[item_num].next_active;
	else
	{
		for (linknum = next_item_active; linknum != NO_ITEM; linknum = items[linknum].next_active)
		{
			if (items[linknum].next_active == item_num)
			{
				items[linknum].next_active = items[item_num].next_active;
				break;
			}
		}
	}
}

void RemoveDrawnItem(short item_num)
{
	ITEM_INFO* item;
	short linknum;

	item = &items[item_num];
	linknum = room[item->room_number].item_number;

	if (linknum == item_num)
		room[item->room_number].item_number = item->next_item;
	else
	{
		for (; linknum != NO_ITEM; linknum = items[linknum].next_item)
		{
			if (items[linknum].next_item == item_num)
			{
				items[linknum].next_item = item->next_item;
				break;
			}
		}
	}
}

void AddActiveItem(short item_num)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (objects[item->object_number].control)
	{
		if (!item->active)
		{
			item->active = 1;
			item->next_active = next_item_active;
			next_item_active = item_num;
		}
	}
	else
		item->status = ITEM_INACTIVE;
}

void inject_items(bool replace)
{
	INJECT(0x0043AA20, InitialiseItemArray, replace);
	INJECT(0x0043AA90, KillItem, replace);
	INJECT(0x0043ABE0, CreateItem, replace);
	INJECT(0x0043AC30, InitialiseItem, replace);
	INJECT(0x0043AE40, RemoveActiveItem, replace);
	INJECT(0x0043AEE0, RemoveDrawnItem, replace);
	INJECT(0x0043AF60, AddActiveItem, replace);
}
