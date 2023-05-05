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
#include "../specific/smain.h"
#include "../3dsystem/3d_gen.h"
#include "../specific/game.h"
#include "collide.h"
#include "box.h"
#include "sound.h"
#include "lara.h"
#include "lara2gun.h"
#include "../specific/input.h"
#include "camera.h"
#include "savegame.h"
#include "../newstuff/LaraDraw.h"

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

		if (obj_num == SMASH_OBJECT1 && CurrentLevel != LV_CRASH || obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT2 || obj_num == SMASH_OBJECT3)
			SmashWindow(target_num);
		else if (obj_num == CARCASS || obj_num == EXTRAFX6)
		{
			if (target->status != ITEM_ACTIVE)
			{
				target->status = ITEM_ACTIVE;
				AddActiveItem(target_num);
				KillItem(item_number);
				return;
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
					else
					{
						HitTarget(target, 0, weapons[LG_HARPOON].damage << item->item_flags[0]);
						savegame.ammo_hit++;
					}
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

void ControlRocket(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	FLOOR_INFO* floor;
	PHD_VECTOR oldPos;
	PHD_VECTOR pos;
	PHD_VECTOR vel;
	PHD_3DPOS bPos;
	short* bounds;
	long abovewater, speed, c, rad, exploded, r, g, b, s, nx, nz, nn, ox, oz, on, nn1, on1;
	short oldRoom, room_number, target_num, obj_num;

	item = &items[item_number];
	oldPos.x = item->pos.x_pos;
	oldPos.y = item->pos.y_pos;
	oldPos.z = item->pos.z_pos;
	oldRoom = item->room_number;

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		if (item->speed <= 128)
		{
			item->speed += (item->speed >> 2) + 4;

			if (item->speed > 128)
				item->speed = 128;
		}
		else
			item->speed -= item->speed >> 2;

		abovewater = 0;
		item->pos.z_rot += 182 * ((item->speed >> 3) + 3);
	}
	else
	{
		if (item->speed < 512)
			item->speed += (item->speed >> 2) + 4;

		abovewater = 1;
		item->pos.z_rot += 182 * ((item->speed >> 2) + 7);
	}

	item->shade = -0x3DF0;

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	phd_PushMatrix();
	phd_TranslateRel(0, 0, -128);
	pos.x = phd_mxptr[M03] >> W2V_SHIFT;
	pos.y = phd_mxptr[M13] >> W2V_SHIFT;
	pos.z = phd_mxptr[M23] >> W2V_SHIFT;
	phd_PopMatrix();

	phd_TranslateRel(0, 0, -1536 - (GetRandomControl() & 0x1FF));
	vel.x = phd_mxptr[M03] >> W2V_SHIFT;
	vel.y = phd_mxptr[M13] >> W2V_SHIFT;
	vel.z = phd_mxptr[M23] >> W2V_SHIFT;
	phd_PopMatrix();

	if (wibble & 4)
		TriggerRocketFlame(pos.x, pos.y, pos.z, vel.x - pos.x, vel.y - pos.y, vel.z - pos.z, item_number);

	TriggerRocketSmoke(item->pos.x_pos + pos.x, item->pos.y_pos + pos.y, item->pos.z_pos + pos.z, -1);

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		bPos.x_pos = item->pos.x_pos + pos.x;
		bPos.y_pos = item->pos.y_pos + pos.y;
		bPos.z_pos = item->pos.z_pos + pos.z;
		CreateBubble(&bPos, item->room_number, 4, 8);
	}

	vel.x = pos.x + (GetRandomControl() & 0xF) + item->pos.x_pos - 8;
	vel.y = pos.y + (GetRandomControl() & 0xF) + item->pos.y_pos - 8;
	vel.z = pos.z + (GetRandomControl() & 0xF) + item->pos.z_pos - 8;
	r = (GetRandomControl() & 0x1F) + 224;
	g = (GetRandomControl() & 0x3F) + 128;
	b = GetRandomControl() & 0x3F;
	TriggerDynamic(vel.x, vel.y, vel.z, 14, r, g, b);

	speed = (item->speed * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	item->pos.x_pos += (speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.y_pos -= (item->speed * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;
	item->pos.z_pos += (speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	c = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->pos.y_pos >= item->floor || item->pos.y_pos <= c)
	{
		exploded = 1;
		rad = WALL_SIZE << item->item_flags[0];	//blast radius is 1 square, 2 for biggun rockets
	}
	else
	{
		exploded = 0;
		rad = 0;
	}

	if (room[item->room_number].flags & ROOM_UNDERWATER && abovewater)
	{
		splash_setup.x = item->pos.x_pos;
		splash_setup.y = room[item->room_number].maxceiling;
		splash_setup.z = item->pos.z_pos;
		splash_setup.InnerXZoff = 16;
		splash_setup.InnerXZsize = 12;
		splash_setup.InnerYsize = -96;
		splash_setup.InnerXZvel = 160;
		splash_setup.InnerYvel = -0x4000;
		splash_setup.InnerGravity = 128;
		splash_setup.InnerFriction = 7;
		splash_setup.MiddleXZoff = 24;
		splash_setup.MiddleXZsize = 24;
		splash_setup.MiddleYsize = -64;
		splash_setup.MiddleXZvel = 224;
		splash_setup.MiddleYvel = -0x2000;
		splash_setup.MiddleGravity = 72;
		splash_setup.MiddleFriction = 8;
		splash_setup.OuterXZoff = 32;
		splash_setup.OuterXZsize = 32;
		splash_setup.OuterXZvel = 272;
		splash_setup.OuterFriction = 9;
		SetupSplash(&splash_setup);
	}

	GetNearByRooms(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, rad << 2, rad << 2, item->room_number);

	for (r = 0; r < number_draw_rooms; r++)
	{
		for (target_num = room[draw_rooms[r]].item_number; target_num != NO_ITEM; target_num = target->next_item)
		{
			target = &items[target_num];
			obj_num = target->object_number;

			if (target == lara_item || !target->collidable)
				continue;

			if (obj_num != SMASH_WINDOW && obj_num != SMASH_OBJECT1 && obj_num != SMASH_OBJECT2 &&
				obj_num != SMASH_OBJECT3 && obj_num != CARCASS && obj_num != EXTRAFX6 && obj_num != FLYING_MUTANT_EMITTER &&
				(!objects[obj_num].intelligent || target->status == ITEM_INVISIBLE || !objects[obj_num].collision))
				continue;

			bounds = GetBoundsAccurate(target);

			if (item->pos.y_pos + rad < target->pos.y_pos + bounds[2] || item->pos.y_pos - rad > target->pos.y_pos + bounds[3])
				continue;

			s = phd_sin(target->pos.y_rot);
			c = phd_cos(target->pos.y_rot);
			
			nx = item->pos.x_pos - target->pos.x_pos;
			nz = item->pos.z_pos - target->pos.z_pos;
			nn = (c * nx - s * nz) >> W2V_SHIFT;

			ox = oldPos.x - target->pos.x_pos;
			oz = oldPos.z - target->pos.z_pos;
			on = (c * ox - s * oz) >> W2V_SHIFT;

			nn1 = (s * nx + c * nz) >> W2V_SHIFT;
			on1 = (s * ox + c * oz) >> W2V_SHIFT;

			if (nn + rad < bounds[0] && ox + rad < bounds[0] || nn - rad > bounds[1] && ox - rad > bounds[1] ||
				nn1 + rad < bounds[4] && on1 + rad < bounds[4] || nn1 - rad > bounds[5] && on1 - rad > bounds[5])
				continue;

			if (obj_num == SMASH_OBJECT1 && CurrentLevel == LV_CRASH)
			{
				if (item->item_flags[0] == 1)
					SmashWindow(target_num);

				if (!exploded)
				{
					exploded = 1;
					rad = WALL_SIZE << item->item_flags[0];
					r = -1;
					break;
				}
			}
			else if (obj_num == SMASH_OBJECT1 && CurrentLevel != LV_CRASH || obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT2 || obj_num == SMASH_OBJECT3)
				SmashWindow(target_num);
			else if (obj_num == CARCASS || obj_num == EXTRAFX6)
			{
				if (target->status != ITEM_ACTIVE)
				{
					target->status = ITEM_ACTIVE;
					AddActiveItem(target_num);

					if (!exploded)
					{
						exploded = 1;
						rad = WALL_SIZE << item->item_flags[0];
						r = -1;
						break;
					}
				}
			}
			else if (obj_num != SMASH_OBJECT1)
			{
				if (obj_num == TRIBEBOSS && TribeBossShieldOn)
					FindClosestShieldPoint(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, target);
				else
					HitTarget(target, 0, 30 << item->item_flags[0]);

				savegame.ammo_hit++;

				if (target->hit_points <= 0)
				{
					if (obj_num != TRIBEBOSS && obj_num != WILLARD_BOSS && obj_num != TONY && obj_num != LON_BOSS &&
						obj_num != ELECTRIC_CLEANER && obj_num != WHALE && obj_num != FLYING_MUTANT_EMITTER)
					{
						if (obj_num == LIZARD_MAN && lizard_man_active)
							lizard_man_active = 0;

						item_after_projectile = target->next_active;
						CreatureDie(target_num, 1);
					}
				}

				if (!exploded)
				{
					exploded = 1;
					rad = WALL_SIZE << item->item_flags[0];
					r = -1;
					break;
				}
			}
		}
	}

	if (exploded)
	{
		if (room[oldRoom].flags & ROOM_UNDERWATER)
		{
			item->pos.x_pos = oldPos.x;
			item->pos.y_pos = oldPos.y;
			item->pos.z_pos = oldPos.z;
			ItemNewRoom(item_number, oldRoom);
			TriggerUnderwaterExplosion(item);
		}
		else
		{
			TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 3, -2, 0, item->room_number);

			for (int i = 0; i < 2; i++)
				TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 3, -1, 0, item->room_number);
		}

		AlertNearbyGuards(item);
		SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);
		SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
		KillItem(item_number);
	}
}

void ControlGrenade(short item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* target;
	PHD_VECTOR oldPos;
	PHD_VECTOR pos;
	PHD_VECTOR vel;
	short* bounds;
	long abovewater, exploded, rad, r, s, c, nx, nz, nn, ox, oz, on, nn1, on1;
	short yrot, target_num, obj_num;

	item = &items[item_number];
	oldPos.x = item->pos.x_pos;
	oldPos.y = item->pos.y_pos;
	oldPos.z = item->pos.z_pos;
	item->shade = -0x3DF0;

	if (room[item->room_number].flags & ROOM_UNDERWATER)
	{
		abovewater = 0;
		item->fallspeed += (5 - item->fallspeed) >> 1;
		item->speed -= item->speed >> 2;

		if (item->speed)
		{
			item->pos.z_rot += 182 * ((item->speed >> 4) + 3);

			if (item->required_anim_state)
				item->pos.y_rot += 182 * ((item->speed >> 2) + 3);
			else
				item->pos.x_rot += 182 * ((item->speed >> 2) + 3);
		}
	}
	else
	{
		item->fallspeed += 3;
		abovewater = 1;

		if (item->speed)
		{
			item->pos.z_rot += 182 * ((item->speed >> 2) + 7);

			if (item->required_anim_state)
				item->pos.y_rot += 182 * ((item->speed >> 1) + 7);
			else
				item->pos.x_rot += 182 * ((item->speed >> 1) + 7);
		}
	}

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(0, 0, -64);
	pos.x = phd_mxptr[M03] >> W2V_SHIFT;
	pos.y = phd_mxptr[M13] >> W2V_SHIFT;
	pos.z = phd_mxptr[M23] >> W2V_SHIFT;
	phd_PopMatrix();

	if (item->speed && abovewater)
		TriggerRocketSmoke(item->pos.x_pos + pos.x, item->pos.y_pos + pos.y, item->pos.z_pos + pos.z, -1);

	vel.x = (item->speed * phd_sin(item->goal_anim_state)) >> W2V_SHIFT;
	vel.y = item->fallspeed;
	vel.z = (item->speed * phd_cos(item->goal_anim_state)) >> W2V_SHIFT;
	item->pos.x_pos += vel.x;
	item->pos.y_pos += vel.y;
	item->pos.z_pos += vel.z;

	yrot = item->pos.y_rot;
	item->pos.y_rot = item->goal_anim_state;
	DoProperDetection(item_number, oldPos.x, oldPos.y, oldPos.z, vel.x, vel.y, vel.z);
	item->goal_anim_state = item->pos.y_rot;
	item->pos.y_rot = yrot;

	if (room[item->room_number].flags & ROOM_UNDERWATER && abovewater)
	{
		splash_setup.x = item->pos.x_pos;
		splash_setup.y = room[item->room_number].maxceiling;
		splash_setup.z = item->pos.z_pos;
		splash_setup.InnerXZoff = 16;
		splash_setup.InnerXZsize = 12;
		splash_setup.InnerYsize = -96;
		splash_setup.InnerXZvel = 160;
		splash_setup.InnerGravity = 128;
		splash_setup.InnerYvel = -2048 - (item->fallspeed << 5);
		splash_setup.InnerFriction = 7;
		splash_setup.MiddleXZoff = 24;
		splash_setup.MiddleXZsize = 24;
		splash_setup.MiddleYsize = -64;
		splash_setup.MiddleXZvel = 224;
		splash_setup.MiddleGravity = 72;
		splash_setup.MiddleYvel = -1024 - (item->fallspeed << 4);
		splash_setup.MiddleFriction = 8;
		splash_setup.OuterXZoff = 32;
		splash_setup.OuterXZsize = 32;
		splash_setup.OuterXZvel = 272;
		splash_setup.OuterFriction = 9;
		SetupSplash(&splash_setup);
	}

	exploded = 0;
	rad = 0;

	if (item->hit_points)
	{
		item->hit_points--;

		if (!item->hit_points)
		{
			rad = WALL_SIZE;
			exploded = 1;
		}
	}

	GetNearByRooms(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, rad << 2, rad << 2, item->room_number);

	for (r = 0; r < number_draw_rooms; r++)
	{
		for (target_num = room[draw_rooms[r]].item_number; target_num != NO_ITEM; target_num = target->next_item)
		{
			target = &items[target_num];
			obj_num = target->object_number;

			if (target == lara_item || !target->collidable)
				continue;

			if (obj_num != SMASH_WINDOW && obj_num != SMASH_OBJECT1 && obj_num != SMASH_OBJECT2 &&
				obj_num != SMASH_OBJECT3 && obj_num != CARCASS && obj_num != EXTRAFX6 && obj_num != FLYING_MUTANT_EMITTER &&
				(!objects[obj_num].intelligent || target->status == ITEM_INVISIBLE || !objects[obj_num].collision))
				continue;

			bounds = GetBestFrame(target);

			if (item->pos.y_pos + rad < target->pos.y_pos + bounds[2] || item->pos.y_pos - rad > target->pos.y_pos + bounds[3])
				continue;
			
			s = phd_sin(target->pos.y_rot);
			c = phd_cos(target->pos.y_rot);

			nx = item->pos.x_pos - target->pos.x_pos;
			nz = item->pos.z_pos - target->pos.z_pos;
			nn = (c * nx - s * nz) >> W2V_SHIFT;

			ox = oldPos.x - target->pos.x_pos;
			oz = oldPos.z - target->pos.z_pos;
			on = (c * ox - s * oz) >> W2V_SHIFT;

			nn1 = (s * nx + c * nz) >> W2V_SHIFT;
			on1 = (s * ox + c * oz) >> W2V_SHIFT;

			if (nn + rad < bounds[0] && ox + rad < bounds[0] || nn - rad > bounds[1] && ox - rad > bounds[1] ||
				nn1 + rad < bounds[4] && on1 + rad < bounds[4] || nn1 - rad > bounds[5] && on1 - rad > bounds[5])
				continue;

			if (obj_num == SMASH_OBJECT1 && CurrentLevel != LV_CRASH)
				SmashWindow(target_num);
			else if (obj_num == SMASH_WINDOW || obj_num == SMASH_OBJECT2 || obj_num == SMASH_OBJECT3)
				SmashWindow(target_num);
			else if (obj_num == CARCASS || obj_num == EXTRAFX6)
			{
				if (target->status != ITEM_ACTIVE)
				{
					target->status = ITEM_ACTIVE;
					AddActiveItem(target_num);

					if (!exploded)
					{
						exploded = 1;
						rad = WALL_SIZE;
						r = -1;
						break;
					}
				}
			}
			else if (obj_num != SMASH_OBJECT1)
			{
				if (obj_num == TRIBEBOSS && TribeBossShieldOn)
					FindClosestShieldPoint(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, target);
				else
					HitTarget(target, 0, 20);

				savegame.ammo_hit++;

				if (target->hit_points <= 0)
				{
					if (obj_num != TRIBEBOSS && obj_num != WHALE && obj_num != WILLARD_BOSS && obj_num != TONY &&
						obj_num != LON_BOSS && obj_num != ELECTRIC_CLEANER && obj_num != FLYING_MUTANT_EMITTER)
					{
						if (obj_num == LIZARD_MAN && lizard_man_active)
							lizard_man_active = 0;

						item_after_projectile = target->next_active;
						CreatureDie(target_num, 1);
					}
				}

				if (!exploded)
				{
					exploded = 1;
					rad = WALL_SIZE;
					r = -1;
					break;
				}
			}
		}
	}

	if (exploded)
	{
		if (room[item->room_number].flags & ROOM_UNDERWATER)
			TriggerUnderwaterExplosion(item);
		else
		{
			TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 3, -2, 0, item->room_number);

			for (int i = 0; i < 2; i++)
				TriggerExplosionSparks(oldPos.x, oldPos.y, oldPos.z, 3, -1, 0, item->room_number);
		}

		AlertNearbyGuards(item);
		SoundEffect(SFX_EXPLOSION1, &item->pos, 0x1800000 | SFX_SETPITCH);
		SoundEffect(SFX_EXPLOSION2, &item->pos, SFX_DEFAULT);
		KillItem(item_number);
	}
}

void draw_shotgun_meshes(long weapon_type)
{
	lara.back_gun = 0;
	lara.mesh_ptrs[HAND_R] = meshes[objects[WeaponObject(weapon_type)].mesh_index + HAND_R];
}

void undraw_shotgun_meshes(long weapon_type)
{
	lara.back_gun = (short)WeaponObject(weapon_type);
	lara.mesh_ptrs[HAND_R] = meshes[objects[LARA].mesh_index + HAND_R];
}

void ready_shotgun(long weapon_type)
{
	lara.gun_status = LG_READY;
	lara.target = 0;

	lara.right_arm.x_rot = 0;
	lara.right_arm.y_rot = 0;
	lara.right_arm.z_rot = 0;
	lara.right_arm.lock = 0;
	lara.right_arm.frame_number = 0;
	lara.right_arm.frame_base = objects[WeaponObject(weapon_type)].frame_base;

	lara.left_arm.x_rot = 0;
	lara.left_arm.y_rot = 0;
	lara.left_arm.z_rot = 0;
	lara.left_arm.lock = 0;
	lara.left_arm.frame_number = 0;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
}

void draw_shotgun(long weapon_type)
{
	ITEM_INFO* item;

	if (lara.weapon_item == NO_ITEM)
	{
		lara.weapon_item = CreateItem();
		item = &items[lara.weapon_item];
		item->object_number = (short)WeaponObject(weapon_type);

		if (weapon_type == LG_ROCKET)
			item->anim_number = objects[ROCKET_GUN].anim_index + 1;
		else if (weapon_type == LG_GRENADE)
			item->anim_number = objects[GRENADE_GUN].anim_index;
		else
			item->anim_number = objects[item->object_number].anim_index + 1;

		item->status = ITEM_ACTIVE;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
		item->room_number = NO_ROOM;
		lara.right_arm.frame_base = objects[item->object_number].frame_base;
		lara.left_arm.frame_base = lara.right_arm.frame_base;
	}
	else
		item = &items[lara.weapon_item];

	AnimateItem(item);

	if (!item->current_anim_state || item->current_anim_state == 6)
		ready_shotgun(weapon_type);
	else if (item->frame_number - anims[item->anim_number].frame_base == weapons[weapon_type].draw_frame)
		draw_shotgun_meshes(weapon_type);
	else if (lara.water_status == LARA_UNDERWATER)
		item->goal_anim_state = 6;

	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.right_arm.anim_number = item->anim_number;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
	lara.left_arm.frame_number = lara.right_arm.frame_number;
	lara.left_arm.anim_number = lara.right_arm.anim_number;
}

void undraw_shotgun(long weapon_type)
{
	ITEM_INFO* item;

	item = &items[lara.weapon_item];

	if (lara.water_status == LARA_SURFACE)
		item->goal_anim_state = 9;
	else
		item->goal_anim_state = 3;

	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
	{
		lara.gun_status = LG_ARMLESS;
		lara.target = 0;
		lara.right_arm.lock = 0;
		lara.left_arm.lock = 0;
		KillItem(lara.weapon_item);
		lara.weapon_item = NO_ITEM;
		lara.right_arm.frame_number = 0;
		lara.left_arm.frame_number = 0;
	}
	else if (item->current_anim_state == 3 && item->frame_number - anims[item->anim_number].frame_base == (weapon_type == LG_GRENADE ? 16 : 21))
		undraw_shotgun_meshes(weapon_type);

	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.left_arm.frame_number = lara.right_arm.frame_number;
	lara.right_arm.anim_number = item->anim_number;
	lara.left_arm.anim_number = lara.right_arm.anim_number;
}

void FireHarpoon()
{
	ITEM_INFO* item;
	GAME_VECTOR pos;
	long dx, dy, dz, dist;
	short item_number;

	if (lara.harpoon.ammo <= 0)
		return;

	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	item = &items[item_number];
	item->shade = -0x3DF0;
	item->object_number = HARPOON_BOLT;
	item->room_number = lara_item->room_number;
	pos.x = -2;
	pos.y = 373;
	pos.z = 77;
	GetLaraMeshPos((PHD_VECTOR*)&pos, LMX_HAND_R);
	item->pos.x_pos = pos.x;
	item->pos.y_pos = pos.y;
	item->pos.z_pos = pos.z;
	InitialiseItem(item_number);

	if (lara.target)
	{
		find_target_point(lara.target, &pos);
		dx = pos.x - item->pos.x_pos;
		dy = pos.y - item->pos.y_pos;
		dz = pos.z - item->pos.z_pos;
		dist = phd_sqrt(SQUARE(dx) + SQUARE(dz));
		item->pos.x_rot = -(short)phd_atan(dist, dy);
		item->pos.y_rot = (short)phd_atan(dz, dx);
	}
	else
	{
		item->pos.x_rot = lara.torso_x_rot + lara_item->pos.x_rot;
		item->pos.y_rot = lara.torso_y_rot + lara_item->pos.y_rot;
	}

	item->pos.z_rot = 0;
	item->fallspeed = short((-256 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
	item->speed = short((256 * phd_cos(item->pos.x_rot)) >> W2V_SHIFT);
	item->hit_points = 256;
	AddActiveItem(item_number);

	if (!savegame.bonus_flag)
		lara.harpoon.ammo--;

	savegame.ammo_used++;
}

void FireRocket()
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long lp;
	short item_number;

	if (lara.rocket.ammo <= 0)
		return;

	lara.has_fired = 1;
	item_number = CreateItem();
	
	if (item_number == NO_ITEM)
		return;

	item = &items[item_number];
	item->object_number = ROCKET;
	item->room_number = lara_item->room_number;

	pos.x = 0;
	pos.y = 180;
	pos.z = 72;
	GetLaraMeshPos(&pos, LMX_HAND_R);

	item->pos.x_pos = pos.x;
	item->pos.y_pos = pos.y;
	item->pos.z_pos = pos.z;

	pos2.x = 0;
	pos2.y = 1204;
	pos2.z = 72;
	GetLaraMeshPos(&pos2, LMX_HAND_R);

	SmokeCountL = 32;
	SmokeWeapon = LG_ROCKET;

	for (lp = 0; lp < 5; lp++)
		TriggerGunSmoke(pos.x, pos.y, pos.z, pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, 1, SmokeWeapon, SmokeCountL);

	InitialiseItem(item_number);
	item->pos.x_rot = lara.left_arm.x_rot + lara_item->pos.x_rot;
	item->pos.y_rot = lara.left_arm.y_rot + lara_item->pos.y_rot;
	item->pos.z_rot = 0;

	if (!lara.left_arm.lock)
	{
		item->pos.x_rot += lara.torso_x_rot;
		item->pos.y_rot += lara.torso_y_rot;
	}

	item->speed = 16;
	item->item_flags[0] = 0;
	AddActiveItem(item_number);

	if (!savegame.bonus_flag)
		lara.rocket.ammo--;

	savegame.ammo_used++;

	phd_PushUnitMatrix();
	phd_SetTrans(0, 0, 0);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	phd_PushMatrix();
	phd_TranslateRel(0, 0, -128);
	pos.x = phd_mxptr[M03] >> W2V_SHIFT;
	pos.y = phd_mxptr[M13] >> W2V_SHIFT;
	pos.z = phd_mxptr[M23] >> W2V_SHIFT;
	phd_PopMatrix();

	for (lp = 0; lp < 8; lp++)
	{
		phd_PushMatrix();
		phd_TranslateRel(0, 0, -(GetRandomControl() & 0x7FF));
		pos2.x = phd_mxptr[M03] >> W2V_SHIFT;
		pos2.y = phd_mxptr[M13] >> W2V_SHIFT;
		pos2.z = phd_mxptr[M23] >> W2V_SHIFT;
		phd_PopMatrix();

		TriggerRocketFlame(pos.x, pos.y, pos.z, pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, item_number);
	}

	phd_PopMatrix();

	SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x2000000 | SFX_SETPITCH);
}

void FireGrenade()
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long h;
	short item_number;

	if (lara.grenade.ammo <= 0)
		return;

	lara.has_fired = 1;
	item_number = CreateItem();

	if (item_number == NO_ITEM)
		return;

	item = &items[item_number];
	item->shade = -0x3DF0;
	item->object_number = GRENADE;
	item->room_number = lara_item->room_number;

	pos.x = 0;
	pos.y = 276;
	pos.z = 80;
	GetLaraMeshPos(&pos, LMX_HAND_R);

	item->pos.x_pos = pos.x;
	item->pos.y_pos = pos.y;
	item->pos.z_pos = pos.z;

	pos2.x = 0;
	pos2.y = 1204;
	pos2.z = 72;
	GetLaraMeshPos(&pos2, LMX_HAND_R);

	floor = GetFloor(pos.x, pos.y, pos.z, &item->room_number);
	h = GetHeight(floor, pos.x, pos.y, pos.z);

	if (h < pos.y)
	{
		item->pos.x_pos = lara_item->pos.x_pos;
		item->pos.y_pos = pos.y;
		item->pos.z_pos = lara_item->pos.z_pos;
		item->room_number = lara_item->room_number;
	}

	pos.x = 0;
	pos.y = 1204;
	pos.z = 80;
	GetLaraMeshPos(&pos, LMX_HAND_R);

	SmokeCountL = 32;
	SmokeWeapon = LG_GRENADE;

	for (int i = 0; i < 5; i++)
		TriggerGunSmoke(pos2.x, pos2.y, pos2.z, pos.x - pos2.x, pos.y - pos2.y, pos.z - pos2.z, 1, SmokeWeapon, SmokeCountL);

	InitialiseItem(item_number);
	item->pos.x_rot = lara.left_arm.x_rot + lara_item->pos.x_rot;
	item->pos.y_rot = lara.left_arm.y_rot + lara_item->pos.y_rot;
	item->pos.z_rot = 0;

	if (!lara.left_arm.lock)
	{
		item->pos.x_rot += lara.torso_x_rot;
		item->pos.y_rot += lara.torso_y_rot;
	}

	item->speed = 128;
	item->fallspeed = short(-(item->speed * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
	item->current_anim_state = item->pos.x_rot;
	item->goal_anim_state = item->pos.y_rot;
	item->required_anim_state = 0;
	item->hit_points = 120;
	AddActiveItem(item_number);

	if (!savegame.bonus_flag)
		lara.grenade.ammo--;

	savegame.ammo_used++;
}

void FireShotgun()
{
	PHD_VECTOR pos;
	PHD_VECTOR pos2;
	long fired, lp;
	short angles[2];
	short dangles[2];

	angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
	angles[1] = lara.left_arm.x_rot;

	if (!lara.left_arm.lock)
	{
		angles[0] += lara.torso_y_rot;
		angles[1] += lara.torso_x_rot;
	}

	fired = 0;

	for (int i = 0; i < 6; i++)
	{
		dangles[0] = short(angles[0] + 3640 * (GetRandomControl() - 0x4000) / 0x10000);
		dangles[1] = short(angles[1] + 3640 * (GetRandomControl() - 0x4000) / 0x10000);

		if (FireWeapon(LG_SHOTGUN, lara.target, lara_item, dangles))
			fired = 1;
	}

	if (fired)
	{
		pos.x = 0;
		pos.y = 228;
		pos.z = 32;
		GetLaraMeshPos(&pos, LMX_HAND_R);

		pos2.x = 0;
		pos2.y = 1508;
		pos2.z = 32;
		GetLaraMeshPos(&pos2, LMX_HAND_R);

		SmokeCountL = 32;
		SmokeWeapon = LG_SHOTGUN;

		for (lp = 0; lp < 7; lp++)
			TriggerGunSmoke(pos.x, pos.y, pos.z, pos2.x - pos.x, pos2.y - pos.y, pos2.z - pos.z, 1, SmokeWeapon, SmokeCountL);

		for (lp = 0; lp < 12; lp++)
			TriggerShotgunSparks(pos.x, pos.y, pos.z, (pos2.x - pos.x) << 1, (pos2.y - pos.y) << 1, (pos2.z - pos.z) << 1);

		lara.right_arm.flash_gun = weapons[LG_SHOTGUN].flash_time;
		SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x1400000 | SFX_SETPITCH);
		SoundEffect(weapons[LG_SHOTGUN].sample_num, &lara_item->pos, SFX_DEFAULT);
	}
}

void FireM16(long running)
{
	short angles[2];

	angles[0] = lara.left_arm.y_rot + lara_item->pos.y_rot;
	angles[1] = lara.left_arm.x_rot;

	if (!lara.left_arm.lock)
	{
		angles[0] += lara.torso_y_rot;
		angles[1] += lara.torso_x_rot;
	}

	if (FireWeapon(LG_M16, lara.target, lara_item, angles))
	{
		SmokeCountL = 24;
		SmokeWeapon = LG_M16;
		TriggerGunShell(1, GUNSHELL, LG_M16);
		lara.right_arm.flash_gun = weapons[LG_M16].flash_time;
	}
}

void AnimateShotgun(long weapon_type)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	static long m16_firing, harpoon_fired;
	long running;

	if (SmokeCountL)
	{
		switch (SmokeWeapon)
		{
		case LG_ROCKET:
			pos.x = 0;
			pos.y = 84;
			pos.z = 72;
			break;

		case LG_GRENADE:
			pos.x = 0;
			pos.y = 180;
			pos.z = 80;
			break;

		case LG_SHOTGUN:
			pos.x = -16;
			pos.y = 228;
			pos.z = 32;
			break;

		case LG_M16:
			pos.x = 0;
			pos.y = 228;
			pos.z = 96;
			break;
		}

		GetLaraMeshPos(&pos, LMX_HAND_R);
		TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
	}

	item = &items[lara.weapon_item];
	running = weapon_type == LG_M16 && lara_item->speed;

	switch (item->current_anim_state)
	{
	case 0:
		m16_firing = 0;

		if (harpoon_fired)
		{
			item->goal_anim_state = 5;
			harpoon_fired = 0;
		}
		else if (lara.water_status == LARA_UNDERWATER || running)
			item->goal_anim_state = 6;
		else if (input & IN_ACTION && !lara.target || lara.left_arm.lock)
			item->goal_anim_state = 2;
		else
			item->goal_anim_state = 4;

		break;

	case 2:

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->goal_anim_state = 4;

			if (lara.water_status != LARA_UNDERWATER && !running && !harpoon_fired)
			{
				if (input & IN_ACTION && (!lara.target || lara.left_arm.lock))
				{
					if (weapon_type == LG_HARPOON)
					{
						FireHarpoon();

						if (!(lara.harpoon.ammo & 3))
							harpoon_fired = 1;
					}
					else if (weapon_type == LG_ROCKET)
						FireRocket();
					else if (weapon_type == LG_GRENADE)
						FireGrenade();
					else if (weapon_type == LG_M16)
					{
						FireM16(0);
						SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
						SoundEffect(SFX_HECKLER_KOCH_FIRE, &lara_item->pos, SFX_DEFAULT);
						m16_firing = 1;
					}
					else
						FireShotgun();

					item->goal_anim_state = 2;
				}
				else if (lara.left_arm.lock)
					item->goal_anim_state = 0;
			}

			if (item->goal_anim_state != 2 && m16_firing)
			{
				SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
				m16_firing = 0;
			}
		}
		else if (m16_firing)
		{
			SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x5000000 | SFX_SETPITCH);
			SoundEffect(SFX_HECKLER_KOCH_FIRE, &lara_item->pos, SFX_DEFAULT);
		}
		else if (weapon_type == LG_SHOTGUN && !(input & IN_ACTION) && !lara.left_arm.lock)
			item->goal_anim_state = 4;

		if (item->frame_number - anims[item->anim_number].frame_base == 12 && weapon_type == LG_SHOTGUN)
			TriggerGunShell(1, SHOTGUNSHELL, LG_SHOTGUN);

		break;

	case 6:
		m16_firing = 0;

		if (harpoon_fired)
		{
			item->goal_anim_state = 5;
			harpoon_fired = 0;
		}
		else if (lara.water_status != LARA_UNDERWATER && !running)
			item->goal_anim_state = 0;
		else if (input & IN_ACTION && !lara.target || lara.left_arm.lock)
			item->goal_anim_state = 8;
		else
			item->goal_anim_state = 7;

		break;

	case 8:

		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			item->goal_anim_state = 7;

			if ((lara.water_status == LARA_UNDERWATER || running) && !harpoon_fired)
			{
				if (input & IN_ACTION && (!lara.target || lara.left_arm.lock))
				{
					if (weapon_type == LG_HARPOON)
					{
						FireHarpoon();

						if (!(lara.harpoon.ammo & 3))
							harpoon_fired = 1;
					}
					else
						FireM16(1);

					item->goal_anim_state = 8;
				}
				else if (lara.left_arm.lock)
					item->goal_anim_state = 6;
			}
		}

		if (weapon_type == LG_M16 && item->goal_anim_state == 8)
		{
			SoundEffect(SFX_EXPLOSION1, &lara_item->pos, 0x4000000 | SFX_SETPITCH);
			SoundEffect(SFX_HECKLER_KOCH_FIRE, &lara_item->pos, 0);
		}

		break;
	}

	AnimateItem(item);
	lara.right_arm.frame_base = anims[item->anim_number].frame_ptr;
	lara.right_arm.frame_number = item->frame_number - anims[item->anim_number].frame_base;
	lara.right_arm.anim_number = item->anim_number;
	lara.left_arm.frame_base = lara.right_arm.frame_base;
	lara.left_arm.frame_number = lara.right_arm.frame_number;
	lara.left_arm.anim_number = lara.right_arm.anim_number;
}

void RifleHandler(long weapon_type)
{
	WEAPON_INFO* winfo;
	PHD_VECTOR pos;
	long r, g, b;
	static short FuckYou;

	winfo = &weapons[weapon_type];

	if (input & IN_ACTION)
		LaraTargetInfo(winfo);
	else
		lara.target = 0;

	if (!lara.target)
		LaraGetNewTarget(winfo);

	AimWeapon(winfo, &lara.left_arm);

	if (lara.left_arm.lock)
	{
		lara.torso_x_rot = lara.left_arm.x_rot;
		lara.torso_y_rot = lara.left_arm.y_rot;

		if (camera.old_type != LOOK_CAMERA)
		{
			lara.head_x_rot = 0;
			lara.head_y_rot = 0;
		}
	}

	lara.torso_x_rot += FuckYou;
	lara.left_arm.x_rot += FuckYou;

	if (weapon_type == LG_MAGNUMS)
		AnimatePistols(LG_MAGNUMS);
	else
		AnimateShotgun(weapon_type);

	if (lara.right_arm.flash_gun)
	{
		r = (GetRandomControl() & 0x3F) + 192;
		g = (GetRandomControl() & 0x1F) + 128;
		b = GetRandomControl() & 0x3F;

		if (weapon_type == LG_SHOTGUN || weapon_type == LG_M16)
		{
			pos.x = lara_item->pos.x_pos + (1024 * phd_sin(lara_item->pos.y_rot) >> W2V_SHIFT) + (GetRandomControl() & 0xFF) - 128;
			pos.y = lara_item->pos.y_pos + ((GetRandomControl() & 0x7F) - 575);
			pos.z = lara_item->pos.z_pos + (1024 * phd_cos(lara_item->pos.y_rot) >> W2V_SHIFT) + (GetRandomControl() & 0xFF) - 128;
			TriggerDynamic(pos.x, pos.y, pos.z, weapon_type == LG_SHOTGUN ? 12 : 11, r, g, b);
		}
		else if (weapon_type == LG_MAGNUMS)
		{
			pos.x = (GetRandomControl() & 0xFF) - 128;
			pos.y = (GetRandomControl() & 0x7F) - 63;
			pos.z = (GetRandomControl() & 0xFF) - 128;
			GetLaraMeshPos(&pos, LMX_HAND_R);
			TriggerDynamic(pos.x, pos.y, pos.z, 12, r, g, b);
		}
	}
}

void TriggerUnderwaterExplosion(ITEM_INFO* item)
{
	long y, wh;

	TriggerExplosionBubble(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);
	TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -2, 1, item->room_number);

	for (int i = 0; i < 3; i++)
		TriggerExplosionSparks(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, 2, -1, 1, item->room_number);

	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (wh != NO_HEIGHT)
	{
		y = item->pos.y_pos - wh;

		if (y < 2048)
		{
			wh = 2048 - y;
			y = wh >> 6;

			splash_setup.x = item->pos.x_pos;
			splash_setup.y = room[item->room_number].maxceiling;
			splash_setup.z = item->pos.z_pos;
			splash_setup.InnerYsize = -96;
			splash_setup.InnerXZvel = 160;
			splash_setup.InnerGravity = 96;
			splash_setup.InnerXZoff = short(y + 16);
			splash_setup.InnerXZsize = short(y + 12);
			splash_setup.InnerFriction = 7;
			splash_setup.InnerYvel = short((-512 - wh) << 3);
			splash_setup.MiddleXZoff = short(y + 24);
			splash_setup.MiddleXZsize = short(y + 24);
			splash_setup.MiddleYsize = -64;
			splash_setup.MiddleXZvel = 224;
			splash_setup.MiddleYvel = short((-768 - wh) << 2);
			splash_setup.MiddleGravity = 56;
			splash_setup.MiddleFriction = 8;
			splash_setup.OuterXZoff = short(y + 32);
			splash_setup.OuterXZsize = short(y + 32);
			splash_setup.OuterXZvel = 272;
			splash_setup.OuterFriction = 9;
			SetupSplash(&splash_setup);
		}
	}
}
