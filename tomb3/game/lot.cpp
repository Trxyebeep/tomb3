#include "../tomb3/pch.h"
#include "lot.h"
#include "../specific/init.h"
#include "objects.h"
#include "box.h"
#include "control.h"
#include "camera.h"
#include "lara.h"

long slots_used;
CREATURE_INFO* baddie_slots;
static long nonlot_slots_used;
static CREATURE_INFO* non_lot_slots;

void InitialiseLOTarray()
{
	CREATURE_INFO* creature;

	baddie_slots = (CREATURE_INFO*)game_malloc(MAX_LOT * sizeof(CREATURE_INFO));

	for (int i = 0; i < MAX_LOT; i++)
	{
		creature = &baddie_slots[i];
		creature->item_num = NO_ITEM;
		creature->LOT.node = (BOX_NODE*)game_malloc(sizeof(BOX_NODE) * number_boxes);
	}

	slots_used = 0;

	non_lot_slots = (CREATURE_INFO*)game_malloc(MAX_NONLOT * sizeof(CREATURE_INFO));

	for (int i = 0; i < MAX_NONLOT; i++)
	{
		creature = &non_lot_slots[i];
		creature->item_num = NO_ITEM;
	}

	nonlot_slots_used = 0;
}

void InitialiseNonLotAI(short item_number, long slot)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	item = &items[item_number];
	creature = &non_lot_slots[slot];

	if (item_number == lara.item_number)
		lara.creature = creature;
	else
		item->data = creature;

	creature->item_num = item_number;
	creature->mood = BORED_MOOD;
	creature->joint_rotation[0] = 0;
	creature->joint_rotation[1] = 0;
	creature->joint_rotation[2] = 0;
	creature->joint_rotation[3] = 0;
	creature->alerted = 0;
	creature->head_left = 0;
	creature->head_right = 0;
	creature->reached_goal = 0;
	creature->hurt_by_lara = 0;
	creature->patrol2 = 0;
	creature->maximum_turn = 182;
	creature->flags = 0;
	creature->enemy = 0;
	creature->LOT.step = 256;
	creature->LOT.drop = -512;
	creature->LOT.block_mask = 0x4000;
	creature->LOT.fly = 0;

	switch (item->object_number)
	{
	case LARA:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 256;
		break;

	case WHALE:
	case DIVER:
	case CROW:
	case VULTURE:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 16;

		if (item->object_number == WHALE)
			creature->LOT.block_mask = 0x8000;

		break;

	case LIZARD_MAN:
	case MP1:
		creature->LOT.step = 1024;
		creature->LOT.drop = -1024;
		break;
	}

	nonlot_slots_used++;
}

long EnableNonLotAI(short item_number, long Always)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;
	long x, y, z, slot, worstslot, dist, worstdist;

	item = &items[item_number];

	if (nonlot_slots_used < MAX_NONLOT)
	{
		for (int i = 0; i < MAX_NONLOT; i++)
		{
			creature = &non_lot_slots[i];

			if (creature->item_num == NO_ITEM)
			{
				InitialiseNonLotAI(item_number, i);
				return 1;
			}
		}
	}

	if (Always)
		worstdist = 0;
	else
	{
		x = (item->pos.x_pos - camera.pos.x) >> 8;
		y = (item->pos.y_pos - camera.pos.y) >> 8;
		z = (item->pos.z_pos - camera.pos.z) >> 8;
		worstdist = SQUARE(x) + SQUARE(y) + SQUARE(z);
	}

	worstslot = -1;

	for (slot = 0; slot < MAX_NONLOT; slot++)
	{
		creature = &non_lot_slots[slot];
		item = &items[creature->item_num];
		x = (item->pos.x_pos - camera.pos.x) >> 8;
		y = (item->pos.y_pos - camera.pos.y) >> 8;
		z = (item->pos.z_pos - camera.pos.z) >> 8;
		dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

		if (dist > worstdist)
		{
			worstslot = slot;
			worstdist = dist;
		}
	}

	if (worstslot >= 0)
	{
		items[non_lot_slots[worstslot].item_num].status = ITEM_INVISIBLE;
		DisableBaddieAI(non_lot_slots[worstslot].item_num);
		InitialiseNonLotAI(item_number, worstslot);
		return 1;
	}

	return 0;
}

void DisableBaddieAI(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	item = &items[item_number];

	if (item_number == lara.item_number)
	{
		creature = lara.creature;
		lara.creature = 0;
	}
	else
	{
		creature = (CREATURE_INFO*)item->data;
		item->data = 0;
	}

	if (creature)
	{
		creature->item_num = NO_ITEM;

		if (objects[item->object_number].non_lot)
			nonlot_slots_used--;
		else
			slots_used--;
	}
}

void ClearLOT(LOT_INFO* lot)
{
	BOX_NODE* node;

	lot->tail = 2047;
	lot->head = 2047;
	lot->search_number = 0;
	lot->target_box = 2047;
	lot->required_box = 2047;
	node = lot->node;

	for (int i = 0; i < number_boxes; i++)
	{
		node->next_expansion = 2047;
		node->exit_box = 2047;
		node->search_number = 0;
		node++;
	}
}

void CreateZone(ITEM_INFO* item)
{
	CREATURE_INFO* creature;
	ROOM_INFO* r;
	BOX_NODE* node;
	short* zone;
	short* flip;
	short zone_number, flip_number;

	creature = (CREATURE_INFO*)item->data;
	r = &room[item->room_number];
	item->box_number = r->floor[((item->pos.z_pos - r->z) >> WALL_SHIFT) + r->x_size * ((item->pos.x_pos - r->x) >> WALL_SHIFT)].box;

	if (creature->LOT.fly)
	{
		creature->LOT.zone_count = 0;
		node = creature->LOT.node;

		for (int i = 0; i < number_boxes; i++)
		{
			node->box_number = i;
			node++;
			creature->LOT.zone_count++;
		}
	}
	else
	{
		zone = ground_zone[(creature->LOT.step >> 8) - 1][0];
		flip = ground_zone[(creature->LOT.step >> 8) - 1][1];
		zone_number = zone[item->box_number];
		flip_number = flip[item->box_number];
		creature->LOT.zone_count = 0;
		node = creature->LOT.node;

		for (int i = 0; i < number_boxes; i++)
		{
			if (*zone == zone_number || *flip == flip_number)
			{
				node->box_number = i;
				node++;
				creature->LOT.zone_count++;
			}

			zone++;
			flip++;
		}
	}
}

void InitialiseSlot(short item_number, long slot)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;

	item = &items[item_number];
	creature = &baddie_slots[slot];

	if (item_number == lara.item_number)
		lara.creature = creature;
	else
		item->data = creature;

	creature->item_num = item_number;
	creature->mood = BORED_MOOD;
	creature->joint_rotation[0] = 0;
	creature->joint_rotation[1] = 0;
	creature->joint_rotation[2] = 0;
	creature->joint_rotation[3] = 0;
	creature->alerted = 0;
	creature->head_left = 0;
	creature->head_right = 0;
	creature->reached_goal = 0;
	creature->hurt_by_lara = 0;
	creature->patrol2 = 0;
	creature->maximum_turn = 182;
	creature->flags = 0;
	creature->enemy = 0;
	creature->LOT.step = 256;
	creature->LOT.drop = -512;
	creature->LOT.block_mask = 0x4000;
	creature->LOT.fly = 0;

	switch (item->object_number)
	{
	case LARA:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 256;
		break;

	case WHALE:
	case DIVER:
	case CROW:
	case VULTURE:
	case CROCODILE:
	case MUTANT1:
		creature->LOT.step = 20480;
		creature->LOT.drop = -20480;
		creature->LOT.fly = 16;

		if (item->object_number == WHALE)
			creature->LOT.block_mask = 0x8000;

		break;

	case LIZARD_MAN:
	case WILLARD_BOSS:
	case PUNK1:
	case MP1:
	case BOB:
	case CIVVIE:
	case MONKEY:
		creature->LOT.step = 1024;
		creature->LOT.drop = -1024;
		break;

	case LON_BOSS:
		creature->LOT.step = 1024;
		creature->LOT.drop = -768;
		break;

	case SHIVA:
	case TREX:
		creature->LOT.block_mask = 0x8000;
		break;
	}

	ClearLOT(&creature->LOT);

	if (item_number != lara.item_number)
		CreateZone(item);

	slots_used++;
}

long EnableBaddieAI(short item_number, long Always)
{
	ITEM_INFO* item;
	CREATURE_INFO* creature;
	long x, y, z, slot, worstslot, dist, worstdist;

	item = &items[item_number];

	if (lara.item_number == item_number)
	{
		if (lara.creature)
			return 1;
	}
	else
	{
		if (item->data)
			return 1;

		if (objects[item->object_number].non_lot)
			return EnableNonLotAI(item_number, Always);
	}

	if (slots_used < MAX_LOT)
	{
		for (slot = 0; slot < MAX_LOT; slot++)
		{
			creature = &baddie_slots[slot];

			if (creature->item_num == NO_ITEM)
			{
				InitialiseSlot(item_number, slot);
				return 1;
			}
		}
	}

	if (Always)
		worstdist = 0;
	else
	{
		x = (item->pos.x_pos - camera.pos.x) >> 8;
		y = (item->pos.y_pos - camera.pos.y) >> 8;
		z = (item->pos.z_pos - camera.pos.z) >> 8;
		worstdist = SQUARE(x) + SQUARE(y) + SQUARE(z);
	}

	worstslot = -1;

	for (slot = 0; slot < MAX_LOT; slot++)
	{
		creature = &baddie_slots[slot];
		item = &items[creature->item_num];
		x = (item->pos.x_pos - camera.pos.x) >> 8;
		y = (item->pos.y_pos - camera.pos.y) >> 8;
		z = (item->pos.z_pos - camera.pos.z) >> 8;
		dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

		if (dist > worstdist)
		{
			worstslot = slot;
			worstdist = dist;
		}
	}

	if (worstslot >= 0)
	{
		items[baddie_slots[worstslot].item_num].status = ITEM_INVISIBLE;
		DisableBaddieAI(baddie_slots[worstslot].item_num);
		InitialiseSlot(item_number, worstslot);
		return 1;
	}

	return 0;
}
