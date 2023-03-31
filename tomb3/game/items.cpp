#include "../tomb3/pch.h"
#include "items.h"
#include "effect2.h"
#include "objects.h"
#include "demo.h"
#include "control.h"
#include "savegame.h"
#include "lara.h"
#include "effects.h"
#include "../newstuff/map.h"

short next_item_active;
static short next_item_free;
short next_fx_active;
static short next_fx_free;

short body_bag;

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

	if (item->room_number != NO_ROOM)
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
	item->active = 0;
	item->status = ITEM_INACTIVE;
	item->gravity_status = 0;
	item->hit_status = 0;
	item->looked_at = 0;
	item->dynamic_light = 0;
	item->ai_bits = 0;
	item->really_active = 0;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->fallspeed = 0;
	item->speed = 0;
	item->item_flags[0] = 0;
	item->item_flags[1] = 0;
	item->item_flags[2] = 0;
	item->item_flags[3] = 0;
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

void ItemNewRoom(short item_num, short room_num)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	short linknum;

	item = &items[item_num];

	if (item_num == lara.item_number)
	{
		if (!RoomVisited[room_num])
			RoomVisited[room_num] = 1;
	}

	if (item->room_number != NO_ROOM)
	{
		r = &room[item->room_number];
		linknum = r->item_number;

		if (linknum == item_num)
			r->item_number = item->next_item;
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

	item->room_number = room_num;
	item->next_item = room[room_num].item_number;
	room[room_num].item_number = item_num;
}

long GlobalItemReplace(long in, long out)
{
	ITEM_INFO* item;
	ROOM_INFO* r;
	long nReplaced;
	short item_num;

	nReplaced = 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		for (item_num = r->item_number; item_num != NO_ITEM; item_num = item->next_item)
		{
			item = &items[item_num];

			if (item->object_number == in)
			{
				item->object_number = (short)out;
				nReplaced++;
			}
		}
	}

	return nReplaced;
}

void InitialiseFXArray()
{
	FX_INFO* fx;

	fx = effects;
	next_fx_active = NO_ITEM;
	next_fx_free = 0;

	for (int i = 1; i < 50; i++)
	{
		fx->next_fx = i;
		fx++;
	}

	fx->next_fx = NO_ITEM;
}

short CreateEffect(short room_num)
{
	FX_INFO* fx;
	ROOM_INFO* r;
	short fx_num;

	fx_num = next_fx_free;

	if (fx_num != NO_ITEM)
	{
		fx = &effects[fx_num];
		next_fx_free = fx->next_fx;
		r = &room[room_num];
		fx->room_number = room_num;
		fx->next_fx = r->fx_number;
		r->fx_number = fx_num;
		fx->next_active = next_fx_active;
		next_fx_active = fx_num;
		fx->shade = 0x4210;
	}

	return fx_num;
}

void KillEffect(short fx_num)
{
	FX_INFO* fx;
	short linknum;

	DetatchSpark(fx_num, 64);
	fx = &effects[fx_num];

	if (next_fx_active == fx_num)
		next_fx_active = fx->next_active;
	else
	{
		for (linknum = next_fx_active; linknum != NO_ITEM; linknum = effects[linknum].next_active)
		{
			if (effects[linknum].next_active == fx_num)
			{
				effects[linknum].next_active = fx->next_active;
				break;
			}
		}
	}

	linknum = room[fx->room_number].fx_number;

	if (linknum == fx_num)
		room[fx->room_number].fx_number = fx->next_fx;
	else
	{
		for (; linknum != NO_ITEM; linknum = effects[linknum].next_fx)
		{
			if (effects[linknum].next_fx == fx_num)
			{
				effects[linknum].next_fx = fx->next_fx;
				break;
			}
		}
	}

	fx->next_fx = next_fx_free;
	next_fx_free = fx_num;
}

void EffectNewRoom(short fx_num, short room_num)
{
	FX_INFO* fx;
	ROOM_INFO* r;
	short linknum;

	fx = &effects[fx_num];
	r = &room[fx->room_number];

	if (r->fx_number == fx_num)
		r->fx_number = fx->next_fx;
	else
	{
		for (linknum = r->fx_number; linknum != NO_ITEM; linknum = effects[linknum].next_fx)
		{
			if (effects[linknum].next_fx == fx_num)
			{
				effects[linknum].next_fx = fx->next_fx;
				break;
			}
		}
	}

	fx->room_number = room_num;
	fx->next_fx = room[room_num].fx_number;
	room[room_num].fx_number = fx_num;
}

void ClearBodyBag()
{
	ITEM_INFO* item;
	short item_number;

	if (body_bag != NO_ITEM)
	{
		item_number = body_bag;

		while (item_number != NO_ITEM)
		{
			item = &items[item_number];
			KillItem(item_number);
			item_number = item->next_active;
			item->next_active = NO_ITEM;
		}

		body_bag = NO_ITEM;
	}
}
