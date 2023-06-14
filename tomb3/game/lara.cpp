#include "../tomb3/pch.h"
#include "lara.h"
#include "control.h"
#include "collide.h"
#include "laraclmb.h"
#include "../3dsystem/phd_math.h"
#include "laramisc.h"
#include "sound.h"
#include "draw.h"
#include "../3dsystem/3d_gen.h"
#include "effect2.h"
#include "../specific/game.h"
#include "objects.h"
#include "kayak.h"
#include "quadbike.h"
#include "sub.h"
#include "biggun.h"
#include "minecart.h"
#include "larafire.h"
#include "items.h"
#include "../specific/input.h"
#include "camera.h"
#include "../newstuff/LaraDraw.h"
#include "../tomb3/tomb3.h"

static short DeathSlideBounds[12] = { -256, 256, -100, 100, 256, 512, 0, 0, -4550, 4550, 0, 0 };
static PHD_VECTOR DeathSlidePosition = { 0, 0, 371 };

LARA_INFO lara;
ITEM_INFO* lara_item;
short DashTimer;
short ExposureMeter;
uchar LaraOnPad;

static void TiltHer(ITEM_INFO* item, long rad, long height)
{
	FLOOR_INFO* floor;
	FVECTOR plane;
	long wy[4];
	long yT, y, wx, wz, dy;
	short room_number, rotX, rotZ;

	if (!tomb3.crawl_tilt)
		return;

	yT = item->pos.y_pos - height - 162;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, yT, item->pos.z_pos, &room_number);
	y = GetHeight(floor, item->pos.x_pos, yT, item->pos.z_pos);

	if (!OnObject)
	{
		plane.x = -(float)tiltyoff / 4;
		plane.y = -(float)tiltxoff / 4;
	}
	else
	{
		wx = item->pos.x_pos & ~WALL_MASK | 0xFF;
		wz = item->pos.z_pos & ~WALL_MASK | 0xFF;
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[0] = GetHeight(floor, wx, yT, wz);

		wx = item->pos.x_pos & ~WALL_MASK | 0x2FF;
		wz = item->pos.z_pos & ~WALL_MASK | 0xFF;
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[1] = GetHeight(floor, wx, yT, wz);

		wx = item->pos.x_pos & ~WALL_MASK | 0xFF;
		wz = item->pos.z_pos & ~WALL_MASK | 0x2FF;
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[2] = GetHeight(floor, wx, yT, wz);

		plane.x = float(wy[1] - wy[0]) / 512;
		plane.y = float(wy[2] - wy[0]) / 512;
	}

	plane.z = item->pos.y_pos - plane.x * item->pos.x_pos - plane.y * item->pos.z_pos;

	for (int i = 0; i < 4; i++)
	{
		wx = item->pos.x_pos + (rad * phd_sin(item->pos.y_rot + 0x4000 * i) >> W2V_SHIFT);
		wz = item->pos.z_pos + (rad * phd_cos(item->pos.y_rot + 0x4000 * i) >> W2V_SHIFT);
		room_number = item->room_number;
		floor = GetFloor(wx, yT, wz, &room_number);
		wy[i] = GetHeight(floor, wx, yT, wz);

		if (abs(y - wy[i]) > rad / 2)
			wy[i] = long(plane.x * wx + plane.y * wz + plane.z);
	}

	dy = wy[0] - wy[2];
	rotX = (short)phd_atan(2 * rad, dy);

	if (dy > 0 && rotX > 0 || dy < 0 && rotX < 0)
		rotX = -rotX;

	dy = wy[3] - wy[1];
	rotZ = (short)phd_atan(2 * rad, dy);

	if (dy > 0 && rotZ > 0 || dy < 0 && rotZ < 0)
		rotZ = -rotZ;

	if (abs(rotX - item->pos.x_rot) < 546)
		item->pos.x_rot = rotX;
	else if (rotX > item->pos.x_rot)
		item->pos.x_rot += 546;
	else if (rotX < item->pos.x_rot)
		item->pos.x_rot -= 546;

	if (item->pos.x_rot > 0x2000)
		item->pos.x_rot = 0x2000;
	else if (item->pos.x_rot < -0x2000)
		item->pos.x_rot = -0x2000;

	if (abs(rotZ - item->pos.z_rot) < 546)
		item->pos.z_rot = rotZ;
	else if (rotZ > item->pos.z_rot)
		item->pos.z_rot += 546;
	else if (rotZ < item->pos.z_rot)
		item->pos.z_rot -= 546;

	if (item->pos.z_rot > 0x2000)
		item->pos.z_rot = 0x2000;
	else if (item->pos.z_rot < -0x2000)
		item->pos.z_rot = -0x2000;
}

void LaraAboveWater(ITEM_INFO* item, COLL_INFO* coll)
{
	short vehicle;

	coll->old.x = item->pos.x_pos;
	coll->old.y = item->pos.y_pos;
	coll->old.z = item->pos.z_pos;
	coll->old_anim_state = item->current_anim_state;
	coll->old_anim_number = item->anim_number;
	coll->old_frame_number = item->frame_number;
	coll->radius = 100;
	coll->trigger = 0;
	coll->lava_is_pit = 0;
	coll->slopes_are_walls = 0;
	coll->slopes_are_pits = 0;
	coll->enable_spaz = 1;
	coll->enable_baddie_push = 1;

	if (input & IN_LOOK && !lara.extra_anim	&& lara.look)
		LookLeftRight();
	else
		ResetLook();

	lara.look = 1;

	if (lara.skidoo != NO_ITEM)
	{
		vehicle = items[lara.skidoo].object_number;

		if (vehicle == KAYAK)
		{
			if (KayakControl())
				return;
		}
		else if (vehicle == QUADBIKE)
		{
			if (QuadBikeControl())
				return;
		}
		else if (vehicle == UPV)
		{
			if (SubControl())
				return;
		}
		else if (vehicle == BIGGUN)
		{
			if (BigGunControl(coll))
				return;
		}
		else if (vehicle == MINECART)
		{
			if (MineCartControl())
				return;
		}
		else
		{
			LaraGun();
			return;
		}
	}

	if (lara.extra_anim)
		extra_control_routines[item->current_anim_state](item, coll);
	else
		lara_control_routines[item->current_anim_state](item, coll);

	if (item->pos.z_rot < -182)
		item->pos.z_rot += 182;
	else if (item->pos.z_rot > 182)
		item->pos.z_rot -= 182;
	else item->pos.z_rot = 0;

	if (item->pos.x_rot < -182)
		item->pos.x_rot += 182;
	else if (item->pos.x_rot > 182)
		item->pos.x_rot -= 182;
	else item->pos.x_rot = 0;

	if (lara.turn_rate < -364)
		lara.turn_rate += 364;
	else if (lara.turn_rate > 364)
		lara.turn_rate -= 364;
	else
		lara.turn_rate = 0;

	item->pos.y_rot += lara.turn_rate;
	AnimateLara(item);

	if (!lara.extra_anim)
	{
		LaraBaddieCollision(item, coll);

		if (lara.skidoo == NO_ITEM)
			lara_collision_routines[item->current_anim_state](item, coll);
	}

	UpdateLaraRoom(item, -381);
	LaraGun();
	LaraOnPad = 0;
	TestTriggers(coll->trigger, 0);

	if (!LaraOnPad)
		lara_item->item_flags[1] = 0;
}

void lara_void_func(ITEM_INFO* item, COLL_INFO* coll)
{

}

void extra_void_func(ITEM_INFO* item, COLL_INFO* coll)
{

}

long LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	long landspeed, h, y;
	short room_number;

	room_number = item->room_number;
	y = item->pos.y_pos;
	floor = GetFloor(item->pos.x_pos, y, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, y - 762, item->pos.z_pos);
	item->pos.y_pos = h;
	item->floor = h;

	if (abs(y - h) > 256)
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
		item->hit_points += short(-1000 * SQUARE(landspeed) / 196);

	return item->hit_points <= 0;
}

long TestLaraSlide(ITEM_INFO* item, COLL_INFO* coll)
{
	static short old_ang = 1;
	short ang_diff, ang;

	if (abs(coll->tilt_x) <= 2 && abs(coll->tilt_z) <= 2)
		return 0;

	ang = 0;

	if (coll->tilt_x > 2)
		ang = -0x4000;
	else if (coll->tilt_x < -2)
		ang = 0x4000;

	if (coll->tilt_z > 2 && coll->tilt_z > abs(coll->tilt_x))
		ang = -32768;
	else if (coll->tilt_z < -2 && (-coll->tilt_z > abs(coll->tilt_x)))
		ang = 0;

	ang_diff = ang - item->pos.y_rot;
	ShiftItem(item, coll);

	if (ang_diff >= -0x4000 && ang_diff <= 0x4000)
	{
		if (item->current_anim_state != AS_SLIDE || old_ang != ang)
		{
			item->anim_number = ANIM_SLIDE;
			item->frame_number = anims[ANIM_SLIDE].frame_base;
			item->goal_anim_state = AS_SLIDE;
			item->current_anim_state = AS_SLIDE;
			item->pos.y_rot = ang;
			lara.move_angle = ang;
			old_ang = ang;
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
			item->pos.y_rot = ang - 0x8000;
			lara.move_angle = ang;
			old_ang = ang;
		}
	}

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

	slope = abs(coll->left_floor2 - coll->right_floor2) >= 60;

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

		if ((abs(coll->tilt_x)) <= 2 && (abs(coll->tilt_z)) <= 2)
		{
			if (input & IN_FORWARD && item->anim_number == ANIM_SLIDE && tomb3.slide_to_run)
				item->goal_anim_state = AS_RUN;
			else
				item->goal_anim_state = AS_STOP;

			StopSoundEffect(SFX_LARA_SLIPPING);
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

		StopSoundEffect(SFX_LARA_SLIPPING);
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
		item->pos.x_pos -= (100 * phd_sin(coll->facing)) >> W2V_SHIFT;
		item->pos.z_pos -= (100 * phd_cos(coll->facing)) >> W2V_SHIFT;
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
		item->pos.z_pos -= (100 * phd_cos(coll->facing)) >> W2V_SHIFT;
		item->pos.x_pos -= (100 * phd_sin(coll->facing)) >> W2V_SHIFT;
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

	if (!tomb3.duck_roll)
		return;

	if (input & IN_SPRINT && !(input & IN_DRAW))
	{
		if (LaraFloorFront(item, item->pos.y_rot, 512) < 384 && LaraFloorFront(item, item->pos.y_rot, 256) >= -384)
		{
			if ((input & IN_DUCK || lara.keep_ducked && lara.water_status != LARA_WADE) && lara.gun_status == LG_ARMLESS)
			{
				if (lara_item->anim_number == ANIM_DUCKBREATHE || lara_item->anim_number == 245)
				{
					if (lara.gun_type != LG_FLARE || lara.flare_age && lara.flare_age < 900)
					{
						item->anim_number = 218;
						item->frame_number = anims[218].frame_base;
						item->current_anim_state = AS_DUCKROLL;
						item->goal_anim_state = AS_DUCKROLL;
						lara.torso_x_rot = 0;
						lara.torso_y_rot = 0;
					}
				}
			}
		}
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
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);

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
	PHD_3DPOS old;
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
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);
	TiltHer(item, 140, 400);

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

	if (tomb3.flexible_crawl)
		h = item->anim_number != ANIM_ALL4S && item->anim_number != ANIM_ALL4S2 && item->anim_number != 266 && item->anim_number != 268;
	else
		h = item->anim_number != ANIM_ALL4S && item->anim_number != ANIM_ALL4S2;

	if (h)
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
			old = item->pos;
			angle = ushort(item->pos.y_rot + 0x2000) / 0x4000;

			switch (angle)
			{
			case NORTH:
				item->pos.y_rot = 0;
				item->pos.z_pos = (item->pos.z_pos & ~WALL_MASK) + 225;
				break;

			case EAST:
				item->pos.y_rot = 0x4000;
				item->pos.x_pos = (item->pos.x_pos & ~WALL_MASK) + 225;
				break;

			case SOUTH:
				item->pos.y_rot = -0x8000;
				item->pos.z_pos = (item->pos.z_pos | WALL_MASK) - 225;
				break;

			case WEST:
				item->pos.y_rot = -0x4000;
				item->pos.x_pos = (item->pos.x_pos | WALL_MASK) - 225;
				break;
			}

			h = LaraFloorFront(item, item->pos.y_rot, 0);

			if (h > 255 || h < -255 || height_type == BIG_SLOPE)
				item->pos = old;
			else
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
	x = item->pos.x_pos + ((dist * phd_sin(ang)) >> W2V_SHIFT);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + ((dist * phd_cos(ang)) >> W2V_SHIFT);
	height = GetHeight(GetFloor(x, y, z, &room_num), x, y, z);

	if (height != NO_HEIGHT)
		height -= item->pos.y_pos;

	return (short)height;
}

short LaraCeilingFront(ITEM_INFO* item, short ang, long dist)
{
	long x, y, z, height;
	short room_num;

	x = item->pos.x_pos + ((dist * phd_sin(ang)) >> W2V_SHIFT);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + ((dist * phd_cos(ang)) >> W2V_SHIFT);
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

	if (!(input & IN_FORWARD) || (!(input & IN_DUCK) && !lara.keep_ducked))
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
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);
	TiltHer(item, 140, 400);

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
	TiltHer(item, 140, 400);
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
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);
	TiltHer(item, 140, 400);

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
		item->anim_number = ANIM_GRABLEDGEIN;
		item->frame_number = anims[ANIM_GRABLEDGEIN].frame_base;
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

		switch (ushort(item->pos.y_rot + 0x2000) / 0x4000)
		{
		case NORTH:
			item->pos.z_pos = (item->pos.z_pos | WALL_MASK) - 100;
			item->pos.x_pos += coll->shift.x;
			break;

		case EAST:
			item->pos.x_pos = (item->pos.x_pos | WALL_MASK) - 100;
			item->pos.z_pos += coll->shift.z;
			break;

		case SOUTH:
			item->pos.z_pos = (item->pos.z_pos & ~WALL_MASK) + 100;
			item->pos.x_pos += coll->shift.x;
			break;

		case WEST:
			item->pos.x_pos = (item->pos.x_pos & ~WALL_MASK) + 100;
			item->pos.z_pos += coll->shift.z;
			break;
		}
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

	if (abs(coll->left_floor2 - coll->right_floor2) >= 60)
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
		if (tomb3.flexible_crawl)
			item->goal_anim_state = AS_DUCK;
		else
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
	rot = abs(rot);
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

	if (abs(coll->mid_ceiling - coll->front_ceiling) > 50)
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

	if (abs(coll->mid_ceiling - coll->front_ceiling) > 50)
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

		if (input & IN_FORWARD && coll->coll_type != CT_FRONT && abs(coll->mid_ceiling - coll->front_ceiling) < 50)
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

		if (input & IN_FORWARD && coll->front_floor > -850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= 0 &&
			coll->left_floor2 - coll->left_ceiling2 >= 0 && coll->right_floor2 - coll->right_ceiling2 >= 0 && !coll->hit_static)
		{
			if (input & IN_WALK)
				item->goal_anim_state = AS_GYMNAST;
			else
				item->goal_anim_state = AS_NULL;
		}
		else if (input & (IN_FORWARD | IN_DUCK) && coll->front_floor > -850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= -256 &&
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

	if (coll->coll_type == CT_FRONT || abs(coll->mid_ceiling - coll->front_ceiling) > 50)
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
		StopSoundEffect(SFX_LARA_SLIPPING);

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

	if (room[item->room_number].flags & ROOM_SWAMP && coll->mid_floor >= 0)
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

void lara_as_forwardjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->goal_anim_state == AS_SWANDIVE || item->goal_anim_state == AS_REACH)
		item->goal_anim_state = AS_FORWARDJUMP;

	if (item->goal_anim_state != AS_DEATH && item->goal_anim_state != AS_STOP && item->goal_anim_state != AS_RUN)
	{
		if (input & IN_ACTION && lara.gun_status == LG_ARMLESS)
			item->goal_anim_state = AS_REACH;

		if (input & IN_ROLL || input & IN_BACK)
			item->goal_anim_state = AS_TWIST;

		if (input & IN_WALK && lara.gun_status == LG_ARMLESS)
			item->goal_anim_state = AS_SWANDIVE;

		if (item->fallspeed > 131)
			item->goal_anim_state = AS_FASTFALL;
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

void lara_as_walk(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;
	}

	if (input & IN_FORWARD)
	{
		if (lara.water_status == LARA_WADE)
			item->goal_anim_state = AS_WADE;
		else if (input & IN_WALK)
			item->goal_anim_state = AS_WALK;
		else
			item->goal_anim_state = AS_RUN;
	}
	else
		item->goal_anim_state = AS_STOP;
}

void lara_as_run(ITEM_INFO* item, COLL_INFO* coll)
{
	static long jump_ok = 1;
	bool dash;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_DEATH;
		return;
	}

	if (input & IN_ROLL)
	{
		item->anim_number = ANIM_ROLL;
		item->frame_number = anims[ANIM_ROLL].frame_base + 2;
		item->current_anim_state = AS_ROLL;
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (tomb3.flexible_sprint)
		dash = input & IN_SPRINT && DashTimer;
	else
		dash = input & IN_SPRINT && DashTimer == 120;

	if (dash)
	{
		item->goal_anim_state = AS_DASH;
		return;
	}

	if (input & IN_DUCK && lara.water_status != LARA_WADE)
	{
		if (tomb3.flexible_crawl)
			item->goal_anim_state = AS_DUCK;
		else
			item->goal_anim_state = AS_STOP;

		return;
	}

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1456)
			lara.turn_rate = -1456;

		item->pos.z_rot -= 273;

		if (item->pos.z_rot < -2002)
			item->pos.z_rot = -2002;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1456)
			lara.turn_rate = 1456;

		item->pos.z_rot += 273;

		if (item->pos.z_rot > 2002)
			item->pos.z_rot = 2002;
	}

	if (item->anim_number == ANIM_STARTRUN)
		jump_ok = 0;
	else if (item->anim_number != ANIM_RUN || item->frame_number == 4)
		jump_ok = 1;

	if (input & IN_JUMP && jump_ok && !item->gravity_status)
	{
		item->goal_anim_state = AS_FORWARDJUMP;
		return;
	}

	if (input & IN_FORWARD)
	{
		if (lara.water_status == LARA_WADE)
			item->goal_anim_state = AS_WADE;
		else if (input & IN_WALK)
			item->goal_anim_state = AS_WALK;
		else
			item->goal_anim_state = AS_RUN;
	}
	else
		item->goal_anim_state = AS_STOP;
}

void lara_as_fastback(ITEM_INFO* item, COLL_INFO* coll)
{
	item->goal_anim_state = AS_STOP;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -1092)
			lara.turn_rate = -1092;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 1092)
			lara.turn_rate = 1092;
	}
}

void lara_as_turn_r(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	lara.turn_rate += 409;

	if (lara.gun_status == LG_READY && lara.water_status != LARA_WADE)
		item->goal_anim_state = AS_FASTTURN;
	else if (lara.turn_rate > 728)
	{
		if (input & IN_WALK || lara.water_status == LARA_WADE)
			lara.turn_rate = 728;
		else
			item->goal_anim_state = AS_FASTTURN;
	}
	
	if (input & IN_FORWARD)
	{
		if (lara.water_status == LARA_WADE)
			item->goal_anim_state = AS_WADE;
		else if (input & IN_WALK)
			item->goal_anim_state = AS_WALK;
		else
			item->goal_anim_state = AS_RUN;
	}
	else if (!(input & IN_RIGHT))
		item->goal_anim_state = AS_STOP;
}

void lara_as_turn_l(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	lara.turn_rate -= 409;

	if (lara.gun_status == LG_READY && lara.water_status != LARA_WADE)
		item->goal_anim_state = AS_FASTTURN;
	else if (lara.turn_rate < -728)
	{
		if (input & IN_WALK || lara.water_status == LARA_WADE)
			lara.turn_rate = -728;
		else
			item->goal_anim_state = AS_FASTTURN;
	}

	if (input & IN_FORWARD)
	{
		if (lara.water_status == LARA_WADE)
			item->goal_anim_state = AS_WADE;
		else if (input & IN_WALK)
			item->goal_anim_state = AS_WALK;
		else
			item->goal_anim_state = AS_RUN;
	}
	else if (!(input & IN_LEFT))
		item->goal_anim_state = AS_STOP;
}

void lara_as_death(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
}

void lara_as_fastfall(ITEM_INFO* item, COLL_INFO* coll)
{
	item->speed = 95 * item->speed / 100;

	if (item->fallspeed == 154)
		SoundEffect(SFX_LARA_FALL, &item->pos, SFX_DEFAULT);
}

void lara_as_hang(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_LOOK)
		LookUpDown();

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 0;
	camera.target_elevation = -10920;

	if (input & (IN_LEFT | IN_LSTEP))
		item->goal_anim_state = AS_HANGLEFT;
	else if (input & (IN_RIGHT | IN_RSTEP))
		item->goal_anim_state = AS_HANGRIGHT;
}

void lara_as_reach(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 15470;

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_as_splat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
}

void lara_as_compress(ITEM_INFO* item, COLL_INFO* coll)
{
	if (lara.water_status != LARA_WADE)
	{
		if (input & IN_FORWARD && LaraFloorFront(item, item->pos.y_rot, 256) >= -384)
		{
			item->goal_anim_state = AS_FORWARDJUMP;
			lara.move_angle = item->pos.y_rot;
		}
		else if (input & IN_LEFT && LaraFloorFront(item, item->pos.y_rot - 0x4000, 256) >= -384)
		{
			item->goal_anim_state = AS_LEFTJUMP;
			lara.move_angle = item->pos.y_rot - 0x4000;
		}
		else if (input & IN_RIGHT && LaraFloorFront(item, item->pos.y_rot + 0x4000, 256) >= -384)
		{
			item->goal_anim_state = AS_RIGHTJUMP;
			lara.move_angle = item->pos.y_rot + 0x4000;
		}
		else if (input & IN_BACK && LaraFloorFront(item, item->pos.y_rot + 0x8000, 256) >= -384)
		{
			item->goal_anim_state = AS_BACKJUMP;
			lara.move_angle = item->pos.y_rot + 0x8000;
		}
	}

	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;
}

void lara_as_back(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (input & IN_BACK && (input & IN_WALK || lara.water_status == LARA_WADE))
		item->goal_anim_state = AS_BACK;
	else
		item->goal_anim_state = AS_STOP;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;
	}
}

void lara_as_fastturn(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (lara.turn_rate < 0)
	{
		lara.turn_rate = -1456;

		if (!(input & IN_LEFT))
			item->goal_anim_state = AS_STOP;
	}
	else
	{
		lara.turn_rate = 1456;

		if (!(input & IN_RIGHT))
			item->goal_anim_state = AS_STOP;
	}
}

void lara_as_stepright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (!(input & IN_RSTEP))
		item->goal_anim_state = AS_STOP;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;
	}
}

void lara_as_stepleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	if (!(input & IN_LSTEP))
		item->goal_anim_state = AS_STOP;

	if (input & IN_LEFT)
	{
		lara.turn_rate -= 409;

		if (lara.turn_rate < -728)
			lara.turn_rate = -728;
	}
	else if (input & IN_RIGHT)
	{
		lara.turn_rate += 409;

		if (lara.turn_rate > 728)
			lara.turn_rate = 728;
	}
}

void lara_as_slide(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.flags = 2;
	camera.target_elevation = -8190;

	if (input & IN_JUMP && !(input & IN_BACK))
		item->goal_anim_state = AS_FORWARDJUMP;
}

void lara_as_backjump(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 24570;

	if (item->fallspeed > 131)
	{
		item->goal_anim_state = AS_FASTFALL;
		return;
	}

	if (item->goal_anim_state == AS_RUN)
		item->goal_anim_state = AS_STOP;
	else if ((input & IN_FORWARD || input & IN_ROLL) && item->goal_anim_state != 2)
		item->goal_anim_state = AS_TWIST;
}

void lara_as_rightjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->fallspeed > 131)
	{
		item->goal_anim_state = AS_FASTFALL;
		return;
	}

	if (input & IN_LEFT && item->goal_anim_state != AS_STOP)
		item->goal_anim_state = AS_TWIST;
}

void lara_as_leftjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;

	if (item->fallspeed > 131)
	{
		item->goal_anim_state = AS_FASTFALL;
		return;
	}

	if (input & IN_RIGHT && item->goal_anim_state != AS_STOP)
		item->goal_anim_state = AS_TWIST;
}

void lara_as_fallback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->fallspeed > 131)
		item->goal_anim_state = AS_FASTFALL;

	if (input & IN_ACTION && lara.gun_status == LG_ARMLESS)
		item->goal_anim_state = AS_REACH;
}

void lara_as_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 0;
	camera.target_elevation = -10920;

	if (!(input & IN_LEFT) && !(input & IN_LSTEP))
		item->goal_anim_state = AS_HANG;
}

void lara_as_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 0;
	camera.target_elevation = -10920;

	if (!(input & IN_RIGHT) && !(input & IN_RSTEP))
		item->goal_anim_state = AS_HANG;
}

void lara_as_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_JUMP && !(input & IN_FORWARD))
		item->goal_anim_state = AS_BACKJUMP;
}

void lara_as_pushblock(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.flags = 1;
	camera.target_angle = 6370;
	camera.target_elevation = -4550;
}

void lara_as_ppready(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 13650;

	if (!(input & IN_ACTION))
		item->goal_anim_state = AS_STOP;
}

void lara_as_pickup(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = -23660;
	camera.target_elevation = -2730;
	camera.target_distance = 1024;
}

void lara_as_pickupflare(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 23660;
	camera.target_elevation = -2730;
	camera.target_distance = 1024;

	if (item->frame_number == anims[item->anim_number].frame_end - 1)
		lara.gun_status = LG_ARMLESS;
}

void lara_as_switchon(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = 14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;
	camera.speed = 6;
}

void lara_as_usekey(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.look = 0;
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.target_angle = -14560;
	camera.target_elevation = -4550;
	camera.target_distance = 1024;
}

void lara_as_special(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.flags = 1;
	camera.target_angle = 30940;
	camera.target_elevation = -4550;
}

void lara_as_swandive(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;

	if (item->fallspeed > 131 && item->goal_anim_state != AS_DIVE)
		item->goal_anim_state = AS_FASTDIVE;
}

void lara_as_fastdive(ITEM_INFO* item, COLL_INFO* coll)
{
	if (input & IN_ROLL && item->goal_anim_state == AS_FASTDIVE)
		item->goal_anim_state = AS_TWIST;

	coll->enable_spaz = 0;
	coll->enable_baddie_push = 1;
	item->speed = 95 * item->speed / 100;
}

void lara_as_gymnast(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
}

void lara_as_waterout(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->enable_spaz = 0;
	coll->enable_baddie_push = 0;
	camera.flags = 1;
}

void lara_as_wade(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->hit_points <= 0)
	{
		item->goal_anim_state = AS_STOP;
		return;
	}

	camera.target_elevation = -4004;

	if (room[item->room_number].flags & ROOM_SWAMP)
	{
		if (input & IN_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -728)
				lara.turn_rate = -728;

			item->pos.z_rot -= 273;

			if (item->pos.z_rot < -1001)
				item->pos.z_rot = -1001;
		}
		else if (input & IN_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 728)
				lara.turn_rate = 728;

			item->pos.z_rot += 273;

			if (item->pos.z_rot > 1001)
				item->pos.z_rot = 1001;
		}

		if (input & IN_FORWARD)
			item->goal_anim_state = AS_WADE;
		else
			item->goal_anim_state = AS_STOP;
	}
	else
	{
		if (input & IN_LEFT)
		{
			lara.turn_rate -= 409;

			if (lara.turn_rate < -1456)
				lara.turn_rate = -1456;

			item->pos.z_rot -= 273;

			if (item->pos.z_rot < -2002)
				item->pos.z_rot = -2002;
		}
		else if (input & IN_RIGHT)
		{
			lara.turn_rate += 409;

			if (lara.turn_rate > 1456)
				lara.turn_rate = 1456;

			item->pos.z_rot += 273;

			if (item->pos.z_rot > 2002)
				item->pos.z_rot = 2002;
		}

		if (input & IN_FORWARD)
		{
			if (lara.water_status == LARA_ABOVEWATER)
				item->goal_anim_state = AS_RUN;
			else
				item->goal_anim_state = AS_WADE;
		}
		else
			item->goal_anim_state = AS_STOP;
	}
}

void lara_as_deathslide(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	camera.target_angle = 12740;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	coll->trigger = trigger_index;

	if (!(input & IN_ACTION))
	{
		item->goal_anim_state = AS_FORWARDJUMP;
		AnimateLara(item);
		lara_item->gravity_status = 1;
		lara_item->speed = 100;
		lara_item->fallspeed = 40;
		lara.move_angle = item->pos.y_rot;
	}
}

void extra_as_breath(ITEM_INFO* item, COLL_INFO* coll)
{
	item->anim_number = ANIM_BREATH;
	item->frame_number = anims[ANIM_BREATH].frame_base;
	item->current_anim_state = AS_STOP;
	item->goal_anim_state = AS_STOP;
	lara.gun_status = LG_ARMLESS;
	camera.type = CHASE_CAMERA;
	AlterFOV(14560);
	lara.extra_anim = 0;
}

void extra_as_sharkkill(ITEM_INFO* item, COLL_INFO* coll)
{
	long wh;

	camera.target_angle = 29120;
	camera.target_distance = 3072;
	lara.hit_direction = -1;

	if (item->frame_number == anims[item->anim_number].frame_end)
	{
		wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number);

		if (wh != NO_HEIGHT && wh < item->pos.y_pos - 100)
			item->pos.y_pos -= 5;
	}

	if (item->frame_number < anims[item->anim_number].frame_end - 30)
		lara.death_count = 1;
}

void extra_as_airlock(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = 14560;
	camera.target_elevation = -4550;
}

void extra_as_gongbong(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_angle = -4550;
	camera.target_elevation = -3640;
	camera.target_distance = 3072;
}

void extra_as_dinokill(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.flags = 1;
	camera.target_angle = 30940;
	camera.target_elevation = -4550;
	lara.hit_direction = -1;

	if (item->frame_number < anims[item->anim_number].frame_end - 30)
		lara.death_count = 1;
}

void extra_as_startanim(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	TestTriggers(trigger_index, 0);
}

void extra_as_trainkill(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	lara.hit_direction = -1;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (item->frame_number < anims[item->anim_number].frame_end - 30)
		lara.death_count = 1;

	item->gravity_status = 0;
	item->hit_points = -1;
}

void extra_as_rapidsdrown(ITEM_INFO* item, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	short room_number;

	GetLaraCollisionInfo(item, coll);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	item->pos.y_pos = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos) + 384;
	item->pos.y_rot += 1024;
	lara.death_count++;

	if (!(wibble & 3))
		TriggerWaterfallMist(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, GetRandomControl() & 0xFFF);
}

void lara_col_walk(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	coll->slopes_are_pits = 1;
	coll->lava_is_pit = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll) || TestLaraSlide(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		if (item->frame_number >= 29 && item->frame_number <= 47)
		{
			item->anim_number = ANIM_WALK_STOP_RIGHT;
			item->frame_number = anims[ANIM_WALK_STOP_RIGHT].frame_base;
		}
		else if ((item->frame_number >= 22 && item->frame_number <= 28) || (item->frame_number >= 48 && item->frame_number <= 57))
		{
			item->anim_number = ANIM_WALK_STOP_LEFT;
			item->frame_number = anims[ANIM_WALK_STOP_LEFT].frame_base;
		}
		else
			LaraCollideStop(item, coll);
	}

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor > 128)
	{
		if (item->frame_number < 28 || item->frame_number > 45)
		{
			item->anim_number = ANIM_WALKSTEPD_LEFT;
			item->frame_number = anims[ANIM_WALKSTEPD_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_WALKSTEPD_RIGHT;
			item->frame_number = anims[ANIM_WALKSTEPD_RIGHT].frame_base;
		}
	}

	if (coll->mid_floor >= -384 && coll->mid_floor < -128)
	{
		if (item->frame_number < 27 || item->frame_number > 44)
		{
			item->anim_number = ANIM_WALKSTEPUP_LEFT;
			item->frame_number = anims[ANIM_WALKSTEPUP_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_WALKSTEPUP_RIGHT;
			item->frame_number = anims[ANIM_WALKSTEPUP_RIGHT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	item->pos.y_pos += coll->mid_floor;
}

void lara_col_run(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll) || TestLaraVault(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
	{
		item->pos.z_rot = 0;

		if (item->anim_number != ANIM_STARTRUN && TestWall(item, 256, 0, -640))
		{
			item->current_anim_state = AS_SPLAT;

			if (item->frame_number >= 0 && item->frame_number <= 9)
			{
				item->anim_number = ANIM_HITWALLLEFT;
				item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
				return;
			}

			if (item->frame_number >= 10 && item->frame_number <= 21)
			{
				item->anim_number = ANIM_HITWALLRIGHT;
				item->frame_number = anims[ANIM_HITWALLRIGHT].frame_base;
				return;
			}
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

void lara_col_forwardjump(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->speed < 0)
		lara.move_angle = item->pos.y_rot + 0x8000;
	else
		lara.move_angle = item->pos.y_rot;

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

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
		item->pos.y_pos += coll->mid_floor;
		item->speed = 0;
		AnimateLara(item);
	}
}

void lara_col_fastback(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot + 0x8000;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 0;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (coll->mid_floor <= 200)
	{
		if (!TestLaraSlide(item, coll))
		{
			if (LaraDeflectEdge(item, coll))
				LaraCollideStop(item, coll);

			item->pos.y_pos += coll->mid_floor;
		}
	}
	else
	{
		item->anim_number = ANIM_FALLBACK;
		item->frame_number = anims[ANIM_FALLBACK].frame_base;
		item->current_anim_state = AS_FALLBACK;
		item->goal_anim_state = AS_FALLBACK;
		item->gravity_status = 1;
		item->fallspeed = 0;
	}
}

void lara_col_turn_r(ITEM_INFO* item, COLL_INFO* coll)
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

	if (coll->mid_floor > 100 && !(room[item->room_number].flags & ROOM_SWAMP))
	{
		item->anim_number = ANIM_FALLDOWN;
		item->frame_number = anims[ANIM_FALLDOWN].frame_base;
		item->current_anim_state = AS_FORWARDJUMP;
		item->goal_anim_state = AS_FORWARDJUMP;
		item->gravity_status = 1;
		item->fallspeed = 0;
		return;
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor < 0 || !(room[item->room_number].flags & ROOM_SWAMP))
		item->pos.y_pos += coll->mid_floor;
	else if (coll->mid_floor)
		item->pos.y_pos += 2;
}

void lara_col_turn_l(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_turn_r(item, coll);
}

void lara_col_death(ITEM_INFO* item, COLL_INFO* coll)
{
	StopSoundEffect(SFX_LARA_FALL);
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->radius = 400;
	GetLaraCollisionInfo(item, coll);
	ShiftItem(item, coll);
	item->pos.y_pos += coll->mid_floor;
	item->hit_points = -1;
	lara.air = -1;
}

void lara_col_fastfall(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 1;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraSlideEdgeJump(item, coll);

	if (coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = 8;
		else
		{
			item->anim_number = ANIM_LANDFAR;
			item->frame_number = anims[ANIM_LANDFAR].frame_base;
			item->current_anim_state = AS_STOP;
			item->goal_anim_state = AS_STOP;
		}

		StopSoundEffect(SFX_LARA_FALL);
		item->pos.y_pos += coll->mid_floor;
		item->gravity_status = 0;
		item->fallspeed = 0;
	}
}

void lara_col_hang(ITEM_INFO* item, COLL_INFO* coll)
{
	LaraHangTest(item, coll);

	if (item->goal_anim_state != AS_HANG)
		return;

	if (input & IN_FORWARD)
	{
		if (coll->front_floor > -850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= 0 &&
			coll->left_floor2 - coll->left_ceiling2 >= 0 && coll->right_floor2 - coll->right_ceiling2 >= 0 && !coll->hit_static)
		{
			if (input & IN_WALK)
				item->goal_anim_state = AS_GYMNAST;
			else
				item->goal_anim_state = AS_NULL;

			return;
		}

		if (lara.climb_status && item->anim_number == ANIM_GRABLEDGE && item->frame_number == anims[ANIM_GRABLEDGE].frame_base + 21 && coll->mid_ceiling <= -256)
		{
			item->anim_number = ANIM_HANGUP;
			item->frame_number = anims[ANIM_HANGUP].frame_base;
			item->current_anim_state = AS_HANG;
			item->goal_anim_state = AS_HANG;
			return;
		}
	}

	if (input & (IN_FORWARD | IN_DUCK) && coll->front_floor > -850 && coll->front_floor < -650 && coll->front_floor - coll->front_ceiling >= -256 &&
		coll->left_floor2 - coll->left_ceiling2 >= -256 && coll->right_floor2 - coll->right_ceiling2 >= -256 && !coll->hit_static)
	{
		item->goal_anim_state = AS_HANG2DUCK;
		item->required_anim_state = AS_DUCK;
	}
	else if (input & IN_BACK && lara.climb_status && item->anim_number == ANIM_GRABLEDGE && item->frame_number == anims[ANIM_GRABLEDGE].frame_base + 21)
	{
		item->anim_number = ANIM_HANGDOWN;
		item->frame_number = anims[ANIM_HANGDOWN].frame_base;
		item->current_anim_state = AS_HANG;
		item->goal_anim_state = AS_HANG;
	}
}

long LaraTestHangJump(ITEM_INFO* item, COLL_INFO* coll)
{
	short* bounds;
	long edge, edge_catch;
	short angle;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || coll->hit_static)
		return 0;

	if (lara.CanMonkeySwing && coll->coll_type == CT_TOP)
	{
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		item->anim_number = ANIM_GRABLEDGEIN;
		item->frame_number = anims[ANIM_GRABLEDGEIN].frame_base;
		item->current_anim_state = AS_HANG2;
		item->goal_anim_state = AS_HANG2;
		item->gravity_status = 0;
		item->speed = 0;
		item->fallspeed = 0;
		lara.gun_status = LG_HANDSBUSY;
		return 1;
	}

	if (coll->mid_ceiling > -384 || coll->mid_floor < 200 || coll->coll_type != CT_FRONT)
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
		lara.head_x_rot = 0;
		lara.head_y_rot = 0;
		lara.torso_x_rot = 0;
		lara.torso_y_rot = 0;
		item->anim_number = ANIM_GRABLEDGEIN;
		item->frame_number = anims[ANIM_GRABLEDGEIN].frame_base;
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

		switch (ushort(item->pos.y_rot + 0x2000) / 0x4000)
		{
		case NORTH:
			item->pos.z_pos = (item->pos.z_pos | WALL_MASK) - 100;
			item->pos.x_pos += coll->shift.x;
			break;

		case EAST:
			item->pos.x_pos = (item->pos.x_pos | WALL_MASK) - 100;
			item->pos.z_pos += coll->shift.z;
			break;

		case SOUTH:
			item->pos.z_pos = (item->pos.z_pos & ~WALL_MASK) + 100;
			item->pos.x_pos += coll->shift.x;
			break;

		case WEST:
			item->pos.x_pos = (item->pos.x_pos & ~WALL_MASK) + 100;
			item->pos.z_pos += coll->shift.z;
			break;
		}
	}

	item->gravity_status = 1;
	item->pos.y_rot = angle;
	item->speed = 2;
	item->fallspeed = 1;
	lara.gun_status = LG_HANDSBUSY;
	return 1;
}

void lara_col_reach(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 1;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = 0;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);

	if (LaraTestHangJump(item, coll))
		return;

	LaraSlideEdgeJump(item, coll);
	GetLaraCollisionInfo(item, coll);
	ShiftItem(item, coll);

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->fallspeed = 0;
		item->gravity_status = 0;
		item->pos.y_pos += coll->mid_floor;
	}
}

void lara_col_splat(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);
	ShiftItem(item, coll);

	if (coll->mid_floor > -256 && coll->mid_floor < 256)
		item->pos.y_pos += coll->mid_floor;
}

void lara_col_land(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_stop(item, coll);
}

void lara_col_compress(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (coll->mid_ceiling > -100)
	{
		item->anim_number = 11;
		item->frame_number = anims[11].frame_base;
		item->current_anim_state = 2;
		item->goal_anim_state = 2;
		item->gravity_status = 0;
		item->speed = 0;
		item->fallspeed = 0;
		item->pos.x_pos = coll->old.x;
		item->pos.y_pos = coll->old.y;
		item->pos.z_pos = coll->old.z;
	}

	if (coll->mid_floor > -256 && coll->mid_floor < 256)
		item->pos.y_pos += coll->mid_floor;
}

void lara_col_back(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot + 0x8000;

	if (lara.water_status == LARA_WADE)
		coll->bad_pos = -NO_HEIGHT;
	else
		coll->bad_pos = 384;

	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
		LaraCollideStop(item, coll);

	if (LaraFallen(item, coll))
		return;

	if (coll->mid_floor > 128 && coll->mid_floor < 384)
	{
		if (item->frame_number < 964 || item->frame_number > 993)
		{
			item->anim_number = ANIM_BACKSTEPD_LEFT;
			item->frame_number = anims[ANIM_BACKSTEPD_LEFT].frame_base;
		}
		else
		{
			item->anim_number = ANIM_BACKSTEPD_RIGHT;
			item->frame_number = anims[ANIM_BACKSTEPD_RIGHT].frame_base;
		}
	}

	if (TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor >= 0 && room[item->room_number].flags & ROOM_SWAMP)
		item->pos.y_pos += 2;
	else if (lara.water_status == LARA_WADE && coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else
		item->pos.y_pos += coll->mid_floor;
}

void lara_col_stepright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->current_anim_state == AS_STEPRIGHT)
		lara.move_angle = item->pos.y_rot + 0x4000;
	else
		lara.move_angle = item->pos.y_rot - 0x4000;

	item->gravity_status = 0;
	item->fallspeed = 0;

	if (lara.water_status == LARA_WADE)
		coll->bad_pos = -NO_HEIGHT;
	else
		coll->bad_pos = 128;

	coll->bad_neg = -128;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll))
		return;

	if (LaraDeflectEdge(item, coll))
		LaraCollideStop(item, coll);

	if (LaraFallen(item, coll) || TestLaraSlide(item, coll))
		return;

	if (lara.water_status == LARA_WADE && coll->mid_floor >= 50)
		item->pos.y_pos += 50;
	else
		item->pos.y_pos += coll->mid_floor;
}

void lara_col_stepleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_col_stepright(item, coll);
}

void lara_col_slide(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	lara_slide_slope(item, coll);
}

void lara_col_jumper(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->fallspeed = 0;
		item->gravity_status = 0;
		item->pos.y_pos += coll->mid_floor;
	}
}

void lara_col_backjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x8000;
	lara_col_jumper(item, coll);
}

void lara_col_rightjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x4000;
	lara_col_jumper(item, coll);
}

void lara_col_leftjump(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x4000;
	lara_col_jumper(item, coll);
}

void lara_col_fallback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x8000;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);
	
	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		if (LaraLandedBad(item, coll))
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->fallspeed = 0;
		item->gravity_status = 0;
		item->pos.y_pos += coll->mid_floor;
	}
}

void lara_col_hangleft(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot - 0x4000;
	LaraHangTest(item, coll);
	lara.move_angle = item->pos.y_rot - 0x4000;
}

void lara_col_hangright(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x4000;
	LaraHangTest(item, coll);
	lara.move_angle = item->pos.y_rot + 0x4000;
}

void lara_col_slideback(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot + 0x8000;
	lara_slide_slope(item, coll);
}

void lara_default_col(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_pits = 1;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);
}

void lara_col_pushblock(ITEM_INFO* item, COLL_INFO* coll)
{
	lara_default_col(item, coll);
}

void lara_col_roll(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll) || TestLaraSlide(item, coll) || LaraFallen(item, coll))
		return;

	ShiftItem(item, coll);
	item->pos.y_pos += coll->mid_floor;
}

void lara_col_roll2(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot + 0x8000;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	GetLaraCollisionInfo(item, coll);

	if (LaraHitCeiling(item, coll) || TestLaraSlide(item, coll))
		return;

	if (coll->mid_floor > 200)
	{
		item->anim_number = ANIM_FALLBACK;
		item->frame_number = anims[ANIM_FALLBACK].frame_base;
		item->current_anim_state = AS_FALLBACK;
		item->goal_anim_state = AS_FALLBACK;
		item->fallspeed = 0;
		item->gravity_status = 1;
		return;
	}

	ShiftItem(item, coll);
	item->pos.y_pos += coll->mid_floor;
}

void lara_col_swandive(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (coll->mid_floor <= 0 && item->fallspeed > 0)
	{
		item->goal_anim_state = AS_STOP;
		item->fallspeed = 0;
		item->gravity_status = 0;
		item->pos.y_pos += coll->mid_floor;
	}
}

void lara_col_fastdive(ITEM_INFO* item, COLL_INFO* coll)
{
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 192;
	GetLaraCollisionInfo(item, coll);
	LaraDeflectEdgeJump(item, coll);

	if (item->fallspeed > 0 && coll->mid_floor <= 0)
	{
		if (item->fallspeed > 133)
			item->goal_anim_state = AS_DEATH;
		else
			item->goal_anim_state = AS_STOP;

		item->fallspeed = 0;
		item->gravity_status = 0;
		item->pos.y_pos += coll->mid_floor;
	}
}

void lara_col_wade(ITEM_INFO* item, COLL_INFO* coll)
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

		if ((coll->front_type == WALL || coll->front_type == SPLIT_TRI) && coll->front_floor < -640 && !(room[item->room_number].flags & ROOM_SWAMP))
		{
			item->current_anim_state = AS_SPLAT;

			if (item->frame_number >= 0 && item->frame_number <= 9)
			{
				item->anim_number = ANIM_HITWALLLEFT;
				item->frame_number = anims[ANIM_HITWALLLEFT].frame_base;
				return;
			}

			if (item->frame_number >= 10 && item->frame_number <= 21)
			{
				item->anim_number = ANIM_HITWALLRIGHT;
				item->frame_number = anims[ANIM_HITWALLRIGHT].frame_base;
				return;
			}
		}

		LaraCollideStop(item, coll);
	}

	if (coll->mid_floor >= -384 && coll->mid_floor < -128 && !(room[item->room_number].flags & ROOM_SWAMP))
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

	if (coll->mid_floor >= 50 && !(room[item->room_number].flags & ROOM_SWAMP))
		item->pos.y_pos += 50;
	else
	{
		if (coll->mid_floor < 0 || !(room[item->room_number].flags & ROOM_SWAMP))
			item->pos.y_pos += coll->mid_floor;
		else if (coll->mid_floor)
			item->pos.y_pos += 2;
	}
}

void LaraHangTest(ITEM_INFO* item, COLL_INFO* coll)
{
	long flag, hdif;
	short angle;

	flag = 0;
	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = NO_HEIGHT;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (coll->front_floor < 200)
		flag = 1;

	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	angle = (ushort)(item->pos.y_rot + 0x2000) / 0x4000;

	switch (angle)
	{
	case NORTH:
		item->pos.z_pos += 4;
		break;

	case EAST:
		item->pos.x_pos += 4;
		break;

	case SOUTH:
		item->pos.z_pos -= 4;
		break;

	case WEST:
		item->pos.x_pos -= 4;
		break;
	}

	coll->bad_pos = -NO_HEIGHT;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	GetLaraCollisionInfo(item, coll);

	if (lara.climb_status)
	{
		if (!(input & IN_ACTION) || item->hit_points <= 0)
		{
			item->anim_number = ANIM_FALLDOWN;
			item->frame_number = anims[ANIM_FALLDOWN].frame_base;
			item->current_anim_state = AS_FORWARDJUMP;
			item->goal_anim_state = AS_FORWARDJUMP;
			item->pos.y_pos += 256;
			item->gravity_status = 1;
			item->speed = 2;
			item->fallspeed = 1;
			lara.gun_status = LG_ARMLESS;
		}
		else if (LaraTestHangOnClimbWall(item, coll))
		{
			if (item->anim_number == ANIM_GRABLEDGE && item->frame_number == anims[ANIM_GRABLEDGE].frame_base + 21)
			{
				if (LaraTestClimbStance(item, coll))
					item->goal_anim_state = AS_CLIMBSTNC;
			}
		}
		else
		{
			item->pos.x_pos = coll->old.x;
			item->pos.y_pos = coll->old.y;
			item->pos.z_pos = coll->old.z;
			item->anim_number = ANIM_GRABLEDGE;
			item->frame_number = anims[ANIM_GRABLEDGE].frame_base + 21;
			item->current_anim_state = AS_HANG;
			item->goal_anim_state = AS_HANG;
		}
	}
	else
	{
		if (!(input & IN_ACTION) || item->hit_points <= 0 || coll->front_floor > 0)
		{
			item->anim_number = ANIM_STOPHANG;
			item->frame_number = anims[ANIM_STOPHANG].frame_base + 9;
			item->current_anim_state = AS_UPJUMP;
			item->goal_anim_state = AS_UPJUMP;
			item->pos.x_pos += coll->shift.x;
			item->pos.y_pos += GetBoundsAccurate(item)[3];
			item->pos.z_pos += coll->shift.z;
			item->gravity_status = 1;
			item->speed = 2;
			item->fallspeed = 1;
			lara.gun_status = LG_ARMLESS;
		}
		else
		{
			hdif = coll->front_floor - GetBoundsAccurate(item)[2];

			if (abs(coll->left_floor2 - coll->right_floor2) >= 60 || coll->mid_ceiling >= 0 ||
				coll->coll_type != CT_FRONT || flag || coll->hit_static || hdif < -60 || hdif > 60)
			{
				item->pos.x_pos = coll->old.x;
				item->pos.y_pos = coll->old.y;
				item->pos.z_pos = coll->old.z;

				if (item->current_anim_state == AS_HANGLEFT || item->current_anim_state == AS_HANGRIGHT)
				{
					item->anim_number = ANIM_GRABLEDGE;
					item->frame_number = anims[ANIM_GRABLEDGE].frame_base + 21;
					item->current_anim_state = AS_HANG;
					item->goal_anim_state = AS_HANG;
				}
			}
			else
			{
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

				item->pos.y_pos += hdif;
			}
		}
	}
}

void lara_as_duckroll(ITEM_INFO* item, COLL_INFO* coll)
{
	camera.target_elevation = -3640;
	item->goal_anim_state = AS_DUCK;
	lara.IsDucked = 1;	//she's ducked for all intents and purposes
}

void lara_col_duckroll(ITEM_INFO* item, COLL_INFO* coll)
{
	item->gravity_status = 0;
	item->fallspeed = 0;
	lara.move_angle = item->pos.y_rot;
	coll->bad_pos = 384;
	coll->facing = item->pos.y_rot;
	coll->bad_neg = -384;
	coll->bad_ceiling = 0;
	coll->slopes_are_walls = 1;
	coll->radius = 200;
	GetCollisionInfo(coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, -400);

	if (LaraFallen(item, coll))
		lara.gun_status = LG_ARMLESS;
	else if (!TestLaraSlide(item, coll))
	{
		if (coll->mid_ceiling >= -362)
			lara.keep_ducked = 1;
		else
			lara.keep_ducked = 0;

		if (coll->mid_floor < coll->bad_neg || coll->front_floor > coll->bad_pos)
		{
			item->pos.x_pos = coll->old.x;
			item->pos.y_pos = coll->old.y;
			item->pos.z_pos = coll->old.z;
			return;
		}

		ShiftItem(item, coll);

		if (!LaraHitCeiling(item, coll))
			item->pos.y_pos += coll->mid_floor;
	}
}

void LookUpDown()
{
	camera.type = LOOK_CAMERA;

	if (input & IN_FORWARD)
	{
		input -= IN_FORWARD;

		if (lara.head_x_rot > -6370)
			lara.head_x_rot -= 364;
	}
	else if (input & IN_BACK)
	{
		input -= IN_BACK;

		if (lara.head_x_rot < 5460)
			lara.head_x_rot += 364;
	}

	if (lara.gun_status != LG_HANDSBUSY && lara.skidoo == NO_ITEM && !lara.left_arm.lock && !lara.right_arm.lock)
		lara.torso_x_rot = lara.head_x_rot;
}

void LookLeftRight()
{
	camera.type = LOOK_CAMERA;

	if (input & IN_LEFT)
	{
		input -= IN_LEFT;

		if (lara.head_y_rot > -8008)
			lara.head_y_rot -= 364;
	}
	else if (input & IN_RIGHT)
	{
		input -= IN_RIGHT;

		if (lara.head_y_rot < 8008)
			lara.head_y_rot += 364;
	}

	if (lara.gun_status != LG_HANDSBUSY && lara.skidoo == NO_ITEM && !lara.left_arm.lock && !lara.right_arm.lock)
		lara.torso_y_rot = lara.head_y_rot;
}

void ResetLook()
{
	if (camera.type != LOOK_CAMERA)
	{
		if (lara.head_x_rot > -364 && lara.head_x_rot < 364)
			lara.head_x_rot = 0;
		else
			lara.head_x_rot -= lara.head_x_rot / 8;

		if (lara.head_y_rot > -364 && lara.head_y_rot < 364)
			lara.head_y_rot = 0;
		else
			lara.head_y_rot -= lara.head_y_rot / 8;

		if (lara.gun_status != LG_HANDSBUSY && lara.skidoo == NO_ITEM)
		{
			lara.torso_x_rot = lara.head_x_rot;
			lara.torso_y_rot = lara.head_y_rot;
		}
	}
}

long GetStaticObjects(ITEM_INFO* item, short ang, long hite, long rad, long dist)
{
	ROOM_INFO* r;
	MESH_INFO* mesh;
	STATIC_INFO* sinfo;
	long x, y, z;
	long xmin, xmax, ymin, ymax, zmin, zmax;
	long inxmin, inxmax, inymin, inymax, inzmin, inzmax;

	x = item->pos.x_pos + ((dist * phd_sin(ang)) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos + ((dist * phd_cos(ang)) >> W2V_SHIFT);

	inxmin = x - rad;
	inxmax = x + rad;
	inymin = y - hite;
	inymax = y;
	inzmin = z - rad;
	inzmax = z + rad;
	GetNearByRooms(x, y, z, rad + 50, hite + 50, item->room_number);

	for (int i = 0; i < number_draw_rooms; i++)
	{
		r = &room[draw_rooms[i]];

		for (int j = 0; j < r->num_meshes; j++)
		{
			mesh = &r->mesh[j];
			sinfo = &static_objects[mesh->static_number];

			if (sinfo->flags & 1)
				continue;

			ymin = mesh->y + sinfo->y_minc;
			ymax = mesh->y + sinfo->y_maxc;

			switch (mesh->y_rot)
			{
			case 0x4000:
				xmin = mesh->x + sinfo->z_minc;
				xmax = mesh->x + sinfo->z_maxc;
				zmin = mesh->z - sinfo->x_maxc;
				zmax = mesh->z - sinfo->x_minc;
				break;

			case -0x4000:
				xmin = mesh->x - sinfo->z_maxc;
				xmax = mesh->x - sinfo->z_minc;
				zmin = mesh->z + sinfo->x_minc;
				zmax = mesh->z + sinfo->x_maxc;
				break;

			case -0x8000:
				xmin = mesh->x - sinfo->x_maxc;
				xmax = mesh->x - sinfo->x_minc;
				zmin = mesh->z - sinfo->z_maxc;
				zmax = mesh->z - sinfo->z_minc;
				break;

			default:
				xmin = mesh->x + sinfo->x_minc;
				xmax = mesh->x + sinfo->x_maxc;
				zmin = mesh->z + sinfo->z_minc;
				zmax = mesh->z + sinfo->z_maxc;
				break;
			}

			if (inxmax > xmin && inxmin < xmax && inymax > ymin && inymin < ymax && inzmax > zmin && inzmin < zmax)
				return 1;
		}
	}

	return 0;
}

void ControlDeathSlide(short item_number)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	GAME_VECTOR* old;
	long x, y, z, h, c;
	short room_number;

	item = &items[item_number];

	if (item->status != ITEM_ACTIVE)
		return;

	if (item->flags & IFL_INVISIBLE)
	{
		if (item->current_anim_state == 1)
		{
			AnimateItem(item);
			return;
		}

		AnimateItem(item);

		if (item->fallspeed < 100)
			item->fallspeed += 5;

		item->pos.x_pos += item->fallspeed * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
		item->pos.y_pos += item->fallspeed >> 2;
		item->pos.z_pos += item->fallspeed * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		room_number = item->room_number;
		GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);

		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);

		if (lara_item->current_anim_state == AS_DEATHSLIDE)
		{
			lara_item->pos.x_pos = item->pos.x_pos;
			lara_item->pos.y_pos = item->pos.y_pos;
			lara_item->pos.z_pos = item->pos.z_pos;
		}

		x = item->pos.x_pos + (phd_sin(item->pos.y_rot) >> 4);
		y = item->pos.y_pos + 64;
		z = item->pos.z_pos + (phd_cos(item->pos.y_rot) >> 4);
		floor = GetFloor(x, y, z, &room_number);
		h = GetHeight(floor, x, y, z);
		c = GetCeiling(floor, x, y, z);

		if (h <= y + 256 || c >= y - 256)
		{
			if (lara_item->current_anim_state == AS_DEATHSLIDE)
			{
				lara_item->goal_anim_state = 3;
				AnimateLara(lara_item);
				lara_item->gravity_status = 1;
				lara_item->speed = item->fallspeed;
				lara_item->fallspeed = item->fallspeed >> 2;
			}

			SoundEffect(SFX_DEATH_SLIDE_STOP, &item->pos, SFX_DEFAULT);
			RemoveActiveItem(item_number);
			item->status = ITEM_INACTIVE;
			item->flags -= IFL_INVISIBLE;
		}
		else
			SoundEffect(SFX_DEATH_SLIDE_GO, &item->pos, SFX_DEFAULT);
	}
	else
	{
		old = (GAME_VECTOR*)item->data;
		item->pos.x_pos = old->x;
		item->pos.y_pos = old->y;
		item->pos.z_pos = old->z;

		if (old->room_number != item->room_number)
			ItemNewRoom(item_number, old->room_number);

		item->status = ITEM_INACTIVE;
		item->anim_number = objects[item->object_number].anim_index;
		item->frame_number = anims[item->anim_number].frame_base;
		item->current_anim_state = 1;
		item->goal_anim_state = 1;
		RemoveActiveItem(item_number);
	}
}

void DeathSlideCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;

	if (input & IN_ACTION && !l->gravity_status && lara.gun_status == LG_ARMLESS && l->current_anim_state == AS_STOP)
	{
		item = &items[item_number];

		if (item->status == ITEM_INACTIVE)
		{
			if (TestLaraPosition(DeathSlideBounds, item, l))
			{
				AlignLaraPosition(&DeathSlidePosition, item, l);
				lara.gun_status = LG_HANDSBUSY;
				l->goal_anim_state = AS_DEATHSLIDE;

				do AnimateLara(l); while (l->current_anim_state != AS_NULL);

				if (!item->active)
					AddActiveItem(item_number);

				item->status = ITEM_ACTIVE;
				item->flags |= IFL_INVISIBLE;
			}
		}
	}
}
