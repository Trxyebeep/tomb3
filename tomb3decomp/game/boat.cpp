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

void InitialiseBoat(short item_number)
{
	ITEM_INFO* item;
	BOAT_INFO* boat;

	item = &items[item_number];
	boat = (BOAT_INFO*)game_malloc(sizeof(BOAT_INFO), 0);
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
	l->current_anim_state = 0;
	l->goal_anim_state = 0;

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
		if (lara_item->current_anim_state != 8)
		{
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 18;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->current_anim_state = 8;
			lara_item->goal_anim_state = 8;
		}
	}
	else if (item->pos.y_pos < boat->water - 128 && item->fallspeed > 0)
	{
		if (lara_item->current_anim_state != 6)
		{
			lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 15;
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->current_anim_state = 6;
			lara_item->goal_anim_state = 6;
		}
	}
	else if (collide)
	{
		if (lara_item->current_anim_state != 5)
		{
			lara_item->anim_number = short(objects[VEHICLE_ANIM].anim_index + collide);
			lara_item->frame_number = anims[lara_item->anim_number].frame_base;
			lara_item->current_anim_state = 5;
			lara_item->goal_anim_state = 5;
		}
	}
	else
	{
		switch (lara_item->current_anim_state)
		{
		case 1:

			if (input & IN_ROLL && !item->speed)
			{
				if (input & (IN_RSTEP | IN_RIGHT) && CanGetOff(item->pos.y_rot + 0x4000))
					lara_item->goal_anim_state = 3;
				else if (input & (IN_LSTEP | IN_LEFT) && CanGetOff(item->pos.y_rot - 0x4000))
					lara_item->goal_anim_state = 4;
			}

			if (item->speed > 0)
				lara_item->goal_anim_state = 2;

			break;

		case 2:

			if (item->speed <= 0)
				lara_item->goal_anim_state = 1;
			else if (input & (IN_RSTEP | IN_RIGHT))
				lara_item->goal_anim_state = 7;
			else if (input & (IN_LSTEP | IN_LEFT))
				lara_item->goal_anim_state = 9;

			break;

		case 6:
			lara_item->goal_anim_state = 2;
			break;

		case 7:

			if (item->speed <= 0)
				lara_item->goal_anim_state = 1;
			else if (!(input & (IN_RSTEP | IN_RIGHT)))
				lara_item->goal_anim_state = 2;

			break;

		case 9:

			if (item->speed <= 0)
				lara_item->goal_anim_state = 1;
			else if (!(input & (IN_LSTEP | IN_LEFT)))
				lara_item->goal_anim_state = 2;

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
	sX = newPos->x & (WALL_SIZE - 1);
	sZ = newPos->z & (WALL_SIZE - 1);

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

void inject_boat(bool replace)
{
	INJECT(0x00411FE0, InitialiseBoat, replace);
	INJECT(0x00412040, BoatCheckGeton, replace);
	INJECT(0x004121B0, BoatCollision, replace);
	INJECT(0x00413CF0, DrawBoat, replace);
	INJECT(0x00412330, BoatUserControl, replace);
	INJECT(0x00412730, CanGetOff, replace);
	INJECT(0x00412500, BoatAnimation, replace);
	INJECT(0x00413290, TestWaterHeight, replace);
	INJECT(0x00413900, DoShift, replace);
}
