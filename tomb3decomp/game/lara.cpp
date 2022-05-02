#include "../tomb3/pch.h"
#include "lara.h"
#include "control.h"
#include "collide.h"
#include "laraanim.h"
#include "laraclmb.h"
#include "../3dsystem/phd_math.h"
#include "laramisc.h"
#include "sound.h"
#include "draw.h"

long LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	long landspeed, h, y;
	short room_number;

	room_number = item->room_number;
	y = item->pos.y_pos;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	item->pos.y_pos = y;
	item->floor = y;

	if (ABS(y - item->pos.y_pos) > 256)
		item->pos.y_pos = y;

	LaraOnPad = 0;
	TestTriggers(trigger_index, 0);

	if (!LaraOnPad)
		lara_item->item_flags[1] = 0;

	item->pos.y_pos = y;
	landspeed = item->fallspeed - 140;

	if (landspeed <= 0)
		return 0;

	if (landspeed > 14)
		item->hit_points = -1;
	else
		item->hit_points += short(-1000 * landspeed * landspeed / 196);

	return item->hit_points <= 0;
}

long TestLaraSlide(ITEM_INFO* item, COLL_INFO* coll)
{
	static short old_ang = 1;
	short ang_diff, ang;

	if (ABS(coll->tilt_x) <= 2 && ABS(coll->tilt_z) <= 2)
		return 0;

	ang = 0;

	if (coll->tilt_x > 2)
		ang = -16384;
	else if (coll->tilt_x < -2)
		ang = 16384;

	if (coll->tilt_z > 2 && coll->tilt_z > ABS(coll->tilt_x))
		ang = -32768;
	else if (coll->tilt_z < -2 && (-coll->tilt_z > ABS(coll->tilt_x)))
		ang = 0;

	ang_diff = ang - item->pos.y_rot;
	ShiftItem(item, coll);

	if (ang_diff >= -16384 && ang_diff <= 16384)
	{
		if (item->current_anim_state != AS_SLIDE || old_ang != ang)
		{
			item->anim_number = ANIM_SLIDE;
			item->frame_number = anims[ANIM_SLIDE].frame_base;
			item->goal_anim_state = AS_SLIDE;
			item->current_anim_state = AS_SLIDE;
			item->pos.y_rot = ang;
		}
	}
	else
	{
		if (item->current_anim_state != AS_SLIDEBACK || old_ang != ang)
		{
			item->anim_number = ANIM_SLIDEBACK;
			item->frame_number = anims[ANIM_SLIDEBACK].frame_base;
			item->goal_anim_state = AS_SLIDEBACK;
			item->current_anim_state = AS_SLIDEBACK;
			item->pos.y_rot = ang - 32768;
		}
	}

	lara.move_angle = ang;
	old_ang = ang;
	return 1;
}

long LaraTestClimbStance(ITEM_INFO* item, COLL_INFO* coll)
{
	long shift_r, shift_l;

	if (LaraTestClimbPos(item, coll->radius, coll->radius + 120, -700, 512, &shift_r) != 1)
		return 0;

	if (LaraTestClimbPos(item, coll->radius, -120 - coll->radius, -700, 512, &shift_l) != 1)
		return 0;

	if (shift_r)
	{
		if (shift_l)
		{
			if (shift_l < 0 != shift_r < 0)
				return 0;

			if (shift_r < 0 && shift_l < shift_r)
				shift_r = shift_l;
			else if (shift_r > 0 && shift_l > shift_r)
				shift_r = shift_l;
		}

		item->pos.y_pos += shift_r;
	}
	else if (shift_l)
		item->pos.y_pos += shift_l;

	return 1;
}

long TestLaraVault(ITEM_INFO* item, COLL_INFO* coll)
{
	long slope;
	short angle;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || coll->coll_type != CT_FRONT)
		return 0;

	angle = item->pos.y_rot;

	if (angle >= -5460 && angle <= 5460)
		angle = 0;
	else if (angle >= 10924 && angle <= 21844)
		angle = 0x4000;
	else if (angle >= 27307 || angle <= -27307)
		angle = -0x8000;
	else if (angle >= -21844 && angle <= -10924)
		angle = -0x4000;

	if (angle & 0x3FFF)
		return 0;

	slope = ABS(coll->left_floor2 - coll->right_floor2) >= 60;

	if (coll->front_floor >= -640 && coll->front_floor <= -384)
	{
		if (slope || coll->front_floor - coll->front_ceiling < 0 || coll->left_floor2 - coll->left_ceiling2 < 0 ||
			coll->right_floor2 - coll->right_ceiling2 < 0 || (room[lara_item->room_number].flags & ROOM_SWAMP && lara.water_surface_dist < -768))
			return 0;

		item->anim_number = ANIM_VAULT2;
		item->frame_number = anims[ANIM_VAULT2].frame_base;
		item->current_anim_state = AS_NULL;
		item->goal_anim_state = AS_STOP;
		item->pos.y_pos += coll->front_floor + 512;
		lara.gun_status = LG_HANDSBUSY;
	}
	else if (coll->front_floor >= -896 && coll->front_floor <= -640)
	{
		if (slope || coll->front_floor - coll->front_ceiling < 0 || coll->left_floor2 - coll->left_ceiling2 < 0 ||
			coll->right_floor2 - coll->right_ceiling2 < 0 || (room[lara_item->room_number].flags & ROOM_SWAMP && lara.water_surface_dist < -768))
			return 0;

		item->anim_number = ANIM_VAULT3;
		item->frame_number = anims[ANIM_VAULT3].frame_base;
		item->current_anim_state = AS_NULL;
		item->goal_anim_state = AS_STOP;
		item->pos.y_pos += coll->front_floor + 768;
		lara.gun_status = LG_HANDSBUSY;
	}
	else if (!slope && coll->front_floor >= -1920 && coll->front_floor <= -896)
	{
		if (room[lara_item->room_number].flags & ROOM_SWAMP)
			return 0;

		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		item->current_anim_state = AS_STOP;
		item->goal_anim_state = AS_UPJUMP;
		lara.calc_fallspeed = -3 - (short)phd_sqrt(-9600 - 12 * coll->front_floor);
		AnimateLara(item);
	}
	else if (lara.climb_status)
	{
		if (coll->front_floor <= -1920 && lara.water_status != LARA_WADE && coll->left_floor2 <= -1920 && coll->right_floor2 <= -2048 && coll->mid_ceiling <= -1158)
		{
			item->anim_number = ANIM_STOP;
			item->frame_number = anims[ANIM_STOP].frame_base;
			item->current_anim_state = AS_STOP;
			item->goal_anim_state = AS_UPJUMP;
			lara.calc_fallspeed = -116;
			AnimateLara(item);
		}
		else if ((coll->front_floor < -1024 || coll->front_ceiling >= 506) && coll->mid_ceiling <= -518)
		{
			ShiftItem(item, coll);

			if (LaraTestClimbStance(item, coll))
			{
				item->anim_number = ANIM_STOP;
				item->frame_number = anims[ANIM_STOP].frame_base;
				item->current_anim_state = AS_STOP;
				item->goal_anim_state = AS_CLIMBSTNC;
				AnimateLara(item);
				item->pos.y_rot = angle;
				lara.gun_status = LG_HANDSBUSY;
				return 1;
			}

			return 0;
		}
	}
	else
		return 0;

	item->pos.y_rot = angle;
	ShiftItem(item, coll);
	return 1;
}

void lara_slide_slope(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -512;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	LaraDeflectEdge(item, coll);

	if (coll->mid_floor <= 200)
	{
		TestLaraSlide(item, coll);
		item->pos.y_pos += coll->mid_floor;

		if ((ABS(coll->tilt_x)) <= 2 && (ABS(coll->tilt_z)) <= 2)
		{
			item->goal_anim_state = AS_STOP;
			StopSoundEffect(3);
		}
	}
	else
	{
		if (item->current_anim_state == AS_SLIDE)
		{
			item->anim_number = ANIM_FALLDOWN;
			item->frame_number = anims[ANIM_FALLDOWN].frame_base;
			item->current_anim_state = AS_FORWARDJUMP;
			item->goal_anim_state = AS_FORWARDJUMP;
		}
		else
		{
			item->anim_number = ANIM_FALLBACK;
			item->frame_number = anims[ANIM_FALLBACK].frame_base;
			item->current_anim_state = AS_FALLBACK;
			item->goal_anim_state = AS_FALLBACK;
		}

		StopSoundEffect(3);
		item->gravity_status = 1;
		item->fallspeed = 0;
	}
}

void GetLaraCollisionInfo(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 762);
}

long LaraHitCeiling(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type != CT_TOP && coll->coll_type != CT_CLAMP)
		return 0;

	item->pos.x_pos = coll->old.x;
	item->pos.y_pos = coll->old.y;
	item->pos.z_pos = coll->old.z;
	item->goal_anim_state = AS_STOP;
	item->current_anim_state = AS_STOP;
	item->anim_number = ANIM_STOP;
	item->frame_number = anims[ANIM_STOP].frame_base;
	item->speed = 0;
	item->fallspeed = 0;
	item->gravity_status = 0;
	return 1;
}

long LaraDeflectEdge(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->coll_type == CT_TOP_FRONT)
	{
		ShiftItem(item, coll);
		item->current_anim_state = AS_STOP;
		item->goal_anim_state = AS_STOP;
		item->speed = 0;
		item->gravity_status = 0;
		return 1;
	}
	
	if (coll->coll_type == CT_LEFT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot += 910;
	}
	else if (coll->coll_type == CT_RIGHT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot -= 910;
	}

	return 0;
}

void LaraDeflectEdgeJump(ITEM_INFO* item, COLL_INFO* coll)
{
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_FRONT:
	case CT_TOP_FRONT:

		if (!lara.climb_status || item->speed != 2)
		{
			if (coll->mid_floor > 512)
			{
				item->current_anim_state = AS_FASTFALL;
				item->goal_anim_state = AS_FASTFALL;
				item->anim_number = ANIM_FASTSPLAT;
				item->frame_number = anims[ANIM_FASTSPLAT].frame_base + 1;
			}
			else if (coll->mid_floor <= 128)
			{
				item->current_anim_state = AS_LAND;
				item->goal_anim_state = AS_LAND;
				item->anim_number = ANIM_LAND;
				item->frame_number = anims[ANIM_LAND].frame_base;
			}

			item->speed /= 4;
			lara.move_angle += 0x8000;

			if (item->fallspeed <= 0)
				item->fallspeed = 1;
		}

		break;

	case CT_LEFT:
		item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 910;
		break;

	case CT_TOP:

		if (item->fallspeed <= 0)
			item->fallspeed = 1;

		break;

	case CT_CLAMP:
		item->pos.x_pos -= (100 * phd_sin(coll->facing)) >> 14;
		item->pos.z_pos -= (100 * phd_cos(coll->facing)) >> 14;
		item->speed = 0;
		coll->mid_floor = 0;

		if (item->fallspeed <= 0)
			item->fallspeed = 16;

		break;
	}
}

void LaraSlideEdgeJump(ITEM_INFO* item, COLL_INFO* coll)
{
	ShiftItem(item, coll);

	switch (coll->coll_type)
	{
	case CT_LEFT:
		item->pos.y_rot += 910;
		break;

	case CT_RIGHT:
		item->pos.y_rot -= 910;
		break;

	case CT_TOP:
	case CT_TOP_FRONT:
		if (item->fallspeed <= 0)
			item->fallspeed = 1;
		break;

	case CT_CLAMP:
		item->pos.z_pos -= (100 * phd_cos(coll->facing)) >> 14;
		item->pos.x_pos -= (100 * phd_sin(coll->facing)) >> 14;
		item->speed = 0;
		coll->mid_floor = 0;

		if (item->fallspeed <= 0)
			item->fallspeed = 16;

		break;
	}
}

long TestHangSwingIn(ITEM_INFO* item, short angle)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short room_number;

	x = item->pos.x_pos;
	y = item->pos.y_pos;
	z = item->pos.z_pos;
	room_number = item->room_number;

	switch (angle)
	{
	case 0:
		z += 256;
		break;

	case 0x4000:
		x += 256;
		break;

	case -0x4000:
		x -= 256;
		break;

	case -0x8000:
		z -= 256;
		break;
	}

	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (h != NO_HEIGHT && h - y > 0 && c - y < -400 && y - c - 819 > -72)
		return 1;

	return 0;
}

long LaraFallen(ITEM_INFO* item, COLL_INFO* coll)
{
	if (lara.water_status != LARA_WADE && coll->mid_floor > 384)
	{
		item->anim_number = ANIM_FALLDOWN;
		item->frame_number = anims[ANIM_FALLDOWN].frame_base;
		item->current_anim_state = AS_FORWARDJUMP;
		item->goal_anim_state = AS_FORWARDJUMP;
		item->fallspeed = 0;
		item->gravity_status = 1;
		return 1;
	}

	return 0;
}

void lara_as_duck(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.IsDucked = 1;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & (IN_FORWARD | IN_BACK) && lara.gun_status == LG_ARMLESS && item->frame_number > anims[ANIM_DUCKBREATHE].frame_base + 10)
	{
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		item->goal_anim_state = AS_ALL4S;
	}
}

void lara_col_duck(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->facing = item->pos.y_rot;
	coll->radius = 200;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraFallen(item, coll))
		lara.gun_status = LG_ARMLESS;
	else
	{
		if (coll->mid_ceiling >= -362)
			lara.keep_ducked = 1;
		else
			lara.keep_ducked = 0;

		ShiftItem(item, coll);
		item->pos.y_pos += coll->mid_floor;

		if ((!(input & IN_DUCK) || lara.water_status == LARA_WADE) && !lara.keep_ducked && item->anim_number == ANIM_DUCKBREATHE)
			item->goal_anim_state = AS_STOP;
	}
}

void lara_as_all4s(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_DEATH;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if (item->anim_number == ANIM_DUCK_TO_ALL4S)
		lara.gun_status = LG_HANDSBUSY;

	if (TestLaraSlide(item, coll))
		return;

	camera.target_elevation = -4186;
}

void lara_col_all4s(ITEM_INFO* item, COLL_INFO* coll)
{
	long h;
	short angle;

	item->gravity_status = 0;
	item->fallspeed = 0;

	if (item->goal_anim_state == AS_CRAWL2HANG)
		return;

	lara.move_angle = item->pos.y_rot;
	coll->facing = lara.move_angle;
	coll->radius = 200;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraFallen(item, coll))
	{
		lara.gun_status = LG_ARMLESS;
		return;
	}

	if (coll->mid_ceiling >= -362)
		lara.keep_ducked = 1;
	else
		lara.keep_ducked = 0;

	ShiftItem(item, coll);
	item->pos.y_pos += coll->mid_floor;

	if (!(input & IN_DUCK) && !lara.keep_ducked || input & IN_DRAW)
	{
		item->goal_anim_state = AS_DUCK;
		return;
	}

	if (item->anim_number != ANIM_ALL4S && item->anim_number != ANIM_ALL4S2)
		return;

	if (input & IN_FORWARD)
	{
		h = LaraFloorFront(item, item->pos.y_rot, 256);

		if (h < 255 && h > -255 && height_type != BIG_SLOPE)
			item->goal_anim_state = AS_CRAWL;
	}
	else if (input & IN_BACK)
	{
		h = LaraCeilingFront(item, item->pos.y_rot, -300);

		if (h == NO_HEIGHT || h > 256)
			return;

		h = LaraFloorFront(item, item->pos.y_rot, -300);

		if (h < 255 && h > -255 && height_type != BIG_SLOPE)
		{
			item->goal_anim_state = AS_CRAWLBACK;
			return;
		}

		if (input & IN_ACTION && h > 768 && !GetStaticObjects(item, item->pos.y_rot + 0x8000, 512, 50, 300))
		{
			angle = (ushort)(item->pos.y_rot + 0x2000) / 0x4000;

			switch (angle)
			{
			case NORTH:
				item->pos.y_rot = 0;
				item->pos.z_pos = (item->pos.z_pos & ~1023) + 225;
				break;

			case EAST:
				item->pos.y_rot = 0x4000;
				item->pos.x_pos = (item->pos.x_pos & ~1023) + 225;
				break;

			case SOUTH:
				item->pos.y_rot = -0x8000;
				item->pos.z_pos = (item->pos.z_pos | 0x3FF) - 225;
				break;

			case WEST:
				item->pos.y_rot = -0x4000;
				item->pos.x_pos = (item->pos.x_pos | 0x3FF) - 225;
				break;
			}

			item->goal_anim_state = AS_CRAWL2HANG;
		}
	}
	else if (input & IN_LEFT)
	{
		item->anim_number = ANIM_ALL4TURNL;
		item->frame_number = anims[ANIM_ALL4TURNL].frame_base;
		item->current_anim_state = AS_ALL4TURNL;
		item->goal_anim_state = AS_ALL4TURNL;
	}
	else if (input & IN_RIGHT)
	{
		item->anim_number = ANIM_ALL4TURNR;
		item->frame_number = anims[ANIM_ALL4TURNR].frame_base;
		item->current_anim_state = AS_ALL4TURNR;
		item->goal_anim_state = AS_ALL4TURNR;
	}
}

short LaraFloorFront(ITEM_INFO* item, short ang, long dist)
{
	long x, y, z, height;
	short room_num;

	room_num = item->room_number;
	x = item->pos.x_pos + ((dist * phd_sin(ang)) >> 14);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + ((dist * phd_cos(ang)) >> 14);
	height = GetHeight(GetFloor(x, y, z, &room_num), x, y, z);

	if (height != NO_HEIGHT)
		height -= item->pos.y_pos;

	return (short)height;
}

short LaraCeilingFront(ITEM_INFO* item, short ang, long dist)
{
	long x, y, z, height;
	short room_num;

	x = item->pos.x_pos + ((dist * phd_sin(ang)) >> 14);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + ((dist * phd_cos(ang)) >> 14);
	room_num = item->room_number;
	height = GetCeiling(GetFloor(x, y, z, &room_num), x, y, z);

	if (height != NO_HEIGHT)
		height += 762 - item->pos.y_pos;

	return (short)height;
}

void lara_as_crawl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	camera.target_elevation = -4186;

	if (TestLaraSlide(item, coll))
		return;

	if (!(input & IN_FORWARD) && (!(input & IN_DUCK) && !lara.keep_ducked))
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -546)
			lara.turn_rate = -546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 546)
			lara.turn_rate = 546;
	}
}

void lara_col_crawl(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->radius = 200;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraDeflectEdgeDuck(item, coll))
	{
		item->current_anim_state = AS_ALL4S;
		item->goal_anim_state = AS_ALL4S;

		if (item->anim_number != ANIM_ALL4S)
		{
			item->anim_number = ANIM_ALL4S;
			item->frame_number = anims[ANIM_ALL4S].frame_base;
		}
	}
	else if (LaraFallen(item, coll))
		lara.gun_status = LG_ARMLESS;
	else
	{
		ShiftItem(item, coll);
		item->pos.y_pos += coll->mid_floor;
	}
}

long LaraDeflectEdgeDuck(ITEM_INFO* item, COLL_INFO* coll)
{
	if (coll->coll_type == CT_FRONT || coll->coll_type == CT_TOP_FRONT)
	{
		ShiftItem(item, coll);
		item->speed = 0;
		item->gravity_status = 0;
		return 1;
	}

	if (coll->coll_type == CT_LEFT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot += 364;
	}
	else if (coll->coll_type == CT_RIGHT)
	{
		ShiftItem(item, coll);
		item->pos.y_rot -= 364;
	}

	return 0;
}

void lara_as_all4turnl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	camera.target_elevation = -4186;

	if (!TestLaraSlide(item, coll))
	{
		item->pos.y_rot -= 273;

		if (!(input & IN_LEFT))
			item->goal_anim_state = AS_ALL4S;
	}
}

void lara_col_all4turnl(ITEM_INFO* item, COLL_INFO* coll)
{
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);
}

void lara_as_all4turnr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	camera.target_elevation = -4186;

	if (!TestLaraSlide(item, coll))
	{
		item->pos.y_rot += 273;

		if (!(input & IN_RIGHT))
			item->goal_anim_state = AS_ALL4S;
	}
}

void lara_col_all4turnr(ITEM_INFO* item, COLL_INFO* coll)
{
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);
}

void lara_as_crawlb(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_ALL4S;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	camera.target_elevation = -4186;

	if (TestLaraSlide(item, coll))
		return;

	if (input & IN_BACK)
	{
		if (input & IN_RIGHT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -546)
				lara.turn_rate = -546;
		}
		else if (input & IN_LEFT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 546)
				lara.turn_rate = 546;
		}
	}
	else
		item->goal_anim_state = AS_ALL4S;
}

void lara_col_crawlb(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->radius = 250;
	coll->bad_pos = 255;
	coll->bad_neg = -255;
	coll->bad_ceiling = 400;
	lara.move_angle = item->pos.y_rot + 0x8000;
	coll->facing = lara.move_angle;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 400);

	if (LaraDeflectEdgeDuck(item, coll))
	{
		item->current_anim_state = AS_ALL4S;
		item->goal_anim_state = AS_ALL4S;

		if (item->anim_number != ANIM_ALL4S)
		{
			item->anim_number = ANIM_ALL4S;
			item->frame_number = anims[ANIM_ALL4S].frame_base;
		}
	}
	else if (LaraFallen(item, coll))
		lara.gun_status = 0;
	else
	{
		ShiftItem(item, coll);
		item->pos.y_pos += coll->mid_floor;
		lara.move_angle = item->pos.y_rot;
	}
}

void lara_col_crawl2hang(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long edge, edge_catch;
	short angle;

	if (item->anim_number != ANIM_CRAWL_TO_HANG)
		return;

	item->fallspeed = 512;
	item->pos.y_pos |= 255;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	lara.move_angle = item->pos.y_rot;
	coll->facing = lara.move_angle;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 870);
	edge_catch = LaraTestEdgeCatch(item, coll, &edge);

	if (edge_catch <= 0 && !LaraTestHangOnClimbWall(item, coll))
		return;

	angle = item->pos.y_rot;

	if (angle >= -6370 && angle <= 6370)
		angle = 0;
	else if (angle >= 10014 && angle <= 22754)
		angle = 16384;
	else if (angle >= 26397 || angle <= -26397)
		angle = -32768;
	else if (angle >= -22754 && angle <= -10014)
		angle = -16384;

	if (angle & 0x3FFF)
		return;

	if (TestHangSwingIn(item, angle))
	{
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		item->anim_number = ANIM_HANG2STOP;
		item->frame_number = anims[ANIM_HANG2STOP].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
	else
	{
		item->anim_number = ANIM_GRABLEDGE;
		item->frame_number = anims[ANIM_GRABLEDGE].frame_base;
		item->current_anim_state = AS_HANG;
		item->goal_anim_state = AS_HANG;
	}

	bounds = GetBoundsAccurate(item);

	if (edge_catch <= 0)
		item->pos.y_pos = edge - bounds[2];
	else
	{
		item->pos.y_pos += coll->front_floor - bounds[2];
		item->pos.x_pos += coll->shift.x;
		item->pos.z_pos += coll->shift.z;
	}

	item->gravity_status = 1;
	item->pos.y_rot = angle;
	item->speed = 2;
	item->fallspeed = 1;
	lara.gun_status = LG_HANDSBUSY;
}

long LaraTestEdgeCatch(ITEM_INFO* item, COLL_INFO* coll, long* edge)
{
	short* bounds;
	long hdif;

	bounds = GetBoundsAccurate(item);
	hdif = coll->front_floor - bounds[2];

	if ((hdif < 0 && hdif + item->fallspeed < 0) || (hdif > 0 && hdif + item->fallspeed > 0))
	{
		hdif = item->pos.y_pos + bounds[2];

		if ((hdif >> 8) != ((hdif + item->fallspeed) >> 8))
		{
			if (item->fallspeed > 0)
				*edge = (hdif + item->fallspeed) & ~255;
			else
				*edge = hdif & ~255;

			return -1;
		}

		return 0;
	}

	if (ABS(coll->left_floor2 - coll->right_floor2) >= 60)
		return 0;

	return 1;
}

long LaraTestHangOnClimbWall(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long shift, pos;
	short angle;

	if (!lara.climb_status || item->fallspeed < 0)
		return 0;

	angle = (ushort)(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
	case SOUTH:
		item->pos.z_pos += coll->shift.z;
		break;

	case EAST:
	case WEST:
		item->pos.x_pos += coll->shift.x;
		break;
	}

	bounds = GetBoundsAccurate(item);

	if (!LaraTestClimbPos(item, coll->radius, coll->radius, bounds[2], bounds[3] - bounds[2], &shift))
		return 0;

	if (!LaraTestClimbPos(item, coll->radius, -coll->radius, bounds[2], bounds[3] - bounds[2], &shift))
		return 0;

	pos = LaraTestClimbPos(item, coll->radius, 0, bounds[2], bounds[3] - bounds[2], &shift);

	if (!pos)
		return 0;

	if (pos != 1)
	{
		item->pos.y_pos += shift;
		return 1;
	}

	return pos;
}

long TestWall(ITEM_INFO* item, long front, long right, long down)
{
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short angle, room_num;

	room_num = item->room_number;
	x = item->pos.x_pos;
	y = down + item->pos.y_pos;
	z = item->pos.z_pos;
	angle = (ushort)(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
		x -= right;
		break;

	case EAST:
		z -= right;
		break;

	case SOUTH:
		x += right;
		break;

	case WEST:
		z += right;
		break;
	}

	GetFloor(x, y, z, &room_num);

	switch (angle)
	{
	case NORTH:
		z += front;
		break;

	case EAST:
		x += front;
		break;

	case SOUTH:
		z -= front;
		break;

	case WEST:
		x -= front;
		break;
	}

	floor = GetFloor(x, y, z, &room_num);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (h == NO_HEIGHT)
		return 1;

	if (h - y <= 0 || c - y >= 0)
		return 2;

	return 0;
}

void LaraCollideStop(ITEM_INFO* item, COLL_INFO* coll)
{
	switch (coll->old_anim_state)
	{
	case AS_STOP:
	case AS_TURN_R:
	case AS_TURN_L:
	case AS_FASTTURN:

		item->current_anim_state = coll->old_anim_state;
		item->anim_number = coll->old_anim_number;
		item->frame_number = coll->old_frame_number;

		if (input & IN_LEFT)
			item->goal_anim_state = AS_TURN_L;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_TURN_R;
		else
			item->goal_anim_state = AS_STOP;

		AnimateLara(item);
		break;

	default:
		item->anim_number = ANIM_STOP;
		item->frame_number = anims[ANIM_STOP].frame_base;
		break;
	}
}

void lara_as_dash(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0 || !DashTimer || !(input & IN_SPRINT) || lara.water_status == LARA_WADE)
	{
		item->goal_anim_state = AS_RUN;
		return;
	}

	DashTimer--;

	if (input & IN_DUCK)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;

		item->pos.z_rot -= 273;

		if (item->pos.z_rot < -2912)
			item->pos.z_rot = -2912;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;

		item->pos.z_rot += 273;

		if (item->pos.z_rot > 2912)
			item->pos.z_rot = 2912;
	}

	if (input & IN_JUMP && !item->gravity_status)
		item->goal_anim_state = AS_DASHDIVE;
	else if (input & IN_FORWARD)
	{
		if (input & IN_WALK)
			item->goal_anim_state = AS_WALK;
		else
			item->goal_anim_state = AS_DASH;
	}
	else if (!(input & (IN_LEFT | IN_RIGHT)))
		item->goal_anim_state = AS_STOP;
}

void lara_col_dash(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll) || TestLaraVault(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		item->pos.z_rot = 0;

		if (TestWall(item, 256, 0, -640))
		{
			item->anim_number = ANIM_HITWALLLEFT;
			item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
			item->current_anim_state = AS_SPLAT;
			return;
		}

		LaraCollideStop(item, coll);
	}

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (item->frame_number < 3 || item->frame_number > 14)
		{
			item->anim_number = ANIM_RUNSTEPUP_RIGHT;
			item->frame_number = anims[ANIM_RUNSTEPUP_RIGHT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_RUNSTEPUP_LEFT;
			item->frame_number = anims[ANIM_RUNSTEPUP_LEFT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor < 50)
		item->pos.y_pos += coll->mid_floor;
	else
		item->pos.y_pos += 50;
}

void lara_as_dashdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN && item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_col_dashdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot + 0x8000;
	else
		lara.move_angle = item->pos.y_rot;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -256;
	coll->bad_ceiling = 192;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (LaraFallen(item, coll))
		return;

	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot;

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else if (lara.water_status == LARA_WADE || !(input & IN_FORWARD) || input & IN_WALK)
			item->goal_anim_state = AS_STOP;
		else
			item->goal_anim_state = AS_RUN;

		item->fallspeed = 0;
		item->gravity_status = 0;
		item->speed = 0;
		item->pos.y_pos += coll->mid_floor;
		AnimateLara(item);
	}

	ShiftItem(item, coll);
	item->pos.y_pos += coll->mid_floor;
}

void MonkeySwingFall(ITEM_INFO* item)
{
	item->anim_number = ANIM_STOPHANG;
	item->frame_number = anims[ANIM_STOPHANG].frame_base + 9;
	item->current_anim_state = AS_UPJUMP;
	item->goal_anim_state = AS_UPJUMP;
	item->gravity_status = 1;
	item->speed = 2;
	item->fallspeed = 1;
	item->pos.y_pos += 256;
	lara.gun_status = LG_ARMLESS;
}

void MonkeySwingSnap(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->pos.y_pos = GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 704;
}

short GetDirOctant(long rot)
{
	rot = ABS(rot);
	return rot >= 0x2000 && rot <= 0x6000;
}

short TestMonkeyLeft(ITEM_INFO* item, COLL_INFO* coll)
{
	short oct;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot - 0x4000;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 0;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	if (ABS(coll->mid_ceiling - coll->front_ceiling) > 50)
		return 0;

	if (coll->coll_type != CT_NONE)
	{
		oct = GetDirOctant(item->pos.y_rot);

		if (!oct && coll->coll_type == CT_FRONT)
			return 0;

		if (!oct && coll->coll_type == CT_LEFT || oct == 1 && (coll->coll_type == CT_RIGHT || coll->coll_type == CT_LEFT))
			return 0;
	}

	return 1;
}

short TestMonkeyRight(ITEM_INFO* item, COLL_INFO* coll)
{
	short oct;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot + 0x4000;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 0;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	if (ABS(coll->mid_ceiling - coll->front_ceiling) > 50)
		return 0;

	if (coll->coll_type != CT_NONE)
	{
		oct = GetDirOctant(item->pos.y_rot);

		if (!oct && coll->coll_type == CT_FRONT)
			return 0;

		if (oct == 1 && (coll->coll_type == CT_FRONT || coll->coll_type == CT_RIGHT || coll->coll_type == CT_LEFT))
			return 0;
	}
	
	return 1;
}

void lara_as_hang2(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;

	if (lara.CanMonkeySwing)
	{
		if (input & IN_ACTION && item->hit_points > 0)
		{
			camera.target_angle = 0;
			camera.target_elevation = -10920;
		}
		else
			MonkeySwingFall(item);
	}
	else if (input & IN_LOOK)
		LookUpDown();
}

void lara_col_hang2(ITEM_INFO* item, COLL_INFO* coll)
{
	if (lara.CanMonkeySwing)
	{
		coll->bad_pos = -NO_HEIGHT;
		coll->bad_neg = NO_HEIGHT;
		coll->bad_ceiling = 0;
		lara.move_angle = item->pos.y_rot;
		coll->facing = lara.move_angle;
		coll->radius = 100;
		coll->slopes_are_walls = 0;
		GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

		if (input & IN_FORWARD && coll->coll_type != CT_FRONT && ABS(coll->mid_ceiling - coll->front_ceiling) < 50)
			item->goal_anim_state = AS_MONKEYSWING;
		else if (input & IN_LSTEP && TestMonkeyLeft(item, coll))
			item->goal_anim_state = AS_MONKEYL;
		else if (input & IN_RSTEP && TestMonkeyRight(item, coll))
			item->goal_anim_state = AS_MONKEYR;
		else if (input & IN_LEFT)
			item->goal_anim_state = AS_HANGTURNL;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_HANGTURNR;

		MonkeySwingSnap(item, coll);
	}
	else
	{
		LaraHangTest(item, coll);

		if (item->goal_anim_state != AS_HANG2)
			return;

		if (input & IN_FORWARD && coll->front_floor > 850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= 0 &&
			coll->left_floor2 - coll->left_ceiling2 >= 0 && coll->right_floor2 - coll->right_ceiling2 >= 0 && !coll->hit_static)
		{
			if (input & IN_WALK)
				item->goal_anim_state = AS_GYMNAST;
			else
				item->goal_anim_state = AS_NULL;
		}
		else if (input & (IN_FORWARD | IN_DUCK) && coll->front_floor > 850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= -256 &&
			coll->left_floor2 - coll->left_ceiling2 >= -256 && coll->right_floor2 - coll->right_ceiling2 >= -256 && !coll->hit_static)
		{
			item->goal_anim_state = AS_HANG2DUCK;
			item->required_anim_state = AS_DUCK;
		}
		else if (input & IN_LEFT || input & IN_LSTEP)
			item->goal_anim_state = AS_HANGLEFT;
		else if (input & IN_RIGHT || input & IN_RSTEP)
			item->goal_anim_state = AS_HANGRIGHT;
	}
}

void lara_as_monkeyswing(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;

	if (input & IN_FORWARD)
		item->goal_anim_state = AS_MONKEYSWING;
	else
		item->goal_anim_state = AS_HANG2;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -546)
			lara.turn_rate = -546;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 546)
			lara.turn_rate = 546;
	}
}

void lara_col_monkeyswing(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 0;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);

	if (coll->coll_type == CT_FRONT || ABS(coll->mid_ceiling - coll->front_ceiling) > 50)
	{
		item->anim_number = ANIM_MONKEYHANG;
		item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
	else
	{
		camera.target_elevation = 1820;
		MonkeySwingSnap(item, coll);
	}
}

void lara_as_monkeyl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;

	if (input & IN_LSTEP)
		item->goal_anim_state = AS_MONKEYL;
	else
		item->goal_anim_state = AS_HANG2;

	camera.target_elevation = 1820;
}

void lara_col_monkeyl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	if (TestMonkeyLeft(item, coll))
		MonkeySwingSnap(item, coll);
	else
	{
		item->anim_number = ANIM_MONKEYHANG;
		item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
}

void lara_as_monkeyr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;

	if (input & IN_RSTEP)
		item->goal_anim_state = AS_MONKEYR;
	else
		item->goal_anim_state = AS_HANG2;

	camera.target_elevation = 1820;
}

void lara_col_monkeyr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	if (TestMonkeyRight(item, coll))
		MonkeySwingSnap(item, coll);
	else
	{
		item->anim_number = ANIM_MONKEYHANG;
		item->frame_number = anims[ANIM_MONKEYHANG].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
}

void lara_as_hangturnl(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	camera.target_elevation = 1820;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	item->pos.y_rot -= 273;

	if (!(input & IN_LEFT))
		item->goal_anim_state = AS_HANG2;
}

void lara_as_hangturnr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_HANG2;
		return;
	}

	camera.target_elevation = 1820;
	lara.torso_x_rot = 0;
	lara.torso_y_rot = 0;
	item->pos.y_rot += 273;

	if (!(input & IN_RIGHT))
		item->goal_anim_state = AS_HANG2;
}

void lara_col_hangturnlr(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(input & IN_ACTION) || !lara.CanMonkeySwing)
	{
		MonkeySwingFall(item);
		return;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	lara.move_angle = item->pos.y_rot;
	coll->facing = lara.move_angle;
	coll->radius = 100;
	coll->slopes_are_walls = 1;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 600);
	MonkeySwingSnap(item, coll);
}

void lara_as_monkey180(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	item->goal_anim_state = AS_HANG2;
}

void lara_col_monkey180(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_monkeyswing(item, coll);
}

void lara_as_stop(ITEM_INFO* item, COLL_INFO* coll)
{
	short fheight, rheight, h, c, goin;

	fheight = NO_HEIGHT;
	rheight = NO_HEIGHT;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_DEATH;
		return;
	}

	if (item->anim_number != 226 && item->anim_number != 228)
		StopSoundEffect(3);

	if (input & IN_ROLL && lara.water_status != LARA_WADE)
	{
		item->anim_number = ANIM_ROLL;
		item->frame_number = anims[ANIM_ROLL].frame_base + 2;
		item->current_anim_state = AS_ROLL;
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_DUCK && lara.water_status != LARA_WADE && item->current_anim_state == AS_STOP && (lara.gun_status == LG_ARMLESS ||
		lara.gun_type == LG_UNARMED || lara.gun_type == LG_PISTOLS || lara.gun_type == LG_MAGNUMS || lara.gun_type == LG_UZIS || lara.gun_type == LG_FLARE))
	{
		item->goal_anim_state = AS_DUCK;
		return;
	}

	item->goal_anim_state = AS_STOP;

	if (input & IN_LOOK)
		LookUpDown();

	if (input & IN_FORWARD)
		fheight = LaraFloorFront(item, item->pos.y_rot, 104);
	else if (input & IN_BACK)
		rheight = LaraFloorFront(item, item->pos.y_rot + 0x8000, 104);

	if (room[item->room_number].flags & ROOM_SWAMP)
	{
		if (input & IN_LEFT)
			item->goal_anim_state = AS_TURN_L;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_TURN_R;
	}
	else
	{
		if (input & IN_LSTEP)
		{
			h = LaraFloorFront(item, item->pos.y_rot - 0x4000, 148);
			c = LaraCeilingFront(item, item->pos.y_rot - 0x4000, 148);

			if (h < 128 && h > -128 && height_type != BIG_SLOPE && c <= 0)
				item->goal_anim_state = AS_STEPLEFT;
		}
		else if (input & IN_RSTEP)
		{
			h = LaraFloorFront(item, item->pos.y_rot + 0x4000, 148);
			c = LaraCeilingFront(item, item->pos.y_rot + 0x4000, 148);

			if (h < 128 && h > -128 && height_type != BIG_SLOPE && c <= 0)
				item->goal_anim_state = AS_STEPRIGHT;
		}
		else if (input & IN_LEFT)
			item->goal_anim_state = AS_TURN_L;
		else if (input & IN_RIGHT)
			item->goal_anim_state = AS_TURN_R;
	}

	if (lara.water_status == LARA_WADE)
	{
		if (input & IN_JUMP && !(room[item->room_number].flags & ROOM_SWAMP))
			item->goal_anim_state = AS_COMPRESS;

		if (input & IN_FORWARD)
		{
			goin = 1;

			if (room[item->room_number].flags & ROOM_SWAMP)
			{
				if (fheight > -383)
				{
					lara_as_wade(item, coll);
					goin = 0;
				}
			}
			else if (fheight < 383 && fheight > -383)
			{
				lara_as_wade(item, coll);
				goin = 0;
			}

			if (goin)
			{
				lara.move_angle = item->pos.y_rot;
				coll->bad_ceiling = 0;
				coll->bad_pos = 32512;
				coll->bad_neg = -384;
				coll->radius = 102;
				coll->slopes_are_walls = 1;
				GetLaraCollisionInfo(item, coll);

				if (TestLaraVault(item, coll))
					return;

				coll->radius = 100;
			}
		}
		else if (input & IN_BACK && rheight < 383 && rheight > -383)
			lara_as_back(item, coll);
	}
	else
	{
		if (input & IN_JUMP)
			item->goal_anim_state = AS_COMPRESS;
		else if (input & IN_FORWARD)
		{
			h = LaraFloorFront(item, item->pos.y_rot, 104);
			c = LaraCeilingFront(item, item->pos.y_rot, 104);

			if (height_type == 2 && h < 0)
				item->goal_anim_state = AS_STOP;
			else if (c > 0)
				item->goal_anim_state = 2;
			else
			{
				if (input & IN_WALK)
					lara_as_walk(item, coll);
				else
					lara_as_run(item, coll);
			}
		}
		else if (input & IN_BACK)
		{
			if (input & IN_WALK)
			{
				if (rheight < 383 && rheight > -383 && height_type != 2)
					lara_as_back(item, coll);
			}
			else if (rheight > -383)
				item->goal_anim_state = AS_FASTBACK;
		}
	}
}

void lara_col_stop(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll) || LaraFallen(item, coll) || TestLaraSlide(item, coll))
		return;

	ShiftItem(item, coll);

	if (room[item->room_number].flags & ROOM_SWAMP)
		item->pos.y_pos += 2;
	else
		item->pos.y_pos += coll->mid_floor;
}

void lara_as_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_col_upjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;

	if (item->speed < 0)
		coll->facing = lara.move_angle + 0x8000;
	else
		coll->facing = lara.move_angle;

	coll->hit_ceiling = 0;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 870);

	if (LaraTestHangJumpUp(item, coll))
		return;

	ShiftItem(item, coll);

	if (coll->coll_type == CT_CLAMP || coll->coll_type == CT_TOP || coll->coll_type == CT_TOP_FRONT || coll->hit_ceiling)
		item->fallspeed = 1;

	if (coll->coll_type == CT_NONE)
	{
		if (item->fallspeed < -70)
		{
			if (input & IN_FORWARD && item->speed < 5)
				item->speed++;
			else if (input & IN_BACK && item->speed > -5)
				item->speed -= 2;
		}
	}
	else
		item->speed = item->speed <= 0 ? -2 : 2;

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->gravity_status = 0;
		item->fallspeed = 0;
		item->pos.y_pos += coll->mid_floor;
	}
}

long LaraTestHangJumpUp(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long edge, edge_catch;
	short angle;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || coll->hit_static)
		return 0;

	if (lara.CanMonkeySwing && coll->coll_type == CT_TOP)
	{
		item->anim_number = ANIM_UPJUMPGRAB;
		item->frame_number = anims[ANIM_UPJUMPGRAB].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
		item->gravity_status = 0;
		item->speed = 0;
		item->fallspeed = 0;
		lara.gun_status = LG_HANDSBUSY;
		MonkeySwingSnap(item, coll);
		return 1;
	}

	if (coll->coll_type != CT_FRONT || coll->mid_ceiling > -384)
		return 0;

	edge_catch = LaraTestEdgeCatch(item, coll, &edge);

	if (!edge_catch || edge_catch < 0 && !LaraTestHangOnClimbWall(item, coll))
		return 0;

	angle = item->pos.y_rot;

	if (angle >= -6370 && angle <= 6370)
		angle = 0;
	else if (angle >= 10014 && angle <= 22754)
		angle = 0x4000;
	else if (angle >= 26397 || angle <= -26397)
		angle = -0x8000;
	else if (angle >= -22754 && angle <= -10014)
		angle = -0x4000;

	if (angle & 0x3FFF)
		return 0;

	if (TestHangSwingIn(item, angle))
	{
		item->anim_number = ANIM_UPJUMPGRAB;
		item->frame_number = anims[ANIM_UPJUMPGRAB].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
	}
	else
	{
		item->anim_number = ANIM_GRABLEDGE;
		item->frame_number = anims[ANIM_GRABLEDGE].frame_base + 12;
		item->current_anim_state = AS_HANG;
		item->goal_anim_state = AS_HANG;
	}

	bounds = GetBoundsAccurate(item);

	if (edge_catch <= 0)
		item->pos.y_pos = edge - bounds[2] + 4;
	else
		item->pos.y_pos += coll->front_floor - bounds[2];

	item->pos.x_pos += coll->shift.x;
	item->pos.z_pos += coll->shift.z;
	item->pos.y_rot = angle;
	item->gravity_status = 0;
	item->speed = 0;
	item->fallspeed = 0;
	lara.gun_status = LG_HANDSBUSY;
	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	return 1;
}

void inject_lara(bool replace)
{
	INJECT(0x00444C20, LaraLandedBad, replace);
	INJECT(0x0043D780, TestLaraSlide, replace);
	INJECT(0x0043D8C0, LaraTestClimbStance, replace);
	INJECT(0x0043D980, TestLaraVault, replace);
	INJECT(0x0043DCF0, lara_slide_slope, replace);
	INJECT(0x0043DE00, GetLaraCollisionInfo, replace);
	INJECT(0x00444D10, LaraHitCeiling, replace);
	INJECT(0x00444F90, LaraDeflectEdge, replace);
	INJECT(0x0043DE40, LaraDeflectEdgeJump, replace);
	INJECT(0x0043DFC0, LaraSlideEdgeJump, replace);
	INJECT(0x0043E730, TestHangSwingIn, replace);
	INJECT(0x00444E80, LaraFallen, replace);
	INJECT(0x0043EA20, lara_as_duck, replace);
	INJECT(0x0043EAA0, lara_col_duck, replace);
	INJECT(0x0043EBA0, lara_as_all4s, replace);
	INJECT(0x0043EC20, lara_col_all4s, replace);
	INJECT(0x00444D80, LaraFloorFront, replace);
	INJECT(0x00444E00, LaraCeilingFront, replace);
	INJECT(0x0043EF10, lara_as_crawl, replace);
	INJECT(0x0043EFE0, lara_col_crawl, replace);
	INJECT(0x0043F0D0, LaraDeflectEdgeDuck, replace);
	INJECT(0x0043F150, lara_as_all4turnl, replace);
	INJECT(0x0043F1C0, lara_col_all4turnl, replace);
	INJECT(0x0043F1F0, lara_as_all4turnr, replace);
//	INJECT(----------, lara_col_all4turnr, replace);	//replaced with lara_col_all4turnl
	INJECT(0x0043F260, lara_as_crawlb, replace);
	INJECT(0x0043F330, lara_col_crawlb, replace);
	INJECT(0x0043F430, lara_col_crawl2hang, replace);
	INJECT(0x00444EE0, LaraTestEdgeCatch, replace);
	INJECT(0x0043E0B0, LaraTestHangOnClimbWall, replace);
	INJECT(0x0043F870, TestWall, replace);
	INJECT(0x0043F980, LaraCollideStop, replace);
	INJECT(0x0043F600, lara_as_dash, replace);
	INJECT(0x0043F720, lara_col_dash, replace);
	INJECT(0x0043FA40, lara_as_dashdive, replace);
	INJECT(0x0043FA70, lara_col_dashdive, replace);
	INJECT(0x0043FC10, MonkeySwingFall, replace);
	INJECT(0x0043FE90, MonkeySwingSnap, replace);
	INJECT(0x0043FFB0, GetDirOctant, replace);
	INJECT(0x0043FEE0, TestMonkeyLeft, replace);
	INJECT(0x0043FFD0, TestMonkeyRight, replace);
	INJECT(0x0043FB90, lara_as_hang2, replace);
	INJECT(0x0043FC70, lara_col_hang2, replace);
	INJECT(0x004400A0, lara_as_monkeyswing, replace);
	INJECT(0x00440140, lara_col_monkeyswing, replace);
	INJECT(0x00440220, lara_as_monkeyl, replace);
	INJECT(0x00440280, lara_col_monkeyl, replace);
	INJECT(0x004402F0, lara_as_monkeyr, replace);
	INJECT(0x00440350, lara_col_monkeyr, replace);
	INJECT(0x004403C0, lara_as_hangturnl, replace);
	INJECT(0x004404A0, lara_as_hangturnr, replace);
	INJECT(0x00440400, lara_col_hangturnlr, replace);
	INJECT(0x004404E0, lara_as_monkey180, replace);
	INJECT(0x00440500, lara_col_monkey180, replace);
	INJECT(0x00440520, lara_as_stop, replace);
	INJECT(0x00440960, lara_col_stop, replace);
	INJECT(0x00440A20, lara_as_upjump, replace);
	INJECT(0x00440A40, lara_col_upjump, replace);
	INJECT(0x00440B90, LaraTestHangJumpUp, replace);
}
