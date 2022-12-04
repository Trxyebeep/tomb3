#include "../tomb3/pch.h"
#include "lot.h"
#include "../specific/init.h"

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

void inject_lot(bool replace)
{
	INJECT(0x00452F10, InitialiseLOTarray, replace);
}
