#include "../tomb3/pch.h"
#include "objects.h"
#include "collide.h"
#include "../specific/init.h"

long OnDrawBridge(ITEM_INFO* item, long z, long x)
{
	long ix, iz;

	ix = item->pos.x_pos >> WALL_SHIFT;
	iz = item->pos.z_pos >> WALL_SHIFT;
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;

	if (!item->pos.y_rot && x == ix && (z == iz - 1 || z == iz - 2))
		return 1;

	if (item->pos.y_rot == 0x8000 && x == ix && (z == iz + 1 || z == iz + 2))
		return 1;

	if (item->pos.y_rot == 0x4000 && z == iz && (x == ix - 1 || x == ix - 2))
		return 1;

	if (item->pos.y_rot == -0x4000 && z == iz && (x == ix + 1 || x == ix + 2))
	return 1;

	return 0;
}

void DrawBridgeFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	if (item->current_anim_state == 1 && OnDrawBridge(item, z, x) && y <= item->pos.y_pos)
	{
		OnObject = 1;
		*h = item->pos.y_pos;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void DrawBridgeCeiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	if (item->current_anim_state == 1 && OnDrawBridge(item, z, x) && y > item->pos.y_pos)
		*c = item->pos.y_pos + 256;
}

void DrawBridgeCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (!item->current_anim_state)
		DoorCollision(item_number, l, coll);
}

void InitialiseLift(short item_number)
{
	ITEM_INFO* item;
	LIFT_INFO* lift;

	item = &items[item_number];
	lift = (LIFT_INFO*)game_malloc(sizeof(LIFT_INFO), 0);
	item->data = lift;
	lift->start_height = item->pos.y_pos;
	lift->wait_time = 0;
}

void inject_objects(bool replace)
{
	INJECT(0x00459330, OnDrawBridge, replace);
	INJECT(0x004593F0, DrawBridgeFloor, replace);
	INJECT(0x00459450, DrawBridgeCeiling, replace);
	INJECT(0x00459490, DrawBridgeCollision, replace);
	INJECT(0x004594C0, InitialiseLift, replace);
}
