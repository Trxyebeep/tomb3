#include "../tomb3/pch.h"
#include "effect2.h"
#include "objects.h"
#include "items.h"

void TriggerDynamic(long x, long y, long z, long falloff, long r, long g, long b)
{
	DYNAMIC* dl;

	if (number_dynamics != 32)
	{
		dl = &dynamics[number_dynamics];
		dl->x = x;
		dl->y = y;
		dl->z = z;
		dl->falloff = ushort(falloff << 8);
		dl->on = 1;
		dl->r = (uchar)r;
		dl->g = (uchar)g;
		dl->b = (uchar)b;
		number_dynamics++;
	}
}

void ClearDynamics()
{
	number_dynamics = 0;

	for (int i = 0; i < 32; i++)
		dynamics[i].on = 0;
}

void KillEverything()
{
	ITEM_INFO* item;
	FX_INFO* fx;
	short item_number, nex, obj_num;

	item_number = next_item_active;

	while (item_number != NO_ITEM)
	{
		item = &items[item_number];
		nex = item->next_active;
		obj_num = item->object_number;

		if (obj_num != LARA && obj_num != FLARE && (obj_num < DOOR_TYPE1 || obj_num > DOOR_TYPE8) && item->active && !(item->flags & IFL_REVERSE))
			KillItem(item_number);

		item_number = nex;
	}

	item_number = next_fx_active;

	while (item_number != NO_ITEM)
	{
		fx = &effects[item_number];
		nex = fx->next_active;
		obj_num = fx->object_number;

		if (objects[obj_num].control && (obj_num != FLAME || fx->counter >= 0))
			KillEffect(item_number);

		item_number = nex;
	}

	for (int i = 0; i < 8; i++)
		lead_info[i].on = 0;

	KillEverythingFlag = 0;
}

void inject_effect2(bool replace)
{
	INJECT(0x0042DE00, TriggerDynamic, replace);
	INJECT(0x0042DE60, ClearDynamics, replace);
	INJECT(0x0042D9A0, KillEverything, replace);
}
