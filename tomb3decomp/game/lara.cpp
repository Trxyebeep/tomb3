#include "../tomb3/pch.h"
#include "lara.h"
#include "control.h"
#include "collide.h"
#include "laraanim.h"
#include "laraclmb.h"
#include "../3dsystem/phd_math.h"
#include "laramisc.h"
#include "sound.h"

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

	if (!(input & IN_ACTION) || lara.gun_status != LG_NO_ARMS || coll->coll_type != CT_FRONT)
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
		lara.gun_status = LG_HANDS_BUSY;
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
		lara.gun_status = LG_HANDS_BUSY;
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
		if (coll->front_floor <= -1920 && lara.water_status != LW_WADE && coll->left_floor2 <= -1920 && coll->right_floor2 <= -2048 && coll->mid_ceiling <= -1158)
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
				lara.gun_status = LG_HANDS_BUSY;
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
	if (lara.water_status != LW_WADE && coll->mid_floor > 384)
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

	if (input & (IN_FORWARD | IN_BACK) && lara.gun_status == LG_NO_ARMS && item->frame_number > anims[ANIM_DUCKBREATHE].frame_base + 10)
	{
		lara.torso_y_rot = 0;
		lara.torso_x_rot = 0;
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
		lara.gun_status = LG_NO_ARMS;
	else
	{
		if (coll->mid_ceiling >= -362)
			lara.keep_ducked = 1;
		else
			lara.keep_ducked = 0;

		ShiftItem(item, coll);
		item->pos.y_pos += coll->mid_floor;

		if ((!(input & IN_DUCK) || lara.water_status == LW_WADE) && !lara.keep_ducked && item->anim_number == ANIM_DUCKBREATHE)
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

	lara.torso_y_rot = 0;
	lara.torso_x_rot = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if (item->anim_number == ANIM_DUCK_TO_ALL4S)
		lara.gun_status = LG_HANDS_BUSY;

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
		lara.gun_status = LG_NO_ARMS;
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
}
