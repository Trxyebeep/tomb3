#include "../tomb3/pch.h"
#include "sub.h"
#include "../specific/init.h"
#include "control.h"

void SubInitialise(short item_number)
{
	ITEM_INFO* item;
	SUBINFO* sub;

	item = &items[item_number];
	sub = (SUBINFO*)game_malloc(sizeof(SUBINFO), 0);
	item->data = sub;
	sub->Rot = 0;
	sub->Vel = 0;
	sub->Flags = 2;
	sub->WeaponTimer = 0;

	for (int i = 0; i < 32; i++)
	{
		SubWakePts[i][0].life = 0;
		SubWakePts[i][1].life = 0;
	}
}

static long GetOnSub(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long dx, dy, dz, dist, h;
	short room_number;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || lara_item->gravity_status)
		return 0;

	item = &items[item_number];
	dy = lara_item->pos.y_pos - item->pos.y_pos - 128;

	if (ABS(dy) > 256)
		return 0;

	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 0x40000)
		return 0;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < -32000)
		return 0;

	return 1;
}

void inject_sub(bool replace)
{
	INJECT(0x004685C0, SubInitialise, replace);
	INJECT(0x00468780, GetOnSub, replace);
}
