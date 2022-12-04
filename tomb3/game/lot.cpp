#include "../tomb3/pch.h"
#include "lot.h"
#include "../specific/init.h"
#include "objects.h"

void InitialiseLOTarray()
{
	CREATURE_INFO* creature;

	baddie_slots = (CREATURE_INFO*)game_malloc(5 * sizeof(CREATURE_INFO), 33);

	for (int i = 0; i < 5; i++)
	{
		creature = &baddie_slots[i];
		creature->item_num = NO_ITEM;
		creature->LOT.node = (BOX_NODE*)game_malloc(sizeof(BOX_NODE) * number_boxes, 34);
	}

	slots_used = 0;

	non_lot_slots = (CREATURE_INFO*)game_malloc(12 * sizeof(CREATURE_INFO), 33);

	for (int i = 0; i < 12; i++)
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
		lara.creature = &non_lot_slots[slot];
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

void inject_lot(bool replace)
{
	INJECT(0x00452F10, InitialiseLOTarray, replace);
	INJECT(0x00453740, InitialiseNonLotAI, replace);
}
