#include "../tomb3/pch.h"
#include "minecart.h"
#include "../specific/init.h"
#include "collide.h"
#include "sphere.h"
#include "control.h"

void MineCartInitialise(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->data = game_malloc(sizeof(CARTINFO), 0);
}

static long GetInMineCart(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	long dx, dz, dist, h;
	short room_number;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || l->gravity_status ||
		!TestBoundsCollide(item, l, coll->radius) || !TestCollision(item, l))
		return 0;

	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 200000)
		return 0;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < -32000)
		return 0;

	return 1;
}

void inject_minecart(bool replace)
{
	INJECT(0x00453930, MineCartInitialise, replace);
	INJECT(0x00453AB0, GetInMineCart, replace);
}
