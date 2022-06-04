#include "../tomb3/pch.h"
#include "collide.h"
#include "sphere.h"
#include "control.h"

void ShiftItem(ITEM_INFO* item, COLL_INFO* coll)
{
	item->pos.x_pos += coll->shift.x;
	item->pos.y_pos += coll->shift.y;
	item->pos.z_pos += coll->shift.z;
	coll->shift.z = 0;
	coll->shift.y = 0;
	coll->shift.x = 0;
}

void ObjectCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l) && coll->enable_baddie_push)
		ItemPushLara(item, l, coll, 0, 1);
}

void ObjectCollisionSub(short item_num, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	item = &items[item_num];

	if (TestBoundsCollide(item, l, coll->radius) && TestCollision(item, l))
		ItemPushLara(item, l, coll, 0, 0);
}

void GetNewRoom(long x, long y, long z, short room_number)
{
	long r;

	GetFloor(x, y, z, &room_number);

	for (r = 0; r < number_draw_rooms; r++)
	{
		if (draw_rooms[r] == room_number)
			break;
	}

	if (r == number_draw_rooms)
	{
		draw_rooms[number_draw_rooms] = room_number;
		number_draw_rooms++;
	}
}

void GetNearByRooms(long x, long y, long z, long r, long h, short room_number)
{
	draw_rooms[0] = room_number;
	number_draw_rooms = 1;
	GetNewRoom(r + x, y, r + z, room_number);
	GetNewRoom(x - r, y, r + z, room_number);
	GetNewRoom(r + x, y, z - r, room_number);
	GetNewRoom(x - r, y, z - r, room_number);
	GetNewRoom(r + x, y - h, r + z, room_number);
	GetNewRoom(x - r, y - h, r + z, room_number);
	GetNewRoom(r + x, y - h, z - r, room_number);
	GetNewRoom(x - r, y - h, z - r, room_number);
}

void inject_collide(bool replace)
{
	INJECT(0x0041E690, ShiftItem, replace);
	INJECT(0x0041EBD0, ObjectCollision, replace);
	INJECT(0x0041EC30, ObjectCollisionSub, replace);
	INJECT(0x0041E630, GetNewRoom, replace);
	INJECT(0x0041E560, GetNearByRooms, replace);
}
