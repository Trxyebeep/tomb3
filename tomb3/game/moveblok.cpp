#include "../tomb3/pch.h"
#include "moveblok.h"
#include "control.h"
#include "collide.h"
#include "draw.h"

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

void AlterFloorHeight(ITEM_INFO* item, long height)
{
	FLOOR_INFO* floor;
	FLOOR_INFO* ceiling;
	short room_num;

	room_num = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
	ceiling = GetFloor(item->pos.x_pos, item->pos.y_pos + height - WALL_SIZE, item->pos.z_pos, &room_num);

	if (floor->floor == -127)
		floor->floor = ceiling->ceiling + char(height >> 8);
	else
	{
		floor->floor += char(height >> 8);

		if (floor->floor == ceiling->ceiling)
			floor->floor = -127;
	}

	if (boxes[floor->box].overlap_index & 0x8000)
	{
		if (height >= 0)
			boxes[floor->box].overlap_index &= ~0x4000;
		else
			boxes[floor->box].overlap_index |= 0x4000;
	}
}

static long TestBlockMovable(ITEM_INFO* item, long blockhite)
{
	FLOOR_INFO* floor;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (floor->floor == -127 || floor->floor << 8 == item->pos.y_pos - blockhite)
		return 1;

	return 0;
}

static long TestBlockPush(ITEM_INFO* item, long blockhite, ushort quadrant)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	COLL_INFO coll;
	long x, y, z;
	short room_number;

	if (!TestBlockMovable(item, blockhite))
		return 0;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	room_number = item->room_number;

	switch (quadrant)
	{
	case NORTH:
		z += WALL_SIZE;
		break;

	case EAST:
		x += WALL_SIZE;
		break;

	case SOUTH:
		z -= WALL_SIZE;
		break;

	case WEST:
		x -= WALL_SIZE;
		break;
	}

	floor = GetFloor(x, y, z, &room_number);
	r = &room[room_number];

	if (r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper)
		return 0;

	coll.quadrant = quadrant;
	coll.radius = 500;

	if (CollideStaticObjects(&coll, x, y, z, room_number, 1000))
		return 0;

	if (floor->floor << 8 != y)
		return 0;

	GetHeight(floor, x, y, z);

	if (height_type != WALL)
		return 0;

	y -= blockhite - 100;
	floor = GetFloor(x, y, z, &room_number);

	if (GetCeiling(floor, x, y, z) > y)
		return 0;

	item->item_flags[0] = lara_item->pos.y_rot;
	return 1;
}

static long TestBlockPull(ITEM_INFO* item, long blockhite, ushort quadrant)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	COLL_INFO coll;
	long x, y, y2, z, xAdd, zAdd;
	short room_number;

	if (!TestBlockMovable(item, blockhite))
		return 0;

	xAdd = 0;
	zAdd = 0;

	switch (quadrant)
	{
	case NORTH:
		zAdd = -WALL_SIZE;
		break;

	case EAST:
		xAdd = -WALL_SIZE;
		break;

	case SOUTH:
		zAdd = WALL_SIZE;
		break;

	case WEST:
		xAdd = WALL_SIZE;
		break;
	}

	x = item->pos.x_pos + xAdd;
	y = item->pos.y_pos;
	z = item->pos.z_pos + zAdd;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	r = &room[room_number];

	if (r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper)
		return 0;

	coll.quadrant = quadrant;
	coll.radius = 500;

	if (CollideStaticObjects(&coll, x, y, z, room_number, 1000))
		return 0;

	if (floor->floor << 8 != y)
		return 0;

	y2 = y - blockhite;
	floor = GetFloor(x, y2, z, &room_number);

	if (floor->ceiling << 8 > y2)
		return 0;

	x += xAdd;
	z += zAdd;
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);

	if (floor->floor << 8 != y)
		return 0;

	y2 = y - 762;
	floor = GetFloor(x, y2, z, &room_number);

	if (floor->ceiling << 8 > y2)
		return 0;

	x = lara_item->pos.x_pos + xAdd;
	y = lara_item->pos.y_pos;
	z = lara_item->pos.z_pos + zAdd;
	room_number = lara_item->room_number;
	GetFloor(x, y, z, &room_number);

	coll.quadrant = (quadrant - 2) & 3;
	coll.radius = 100;

	if (CollideStaticObjects(&coll, x, y, z, room_number, 762))
		return 0;

	item->item_flags[0] = lara_item->pos.y_rot + 0x8000;
	return 1;
}

void DrawUnclippedItem(ITEM_INFO* item)
{
	long t, b, l, r;

	t = phd_top;
	l = phd_left;
	b = phd_bottom;
	r = phd_right;
	phd_top = 0;
	phd_left = 0;
	phd_bottom = phd_winymax;
	phd_right = phd_winxmax;
	DrawAnimatingItem(item);
	phd_top = t;
	phd_left = l;
	phd_bottom = b;
	phd_right = r;
}

void SetupCleanerFromSavegame(ITEM_INFO* item, long block)
{
	ROOM_INFO* r;
	long x, y, z, xAdd, zAdd;
	short room_number;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;

	if (!block && (x & 0x3FF) == 512 && (z & 0x3FF) == 512)
	{
		room_number = item->room_number;
		GetFloor(x, y, z, &room_number);
		r = &room[room_number];
		r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 1;
		return;
	}

	if (!item->pos.y_rot)
	{
		if (!block)
		{
			if ((z & 0x3FF) < 512)
				z = (z & ~0x3FF) - 512;
			else
				z = (z & ~0x3FF) + 512;
		}

		xAdd = 0;
		zAdd = 1024;
	}
	else if (item->pos.y_rot == 0x4000)
	{
		if (!block)
		{
			if ((x & 0x3FF) < 512)
				x = (x & ~0x3FF) - 512;
			else
				x = (x & ~0x3FF) + 512;
		}

		xAdd = 1024;
		zAdd = 0;
	}
	else if (item->pos.y_rot == 0x8000)
	{
		if (!block)
		{
			if ((z & 0x3FF) > 512)
				z |= 0x3FF;
			else
				z &= ~0x3FF;

			z += 512;
		}

		xAdd = 0;
		zAdd = -1024;
	}
	else
	{
		if (!block)
		{
			if ((x & 0x3FF) > 512)
				x |= 0x3FF;
			else
				x &= ~0x3FF;

			x += 512;
		}

		xAdd = -1024;
		zAdd = 0;
	}

	room_number = item->room_number;
	GetFloor(x, y, z, &room_number);
	r = &room[room_number];
	r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 1;

	x += xAdd;
	z += zAdd;
	room_number = item->room_number;
	GetFloor(x, y, z, &room_number);
	r = &room[room_number];
	r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)].stopper = 1;
}

void inject_moveblok(bool replace)
{
	INJECT(0x00456BA0, ClearMovableBlockSplitters, replace);
	INJECT(0x00457690, AlterFloorHeight, replace);
	INJECT(0x004573A0, TestBlockMovable, replace);
	INJECT(0x004571E0, TestBlockPush, replace);
	INJECT(0x004573F0, TestBlockPull, replace);
	INJECT(0x00457790, DrawUnclippedItem, replace);
	INJECT(0x00457800, SetupCleanerFromSavegame, replace);
}
