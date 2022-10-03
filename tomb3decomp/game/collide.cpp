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

short GetTiltType(FLOOR_INFO* floor, long x, long y, long z)
{
	ROOM_INFO* r;
	short* data;
	short type, t0, t1, t2, t3, tilt, x2, z2, x3, y2;

	while (floor->pit_room != 255)
	{
		if (CheckNoColFloorTriangle(floor, x, z) == 1)
			break;

		r = &room[floor->pit_room];
		floor = &r->floor[((z - r->z) >> 10) + (((x - r->x) >> 10) * r->x_size)];
	}

	if (y + 512 < floor->floor << 8)
		return 0;

	if (floor->index)
	{
		data = &floor_data[floor->index];
		type = data[0] & 0x1F;

		if (type == TILT_TYPE)
			return data[1];

		if (type == SPLIT1 || type == SPLIT2 || type == NOCOLF1T || type == NOCOLF2T || type == NOCOLF1B || type == NOCOLF2B)
		{
			tilt = data[1];
			t0 = tilt & 0xF;
			t1 = (tilt >> 4) & 0xF;
			t2 = (tilt >> 8) & 0xF;
			t3 = (tilt >> 12) & 0xF;
			x2 = x & 0x3FF;
			z2 = z & 0x3FF;
			type = type & 0x1F;

			if (type == SPLIT1 || type == NOCOLF1T || type == NOCOLF1B)
			{
				if (x2 > 1024 - z2)
				{
					x3 = t3 - t0;
					y2 = t3 - t2;
				}
				else
				{
					x3 = t2 - t1;
					y2 = t0 - t1;
				}
			}
			else if (x2 > z2)
			{
				x3 = t3 - t0;
				y2 = t0 - t1;
			}
			else
			{
				x3 = t2 - t1;
				y2 = t3 - t2;
			}

			return ((x3 << 8) | (y2 & 0xFF));
		}
	}

	return 0;
}

long CollideStaticObjects(COLL_INFO* coll, long x, long y, long z, short room_number, long hite)
{
	ROOM_INFO* r;
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	long lxmin, lxmax, lymin, lymax, lzmin, lzmax;
	long xmin, xmax, ymin, ymax, zmin, zmax;
	long ls, rs, xs, zs;

	coll->hit_static = 0;
	lxmin = x - coll->radius;
	lxmax = x + coll->radius;
	lymin = y - hite;
	lymax = y;
	lzmin = z - coll->radius;
	lzmax = z + coll->radius;
	GetNearByRooms(x, y, z, coll->radius + 50, hite + 50, room_number);

	for (int i = 0; i < number_draw_rooms; i++)
	{
		r = &room[draw_rooms[i]];
		mesh = r->mesh;

		for (int j = r->num_meshes; j > 0; j--, mesh++)
		{
			sinfo = &static_objects[mesh->static_number];

			if (sinfo->flags & 1)
				continue;

			ymin = mesh->y + sinfo->y_minc;
			ymax = mesh->y + sinfo->y_maxc;

			if (mesh->y_rot == -0x8000)
			{
				xmin = mesh->x - sinfo->x_maxc;
				xmax = mesh->x - sinfo->x_minc;
				zmin = mesh->z - sinfo->z_maxc;
				zmax = mesh->z - sinfo->z_minc;
			}
			else if (mesh->y_rot == -0x4000)
			{
				xmin = mesh->x - sinfo->z_maxc;
				xmax = mesh->x - sinfo->z_minc;
				zmin = mesh->z + sinfo->x_minc;
				zmax = mesh->z + sinfo->x_maxc;
			}
			else if (mesh->y_rot == 0x4000)
			{
				xmin = mesh->x + sinfo->z_minc;
				xmax = mesh->x + sinfo->z_maxc;
				zmin = mesh->z - sinfo->x_maxc;
				zmax = mesh->z - sinfo->x_minc;
			}
			else
			{
				xmin = mesh->x + sinfo->x_minc;
				xmax = mesh->x + sinfo->x_maxc;
				zmin = mesh->z + sinfo->z_minc;
				zmax = mesh->z + sinfo->z_maxc;
			}

			if (lxmax <= xmin || lxmin >= xmax || lymax <= ymin || lymin >= ymax || lzmax <= zmin || lzmin >= zmax)
				continue;

			ls = lxmax - xmin;
			rs = xmax - lxmin;
			xs = ls >= rs ? rs : -ls;

			ls = lzmax - zmin;
			rs = zmax - lzmin;
			zs = ls >= rs ? rs : -ls;

			switch (coll->quadrant)
			{
			case NORTH:

				if (xs > coll->radius || xs < -coll->radius)
				{
					coll->shift.x = coll->old.x - x;
					coll->shift.z = zs;
					coll->coll_type = CT_FRONT;
				}
				else if (xs > 0)
				{
					coll->shift.x = xs;
					coll->shift.z = 0;
					coll->coll_type = CT_LEFT;
				}
				else if (xs < 0)
				{
					coll->shift.x = xs;
					coll->shift.z = 0;
					coll->coll_type = CT_RIGHT;
				}

				break;

			case EAST:

				if (zs > coll->radius || zs < -coll->radius)
				{
					coll->shift.x = xs;
					coll->shift.z = coll->old.z - z;
					coll->coll_type = CT_FRONT;
				}
				else if (zs > 0)
				{
					coll->shift.x = 0;
					coll->shift.z = zs;
					coll->coll_type = CT_RIGHT;
				}
				else if (zs < 0)
				{
					coll->shift.x = 0;
					coll->shift.z = zs;
					coll->coll_type = CT_LEFT;
				}

				break;

			case SOUTH:

				if (xs > coll->radius || xs < -coll->radius)
				{
					coll->shift.x = coll->old.x - x;
					coll->shift.z = zs;
					coll->coll_type = CT_FRONT;
				}
				else if (xs > 0)
				{
					coll->shift.x = xs;
					coll->shift.z = 0;
					coll->coll_type = CT_RIGHT;
				}
				else if (xs < 0)
				{
					coll->shift.x = xs;
					coll->shift.z = 0;
					coll->coll_type = CT_LEFT;
				}

				break;

			case WEST:

				if (zs > coll->radius || zs < -coll->radius)
				{
					coll->shift.x = xs;
					coll->shift.z = coll->old.z - z;
					coll->coll_type = CT_FRONT;
				}
				else if (zs > 0)
				{
					coll->shift.x = 0;
					coll->shift.z = zs;
					coll->coll_type = CT_LEFT;
				}
				else if (zs < 0)
				{
					coll->shift.x = 0;
					coll->shift.z = zs;
					coll->coll_type = CT_RIGHT;
				}

				break;
			}

			coll->hit_static = 1;
			return 1;
		}
	}

	return 0;
}

void inject_collide(bool replace)
{
	INJECT(0x0041E690, ShiftItem, replace);
	INJECT(0x0041EBD0, ObjectCollision, replace);
	INJECT(0x0041EC30, ObjectCollisionSub, replace);
	INJECT(0x0041E630, GetNewRoom, replace);
	INJECT(0x0041E560, GetNearByRooms, replace);
	INJECT(0x0041E730, GetTiltType, replace);
	INJECT(0x0041E170, CollideStaticObjects, replace);
}
