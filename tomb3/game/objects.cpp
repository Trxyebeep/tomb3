#include "../tomb3/pch.h"
#include "objects.h"

long OnDrawBridge(ITEM_INFO* item, long x, long z)
{
	long ix, iz;

	ix = item->pos.x_pos >> WALL_SHIFT;
	iz = item->pos.z_pos >> WALL_SHIFT;
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;

	if (!item->pos.y_rot && z == ix && (x == iz - 1 || x == iz - 2))
		return 1;

	if (item->pos.y_rot == 0x8000 && z == ix && (x == iz + 1 || x == iz + 2))
		return 1;

	if (item->pos.y_rot == 0x4000 && x == iz && (z == ix - 1 || z == ix - 2))
		return 1;

	if (item->pos.y_rot == -0x4000 && x == iz && (z == ix + 1 || z == ix + 2))
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

void inject_objects(bool replace)
{
	INJECT(0x00459330, OnDrawBridge, replace);
	INJECT(0x004593F0, DrawBridgeFloor, replace);
}
