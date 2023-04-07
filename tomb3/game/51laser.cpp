#include "../tomb3/pch.h"
#include "51laser.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "effect2.h"
#include "../specific/game.h"
#include "items.h"
#include "collide.h"
#include "effects.h"
#include "lara.h"

void InitialiseArea51Laser(short item_number)
{
	ITEM_INFO* item;
	long x, z;

	item = &items[item_number];
	x = item->pos.x_pos + (2560 * phd_sin(item->pos.y_rot) >> W2V_SHIFT);
	z = item->pos.z_pos + (2560 * phd_cos(item->pos.y_rot) >> W2V_SHIFT);
	item->item_flags[0] = ((item->pos.x_pos >> 9) & 0xFF) | (((item->pos.z_pos >> 9) & 0xFF) << 8);
	item->item_flags[1] = ((x >> 9) & 0xFF) | (((z >> 9) & 0xFF) << 8);
	item->item_flags[2] = 1;
	item->item_flags[3] = 0;
	item->shadeB = 0;
}

void ControlArea51Laser(short item_number)
{
	ITEM_INFO* item;
	long x, z, dx, dz;
	short room_number;

	item = &items[item_number];

	if (!TriggerActive(item))
		return;

	item->current_anim_state = 0;
	TriggerDynamic(item->pos.x_pos, item->pos.y_pos - 64, item->pos.z_pos,
		(GetRandomControl() & 1) + 8, (GetRandomControl() & 0x1F) + 192, GetRandomControl() & 0x1F, GetRandomControl() & 7);
	item->mesh_bits = -1 - (GetRandomControl() & 0x14);
	dx = abs(((item->item_flags[1] & 0xFF) << 9) - item->pos.x_pos);
	dz = abs(((item->item_flags[1] & 0xFF00) << 1) - item->pos.z_pos);

	if (dx < 768 && dz < 768)
	{
		item->shadeB = 32;
		x = ((((item->item_flags[0] & 0xFF) << 9) + ((-2560 * item->item_flags[2] * phd_sin(item->pos.y_rot)) >> W2V_SHIFT)) >> 9) & 0xFF;
		z = (((((item->item_flags[0] & 0xFF00) << 1) + ((-2560 * (item->item_flags[2] * phd_cos(item->pos.y_rot))) >> W2V_SHIFT)) >> 9) & 0xFF) << 8;
		item->item_flags[1] = short(x | z);
	}

	if (item->item_flags[2] == 1)
	{
		if (item->shadeB)
		{
			if (item->item_flags[3])
			{
				if (item->item_flags[3] > 4)
					item->item_flags[3] -= item->item_flags[3] >> 2;
				else
					item->item_flags[3] = 0;
			}
			else
			{
				item->shadeB--;

				if (item->shadeB == 1)
					item->item_flags[2] = -1;
			}
		}
		else
		{
			item->item_flags[3] += 5;

			if (item->item_flags[3] > 512)
				item->item_flags[3] = 512;
		}
	}
	else if (item->shadeB)
	{
		if (item->item_flags[3])
		{
			if (item->item_flags[3] < -4)
				item->item_flags[3] -= item->item_flags[3] >> 2;
			else
				item->item_flags[3] = 0;
		}
		else
		{
			item->shadeB--;

			if (item->shadeB == 1)
				item->item_flags[2] = -item->item_flags[2];
		}
	}
	else
	{
		item->item_flags[3] -= 5;

		if (item->item_flags[3] < -512)
			item->item_flags[3] = -512;
	}

	item->pos.x_pos += (item->item_flags[3] * phd_sin(item->pos.y_rot)) >> (W2V_SHIFT + 2);
	item->pos.z_pos += (item->item_flags[3] * phd_cos(item->pos.y_rot)) >> (W2V_SHIFT + 2);
	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

	if (room_number != item->room_number)
		ItemNewRoom(item_number, room_number);

	if (TestBoundsCollide(item, lara_item, 64))
	{
		lara_item->hit_points -= 25;
		DoLotsOfBloodD(lara_item->pos.x_pos, item->pos.y_pos - (GetRandomControl() & 0xFF) - 32, lara_item->pos.z_pos,
			(GetRandomControl() & 0x7F) + 128, short(GetRandomControl() << 1), lara_item->room_number, 3);
	}

	AnimateItem(item);
}
