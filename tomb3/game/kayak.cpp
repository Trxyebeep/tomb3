#include "../tomb3/pch.h"
#include "kayak.h"
#include "objects.h"
#include "control.h"
#include "../specific/init.h"
#include "../3dsystem/phd_math.h"
#include "collide.h"
#include "laraflar.h"
#include "items.h"
#include "../specific/game.h"
#include "effect2.h"
#include "lara.h"
#include "camera.h"
#include "effects.h"
#include "../3dsystem/3d_gen.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "../specific/input.h"
#include "../tomb3/tomb3.h"
#include "../newstuff/LaraDraw.h"

void LaraRapidsDrown()
{
	lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 25;
	lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	lara_item->current_anim_state = EXTRA_RAPIDSDROWN;
	lara_item->goal_anim_state = EXTRA_RAPIDSDROWN;
	lara_item->gravity_status = 0;
	lara_item->hit_points = 0;
	lara_item->fallspeed = 0;
	lara_item->speed = 0;
	AnimateItem(lara_item);
	lara.extra_anim = 1;
	lara.gun_type = LG_UNARMED;
	lara.gun_status = LG_HANDSBUSY;
	lara.hit_direction = -1;
}

void KayakInitialise(short item_number)
{
	ITEM_INFO* item;
	KAYAKINFO* kayak;

	item = &items[item_number];
	kayak = (KAYAKINFO*)game_malloc(sizeof(KAYAKINFO));
	item->data = kayak;
	kayak->Flags = 0;
	kayak->Rot = 0;
	kayak->Vel = 0;
	kayak->FallSpeedR = 0;
	kayak->FallSpeedL = 0;
	kayak->FallSpeedF = 0;
	kayak->OldPos = item->pos;

	for (int i = 0; i < 32; i++)
	{
		WakePts[i][0].life = 0;
		WakePts[i][1].life = 0;
	}
}

static long GetInKayak(short item_number, COLL_INFO* coll)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long dx, dz, dist, h;
	ushort tempang;
	short room_number, ang;

	if (input & IN_ACTION && lara.gun_status == LG_UNARMED && !lara_item->gravity_status)
	{

		item = &items[item_number];
		dx = lara_item->pos.x_pos - item->pos.x_pos;
		dz = lara_item->pos.z_pos - item->pos.z_pos;
		dist = SQUARE(dx) + SQUARE(dz);

		if (dist <= 130000)
		{
			room_number = item->room_number;
			floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
			h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

			if (h > -32000)
			{
				ang = (short)phd_atan(item->pos.z_pos - lara_item->pos.z_pos, item->pos.x_pos - lara_item->pos.x_pos) - item->pos.y_rot;
				tempang = lara_item->pos.y_rot - item->pos.y_rot;

				if (ang > -8190 && ang < 24570)
				{
					if (tempang > 8190 && tempang < 24570)
						return -1;
				}
				else
				{
					if (tempang > 40950 && tempang < 57330)
						return 1;
				}
			}
		}
	}

	return 0;
}

void KayakCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	KAYAKINFO* kayak;
	long lr;

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	lr = GetInKayak(item_number, coll);

	if (!lr)
	{
		coll->enable_baddie_push = 1;
		ObjectCollision(item_number, l, coll);
		return;
	}

	lara.skidoo = item_number;
	item = &items[item_number];

	if (lara.gun_type == LG_FLARE)
	{
		CreateFlare(0);
		undraw_flare_meshes();
		lara.flare_control_left = 0;
		lara.gun_type = LG_UNARMED;
		lara.request_gun_type = LG_UNARMED;
	}

	if (lr > 0)
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 3;
	else
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 28;

	l->frame_number = anims[l->anim_number].frame_base;
	l->current_anim_state = KS_CLIMBIN;
	l->goal_anim_state = KS_CLIMBIN;
	lara.water_status = LARA_ABOVEWATER;
	l->pos.x_pos = item->pos.x_pos;
	l->pos.y_pos = item->pos.y_pos;
	l->pos.z_pos = item->pos.z_pos;
	l->pos.x_rot = 0;
	l->pos.y_rot = item->pos.y_rot;
	l->pos.z_rot = 0;
	l->gravity_status = 0;
	l->speed = 0;
	l->fallspeed = 0;

	if (l->room_number != item->room_number)
		ItemNewRoom(lara.item_number, item->room_number);

	kayak = (KAYAKINFO*)item->data;
	kayak->Water = item->pos.y_pos;
	kayak->Flags = 0;
}

static void DoRipple(ITEM_INFO* item, short xoffset, short zoffset)
{
	RIPPLE_STRUCT* ripple;
	long s, c, x, z, wh;
	short room_number;

	c = phd_cos(item->pos.y_rot);
	s = phd_sin(item->pos.y_rot);
	x = item->pos.x_pos + ((zoffset * s + xoffset * c) >> W2V_SHIFT);
	z = item->pos.z_pos + ((zoffset * c - xoffset * s) >> W2V_SHIFT);
	room_number = item->room_number;
	GetFloor(x, item->pos.y_pos, z, &room_number);
	wh = GetWaterHeight(x, item->pos.y_pos, z, room_number);

	if (wh == NO_HEIGHT)
		return;

	ripple = SetupRipple(x, item->pos.y_pos, z, -2 - (GetRandomControl() & 1), 0);
	ripple->init = 0;
}

static long TestHeight(ITEM_INFO* item, long x, long z, PHD_VECTOR* pos)
{
	FLOOR_INFO* floor;
	long s, c, xs, zs, h;
	short room_number;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	zs = phd_sin(item->pos.z_rot);
	xs = phd_sin(item->pos.x_rot);

	pos->x = item->pos.x_pos + ((x * c + z * s) >> W2V_SHIFT);
	pos->y = (item->pos.y_pos + ((x * zs) >> W2V_SHIFT)) - ((z * xs) >> W2V_SHIFT);
	pos->z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);

	room_number = item->room_number;
	GetFloor(pos->x, pos->y, pos->z, &room_number);
	h = GetWaterHeight(pos->x, pos->y, pos->z, room_number);

	if (h == NO_HEIGHT)
	{
		room_number = item->room_number;
		floor = GetFloor(pos->x, pos->y, pos->z, &room_number);
		h = GetHeight(floor, pos->x, pos->y, pos->z);

		if (h == NO_HEIGHT)
			return h;
	}

	return h - 5;
}

static long CanGetOut(ITEM_INFO* item, long lr)
{
	PHD_VECTOR pos;

	return item->pos.y_pos - TestHeight(item, lr >= 0 ? 768 : -768, 0, &pos) <= 0;
}

static void KayakUserInput(ITEM_INFO* item, ITEM_INFO* l, KAYAKINFO* kayak)
{
	PHD_VECTOR pos;
	short* tmpMesh;
	short frame;
	static char lr;

	if (l->hit_points <= 0 && l->current_anim_state != KS_DEATHIN)
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 5;
		l->frame_number = anims[l->anim_number].frame_base;
		l->current_anim_state = KS_DEATHIN;
		l->goal_anim_state = KS_DEATHIN;
	}

	frame = l->frame_number - anims[l->anim_number].frame_base;

	switch (l->current_anim_state)
	{
	case KS_BACK:

		if (!(input & IN_BACK))
			l->goal_anim_state = KS_POSE;

		if (l->anim_number - objects[VEHICLE_ANIM].anim_index == 2)
		{
			if (frame == 8)
			{
				kayak->Rot += 0x800000;
				kayak->Vel -= 0x180000;
			}

			if (frame == 31)
			{
				kayak->Rot -= 0x800000;
				kayak->Vel -= 0x180000;
			}

			if (frame < 15 && (frame & 1) != 0)
				DoRipple(item, 384, -128);
			else if (frame >= 20 && frame <= 34 && frame & 1)
				DoRipple(item, -384, -128);
		}

		break;

	case KS_POSE:

		if (input & IN_ROLL && !lara.current_active && !lara.current_xvel && !lara.current_zvel)
		{
			if (input & IN_LEFT && CanGetOut(item, -1))
			{
				l->goal_anim_state = KS_JUMPOUT;
				l->required_anim_state = KS_CLIMBOUTL;
			}
			else if (input & IN_RIGHT && CanGetOut(item, 1))
			{
				l->goal_anim_state = KS_JUMPOUT;
				l->required_anim_state = KS_CLIMBOUTR;
			}
		}
		else if (input & IN_FORWARD)
		{
			l->goal_anim_state = KS_RIGHT;
			kayak->Turn = 0;
			kayak->Forward = 1;
		}
		else if (input & IN_BACK)
			l->goal_anim_state = KS_BACK;
		else if (input & IN_LEFT)
		{
			l->goal_anim_state = KS_LEFT;

			if (kayak->Vel)
				kayak->Turn = 0;
			else
				kayak->Turn = 1;

			kayak->Forward = 0;
		}
		else if (input & IN_RIGHT)
		{
			l->goal_anim_state = KS_RIGHT;

			if (kayak->Vel)
				kayak->Turn = 0;
			else
				kayak->Turn = 1;

			kayak->Forward = 0;
		}
		else if (input & IN_LSTEP && (kayak->Vel || lara.current_xvel || lara.current_zvel))
			l->goal_anim_state = KS_TURNL;
		else if (input & IN_RSTEP && (kayak->Vel || lara.current_xvel || lara.current_zvel))
			l->goal_anim_state = KS_TURNR;

		break;

	case KS_LEFT:

		if (!kayak->Forward)
		{
			if (!(input & IN_LEFT))
				l->goal_anim_state = KS_POSE;
		}
		else
		{
			if (!frame)
				lr = 0;

			if (frame == 2 && !(lr & 0x80))
				lr++;
			else if (frame > 2)
				lr &= ~0x80;

			if (input & IN_FORWARD)
			{
				if (input & IN_LEFT)
				{
					if ((lr & ~0x80) >= 2)
						l->goal_anim_state = KS_RIGHT;
				}
				else
					l->goal_anim_state = KS_RIGHT;
			}
			else
				l->goal_anim_state = KS_POSE;
		}

		if (frame == 7)
		{
			if (kayak->Forward)
			{
				kayak->Rot -= 0x800000;

				if (kayak->Rot < -0x1000000)
					kayak->Rot = -0x1000000;
				
				kayak->Vel += 0x180000;
			}
			else if (kayak->Turn)
			{
				kayak->Rot -= 0x1000000;

				if (kayak->Rot < -0x1000000)
					kayak->Rot = -0x1000000;
			}
			else
			{
				kayak->Rot -= 0xC00000;

				if (kayak->Rot < -0xC00000)
					kayak->Rot = -0xC00000;

				kayak->Vel += 0x100000;
			}
		}

		if (frame > 6 && frame < 24 && frame & 1)
			DoRipple(item, -384, -64);

		break;

	case KS_RIGHT:

		if (!kayak->Forward)
		{
			if (!(input & IN_RIGHT))
				l->goal_anim_state = KS_POSE;
		}
		else
		{
			if (!frame)
				lr = 0;

			if (frame == 2 && !(lr & 0x80))
				lr++;
			else if (frame > 2)
				lr &= ~0x80;

			if (input & IN_FORWARD)
			{
				if (input & IN_RIGHT)
				{
					if ((lr & ~0x80) >= 2)
						l->goal_anim_state = KS_LEFT;
				}
				else
					l->goal_anim_state = KS_LEFT;
			}
			else
				l->goal_anim_state = KS_POSE;
		}

		if (frame == 7)
		{
			if (kayak->Forward)
			{
				kayak->Rot += 0x800000;

				if (kayak->Rot > 0x1000000)
					kayak->Rot = 0x1000000;

				kayak->Vel += 0x180000;
			}
			else if (kayak->Turn)
			{
				kayak->Rot += 0x1000000;

				if (kayak->Rot > 0x1000000)
					kayak->Rot = 0x1000000;
			}
			else
			{
				kayak->Rot += 0xC00000;

				if (kayak->Rot > 0xC00000)
					kayak->Rot = 0xC00000;

				kayak->Vel += 0x100000;
			}
		}

		if (frame > 6 && frame < 24 && frame & 1)
			DoRipple(item, 384, -64);

		break;

	case KS_CLIMBIN:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 4 && frame == 24 && !(kayak->Flags & 0x80))
		{
			tmpMesh = lara.mesh_ptrs[HAND_R];
			lara.mesh_ptrs[HAND_R] = meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R];
			meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = tmpMesh;
			l->mesh_bits &= ~0x7F;
			kayak->Flags |= 0x80;
		}

		break;

	case KS_JUMPOUT:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 14 && frame == 27 && kayak->Flags & 0x80)
		{
			tmpMesh = lara.mesh_ptrs[HAND_R];
			lara.mesh_ptrs[HAND_R] = meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R];
			meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = tmpMesh;
			l->mesh_bits |= 0x7F;
			kayak->Flags &= ~0x80;
		}

		l->goal_anim_state = l->required_anim_state;
		break;

	case KS_TURNL:

		if (!(input & IN_LSTEP) || (!kayak->Vel && !lara.current_xvel && !lara.current_zvel))
			l->goal_anim_state = KS_POSE;
		else if (l->anim_number - objects[VEHICLE_ANIM].anim_index == 26)
		{
			if (kayak->Vel >= 0)
			{
				kayak->Rot -= 0x200000;

				if (kayak->Rot < -0x1000000)
					kayak->Rot = -0x1000000;

				kayak->Vel -= 0x8000;

				if (kayak->Vel < 0)
					kayak->Vel = 0;
			}

			if (kayak->Vel < 0)
			{
				kayak->Vel += 0x8000;
				kayak->Rot += 0x200000;

				if (kayak->Vel > 0)
					kayak->Vel = 0;
			}

			if (!(wibble & 3))
				DoRipple(item, -256, -256);
		}

		break;

	case KS_TURNR:

		if (!(input & IN_RSTEP) || (!kayak->Vel && !lara.current_xvel && !lara.current_zvel))
			l->goal_anim_state = KS_POSE;
		else if (l->anim_number - objects[VEHICLE_ANIM].anim_index == 27)
		{
			if (kayak->Vel >= 0)
			{
				kayak->Rot += 0x200000;

				if (kayak->Rot > 0x1000000)
					kayak->Rot = 0x1000000;

				kayak->Vel -= 0x8000;

				if (kayak->Vel < 0)
					kayak->Vel = 0;
			}

			if (kayak->Vel < 0)
			{
				kayak->Vel += 0x8000;
				kayak->Rot -= 0x200000;

				if (kayak->Vel > 0)
					kayak->Vel = 0;
			}

			if (!(wibble & 3))
				DoRipple(item, 256, -256);
		}

		break;

	case KS_CLIMBOUTL:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 24 && frame == 83)
		{
			pos.x = 0;
			pos.y = 350;
			pos.z = 500;
			GetLaraMeshPos(&pos, LMX_HIPS);
			l->pos.x_pos = pos.x;
			l->pos.y_pos = pos.y;
			l->pos.z_pos = pos.z;
			l->pos.x_rot = 0;
			l->pos.y_rot = item->pos.y_rot - 0x4000;
			l->pos.z_rot = 0;
			l->anim_number = ANIM_FASTFALL;
			l->frame_number = anims[ANIM_FASTFALL].frame_base;
			l->current_anim_state = KS_JUMPOUT;
			l->goal_anim_state = KS_JUMPOUT;
			l->gravity_status = 1;
			l->fallspeed = 0;
			lara.gun_status = 0;
			lara.skidoo = NO_ITEM;
		}

		break;

	case KS_CLIMBOUTR:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 32 && frame == 83)
		{
			pos.x = 0;
			pos.y = 350;
			pos.z = 500;
			GetLaraMeshPos(&pos, LMX_HIPS);
			l->pos.x_pos = pos.x;
			l->pos.y_pos = pos.y;
			l->pos.z_pos = pos.z;
			l->pos.x_rot = 0;
			l->pos.y_rot = item->pos.y_rot + 0x4000;
			l->pos.z_rot = 0;
			l->anim_number = ANIM_FASTFALL;
			l->frame_number = anims[ANIM_FASTFALL].frame_base;
			l->current_anim_state = KS_JUMPOUT;
			l->goal_anim_state = KS_JUMPOUT;
			l->gravity_status = 1;
			l->fallspeed = 0;
			lara.gun_status = 0;
			lara.skidoo = NO_ITEM;
		}

		break;
	}

	if (kayak->Vel > 0)
	{
		kayak->Vel -= 0x8000;

		if (kayak->Vel < 0)
			kayak->Vel = 0;
	}
	else if (kayak->Vel < 0)
	{
		kayak->Vel += 0x8000;

		if (kayak->Vel > 0)
			kayak->Vel = 0;
	}

	if (kayak->Vel > 0x380000)
		kayak->Vel = 0x380000;
	else if (kayak->Vel < -0x380000)
		kayak->Vel = -0x380000;

	item->speed = kayak->Vel >> 16;

	if (kayak->Rot < 0)
	{
		kayak->Rot += 0x50000;

		if (kayak->Rot > 0)
			kayak->Rot = 0;
	}
	else
	{
		kayak->Rot -= 0x50000;

		if (kayak->Rot < 0)
			kayak->Rot = 0;
	}
}

static void DoCurrent(ITEM_INFO* item)
{
	long sinkval, angle, speed, xvel, zvel, shifter, absvel;

	if (lara.current_active)
	{
		sinkval = lara.current_active - 1;
		angle = mGetAngle(camera.fixed[sinkval].x, camera.fixed[sinkval].z, lara_item->pos.x_pos, lara_item->pos.z_pos);
		angle = ((angle - 0x4000) >> 4) & 0xFFF;
		speed = camera.fixed[sinkval].data;
		xvel = (speed * rcossin_tbl[angle << 1]) >> 2;
		zvel = (speed * rcossin_tbl[(angle << 1) + 1]) >> 2;
		lara.current_xvel += short((xvel - lara.current_xvel) >> 4);
		lara.current_zvel += short((zvel - lara.current_zvel) >> 4);
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

	item->pos.x_pos += lara.current_xvel >> 8;
	item->pos.z_pos += lara.current_zvel >> 8;
	lara.current_active = 0;
}

static long DoDynamics(long h, long fallspeed, long* y)
{
	long kick;

	if (h <= *y)
	{
		kick = (h - *y) << 2;

		if (kick < -80)
			kick = -80;

		fallspeed += (kick - fallspeed) >> 3;

		if (*y > h)
			*y = h;
	}
	else
	{
		*y += fallspeed;

		if (*y <= h)
			fallspeed += 6;
		else
		{
			*y = h;
			fallspeed = 0;
		}
	}

	return fallspeed;
}

static long DoShift(ITEM_INFO* item, PHD_VECTOR* newPos, PHD_VECTOR* oldPos)
{
	FLOOR_INFO* floor;
	long nX, nZ, oX, oZ, sX, sZ, x, z, h;
	short room_number;

	nX = newPos->x >> WALL_SHIFT;
	nZ = newPos->z >> WALL_SHIFT;
	oX = oldPos->x >> WALL_SHIFT;
	oZ = oldPos->z >> WALL_SHIFT;
	sX = newPos->x & WALL_MASK;
	sZ = newPos->z & WALL_MASK;
	
	if (nX == oX)
	{
		oldPos->x = 0;

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
			item->pos.z_pos += -1 - sZ;
			return newPos->x - item->pos.x_pos;
		}
	}

	if (nZ == oZ)
	{
		oldPos->z = 0;

		if (nX <= oX)
		{
			item->pos.x_pos += WALL_SIZE - sX;
			return newPos->z - item->pos.z_pos;
		}
		else
		{
			item->pos.x_pos += -1 - sX;
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
			z = 1024 - sZ;
	}

	room_number = item->room_number;
	floor = GetFloor(newPos->x, newPos->y, oldPos->z, &room_number);
	h = GetHeight(floor, newPos->x, newPos->y, oldPos->z);

	if (h < oldPos->y - 256)
	{
		if (newPos->x > oldPos->x)
			x = -1 - sX;
		else
			x = 1024 - sX;
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

static long GetCollisionAnim(ITEM_INFO* item, long x, long z)
{
	long s, c, front, side;

	x = item->pos.x_pos - x;
	z = item->pos.z_pos - z;

	if (!x && !z)
		return 0;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	front = (x * s + z * c) >> W2V_SHIFT;
	side = (x * c - z * s) >> W2V_SHIFT;

	if (abs(front) <= abs(side))
	{
		if (side > 0)
			return 3;
		else
			return 4;
	}
	else
	{
		if (front > 0)
			return 1;
		else
			return 2;
	}
}

static void KayakToBackground(ITEM_INFO* item, KAYAKINFO* kayak)
{
	FLOOR_INFO* floor;
	PHD_VECTOR oldpos[9];
	PHD_VECTOR fPos;
	PHD_VECTOR lPos;
	PHD_VECTOR rPos;
	PHD_VECTOR pos;
	GAME_VECTOR kPos;
	long heights[8];
	long rot, front, left, right, diff, oX, oZ, h;
	long dx, dz, s, c, speed;
	short xRot, zRot, room_number;

	rot = 0;
	kayak->OldPos = item->pos;
	heights[0] = TestHeight(item, 0, 1024, oldpos);
	heights[1] = TestHeight(item, -96, 512, &oldpos[1]);
	heights[2] = TestHeight(item, 96, 512, &oldpos[2]);
	heights[3] = TestHeight(item, -128, 128, &oldpos[3]);
	heights[4] = TestHeight(item, 128, 128, &oldpos[4]);
	heights[5] = TestHeight(item, -128, -320, &oldpos[5]);
	heights[6] = TestHeight(item, 128, -320, &oldpos[6]);
	heights[7] = TestHeight(item, 0, -640, &oldpos[7]);

	for (int i = 0; i < 8; i++)
	{
		if (oldpos[i].y > heights[i])
			oldpos[i].y = heights[i];
	}

	oldpos[8].x = item->pos.x_pos;
	oldpos[8].y = item->pos.y_pos;
	oldpos[8].z = item->pos.z_pos;

	front = TestHeight(item, 0, 1024, &fPos);
	left = TestHeight(item, -128, 128, &lPos);
	right = TestHeight(item, 128, 128, &rPos);

	item->pos.y_rot += kayak->Rot >> 16;
	item->pos.x_pos += (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
	item->pos.z_pos += (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;

	DoCurrent(item);
	kayak->FallSpeedL = DoDynamics(left, kayak->FallSpeedL, &lPos.y);
	kayak->FallSpeedR = DoDynamics(right, kayak->FallSpeedR, &rPos.y);
	kayak->FallSpeedF = DoDynamics(front, kayak->FallSpeedF, &fPos.y);
	item->fallspeed = (short)DoDynamics(kayak->Water, item->fallspeed, &item->pos.y_pos);

	diff = (rPos.y + lPos.y) >> 1;
	xRot = (short)phd_atan(1024, item->pos.y_pos - fPos.y);
	zRot = (short)phd_atan(128, diff - lPos.y);
	item->pos.x_rot = xRot;
	item->pos.z_rot = zRot;
	oX = item->pos.x_pos;
	oZ = item->pos.z_pos;
	
	h = TestHeight(item, 0, -640, &pos);

	if (h < oldpos[7].y - 64)
		rot = DoShift(item, &pos, &oldpos[7]);

	h = TestHeight(item, 128, -320, &pos);

	if (h < oldpos[6].y - 64)
		rot += DoShift(item, &pos, &oldpos[6]);

	h = TestHeight(item, -128, -320, &pos);

	if (h < oldpos[5].y - 64)
		rot += DoShift(item, &pos, &oldpos[5]);

	h = TestHeight(item, 128, 128, &pos);

	if (h < oldpos[4].y - 64)
		rot += DoShift(item, &pos, &oldpos[4]);

	h = TestHeight(item, -128, 128, &pos);

	if (h < oldpos[3].y - 64)
		rot += DoShift(item, &pos, &oldpos[3]);

	h = TestHeight(item, 96, 512, &pos);

	if (h < oldpos[2].y - 64)
		rot += DoShift(item, &pos, &oldpos[2]);

	h = TestHeight(item, -96, 512, &pos);

	if (h < oldpos[1].y - 64)
		rot += DoShift(item, &pos, &oldpos[1]);

	h = TestHeight(item, 0, 1024, &pos);

	if (h < oldpos[0].y - 64)
		rot += DoShift(item, &pos, &oldpos[0]);

	item->pos.y_rot += (short)rot;
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);

	if (h == NO_HEIGHT)
		h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < item->pos.y_pos - 64)
		h = DoShift(item, (PHD_VECTOR*)&item->pos, &oldpos[8]);

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);

	if (h == NO_HEIGHT)
	{
		h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

		if (h == NO_HEIGHT)
		{
			kPos.x = kayak->OldPos.x_pos;
			kPos.y = kayak->OldPos.y_pos;
			kPos.z = kayak->OldPos.z_pos;
			kPos.room_number = item->room_number;
			CameraCollisionBounds(&kPos, 256, 0);
			item->pos.x_pos = kPos.x;
			item->pos.y_pos = kPos.y;
			item->pos.z_pos = kPos.z;
			item->room_number = kPos.room_number;
		}
	}

	if (GetCollisionAnim(item, oX, oZ))
	{
		s = phd_sin(item->pos.y_rot);
		c = phd_cos(item->pos.y_rot);
		dx = item->pos.x_pos - oldpos[8].x;
		dz = item->pos.z_pos - oldpos[8].z;
		speed = (dx * s + dz * c) >> W2V_SHIFT;
		speed <<= 8;

		if ((kayak->Vel > 0 && speed < kayak->Vel) || (kayak->Vel < 0 && speed > kayak->Vel))
			kayak->Vel = speed;

		if (kayak->Vel < -0x380000)
			kayak->Vel = -0x380000;
	}
}

static void KayakSplash(ITEM_INFO* item, long fallspeed, long water)
{
	splash_setup.x = item->pos.x_pos;
	splash_setup.y = item->pos.y_pos;
	splash_setup.z = item->pos.z_pos;
	splash_setup.InnerXZoff = 128;
	splash_setup.InnerXZsize = 48;
	splash_setup.InnerYsize = -384;
	splash_setup.InnerXZvel = 160;
	splash_setup.InnerYvel = short(-fallspeed << 5);
	splash_setup.InnerGravity = 128;
	splash_setup.InnerFriction = 7;
	splash_setup.MiddleXZoff = 192;
	splash_setup.MiddleXZsize = 96;
	splash_setup.MiddleYsize = -256;
	splash_setup.MiddleXZvel = 224;
	splash_setup.MiddleYvel = short(-fallspeed << 4);
	splash_setup.MiddleGravity = 72;
	splash_setup.MiddleFriction = 8;
	splash_setup.OuterXZoff = 256;
	splash_setup.OuterXZsize = 128;
	splash_setup.OuterXZvel = 272;
	splash_setup.OuterFriction = -9;
	SetupSplash(&splash_setup);
	SplashCount = 16;
}

static void DoWake(ITEM_INFO* item, long xoff, long zoff, short rotate)
{
	long s, c, x, z, wh;
	long xv[2];
	long zv[2];
	short room_number, angle1, angle2;

	if (WakePts[CurrentStartWake][rotate].life)
		return;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	x = item->pos.x_pos + ((zoff * s + xoff * c) >> W2V_SHIFT);
	z = item->pos.z_pos + ((zoff * c - xoff * s) >> W2V_SHIFT);
	room_number = item->room_number;
	GetFloor(x, item->pos.y_pos, z, &room_number);
	wh = GetWaterHeight(x, item->pos.y_pos, z, room_number);

	if (wh == NO_HEIGHT)
		return;

	if (item->speed >= 0)
	{
		if (rotate)
		{
			angle1 = item->pos.y_rot + 30940;
			angle2 = item->pos.y_rot + 27300;
		}
		else
		{
			angle1 = item->pos.y_rot - 30940;
			angle2 = item->pos.y_rot - 27300;
		}
	}
	else
	{
		if (rotate)
		{
			angle1 = item->pos.y_rot + 1820;
			angle2 = item->pos.y_rot + 5460;
		}
		else
		{
			angle1 = item->pos.y_rot - 1820;
			angle2 = item->pos.y_rot - 5460;
		}
	}

	xv[0] = (4 * phd_sin(angle1)) >> W2V_SHIFT;
	xv[1] = (6 * phd_sin(angle2)) >> W2V_SHIFT;
	zv[0] = (4 * phd_cos(angle1)) >> W2V_SHIFT;
	zv[1] = (6 * phd_cos(angle2)) >> W2V_SHIFT;
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

static void TriggerRapidsMist(long x, long y, long z)
{
	SPARKS* sptr;

	sptr = &sparks[GetFreeSpark()];
	sptr->On = 1;
	sptr->sR = 128;
	sptr->sG = 128;
	sptr->sB = 128;
	sptr->dR = 192;
	sptr->dG = 192;
	sptr->dB = 192;
	sptr->ColFadeSpeed = 2;
	sptr->FadeToBlack = 4;
	sptr->TransType = 2;
	sptr->extras = 0;
	sptr->Life = (GetRandomControl() & 3) + 6;
	sptr->sLife = sptr->Life;
	sptr->Dynamic = -1;
	sptr->x = (GetRandomControl() & 0xF) + x - 8;
	sptr->y = (GetRandomControl() & 0xF) + y - 8;
	sptr->z = (GetRandomControl() & 0xF) + z - 8;
	sptr->Xvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Yvel = (GetRandomControl() & 0xFF) - 128;
	sptr->Zvel = (GetRandomControl() & 0xFF) - 128;
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

	sptr->Scalar = 4;
	sptr->Def = (uchar)objects[EXPLOSION1].mesh_index;
	sptr->Gravity = 0;
	sptr->MaxYvel = 0;
	sptr->dWidth = (GetRandomControl() & 7) + 16;
	sptr->sWidth = sptr->dWidth >> 1;
	sptr->Width = sptr->sWidth;
	sptr->sHeight = sptr->sWidth;
	sptr->Height = sptr->sWidth;
	sptr->dHeight = sptr->dWidth;
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

static void KayakToBaddieCollision(ITEM_INFO* kayak)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	short* door;
	long dx, dy, dz;
	short numroom, item_num, obj_num;
	short roomies[20];

	numroom = 1;
	roomies[0] = kayak->room_number;
	door = room[kayak->room_number].door;

	if (door)
	{
		for (int i = *door++; i > 0; i--, door += 16)
		{
			roomies[numroom] = *door;
			numroom++;
		}
	}

	for (int i = 0; i < numroom; i++)
	{
		for (item_num = room[roomies[i]].item_number; item_num != NO_ITEM; item_num = item->next_item)
		{
			item = &items[item_num];

			if (item->collidable && item->status != ITEM_INVISIBLE)
			{
				obj_num = item->object_number;
				obj = &objects[obj_num];

				if (obj->collision)
				{
					if (obj_num == SPIKES || obj_num == DARTS || obj_num == TEETH_TRAP ||
						obj_num == BLADE && item->current_anim_state != 1 ||
						obj_num == ICICLES && item->current_anim_state != 3)
					{
						dx = kayak->pos.x_pos - item->pos.x_pos;
						dy = kayak->pos.y_pos - item->pos.y_pos;
						dz = kayak->pos.z_pos - item->pos.z_pos;

						if (dx > -2048 && dx < 2048 && dz > -2048 && dz < 2048 && dy > -2048 && dy < 2048)
						{
							if (TestBoundsCollide(item, kayak, 256))
							{
								DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 256, lara_item->pos.z_pos,
									kayak->speed, kayak->pos.y_rot, lara_item->room_number, 3);
								lara_item->hit_points -= 5;
							}
						}
					}
				}
			}
		}
	}
}

long KayakControl()
{
	ITEM_INFO* item;
	KAYAKINFO* kayak;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	long ofs, h, wh, damage, x, y, z;
	short room_number;
	static short MistXPos[10] = { 32, 96, 170, 220, 300, 400, 400, 300,  200, 64 };
	static short MistZPos[10] = { 900, 750, 600, 450, 300, 150, 0, -150, -300, -450 };
	static char cnt = 0;

	item = &items[lara.skidoo];
	kayak = (KAYAKINFO*)item->data;

	if (input & IN_LOOK)
		LookUpDown();

	ofs = item->fallspeed;
	KayakUserInput(item, lara_item, kayak);
	KayakToBackground(item, kayak);
	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
	TestTriggers(trigger_index, 0);
	wh = GetWaterHeight(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, room_number);
	kayak->Water = wh;

	if (wh == NO_HEIGHT)
	{
		wh = h;
		kayak->Water = h;
		kayak->TrueWater = 0;
	}
	else
	{
		kayak->TrueWater = 1;
		kayak->Water = wh - 5;
	}

	damage = ofs - item->fallspeed;

	if (damage > 128 && !item->fallspeed && wh != NO_HEIGHT)
	{
		if (damage > 160)
			lara_item->hit_points -= short((damage - 160) << 3);

		KayakSplash(item, ofs - item->fallspeed, wh);
	}
	
	if (lara.skidoo != NO_ITEM)
	{
		if (item->room_number != room_number)
		{
			ItemNewRoom(lara.skidoo, room_number);
			ItemNewRoom(lara.item_number, room_number);
		}

		lara_item->pos.x_pos = item->pos.x_pos;
		lara_item->pos.y_pos = item->pos.y_pos + 32;
		lara_item->pos.z_pos = item->pos.z_pos;
		lara_item->pos.x_rot = item->pos.x_rot;
		lara_item->pos.y_rot = item->pos.y_rot;
		lara_item->pos.z_rot = item->pos.z_rot >> 1;
		AnimateItem(lara_item);
		item->anim_number = lara_item->anim_number + objects[KAYAK].anim_index - objects[VEHICLE_ANIM].anim_index;
		item->frame_number = lara_item->frame_number + anims[item->anim_number].frame_base - anims[lara_item->anim_number].frame_base;
		camera.target_elevation = -5460;
		camera.target_distance = 2048;
	}

	if (!(wibble & 0xF) && kayak->TrueWater)
	{
		DoWake(item, -128, 0, 0);
		DoWake(item, 128, 0, 1);
	}

	if (wibble & 7 && !kayak->TrueWater && item->fallspeed < 20)
	{
		cnt ^= 1;

		for (int i = cnt; i < 10; i += 2)
		{
			if (GetRandomControl() & 1)
				x = MistXPos[i] >> 1;
			else
				x = -(MistXPos[i] >> 1);

			y = 50;
			z = MistZPos[i];

			phd_PushUnitMatrix();
			phd_SetTrans(0, 0, 0);
			phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

			if (tomb3.kayak_mist)
			{
				pos.x = (phd_mxptr[M00] * x + phd_mxptr[M01] * y + phd_mxptr[M02] * z) >> W2V_SHIFT;
				pos.y = (phd_mxptr[M10] * x + phd_mxptr[M11] * y + phd_mxptr[M12] * z) >> W2V_SHIFT;
				pos.z = (phd_mxptr[M20] * x + phd_mxptr[M21] * y + phd_mxptr[M22] * z) >> W2V_SHIFT;
				TriggerRapidsMist(item->pos.x_pos + pos.x, item->pos.y_pos + pos.y, item->pos.z_pos + pos.z);
			}

			phd_PopMatrix();
		}
	}

	if (item->speed || lara.current_xvel || lara.current_zvel)
	{
		if (WakeShade < 16)
			WakeShade++;
	}
	else if (WakeShade)
		WakeShade--;

	UpdateWakeFX();
	KayakToBaddieCollision(item);

	if (lara.skidoo == NO_ITEM)
		return 0;
	else
		return 1;
}

void KayakDraw(ITEM_INFO* item)
{
	item->pos.y_pos += 32;
	DrawAnimatingItem(item);
	item->pos.y_pos -= 32;
	S_DrawWakeFX(item);
}
