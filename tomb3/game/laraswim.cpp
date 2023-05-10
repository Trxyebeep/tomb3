#include "../tomb3/pch.h"
#include "laraswim.h"
#include "lara.h"
#include "control.h"
#include "laramisc.h"
#include "../3dsystem/phd_math.h"
#include "collide.h"
#include "larafire.h"
#include "../specific/game.h"
#include "../specific/input.h"
#include "camera.h"

void LaraUnderWater(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -400;
	coll->bad_ceiling = 400;
	coll->old.x = item->pos.x_pos;
	coll->old.y = item->pos.y_pos;
	coll->old.z = item->pos.z_pos;
	coll->radius = 300;
	coll->trigger = 0;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->lava_is_pit = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if (input & IN_LOOK && lara.look)
		LookLeftRight();
	else
		ResetLook();

	lara.look = 1;

	if (lara.extra_anim)
		extra_control_routines[item->current_anim_state](item, coll);
	else
		lara_control_routines[item->current_anim_state](item, coll);

	if (item->pos.z_rot >= -364 && item->pos.z_rot <= 364)
		item->pos.z_rot = 0;
	else if (item->pos.z_rot < 0)
		item->pos.z_rot += 364;
	else
		item->pos.z_rot -= 364;

	if (item->pos.x_rot < -15470)
		item->pos.x_rot = -15470;
	else if (item->pos.x_rot > 15470)
		item->pos.x_rot = 15470;

	if (item->pos.z_rot < -4004)
		item->pos.z_rot = -4004;
	else if (item->pos.z_rot > 4004)
		item->pos.z_rot = 4004;

	if (lara.turn_rate >= -364 && lara.turn_rate <= 364)
		lara.turn_rate = 0;
	else if (lara.turn_rate < -364)
		lara.turn_rate += 364;
	else
		lara.turn_rate -= 364;

	item->pos.y_rot += lara.turn_rate;

	if (lara.current_active && lara.water_status != 3)
		LaraWaterCurrent(coll);

	AnimateLara(item);
	item->pos.x_pos += (((phd_sin(item->pos.y_rot) * item->fallspeed) >> 16) * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;
	item->pos.y_pos -= (phd_sin(item->pos.x_rot) * item->fallspeed) >> 16;
	item->pos.z_pos += (((phd_cos(item->pos.y_rot) * item->fallspeed) >> 16) * phd_cos(item->pos.x_rot)) >> W2V_SHIFT;

	if (lara.water_status != LARA_CHEAT && !lara.extra_anim)
		LaraBaddieCollision(item, coll);

	if (!lara.extra_anim && lara.skidoo == NO_ITEM)
		lara_collision_routines[item->current_anim_state](item, coll);

	UpdateLaraRoom(item, 0);
	LaraGun();
	LaraOnPad = 0;
	TestTriggers(coll->trigger, 0);

	if (!LaraOnPad)
		lara_item->item_flags[1] = 0;
}

void SwimTurn(ITEM_INFO* item)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 364;
	else if (input & IN_BACK)
		item->pos.x_rot += 364;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;

		item->pos.z_rot -= 546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;

		item->pos.z_rot += 546;
	}
}

void lara_as_swim(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = anims[ANIM_WATERROLL].frame_base;
		item->current_anim_state = AS_WATERROLL;
	}
	else
	{
		SwimTurn(item);
		item->fallspeed += 8;

		if (lara.water_status == LARA_CHEAT)
		{
			if (item->fallspeed > 400)
				item->fallspeed = 400;
		}
		else if (item->fallspeed > 200)
				item->fallspeed = 200;

		if (!(input & IN_JUMP))
			item->goal_anim_state = AS_GLIDE;
	}
}

void lara_as_glide(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = anims[ANIM_WATERROLL].frame_base;
		item->current_anim_state = AS_WATERROLL;
	}
	else
	{
		SwimTurn(item);

		if (input & IN_JUMP)
			item->goal_anim_state = AS_SWIM;

		item->fallspeed -= 6;

		if (item->fallspeed < 0)
			item->fallspeed = 0;

		if (item->fallspeed <= 133)
			item->goal_anim_state = AS_TREAD;
	}
}

void lara_as_tread(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->anim_number = ANIM_WATERROLL;
		item->frame_number = anims[ANIM_WATERROLL].frame_base;
		item->current_anim_state = AS_WATERROLL;
	}
	else
	{
		if (input & IN_LOOK)
			LookUpDown();

		SwimTurn(item);

		if (input & IN_JUMP)
			item->goal_anim_state = AS_SWIM;

		item->fallspeed -= 6;

		if (item->fallspeed < 0)
			item->fallspeed = 0;

		if (lara.gun_status == LG_HANDSBUSY)
			lara.gun_status = LG_ARMLESS;
	}
}

void lara_as_dive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_FORWARD)
		item->pos.x_rot -= 182;
}

void lara_as_uwdeath(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	item->fallspeed -= 8;

	if (item->fallspeed <= 0)
		item->fallspeed = 0;

	if (item->pos.x_rot >= -364 && item->pos.x_rot <= 364)
		item->pos.x_rot = 0;
	else if (item->pos.x_rot < 0)
		item->pos.x_rot += 364;
	else
		item->pos.x_rot -= 364;
}

void lara_as_waterroll(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed = 0;
}

void lara_col_swim(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_glide(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_tread(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_dive(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

void lara_col_uwdeath(ITEM_INFO* item, COLL_INFO* coll)
{
	long h;

	item->hit_points = -1;
	lara.air = -1;
	lara.gun_status = 1;
	h = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

	if (h != NO_HEIGHT && h < item->pos.y_pos - 100)
		item->pos.y_pos -= 5;

	LaraSwimCollision(item, coll);
}

void lara_col_waterroll(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraSwimCollision(item, coll);
}

long GetWaterDepth(long x, long y, long z, short room_number)
{
	ROOM_INFO* r;
	FLOOR_INFO* floor;
	long x_floor, y_floor, h;
	short door;

	r = &room[room_number];

	do
	{
		x_floor = (z - r->z) >> WALL_SHIFT;
		y_floor = (x - r->x) >> WALL_SHIFT;

		if (x_floor <= 0)
		{
			x_floor = 0;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (x_floor >= r->x_size - 1)
		{
			x_floor = r->x_size - 1;

			if (y_floor < 1)
				y_floor = 1;
			else if (y_floor > r->y_size - 2)
				y_floor = r->y_size - 2;
		}
		else if (y_floor < 0)
			y_floor = 0;
		else if (y_floor >= r->y_size)
			y_floor = r->y_size - 1;

		floor = &r->floor[x_floor + y_floor * r->x_size];
		door = GetDoor(floor);

		if (door != NO_ROOM)
		{
			room_number = door;
			r = &room[door];
		}

	} while (door != NO_ROOM);

	if (r->flags & (ROOM_UNDERWATER | ROOM_SWAMP))
	{
		while (floor->sky_room != NO_ROOM)
		{
			r = &room[floor->sky_room];

			if (!(r->flags & (ROOM_UNDERWATER | ROOM_SWAMP)))
			{
				h = GetMinimumCeiling(floor, x, z);
				floor = GetFloor(x, y, z, &room_number);
				return GetHeight(floor, x, y, z) - h;
			}

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}

		return 0x7FFF;
	}
	else
	{
		while (floor->pit_room != NO_ROOM)
		{
			r = &room[floor->pit_room];

			if (r->flags & (ROOM_UNDERWATER | ROOM_SWAMP))
			{
				h = GetMaximumFloor(floor, x, z);
				floor = GetFloor(x, y, z, &room_number);
				return GetHeight(floor, x, y, z) - h;
			}

			floor = &r->floor[((z - r->z) >> WALL_SHIFT) + r->x_size * ((x - r->x) >> WALL_SHIFT)];
		}

		return NO_HEIGHT;
	}
}

void LaraTestWaterDepth(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	long wd;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	wd = GetWaterDepth(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);

	if (wd == NO_HEIGHT)
	{
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		item->fallspeed = 0;
	}
	else if (wd <= 512)
	{
		item->anim_number = ANIM_SWIM2QSTND;
		item->frame_number = anims[ANIM_SWIM2QSTND].frame_base;
		item->current_anim_state = AS_WATEROUT;
		item->goal_anim_state = AS_STOP;
		item->pos.x_rot = 0;
		item->pos.z_rot = 0;
		item->gravity_status = 0;
		item->speed = 0;
		item->fallspeed = 0;
		lara.water_status = LARA_WADE;
		item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	}
}

void LaraWaterCurrent(COLL_INFO* coll)
{
	OBJECT_VECTOR* sink;
	long angle, speed, shifter, absvel;

	if (lara.current_active)
	{
		sink = &camera.fixed[lara.current_active - 1];
		speed = sink->data;
		angle = ((mGetAngle(sink->x, sink->z, lara_item->pos.x_pos, lara_item->pos.z_pos) - 0x4000) >> 4) & 0xFFF;
		lara.current_xvel += short((((speed * rcossin_tbl[2 * angle]) >> 2) - lara.current_xvel) >> 4);
		lara.current_zvel += short((((speed * rcossin_tbl[2 * angle + 1]) >> 2) - lara.current_zvel) >> 4);
		lara_item->pos.y_pos += (sink->y - lara_item->pos.y_pos) >> 4;
	}
	else
	{
		absvel = abs(lara.current_xvel);

		if (absvel > 16)
			shifter = 4;
		else if (absvel > 8)
			shifter = 3;
		else
			shifter = 2;

		lara.current_xvel -= lara.current_xvel >> shifter;

		if (abs(lara.current_xvel) < 4)
			lara.current_xvel = 0;

		absvel = abs(lara.current_zvel);

		if (absvel > 16)
			shifter = 4;
		else if (absvel > 8)
			shifter = 3;
		else
			shifter = 2;

		lara.current_zvel -= lara.current_zvel >> shifter;

		if (abs(lara.current_zvel) < 4)
			lara.current_zvel = 0;

		if (!lara.current_xvel && !lara.current_zvel)
			return;
	}

	lara_item->pos.x_pos += lara.current_xvel >> 8;
	lara_item->pos.z_pos += lara.current_zvel >> 8;
	lara.current_active = 0;
	coll->facing = (short)phd_atan(lara_item->pos.z_pos - coll->old.z, lara_item->pos.x_pos - coll->old.x);
	GetCollisionInfo(coll, lara_item->pos.x_pos, lara_item->pos.y_pos + 200, lara_item->pos.z_pos, lara_item->room_number, 400);

	switch (coll->coll_type)
	{
	case CT_FRONT:

		if (lara_item->pos.x_rot > 6370)
			lara_item->pos.x_rot += 364;
		else if (lara_item->pos.x_rot < -6370)
			lara_item->pos.x_rot -= 364;
		else
			lara_item->fallspeed = 0;

		break;

	case CT_TOP:
		lara_item->pos.x_rot -= 364;
		break;

	case CT_TOP_FRONT:
		lara_item->fallspeed = 0;
		break;

	case CT_LEFT:
		lara_item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		lara_item->pos.y_rot -= 910;
		break;
	}

	if (coll->mid_floor < 0)
		lara_item->pos.y_pos += coll->mid_floor;

	ShiftItem(lara_item, coll);
	coll->old.x = lara_item->pos.x_pos;
	coll->old.y = lara_item->pos.y_pos;
	coll->old.z = lara_item->pos.z_pos;
}

void LaraSwimCollision(ITEM_INFO* item, COLL_INFO* coll)
{
	long y;

	if (item->pos.x_rot < -0x4000 || item->pos.x_rot > 0x4000)
		lara.move_angle = item->pos.y_rot + 0x8000;
	else
		lara.move_angle = item->pos.y_rot;

	coll->facing = lara.move_angle;
	y = 762 * phd_sin(item->pos.x_rot) >> W2V_SHIFT;

	if (y < 0)
		y = -y;

	if (y < 200)
		y = 200;

	coll->bad_neg = -y;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos + y / 2, item->pos.z_pos, item->room_number, y);
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_FRONT:

		if (item->pos.x_rot > 8190)
			item->pos.x_rot += 364;
		else if (item->pos.x_rot < -8190)
			item->pos.x_rot -= 364;
		else
			item->fallspeed = 0;

		break;

	case CT_TOP:

		if (item->pos.x_rot < -8190)
			item->pos.x_rot -= 364;

		break;

	case CT_TOP_FRONT:
		item->fallspeed = 0;
		break;

	case CT_LEFT:
		item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 910;
		break;

	case CT_CLAMP:
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
		item->fallspeed = 0;
		return;
	}

	if (coll->mid_floor < 0)
	{
		item->pos.x_rot += 364;
		item->pos.y_pos += coll->mid_floor;
	}

	if (lara.water_status != LARA_CHEAT && !lara.extra_anim)
		LaraTestWaterDepth(item, coll);
}
