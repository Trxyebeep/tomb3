#include "../tomb3/pch.h"
#include "box.h"
#include "objects.h"
#include "../specific/game.h"

void AlertNearbyGuards(ITEM_INFO* item)
{
	ITEM_INFO* target;
	CREATURE_INFO* creature;
	long dx, dy, dz, dist;

	for (int i = 0; i < 5; i++)
	{
		creature = &baddie_slots[i];

		if (creature->item_num == NO_ITEM)
			continue;

		target = &items[creature->item_num];

		if (target->room_number == item->room_number)
		{
			creature->alerted = 1;
			continue;
		}

		dx = (target->pos.x_pos - item->pos.x_pos) >> 6;
		dy = (target->pos.y_pos - item->pos.y_pos) >> 6;
		dz = (target->pos.z_pos - item->pos.z_pos) >> 6;
		dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);

		if (dist < 8000)
			creature->alerted = 1;
	}
}

void InitialiseCreature(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->object_number != ELECTRIC_CLEANER && item->object_number != SHIVA && item->object_number != TARGETS)
		item->pos.y_rot += short((GetRandomControl() - 0x4000) >> 1);

	item->collidable = 1;
	item->data = 0;
}

void inject_box(bool replace)
{
	INJECT(0x00416A30, AlertNearbyGuards, replace);
	INJECT(0x004142E0, InitialiseCreature, replace);
}
