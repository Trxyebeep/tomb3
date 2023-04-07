#include "../tomb3/pch.h"
#include "boat.h"
#include "../specific/init.h"
#include "../3dsystem/phd_math.h"
#include "collide.h"
#include "sphere.h"
#include "objects.h"
#include "items.h"
#include "control.h"
#include "../specific/specific.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "lara.h"
#include "sound.h"
#include "effect2.h"
#include "effects.h"
#include "../specific/game.h"
#include "../specific/input.h"
#include "camera.h"

void InitialiseBoat(short item_number)
{
	ITEM_INFO* item;
	BOAT_INFO* boat;

	item = &items[item_number];
	boat = (BOAT_INFO*)game_malloc(sizeof(BOAT_INFO));
	item->data = boat;
	boat->boat_turn = 0;
	boat->right_fallspeed = 0;
	boat->left_fallspeed = 0;
	boat->tilt_angle = 0;
	boat->extra_rotation = 0;
	boat->water = 0;
	boat->pitch = 0;

	for (int i = 0; i < 32; i++)
	{
		WakePts[i][0].life = 0;
		WakePts[i][1].life = 0;
	}
}

static long BoatCheckGeton(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long dx, dz, pass;
	short ang;

	if (lara.gun_status != LG_ARMLESS)
		return 0;

	item = &items[item_number];
	dx = lara_item->pos.x_pos - item->pos.x_pos;
	dz = lara_item->pos.z_pos - item->pos.z_pos;

	if ((dz * phd_cos(-item->pos.y_rot) - dx * phd_sin(-item->pos.y_rot)) >> W2V_SHIFT > 512)
		return 0;

	pass = 0;
	ang = item->pos.y_rot - lara_item->pos.y_rot;

	if (lara.water_status == LARA_SURFACE || lara.water_status == LARA_WADE)
	{
		if (!(input & IN_ACTION) || lara_item->gravity_status || item->speed)
			return 0;

		if (ang > 0x2000 && ang < 0x6000)
			pass = 1;
		else if (ang > -0x6000 && ang < -0x2000)
			pass = 2;
	}
	else if (lara.water_status == LARA_ABOVEWATER)
	{
		if (lara_item->fallspeed > 0)
		{
			if (lara_item->pos.y_pos + 512 > item->pos.y_pos)
				pass = 3;
		}
		else if (!lara_item->fallspeed)
		{
			if (ang > -0x6000 && ang < 0x6000)
			{
				if (lara_item->pos.x_pos == item->pos.x_pos &&
					lara_item->pos.y_pos == item->pos.y_pos &&
					lara_item->pos.z_pos == item->pos.z_pos)
					pass = 4;
				else
					pass = 3;
			}
		}
	}

	if (pass)
	{
		if (!TestBoundsCollide(item, lara_item, coll->radius))
			return 0;

		if (!TestCollision(item, lara_item))
			return 0;
	}

	return pass;
}

void BoatCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	long geton;

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	geton = BoatCheckGeton(item_number, coll);

	if (!geton)
	{
		coll->enable_baddie_push = 1;
		ObjectCollision(item_number, l, coll);
		return;
	}

	lara.skidoo = item_number;

	if (geton == 1)
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 8;
	else if (geton == 2)
		l->anim_number = objects[VEHICLE_ANIM].anim_index;
	else if (geton == 3)
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 6;
	else
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 1;

	lara.water_status = LARA_ABOVEWATER;
	item = &items[item_number];
	l->pos.x_pos = item->pos.x_pos;
	l->pos.y_pos = item->pos.y_pos - 5;
	l->pos.z_pos = item->pos.z_pos;
	l->pos.x_rot = 0;
	l->pos.y_rot = item->pos.y_rot;
	l->pos.z_rot = 0;
	l->gravity_status = 0;
	l->speed = 0;
	l->fallspeed = 0;
	l->frame_number = anims[l->anim_number].frame_base;
	l->current_anim_state = BOAT_GETON;
	l->goal_anim_state = BOAT_GETON;

	if (l->room_number != item->room_number)
		ItemNewRoom(lara.item_number, item->room_number);

	AnimateItem(l);

	if (item->status != ITEM_ACTIVE)
	{
		AddActiveItem(item_number);
		item->status = ITEM_ACTIVE;
	}

	S_CDPlay(12, 0);
}

void DrawBoat(ITEM_INFO* item)
{
	BOAT_INFO* boat;

	boat = (BOAT_INFO*)item->data;
	item->data = &boat->prop_rot;
	DrawAnimatingItem(item);
	item->data = boat;
	S_DrawWakeFX(item);
}

static long BoatUserControl(ITEM_INFO* item)
{
	BOAT_INFO* boat;
	long no_turn, max_speed;

	boat = (BOAT_INFO*)item->data;
	no_turn = 1;

	if (item->pos.y_pos < boat->water - 128 || boat->water == NO_HEIGHT)
		return 1;

	if ((input & IN_ROLL || input & IN_LOOK) && !item->speed)
	{
		if (!(input & (IN_RSTEP | IN_RIGHT | IN_LSTEP | IN_LEFT)))
			item->speed = 0;
		else if (!(input & IN_ROLL))
			item->speed = 20;

		if (input & IN_LOOK && !item->speed)
			LookUpDown();
	}
	else
	{
		if ((input & (IN_LSTEP | IN_LEFT)) && !(input & IN_JUMP) || (input & (IN_RSTEP | IN_RIGHT)) && input & IN_JUMP)
		{
			if (boat->boat_turn > 0)
				boat->boat_turn -= 45;
			else
			{
				boat->boat_turn -= 22;

				if (boat->boat_turn < -728)
					boat->boat_turn = -728;
			}

			no_turn = 0;
		}
		else if ((input & (IN_RSTEP | IN_RIGHT)) && !(input & IN_JUMP) || (input & (IN_LSTEP | IN_LEFT)) && input & IN_JUMP)
		{
			if (boat->boat_turn < 0)
				boat->boat_turn += 45;
			else
			{
				boat->boat_turn += 22;

				if (boat->boat_turn > 728)
					boat->boat_turn = 728;
			}

			no_turn = 0;
		}

		if (input & IN_JUMP)
		{
			if (item->speed > 0)
				item->speed -= 5;
			else if (item->speed > -20)
				item->speed -= 2;
		}
		else if (input & IN_ACTION)
		{
			if (input & IN_SPRINT)
				max_speed = 185;
			else if (input & IN_WALK)
				max_speed = 36;
			else
				max_speed = 110;

			if (item->speed < max_speed)
				item->speed = short(5 * item->speed / (2 * max_speed) + item->speed + 2);
			else if (item->speed > max_speed + 1)
				item->speed--;
		}
		else if (item->speed >= 0 && item->speed < 20 && (input & (IN_RSTEP | IN_RIGHT | IN_LSTEP | IN_LEFT)))
		{
			if (!item->speed && !(input & IN_ROLL))
				item->speed = 20;
		}
		else if (item->speed > 1)
			item->speed--;
		else
			item->speed = 0;
	}

	return no_turn;
}

static long CanGetOff(long lr)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short angle, room_number;

	item = &items[lara.skidoo];

	if (lr >= 0)
		angle = item->pos.y_rot + 0x4000;
	else
		angle = item->pos.y_rot - 0x4000;

	x = item->pos.x_pos + ((WALL_SIZE * phd_sin(angle)) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos + ((WALL_SIZE * phd_cos(angle)) >> W2V_SHIFT);

	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (h - item->pos.y_pos >= -512 && height_type != BIG_SLOPE && height_type != DIAGONAL && c - item->pos.y_pos <= -762 && h - c >= 762)
		return 1;

	return 0;
}

static void BoatAnimation(ITEM_INFO* item, long collide)
{
	BOAT_INFO* boat;

	boat = (BOAT_INFO*)item->data;

	if (lara_item->hit_points <= 0)
	{
		if (lara_item->current_anim_state != BOAT_DEATH)
		{
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 18;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->current_anim_state = BOAT_DEATH;
			lara_item->goal_anim_state = BOAT_DEATH;
		}
	}
	else if (item->pos.y_pos < boat->water - 128 && item->fallspeed > 0)
	{
		if (lara_item->current_anim_state != BOAT_FALL)
		{
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 15;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->current_anim_state = BOAT_FALL;
			lara_item->goal_anim_state = BOAT_FALL;
		}
	}
	else if (collide)
	{
		if (lara_item->current_anim_state != BOAT_HIT)
		{
			lara_item->anim_number = short(objects[VEHICLE_ANIM].anim_index + collide);
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->current_anim_state = BOAT_HIT;
			lara_item->goal_anim_state = BOAT_HIT;
		}
	}
	else
	{
		switch (lara_item->current_anim_state)
		{
		case BOAT_STILL:

			if (input & IN_ROLL && !item->speed)
			{
				if (input & (IN_RSTEP | IN_RIGHT) && CanGetOff(item->pos.y_rot + 0x4000))
					lara_item->goal_anim_state = BOAT_JUMPR;
				else if (input & (IN_LSTEP | IN_LEFT) && CanGetOff(item->pos.y_rot - 0x4000))
					lara_item->goal_anim_state = BOAT_JUMPL;
			}

			if (item->speed > 0)
				lara_item->goal_anim_state = BOAT_MOVING;

			break;

		case BOAT_MOVING:

			if (item->speed <= 0)
				lara_item->goal_anim_state = BOAT_STILL;
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = BOAT_TURNR;
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = BOAT_TURNL;

			break;

		case BOAT_FALL:
			lara_item->goal_anim_state = BOAT_MOVING;
			break;

		case BOAT_TURNR:

			if (item->speed <= 0)
				lara_item->goal_anim_state = BOAT_STILL;
			else if (!(input & (IN_RSTEP | IN_RIGHT)))
				lara_item->goal_anim_state = BOAT_MOVING;

			break;

		case BOAT_TURNL:

			if (item->speed <= 0)
				lara_item->goal_anim_state = BOAT_STILL;
			else if (!(input & (IN_LSTEP | IN_LEFT)))
				lara_item->goal_anim_state = BOAT_MOVING;

			break;
		}
	}
}

static long TestWaterHeight(ITEM_INFO* item, long z, long x, PHD_VECTOR* pos)
{
	FLOOR_INFO* floor;
	long s, c, h;
	short room_number;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	pos->x = item->pos.x_pos + ((x * c + z * s) >> W2V_SHIFT);
	pos->y = item->pos.y_pos + ((x * phd_sin(item->pos.z_rot)) >> W2V_SHIFT) - ((z * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
	pos->z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);

	room_number = item->room_number;
	GetFloor(pos->x, pos->y, pos->z, &room_number);
	h = GetWaterHeight(pos->x, pos->y, pos->z, room_number);

	if (h == NO_HEIGHT)
	{
		floor = GetFloor(pos->x, pos->y, pos->z, &room_number);
		h = GetHeight(floor, pos->x, pos->y, pos->z);

		if (h == NO_HEIGHT)
			return h;
	}

	return h - 5;
}

static long DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos)
{
	FLOOR_INFO* floor;
	long x, z, nX, nZ, oX, oZ, sX, sZ, h;
	short room_number;

	nX = newPos->x >> WALL_SHIFT;
	nZ = newPos->z >> WALL_SHIFT;
	oX = oldPos->x >> WALL_SHIFT;
	oZ = oldPos->z >> WALL_SHIFT;
	sX = newPos->x & WALL_MASK;
	sZ = newPos->z & WALL_MASK;

	if (nX == oX)
	{
		if (nZ == oZ)
		{
			item->pos.z_pos += (oldPos->z - newPos->z);
			item->pos.x_pos += (oldPos->x - newPos->x);
			return 0;
		}
		else if (nZ <= oZ)
		{
			item->pos.z_pos += WALL_SIZE - sZ;
			return item->pos.x_pos - newPos->x;
		}
		else
		{
			item->pos.z_pos -= 1 + sZ;
			return newPos->x - item->pos.x_pos;
		}
	}
	
	if (nZ == oZ)
	{
		if (nX <= oX)
		{
			item->pos.x_pos += WALL_SIZE - sX;
			return newPos->z - item->pos.z_pos;
		}
		else
		{
			item->pos.x_pos -= 1 + sX;
			return item->pos.z_pos - newPos->z;
		}
	}

	x = 0;
	z = 0;
	room_number = item->room_number;
	floor = GetFloor(oldPos->x, newPos->y, newPos->z, &room_number);
	h = GetHeight(floor, oldPos->x, newPos->y, newPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->z > oldPos->z)
			z = -1 - sZ;
		else
			z = WALL_SIZE - sZ;
	}

	room_number = item->room_number;
	floor = GetFloor(newPos->x, newPos->y, oldPos->z, &room_number);
	h = GetHeight(floor, newPos->x, newPos->y, oldPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->x > oldPos->x)
			x = -1 - sX;
		else
			x = WALL_SIZE - sX;
	}

	if (x && z)
	{
		item->pos.x_pos += x;
		item->pos.z_pos += z;
		return 0;
	}

	if (z)
	{
		item->pos.z_pos += z;

		if (z > 0)
			return item->pos.x_pos - newPos->x;
		else
			return newPos->x - item->pos.x_pos;
	}

	if (x)
	{
		item->pos.x_pos += x;

		if (x > 0)
			return newPos->z - item->pos.z_pos;
		else
			return item->pos.z_pos - newPos->z;
	}

	item->pos.x_pos += oldPos->x - newPos->x;
	item->pos.z_pos += oldPos->z - newPos->z;
	return 0;
}

static long GetCollisionAnim(ITEM_INFO* item, PHD_VECTOR* pos)
{
	long s, c, fb, lr;

	pos->x = item->pos.x_pos - pos->x;
	pos->z = item->pos.z_pos - pos->z;

	if (!pos->x && !pos->z)
		return 0;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	fb = (s * pos->x + c * pos->z) >> W2V_SHIFT;
	lr = (c * pos->x - s * pos->z) >> W2V_SHIFT;

	if (abs(fb) <= abs(lr))
	{
		if (lr > 0)
			return 11;
		else
			return 12;
	}
	else
	{
		if (fb > 0)
			return 14;
		else
			return 13;
	}
}

static void DoBoatShift(long item_number)
{
	ITEM_INFO* item;
	ITEM_INFO* boat;
	long item_num, x, z, dist;

	item = &items[item_number];

	for (item_num = room[item->room_number].item_number; item_num != NO_ITEM; item_num = boat->next_item)
	{
		boat = &items[item_num];
		
		if (boat->object_number == BOAT && item_num != item_number && lara.skidoo != item_num)
		{
			x = boat->pos.z_pos - item->pos.z_pos;
			z = boat->pos.x_pos - item->pos.x_pos;
			dist = SQUARE(x) + SQUARE(z);

			if (dist < 1000000)
			{
				item->pos.x_pos = boat->pos.x_pos - 1000000 * z / dist;
				item->pos.z_pos = boat->pos.z_pos - 1000000 * x / dist;
			}

			break;
		}
	}
}

static long DoBoatDynamics(long height, long fallspeed, long* ypos)
{
	if (height <= *ypos)
	{
		fallspeed += (height - fallspeed - *ypos) >> 3;

		if (*ypos > height)
			*ypos = height;
	}
	else
	{
		*ypos += fallspeed;

		if (*ypos <= height)
			fallspeed += 6;
		else
		{
			*ypos = height;
			fallspeed = 0;
		}
	}

	return fallspeed;
}

static long BoatDynamics(short item_number)
{
	ITEM_INFO* item;
	BOAT_INFO* boat;
	FLOOR_INFO* floor;
	PHD_VECTOR pos, newPos;
	PHD_VECTOR flPos, frPos, blPos, brPos, fmPos, flPos2, frPos2, blPos2, brPos2, fmPos2;
	long front_left, front_right, back_left, back_right, front_mid, front_left2, front_right2, back_left2, back_right2, front_mid2;
	long slip, shift, h, anim, dx, dz, speed;
	short room_number;

	item = &items[item_number];
	boat = (BOAT_INFO*)item->data;
	item->pos.z_rot -= boat->tilt_angle;

	front_left = TestWaterHeight(item, 750, -300, &flPos);
	front_right = TestWaterHeight(item, 750, 300, &frPos);
	back_left = TestWaterHeight(item, -750, -300, &blPos);
	back_right = TestWaterHeight(item, -750, 300, &brPos);
	front_mid = TestWaterHeight(item, 1000, 0, &fmPos);
	pos.x = item->pos.x_pos;
	pos.y = item->pos.y_pos;
	pos.z = item->pos.z_pos;

	if (blPos.y > back_left)
		blPos.y = back_left;

	if (brPos.y > back_right)
		brPos.y = back_right;

	if (flPos.y > front_left)
		flPos.y = front_left;

	if (frPos.y > front_right)
		frPos.y = front_right;

	if (fmPos.y > front_mid)
		fmPos.y = front_mid;

	item->pos.y_rot += short(boat->extra_rotation + boat->boat_turn);
	boat->tilt_angle = short(6 * boat->boat_turn);

	item->pos.x_pos += (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.z_pos += (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

	if (item->speed < 0)
		boat->prop_rot += 6006;
	else
		boat->prop_rot += 546 * item->speed + 364;

	slip = (30 * phd_sin(item->pos.z_rot)) >> W2V_SHIFT;

	if (!slip && item->pos.z_rot)
		slip = item->pos.z_rot <= 0 ? -1 : 1;

	item->pos.x_pos += (slip * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.z_pos -= (slip * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;

	slip = (10 * phd_sin(item->pos.x_rot)) >> W2V_SHIFT;

	if (!slip && item->pos.x_rot)
		slip = item->pos.x_rot <= 0 ? -1 : 1;

	item->pos.x_pos -= (slip * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.z_pos -= (slip * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

	newPos.x = item->pos.x_pos;
	newPos.z = item->pos.z_pos;
	DoBoatShift(item_number);

	shift = 0;
	back_left2 = TestWaterHeight(item, -750, -300, &blPos2);

	if (back_left2 < blPos.y - 128)
		shift = DoShift(item, &blPos2, &blPos);

	back_right2 = TestWaterHeight(item, -750, 300, &brPos2);

	if (back_right2 < brPos.y - 128)
		shift += DoShift(item, &brPos2, &brPos);

	front_left2 = TestWaterHeight(item, 750, -300, &flPos2);

	if (front_left2 < flPos.y - 128)
		shift += DoShift(item, &flPos2, &flPos);

	front_right2 = TestWaterHeight(item, 750, 300, &frPos2);

	if (front_right2 < frPos.y - 128)
		shift += DoShift(item, &frPos2, &frPos);

	if (!slip)
	{
		front_mid2 = TestWaterHeight(item, 1000, 0, &fmPos2);

		if (front_mid2 < fmPos.y - 128)
			DoShift(item, &fmPos2, &fmPos);
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);

	if (h == NO_HEIGHT)
		h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < item->pos.y_pos - 128)
		DoShift(item, (PHD_VECTOR*)&item->pos, &pos);

	boat->extra_rotation = (short)shift;
	anim = GetCollisionAnim(item, &newPos);

	if (slip || anim)
	{
		dx = item->pos.x_pos - pos.x;
		dz = item->pos.z_pos - pos.z;
		speed = (dx * phd_sin(item->pos.y_rot) + dz * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

		if (lara.skidoo == item_number && item->speed > 115 && speed < item->speed - 10)
		{
			lara_item->hit_points -= item->speed;
			lara_item->hit_status = 1;
			SoundEffect(SFX_LARA_INJURY, &lara_item->pos, SFX_DEFAULT);
			speed >>= 1;
			item->speed >>= 1;
		}

		if (slip)
		{
			if (item->speed <= 120)
				item->speed = (short)speed;
		}
		else if (item->speed > 0 && speed < item->speed)
			item->speed = (short)speed;
		else if (item->speed < 0 && speed > item->speed)
			item->speed = (short)speed;

		if (item->speed < -20)
			item->speed = -20;
	}

	return anim;
}

static void BoatSplash(ITEM_INFO* item, long fallspeed, long h)
{
	splash_setup.x = item->pos.x_pos;
	splash_setup.y = h;
	splash_setup.z = item->pos.z_pos;
	splash_setup.InnerYvel = short(-128 * fallspeed);
	splash_setup.InnerXZoff = 64;
	splash_setup.InnerXZsize = 48;
	splash_setup.InnerYsize = -384;
	splash_setup.InnerXZvel = 160;
	splash_setup.InnerGravity = 128;
	splash_setup.InnerFriction = 7;
	splash_setup.MiddleXZoff = 96;
	splash_setup.MiddleXZsize = 96;
	splash_setup.MiddleYsize = -256;
	splash_setup.MiddleXZvel = 224;
	splash_setup.MiddleYvel = short(-64 * fallspeed);
	splash_setup.MiddleGravity = 72;
	splash_setup.MiddleFriction = 8;
	splash_setup.OuterXZoff = 128;
	splash_setup.OuterXZsize = 128;
	splash_setup.OuterXZvel = 272;
	splash_setup.OuterFriction = 9;
	SetupSplash(&splash_setup);
	SplashCount = 16;
}

static void TriggerBoatMist(long x, long y, long z, long speed, short angle, long snow)
{
	SPARKS* sptr;
	long size;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	if (snow)
	{
		sptr->dR = 255;
		sptr->dG = 255;
		sptr->dB = 255;
	}
	else
	{
		sptr->dR = 64;
		sptr->dG = 64;
		sptr->dB = 64;
	}

	sptr->ColFadeSpeed = (GetRandomControl() & 3) + 4;
	sptr->FadeToBlack = uchar(12 - (snow << 3));
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 20;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0x7F) + ((speed * phd_sin(angle)) >> (W2V_SHIFT + 2)) - 64;
	sptr->Yvel = short(12 * speed);
	sptr->Zvel = (GetRandomControl() & 0x7F) + ((speed * phd_cos(angle)) >> (W2V_SHIFT + 2)) - 64;
	sptr->Friction = 3;

	if (GetRandomControl() & 1)
	{
		sptr->Flags = SF_ALTDEF | SF_ROTATE | SF_DEF | SF_SCALE;
		sptr->RotAng = GetRandomControl() & 0xFFF;

		if (GetRandomControl() & 1)
			sptr->RotAdd = -16 - (GetRandomControl() & 0xF);
		else
			sptr->RotAdd = (GetRandomControl() & 0xF) + 16;
	}
	else
		sptr->Flags = SF_ALTDEF | SF_DEF | SF_SCALE;

	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;

	if (snow)
	{
		sptr->Friction = 0;
		sptr->Scalar = 3;
		sptr->Yvel = -sptr->Yvel >> 5;
		sptr->MaxYvel = 0;
		sptr->Gravity = (GetRandomControl() & 0x1F) + 32;
		size = (GetRandomControl() & 7) + 16;
		sptr->Width = (uchar)size;
		sptr->sWidth = sptr->Width;
		sptr->dWidth = sptr->Width;
		sptr->Height = (uchar)size;
		sptr->sHeight = sptr->Height;
		sptr->dHeight = sptr->Height;
	}
	else
	{
		sptr->Scalar = 4;
		sptr->MaxYvel = 0;
		sptr->Gravity = 0;
		size = (GetRandomControl() & 7) + (speed >> 1) + 16;
		sptr->dWidth = (uchar)size;
		sptr->Width = sptr->dWidth >> 2;
		sptr->sWidth = sptr->dWidth >> 2;
		sptr->dHeight = (uchar)size;
		sptr->sHeight = sptr->dHeight >> 2;
		sptr->Height = sptr->dHeight >> 2;
	}
}

static void DoWake(ITEM_INFO* item, long xoff, long zoff, short rotate)
{
	long s, c, x, y, z;
	long xv[2];
	long zv[2];
	short room_number, angle1, angle2;

	if (WakePts[CurrentStartWake][rotate].life)
		return;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	x = item->pos.x_pos + ((zoff * s + xoff * c) >> W2V_SHIFT);
	y = item->pos.y_pos + 128;
	z = item->pos.z_pos + ((zoff * c - xoff * s) >> W2V_SHIFT);
	room_number = item->room_number;
	GetFloor(x, y, z, &room_number);
	GetWaterHeight(x, y, z, room_number);

	if (item->speed >= 0)
	{
		if (rotate)
		{
			angle1 = item->pos.y_rot + 29120;
			angle2 = item->pos.y_rot + 25480;
		}
		else
		{
			angle1 = item->pos.y_rot - 29120;
			angle2 = item->pos.y_rot - 25480;
		}
	}
	else
	{
		if (rotate)
		{
			angle1 = item->pos.y_rot + 3640;
			angle2 = item->pos.y_rot + 7280;
		}
		else
		{
			angle1 = item->pos.y_rot - 3640;
			angle2 = item->pos.y_rot - 7280;
		}
	}

	xv[0] = (4 * phd_sin(angle1)) >> W2V_SHIFT;
	xv[1] = (10 * phd_sin(angle2)) >> W2V_SHIFT;
	zv[0] = (4 * phd_cos(angle1)) >> W2V_SHIFT;
	zv[1] = (10 * phd_cos(angle2)) >> W2V_SHIFT;
	WakePts[CurrentStartWake][rotate].y = item->pos.y_pos + 32;
	WakePts[CurrentStartWake][rotate].life = 64;

	for (int i = 0; i < 2; i++)
	{
		WakePts[CurrentStartWake][rotate].x[i] = x;
		WakePts[CurrentStartWake][rotate].z[i] = z;
		WakePts[CurrentStartWake][rotate].xvel[i] = (short)xv[i];
		WakePts[CurrentStartWake][rotate].zvel[i] = (short)zv[i];
	}

	if (rotate == 1)
		CurrentStartWake = (CurrentStartWake + 1) & 0x1F;
}

static void UpdateWakeFX()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			if (WakePts[j][i].life)
			{
				WakePts[j][i].life--;
				WakePts[j][i].x[0] += WakePts[j][i].xvel[0];
				WakePts[j][i].z[0] += WakePts[j][i].zvel[0];
				WakePts[j][i].x[1] += WakePts[j][i].xvel[1];
				WakePts[j][i].z[1] += WakePts[j][i].zvel[1];
			}
		}
	}
}

void BoatControl(short item_number)
{
	ITEM_INFO* item;
	BOAT_INFO* boat;
	FLOOR_INFO* floor;
	PHD_3DPOS bubble;
	PHD_VECTOR flPos, frPos, pos;
	long hitWall, driving, no_turn, front_left, front_right, h, wh, x, y, z, leaving;
	short room_number, fallspeed, x_rot, z_rot, ang;

	item = &items[item_number];
	boat = (BOAT_INFO*)item->data;
	no_turn = 1;
	driving = 0;
	hitWall = BoatDynamics(item_number);
	front_left = TestWaterHeight(item, 750, -300, &flPos);
	front_right = TestWaterHeight(item, 750, 300, &frPos);

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	GetCeiling(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (lara.skidoo == item_number)
	{
		TestTriggers(trigger_index, 0);
		TestTriggers(trigger_index, 1);
	}

	boat->water = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);
	wh = boat->water;

	if (lara.skidoo == item_number && lara_item->hit_points > 0)
	{
		if (lara_item->current_anim_state && (lara_item->current_anim_state <= BOAT_MOVING || lara_item->current_anim_state > BOAT_JUMPL))
		{
			driving = 1;
			no_turn = BoatUserControl(item);
		}
	}
	else if (item->speed > 1)
		item->speed--;
	else
		item->speed = 0;

	if (no_turn)
	{
		if (boat->boat_turn < -45)
			boat->boat_turn += 45;
		else if (boat->boat_turn > 45)
			boat->boat_turn -= 45;
		else
			boat->boat_turn = 0;
	}

	item->floor = h - 5;

	if (boat->water == NO_HEIGHT)
		boat->water = h;
	else
		boat->water -= 5;

	boat->left_fallspeed = DoBoatDynamics(front_left, boat->left_fallspeed, &flPos.y);
	boat->right_fallspeed = DoBoatDynamics(front_right, boat->right_fallspeed, &frPos.y);

	fallspeed = item->fallspeed;
	item->fallspeed = (short)DoBoatDynamics(boat->water, item->fallspeed, &item->pos.y_pos);

	if (fallspeed - item->fallspeed > 32 && !item->fallspeed && wh != NO_HEIGHT)
		BoatSplash(item, fallspeed - item->fallspeed, wh);

	h = frPos.y + flPos.y;

	if (h >= 0)
		h >>= 1;
	else
		h = -abs(h) >> 1;

	x_rot = (short)phd_atan(750, item->pos.y_pos - h);
	z_rot = (short)phd_atan(300, h - flPos.y);

	item->pos.x_rot += (x_rot - item->pos.x_rot) >> 1;
	item->pos.z_rot += (z_rot - item->pos.z_rot) >> 1;

	if (!x_rot && abs(item->pos.x_rot) < 4)
		item->pos.x_rot = 0;

	if (!z_rot && abs(item->pos.z_rot) < 4)
		item->pos.z_rot = 0;

	if (lara.skidoo == item_number)
	{
		BoatAnimation(item, hitWall);

		if (room_number != item->room_number)
		{
			ItemNewRoom(item_number, room_number);
			ItemNewRoom(lara.item_number, room_number);
		}

		item->pos.z_rot += boat->tilt_angle;
		lara_item->pos.x_pos = item->pos.x_pos;
		lara_item->pos.y_pos = item->pos.y_pos;
		lara_item->pos.z_pos = item->pos.z_pos;
		lara_item->pos.x_rot = item->pos.x_rot;
		lara_item->pos.y_rot = item->pos.y_rot;
		lara_item->pos.z_rot = item->pos.z_rot;
		AnimateItem(lara_item);

		if (lara_item->hit_points > 0)
		{
			item->anim_number = objects[BOAT].anim_index + lara_item->anim_number - objects[VEHICLE_ANIM].anim_index;
			item->frame_number = lara_item->frame_number + anims[item->anim_number].frame_base - anims[lara_item->anim_number].frame_base;
		}

		camera.target_elevation = -3640;
		camera.target_distance = 2048;
	}
	else
	{
		if (room_number != item->room_number)
			ItemNewRoom(item_number, room_number);

		item->pos.z_rot += boat->tilt_angle;
	}

	boat->pitch += (item->speed - boat->pitch) >> 2;

	if (item->speed > 8)
		SoundEffect(SFX_BOAT_MOVING, &item->pos, 25600 * boat->pitch + 0xD50804);	//uhhh todo figure this out
	else if (driving)
		SoundEffect(SFX_BOAT_IDLE, &item->pos, 25600 * boat->pitch + 0xD50804);

	if (lara.skidoo != item_number)
		return;

	if ((lara_item->current_anim_state == BOAT_JUMPR || lara_item->current_anim_state == BOAT_JUMPL) &&
		lara_item->frame_number == anims[lara_item->anim_number].frame_end)
	{
		if (lara_item->current_anim_state == BOAT_JUMPL)
			lara_item->pos.y_rot -= 0x4000;
		else
			lara_item->pos.y_rot += 0x4000;

		lara_item->anim_number = 77;
		lara_item->frame_number = anims[lara_item->anim_number].frame_base;
		lara_item->current_anim_state = AS_FORWARDJUMP;
		lara_item->goal_anim_state = AS_FORWARDJUMP;
		lara_item->gravity_status = 1;
		lara_item->fallspeed = -40;
		lara_item->speed = 20;
		lara_item->pos.x_rot = 0;
		lara_item->pos.z_rot = 0;
		lara.skidoo = NO_ITEM;

		room_number = lara_item->room_number;
		x = lara_item->pos.x_pos + ((360 * phd_sin(lara_item->pos.y_rot)) >> W2V_SHIFT);
		y = lara_item->pos.y_pos - 90;
		z = lara_item->pos.z_pos + ((360 * phd_cos(lara_item->pos.y_rot)) >> W2V_SHIFT);
		floor = GetFloor(x, y, z, &room_number);

		if (GetHeight(floor, x, y, z) >= y - 256)
		{
			lara_item->pos.x_pos = x;
			lara_item->pos.z_pos = z;

			if (room_number != lara_item->room_number)
				ItemNewRoom(lara.item_number, room_number);
		}

		lara_item->pos.y_pos = y;
		item->anim_number = objects[BOAT].anim_index;
		item->frame_number = anims[item->anim_number].frame_base;
	}

	room_number = item->room_number;
	GetFloor(item->pos.x_pos, item->pos.y_pos + 128, item->pos.z_pos, &room_number);
	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos + 128, item->pos.z_pos, room_number);
	wh = wh <= item->pos.y_pos + 32 && wh != NO_HEIGHT;
	leaving = lara_item->current_anim_state == BOAT_JUMPR || lara_item->current_anim_state == BOAT_JUMPL;

	if (!(wibble & 0xF) && wh && !leaving)
	{
		DoWake(item, -384, 0, 0);
		DoWake(item, 384, 0, 1);
	}

	if (!item->speed || !wh || leaving)
	{
		if (WakeShade)
			WakeShade--;
	}
	else if (WakeShade < 16)
		WakeShade++;

	pos.x = 0;
	pos.y = 0;
	pos.z = -80;
	GetJointAbsPosition(item, &pos, 2);
	room_number = item->room_number;
	floor=GetFloor(pos.x, pos.y, pos.z, &room_number);
	wh = GetWaterHeight(pos.x, pos.y, pos.z, room_number);

	if (!item->speed || wh >= pos.y || wh == NO_HEIGHT)
	{
		h = GetHeight(floor, pos.x, pos.y, pos.z);

		if (pos.y > h && !(room[room_number].flags & ROOM_UNDERWATER))
		{
			for (int i = (GetRandomControl() & 3) + 3; i > 0; i--)
			{
				ang = short(item->pos.y_rot + GetRandomControl() + 0x4000);
				TriggerBoatMist(pos.x, pos.y, pos.z, ((GetRandomControl() & 0xF) + 96) << 4, ang, 1);
			}
		}
	}
	else
	{
		TriggerBoatMist(pos.x, pos.y, pos.z, abs(item->speed), item->pos.y_rot + 0x8000, 0);

		if (!(GetRandomControl() & 1))
		{
			bubble.x_pos = (GetRandomControl() & 0x3F) + pos.x - 32;
			bubble.y_pos = pos.y + (GetRandomControl() & 0xF);
			bubble.z_pos = (GetRandomControl() & 0x3F) + pos.z - 32;
			room_number = item->room_number;
			GetFloor(bubble.x_pos, bubble.y_pos, bubble.z_pos, &room_number);
			CreateBubble(&bubble, room_number, 16, 8);
		}
	}

	UpdateWakeFX();
}
