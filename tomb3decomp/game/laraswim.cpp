#include "../tomb3/pch.h"
#include "laraswim.h"
#include "lara.h"
#include "control.h"
#include "laramisc.h"
#include "../3dsystem/phd_math.h"
#include "collide.h"
#include "larafire.h"

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

#ifdef TROYESTUFF
		if (lara.water_status == LARA_CHEAT)
		{
			if (item->fallspeed > 400)
				item->fallspeed = 400;
		}
		else
#endif
			if (item->fallspeed > 200)
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

void inject_laraswim(bool replace)
{
	INJECT(0x0044E950, LaraUnderWater, replace);
	INJECT(0x0044EC10, SwimTurn, replace);
	INJECT(0x0044EBA0, lara_as_swim, replace);
	INJECT(0x0044ECA0, lara_as_glide, replace);
	INJECT(0x0044ED20, lara_as_tread, replace);
	INJECT(0x0044EDB0, lara_as_dive, replace);
	INJECT(0x0044EDD0, lara_as_uwdeath, replace);
	INJECT(0x0044EE30, lara_as_waterroll, replace);
	INJECT(0x0044EE40, lara_col_swim, replace);
	INJECT(0x0044F0A0, lara_col_glide, replace);
	INJECT(0x0044F0C0, lara_col_tread, replace);
	INJECT(0x0044F0E0, lara_col_dive, replace);
	INJECT(0x0044F100, lara_col_uwdeath, replace);
	INJECT(0x0044F160, lara_col_waterroll, replace);
}
