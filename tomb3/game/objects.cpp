#include "../tomb3/pch.h"
#include "objects.h"
#include "collide.h"
#include "../specific/init.h"
#include "control.h"
#include "items.h"

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

void LiftControl(short item_number)
{
	ITEM_INFO* item;
	LIFT_INFO* lift;
	short room_number;

	item = &items[item_number];
	lift = (LIFT_INFO*)item->data;

	if (TriggerActive(item))
	{
		if (item->pos.y_pos < lift->start_height + 5616)
		{
			if (lift->wait_time < 90)
			{
				item->goal_anim_state = 1;
				lift->wait_time++;
			}
			else
			{
				item->goal_anim_state = 0;
				item->pos.y_pos += 16;
			}
		}
		else
		{
			item->goal_anim_state = 1;
			lift->wait_time = 0;
		}
	}
	else if (item->pos.y_pos > lift->start_height + 16)
	{
		if (lift->wait_time < 90)
		{
			item->goal_anim_state = 1;
			lift->wait_time++;
		}
		else
		{
			item->goal_anim_state = 0;
			item->pos.y_pos -= 16;
		}
	}
	else
	{
		item->goal_anim_state = 1;
		lift->wait_time = 0;
	}

	AnimateItem(item);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}

void LiftFloorCeiling(ITEM_INFO* item, long x, long y, long z, long* h, long* c)
{
	long ix, iz, lx, lz, lh, lc, inside;

	ix = item->pos.x_pos >> WALL_SHIFT;
	iz = item->pos.z_pos >> WALL_SHIFT;
	lx = lara_item->pos.x_pos >> WALL_SHIFT;
	lz = lara_item->pos.z_pos >> WALL_SHIFT;
	x >>= WALL_SHIFT;
	z >>= WALL_SHIFT;
	lh = item->pos.y_pos;
	lc = lh - 1280;
	inside = (x == ix || x + 1 == ix) && (z == iz || z - 1 == iz);	//is test point in lift range?
	*h = 0x7FFF;
	*c = -0x7FFF;

	if ((lx == ix || lx + 1 == ix) && (lz == iz || lz - 1 == iz))	//is lara in lift range?
	{
		if (!item->current_anim_state && lara_item->pos.y_pos < lh + 256 && lara_item->pos.y_pos > lc + 256)
		{
			if (inside)
			{
				*h = lh;
				*c = lc + 256;
			}
			else
			{
				*h = NO_HEIGHT;
				*c = 0x7FFF;
			}
		}
		else if (inside)
		{
			if (lara_item->pos.y_pos < lc + 256)
				*h = lc;
			else if (lara_item->pos.y_pos < lh + 256)
			{
				*h = lh;
				*c = lc + 256;
			}
			else
				*c = lh + 256;
		}
	}
	else if (inside)
	{
		if (y <= lc)
			*h = lc;
		else if (y < lh + 256)
		{
			if (item->current_anim_state)
			{
				*h = lh;
				*c = lc + 256;
			}
			else
			{
				*h = NO_HEIGHT;
				*c = 0x7FFF;
			}
		}
		else
			*c = lh + 256;
	}
}

void LiftFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long nH, c;

	LiftFloorCeiling(item, x, y, z, &nH, &c);

	if (nH < *h)
	{
		OnObject = 1;
		*h = nH;
	}
}

void LiftCeiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	long h, nC;

	LiftFloorCeiling(item, x, y, z, &h, &nC);

	if (nC > *c)
		*c = nC;
}

long GetOffset(ITEM_INFO* item, long x, long z)
{
	if (!item->pos.y_rot)
#ifdef TROYESTUFF			// Fixes bridge bug
		return ~x & 0x3FF;
#else
		return -x & 0x3FF;
#endif
	
	if (item->pos.y_rot == -0x8000)
		return x & 0x3FF;
	
	if (item->pos.y_rot == 0x4000)
		return z & 0x3FF;

#ifdef TROYESTUFF
	return ~z & 0x3FF;
#else
	return -z & 0x3FF;
#endif
}

void BridgeFlatFloor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	if (y <= item->pos.y_pos)
	{
		*h = item->pos.y_pos;
		height_type = WALL;
		OnObject = 1;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void BridgeFlatCeiling(ITEM_INFO* item, long x, long y, long z, long* c)
{
	if (y > item->pos.y_pos)
		*c = item->pos.y_pos + 256;
}

void BridgeTilt1Floor(ITEM_INFO* item, long x, long y, long z, long* h)
{
	long level;

	level = item->pos.y_pos + (GetOffset(item, x, z) >> 2);

	if (y <= level)
	{
		*h = level;
		height_type = WALL;
		OnObject = 1;

		if (item == lara_item)
			lara_item->item_flags[0] = 1;
	}
}

void inject_objects(bool replace)
{
	INJECT(0x00459330, OnDrawBridge, replace);
	INJECT(0x004593F0, DrawBridgeFloor, replace);
	INJECT(0x00459450, DrawBridgeCeiling, replace);
	INJECT(0x00459490, DrawBridgeCollision, replace);
	INJECT(0x004594C0, InitialiseLift, replace);
	INJECT(0x00459500, LiftControl, replace);
	INJECT(0x004595E0, LiftFloorCeiling, replace);
	INJECT(0x00459760, LiftFloor, replace);
	INJECT(0x004597A0, LiftCeiling, replace);
	INJECT(0x00459840, GetOffset, replace);
	INJECT(0x004597E0, BridgeFlatFloor, replace);
	INJECT(0x00459820, BridgeFlatCeiling, replace);
	INJECT(0x00459880, BridgeTilt1Floor, replace);
}
