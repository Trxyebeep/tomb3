#include "../tomb3/pch.h"
#include "fusebox.h"
#include "control.h"
#include "../3dsystem/phd_math.h"
#include "effect2.h"
#include "gameflow.h"
#include "objects.h"
#include "../specific/smain.h"
#include "../tomb3/tomb3.h"

void ControlFusebox(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* sophia;
	FLOOR_INFO* floor;
	long x, z;
	short room_num;

	item = &items[item_number];

	if (item->hit_points <= 0 && item->anim_number == objects[item->object_number].anim_index)
	{
		item->anim_number = objects[item->object_number].anim_index + 1;
		item->frame_number = anims[item->anim_number].frame_base;
		x = (item->pos.x_pos + 420 * phd_sin(item->pos.y_rot + 0x8000)) >> W2V_SHIFT;
		z = (item->pos.z_pos + 420 * phd_cos(item->pos.y_rot + 0x8000)) >> W2V_SHIFT;
		TriggerExplosionSparks(x, item->pos.y_pos - 768, z, 2, 0, 0, item->room_number);
		TriggerExplosionSmoke(x, item->pos.y_pos - 768, z, 0);

		if (tomb3.gold)
			room_num = 6;
		else
			room_num = LV_OFFICE;

		if (CurrentLevel == room_num)
		{
			room_num = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_num);
			GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
			TestTriggers(trigger_index, 1);

			for (int i = 0; i < level_items; i++)
			{
				sophia = &items[i];

				if (sophia->object_number == LON_BOSS)
				{
					sophia->item_flags[2] = 1;
					break;
				}
			}
		}
	}

	AnimateItem(item);
}
