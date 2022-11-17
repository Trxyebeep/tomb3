#include "../tomb3/pch.h"
#include "train.h"
#include "../3dsystem/phd_math.h"
#include "control.h"

static long TestHeight(ITEM_INFO* item, long x, long z, short* room_number)
{
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long s, c;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	pos.x = item->pos.x_pos + ((z * s + x * c) >> W2V_SHIFT);
	pos.y = ((x * phd_sin(item->pos.z_rot)) >> W2V_SHIFT) + (item->pos.y_pos - ((z * phd_sin(item->pos.x_rot)) >> W2V_SHIFT));
	pos.z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);
	*room_number = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, room_number);
	return GetHeight(floor, pos.x, pos.y, pos.z);
}

void inject_train(bool replace)
{
	INJECT(0x0046D150, TestHeight, replace);
}
