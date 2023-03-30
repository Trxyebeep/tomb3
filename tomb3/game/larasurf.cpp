#include "../tomb3/pch.h"
#include "larasurf.h"
#include "lara.h"
#include "laramisc.h"
#include "../3dsystem/phd_math.h"
#include "collide.h"
#include "larafire.h"
#include "control.h"
#include "laraswim.h"
#include "../specific/input.h"
#include "camera.h"

void LaraSurface(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = -4004;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -128;
	coll->bad_ceiling = 100;
	coll->old.x = item->pos.x_pos;
	coll->old.y = item->pos.y_pos;
	coll->old.z = item->pos.z_pos;
	coll->radius = 100;
	coll->trigger = 0;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->lava_is_pit = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;

	if (input & IN_LOOK && lara.look)
		LookLeftRight();
	else
		ResetLook();

	lara.look = 1;
	lara_control_routines[item->current_anim_state](item, coll);

	if (item->pos.z_rot >= -364 && item->pos.z_rot <= 364)
		item->pos.z_rot = 0;
	else if (item->pos.z_rot >= 0)
		item->pos.z_rot -= 364;
	else
		item->pos.z_rot += 364;

	if (lara.current_active && lara.water_status != LARA_CHEAT)
		LaraWaterCurrent(coll);

	AnimateLara(item);
	item->pos.x_pos += ((phd_sin(lara.move_angle) * item->fallspeed) >> 16);
	item->pos.z_pos += ((phd_cos(lara.move_angle) * item->fallspeed) >> 16);
	LaraBaddieCollision(item, coll);

	if (lara.skidoo == NO_ITEM)
		lara_collision_routines[item->current_anim_state](item, coll);

	UpdateLaraRoom(item, 100);
	LaraGun();
	LaraOnPad = 0;
	TestTriggers(coll->trigger, 0);

	if (!LaraOnPad)
		lara_item->item_flags[1] = 0;
}

void lara_as_surfswim(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 728;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 728;

	if (!(input & IN_FORWARD) || input & IN_JUMP)
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_BACK))
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfleft(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_LSTEP))
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surfright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	lara.dive_count = 0;

	if (input & IN_LEFT)
		item->pos.y_rot -= 364;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 364;

	if (!(input & IN_RSTEP))
		item->goal_anim_state = AS_SURFTREAD;

	item->fallspeed += 8;

	if (item->fallspeed > 60)
		item->fallspeed = 60;
}

void lara_as_surftread(ITEM_INFO* item, COLL_INFO* coll)
{
	item->fallspeed -= 4;

	if (item->fallspeed < 0)
		item->fallspeed = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_UWDEATH;
		return;
	}

	if (input & IN_LOOK)
	{
		LookUpDown();
		return;
	}

	if (input & IN_LEFT)
		item->pos.y_rot -= 728;
	else if (input & IN_RIGHT)
		item->pos.y_rot += 728;

	if (input & IN_FORWARD)
		item->goal_anim_state = AS_SURFSWIM;
	else if (input & IN_BACK)
		item->goal_anim_state = AS_SURFBACK;

	if (input & IN_LSTEP)
		item->goal_anim_state = AS_SURFLEFT;
	else if (input & IN_RSTEP)
		item->goal_anim_state = AS_SURFRIGHT;

	if (input & IN_JUMP)
	{
		lara.dive_count++;

		if (lara.dive_count == 10)
			item->goal_anim_state = AS_SWIM;
	}
	else
		lara.dive_count = 0;
}

void lara_col_surfswim(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_neg = -384;
	lara.move_angle = item->pos.y_rot;
	LaraSurfaceCollision(item, coll);
	LaraTestWaterClimbOut(item, coll);
}

void lara_col_surfback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x8000;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surfleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x4000;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surfright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x4000;
	LaraSurfaceCollision(item, coll);
}

void lara_col_surftread(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state == AS_SWIM)
	{
		item->anim_number = ANIM_SURFDIVE;
		item->frame_number = anims[ANIM_SURFDIVE].frame_base;
		item->current_anim_state = AS_DIVE;
		item->pos.x_rot = -8190;
		item->fallspeed = 80;
		lara.water_status = LARA_UNDERWATER;
	}

	lara.move_angle = item->pos.y_rot;
	LaraSurfaceCollision(item, coll);
}

long LaraTestWaterClimbOut(ITEM_INFO* item, COLL_INFO* coll)
{
	long hdif;
	short angle;

	if (coll->coll_type != CT_FRONT || !(input & IN_ACTION) || abs(coll->left_floor2 - coll->right_floor2) >= 60 || lara.gun_status != LG_ARMLESS &&
		(lara.gun_status != LG_READY || lara.gun_type != LG_FLARE) || coll->front_ceiling > 0 || coll->mid_ceiling > -384)
		return 0;

	hdif = coll->front_floor + 700;

	if (hdif <= -512 || hdif > 316)
		return 0;

	angle = item->pos.y_rot;

	if (angle >= -0x18E2 && angle <= 0x18E2)
		angle = 0;
	else if (angle >= 0x271E && angle <= 0x58E2)
		angle = 0x4000;
	else if (angle >= 0x671D || angle <= -0x671D)
		angle = -0x8000;
	else if (angle >= -0x58E2 && angle <= -0x271E)
		angle = -0x4000;

	if (angle & 0x3FFF)
		return 0;

	item->pos.y_pos += coll->front_floor + 695;
	UpdateLaraRoom(item, -381);

	if (!angle)
		item->pos.z_pos = (item->pos.z_pos & ~WALL_MASK) + 1124;
	else if (angle == 0x4000)
		item->pos.x_pos = (item->pos.x_pos & ~WALL_MASK) + 1124;
	else if (angle == -0x8000)
		item->pos.z_pos = (item->pos.z_pos & ~WALL_MASK) - 100;
	else if (angle == -0x4000)
		item->pos.x_pos = (item->pos.x_pos & ~WALL_MASK) - 100;

	if (hdif < -128)
	{
		item->anim_number = ANIM_SURFCLIMB;
		item->frame_number = anims[ANIM_SURFCLIMB].frame_base;
	}
	else if (hdif < 128)
	{
		item->anim_number = ANIM_SURF2STND;
		item->frame_number = anims[ANIM_SURF2STND].frame_base;
	}
	else
	{
		item->anim_number = ANIM_SURF2QSTND;
		item->frame_number = anims[ANIM_SURF2QSTND].frame_base;
	}

	item->current_anim_state = AS_WATEROUT;
	item->goal_anim_state = AS_STOP;
	item->pos.y_rot = angle;
	lara.gun_status = LG_HANDSBUSY;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->gravity_status = 0;
	item->speed = 0;
	item->fallspeed = 0;
	lara.water_status = LARA_ABOVEWATER;
	return 1;
}

long LaraTestWaterStepOut(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->mid_type == BIG_SLOPE || coll->mid_type == DIAGONAL || coll->mid_floor >= 0)
		return 0;

	if (coll->mid_floor < -128)
	{
		item->anim_number = ANIM_SURF2WADE1;
		item->frame_number = anims[ANIM_SURF2WADE1].frame_base;
		item->current_anim_state = AS_WATEROUT;
		item->goal_anim_state = AS_STOP;
	}
	else if (item->goal_anim_state == AS_SURFLEFT)
		item->goal_anim_state = AS_STEPLEFT;
	else if (item->goal_anim_state == AS_SURFRIGHT)
		item->goal_anim_state = AS_STEPRIGHT;
	else
	{
		item->anim_number = ANIM_WADE;
		item->frame_number = anims[ANIM_WADE].frame_base;
		item->current_anim_state = AS_WADE;
		item->goal_anim_state = AS_WADE;
	}

	item->pos.y_pos += coll->front_floor + 695;
	UpdateLaraRoom(item, -381);
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->gravity_status = 0;
	item->speed = 0;
	item->fallspeed = 0;
	lara.water_status = LARA_WADE;
	return 1;
}

void LaraSurfaceCollision(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos + 700, item->pos.z_pos, item->room_number, 800);
	ShiftItem(item, coll);

	if (coll->coll_type & (CT_CLAMP | CT_TOP_FRONT | CT_TOP | CT_FRONT) || coll->mid_floor < 0 &&
		(coll->mid_type == BIG_SLOPE || coll->mid_type == DIAGONAL))
	{
		item->fallspeed = 0;
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
	}
	else if (coll->coll_type == CT_LEFT)
		item->pos.y_rot += 910;
	else if (coll->coll_type == CT_RIGHT)
		item->pos.y_rot -= 910;

	if (GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number) - item->pos.y_pos > -100)
		LaraTestWaterStepOut(item, coll);
	else
	{
		item->anim_number = ANIM_SURFDIVE;
		item->frame_number = anims[ANIM_SURFDIVE].frame_base;
		item->current_anim_state = AS_DIVE;
		item->goal_anim_state = AS_SWIM;
		item->pos.x_rot = -8190;
		item->fallspeed = 80;
		lara.water_status = LARA_UNDERWATER;
	}
}
