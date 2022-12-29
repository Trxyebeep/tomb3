#include "../tomb3/pch.h"
#include "moveblok.h"
#include "control.h"

void ClearMovableBlockSplitters(long x, long y, long z, short room_number)
{
	FLOOR_INFO* floor;
	short room_num, height;

	floor = GetFloor(x, y, z, &room_number);
	boxes[floor->box].overlap_index &= ~0x4000;
	height = boxes[floor->box].height;
	room_num = room_number;
	floor = GetFloor(x + WALL_SIZE, y, z, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x + WALL_SIZE, y, z, room_number);
	}

	room_number = room_num;
	floor = GetFloor(x - WALL_SIZE, y, z, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x - WALL_SIZE, y, z, room_number);
	}

	room_number = room_num;
	floor = GetFloor(x, y, z + WALL_SIZE, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x, y, z + WALL_SIZE, room_number);
	}

	room_number = room_num;
	floor = GetFloor(x, y, z - WALL_SIZE, &room_number);

	if (floor->box != 0x7FF)
	{
		if (boxes[floor->box].height == height && boxes[floor->box].overlap_index & 0x8000 && boxes[floor->box].overlap_index & 0x4000)
			ClearMovableBlockSplitters(x, y, z - WALL_SIZE, room_number);
	}
}

void inject_moveblok(bool replace)
{
	INJECT(0x00456BA0, ClearMovableBlockSplitters, replace);
}
