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

void inject_objects(bool replace)
{
	INJECT(0x00459330, OnDrawBridge, replace);
}
