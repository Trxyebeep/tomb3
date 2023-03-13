#include "../tomb3/pch.h"
#include "lasers.h"
#include "control.h"

static void LaserSplitterToggle(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	long active, x, z, xStep, zStep;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (!(boxes[floor->box].overlap_index & 0x8000))
		return;

	active = TriggerActive(item);

	if (active == ((boxes[floor->box].overlap_index & 0x4000) == 0x4000))
		return;

	if (!item->pos.y_rot)
	{
		xStep = 0;
		zStep = -1024;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		xStep = -1024;
		zStep = 0;
	}
	else if (item->pos.y_rot == -0x8000)
	{
		xStep = 0;
		zStep = 1024;
	}
	else
	{
		xStep = 1024;
		zStep = 0;
	}

	x = item->pos.x_pos;
	z = item->pos.z_pos;

	while (floor->box != 2047 && boxes[floor->box].overlap_index & 0x8000)
	{
		if (active)
			boxes[floor->box].overlap_index |= 0x4000;
		else
			boxes[floor->box].overlap_index &= ~0x4000;

		x += xStep;
		z += zStep;
		floor = GetFloor(x, item->pos.y_pos, z, &room_number);
	}
}

void inject_lasers(bool replace)
{
	INJECT(0x0044F830, LaserSplitterToggle, replace);
}
