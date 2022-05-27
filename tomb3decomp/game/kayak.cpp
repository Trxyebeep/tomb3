#include "../tomb3/pch.h"
#include "kayak.h"
#include "objects.h"
#include "control.h"
#include "../specific/init.h"
#include "../3dsystem/phd_math.h"

void LaraRapidsDrown()
{
	lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 25;
	lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	lara_item->current_anim_state = 12;
	lara_item->goal_anim_state = 12;
	lara_item->gravity_status = 0;
	lara_item->hit_points = 0;
	lara_item->fallspeed = 0;
	lara_item->speed = 0;
	AnimateItem(lara_item);
	lara.extra_anim = 1;
	lara.gun_type = LG_UNARMED;
	lara.gun_status = LG_HANDSBUSY;
	lara.hit_direction = -1;
}

void KayakInitialise(short item_number)
{
	ITEM_INFO* item;
	KAYAKINFO* kayak;

	item = &items[item_number];
	kayak = (KAYAKINFO*)game_malloc(sizeof(KAYAKINFO), 0);
	item->data = kayak;
	kayak->Flags = 0;
	kayak->Rot = 0;
	kayak->Vel = 0;
	kayak->FallSpeedR = 0;
	kayak->FallSpeedL = 0;
	kayak->FallSpeedF = 0;
	kayak->OldPos = item->pos;

	for (int i = 0; i < 32; i++)
	{
		WakePts[i][0].life = 0;
		WakePts[i][1].life = 0;
	}
}

static long GetInKayak(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long dx, dz, dist, h;
	ushort tempang;
	short room_number, ang;

	if (input & IN_ACTION && lara.gun_status == LG_UNARMED && !lara_item->gravity_status)
	{

		item = &items[item_number];
		dx = lara_item->pos.x_pos - item->pos.x_pos;
		dz = lara_item->pos.z_pos - item->pos.z_pos;
		dist = SQUARE(dx) + SQUARE(dz);

		if (dist <= 130000)
		{
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (h > -32000)
			{
				ang = (short)phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot;
				tempang = lara_item->pos.y_rot - item->pos.y_rot;

				if (ang > -8190 && ang < 24570)
				{
					if (tempang > 8190 && tempang < 24570)
						return -1;
				}
				else
				{
					if (tempang > 40950 && tempang < 57330)
						return 1;
				}
			}
		}
	}

	return 0;
}

void inject_kayak(bool replace)
{
	INJECT(0x0043B390, LaraRapidsDrown, replace);
	INJECT(0x0043B410, KayakInitialise, replace);
	INJECT(0x0043B620, GetInKayak, replace);
}
