#include "../tomb3/pch.h"
#include "diver.h"

long GetWaterSurface(long x, long y, long z, short room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;

	r = &room[room_number];
	floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];

	if (r->flags & ROOM_UNDERWATER)
	{
		while (floor->sky_room != 0xFF)
		{
			r = &room[floor->sky_room];

			if (!(r->flags & ROOM_UNDERWATER))
				return floor->ceiling << 8;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}
	else
	{
		while (floor->pit_room != 0xFF)
		{
			r = &room[floor->pit_room];

			if (r->flags & ROOM_UNDERWATER)
				return floor->floor << 8;

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}
	}

	return NO_HEIGHT;
}

void inject_diver(bool replace)
{
	INJECT(0x00423D70, GetWaterSurface, replace);
}
