#include "../tomb3/pch.h"
#include "lara1gun.h"
#include "../3dsystem/phd_math.h"
#include "control.h"
#include "items.h"
#include "effect2.h"
#include "objects.h"
#include "draw.h"
#include "gameflow.h"
#include "triboss.h"
#include "effects.h"
#include "larafire.h"
#ifdef RANDO_STUFF
#include "../specific/smain.h"
#endif

void ControlHarpoonBolt(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	FLOOR_INFO* floor;
	short* bounds;
	long oldX, oldZ, c, s, nx, nz, nn, nn1, ox, oz, on, on1;
	short room_number, target_num, obj_num;

	item = &items[item_number];
	oldX = item->pos.x_pos;
	oldZ = item->pos.z_pos;
	item->pos.x_pos += (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.y_pos += item->fallspeed;
	item->pos.z_pos += (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	for (target_num = room[item->room_number].item_number; target_num != NO_ITEM; target_num = target->next_item)
	{
		target = &items[target_num];
		obj_num = target->object_number;

		if (target == lara_item || !target->collidable)
			continue;

		if (obj_num == UPV || obj_num != SMASH_WINDOW && obj_num != SMASH_OBJECT1 && obj_num != SMASH_OBJECT2 && obj_num != SMASH_OBJECT3 &&
			obj_num != CARCASS && obj_num != EXTRAFX6 && (target->status == ITEM_INVISIBLE || !objects[obj_num].collision))
			continue;

		bounds = GetBestFrame(target);

		if (item->pos.y_pos < target->pos.y_pos + bounds[2] || item->pos.y_pos > target->pos.y_pos + bounds[3])
			continue;

		s = phd_sin(target->pos.y_rot);
		c = phd_cos(target->pos.y_rot);

		nx = item->pos.x_pos - target->pos.x_pos;
		nz = item->pos.z_pos - target->pos.z_pos;
		nn = (c * nx - s * nz) >> W2V_SHIFT;

		ox = oldX - target->pos.x_pos;
		oz = oldZ - target->pos.z_pos;
		on = (c * ox - s * oz) >> W2V_SHIFT;

		nn1 = (s * nx + c * nz) >> W2V_SHIFT;
		on1 = (s * ox + c * oz) >> W2V_SHIFT;

		if (nn < bounds[0] && ox < bounds[0] || nn > bounds[1] && ox > bounds[1] ||
			nn1 < bounds[4] && on1 < bounds[4] || nn1 > bounds[5] && on1 > bounds[5])
			continue;

#ifdef RANDO_STUFF
		if (obj_num == SMASH_OBJECT1 && rando.levels[RANDOLEVEL].original_id != LV_CRASH ||
#else
		if (obj_num == SMASH_OBJECT1 && CurrentLevel != LV_CRASH ||
#endif
			obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT2 || obj_num == SMASH_OBJECT3)
			SmashWindow(target_num);
		else if (obj_num == CARCASS || obj_num == EXTRAFX6)
		{
			if (item->status != ITEM_ACTIVE)	//original bug: this doesn't work, need to check target instead of item
			{
				item->status = ITEM_ACTIVE;		//same here
				AddActiveItem(target_num);
			}
		}
		else if (obj_num != SMASH_OBJECT1)
		{
			if (objects[obj_num].intelligent)
			{
				if (obj_num == TRIBEBOSS)
				{
					if (TribeBossShieldOn)
						FindClosestShieldPoint(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, target);
				}
				else
				{
					if (obj_num != LON_BOSS)
						DoLotsOfBlood(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 0, 0, item->room_number, 3);

					HitTarget(target, 0, weapons[LG_HARPOON].damage << item->item_flags[0]);
					savegame.ammo_hit++;
				}
			}

			KillItem(item_number);
			return;
		}
	}

	c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos >= item->floor || item->pos.y_pos <= c)
	{
		if (item->hit_points == 256)
			item->current_anim_state = item->pos.x_rot;

		if (item->hit_points >= 192)
		{
			item->pos.x_rot = item->current_anim_state + (((item->hit_points - 192) * ((rcossin_tbl[512 * (item->hit_points & 7)] >> 1) - 1024)) >> 6);
			item->hit_points--;
		}

		item->hit_points--;

		if (!item->hit_points)
		{
			KillItem(item_number);
			return;
		}

		item->fallspeed = 0;
		item->speed = 0;
	}
	else
	{
		item->pos.z_rot += 6370;

		if (room[item->room_number].flags & ROOM_UNDERWATER)
		{
			if (!(wibble & 0xF))
				CreateBubble(&item->pos, item->room_number, 2, 8);

			TriggerRocketSmoke(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 64);
			item->fallspeed = short((-128 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
			item->speed = short((128 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT);
		}
		else
		{
			item->pos.x_rot -= 182;

			if (item->pos.x_rot < -0x4000)
				item->pos.x_rot = -0x4000;

			item->fallspeed = short((-256 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
			item->speed = short((256 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT);
		}
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);
}

void inject_lara1gun(bool replace)
{
	INJECT(0x004459B0, ControlHarpoonBolt, replace);
}
