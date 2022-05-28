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
#include "laraanim.h"

void LaraRapidsDrown()
{
	lara_item->anim_number = objects[VEHICLE_ANIM].anim_index + 25;
	lara_item->frame_number = anims[lara_item->anim_number].frame_base;
	lara_item->current_anim_state = 12;
	lara_item->goal_anim_state = 12;
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
	kayak = (KAYAKINFO*)game_malloc(sizeof(KAYAKINFO), 0);
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
	l->current_anim_state = 4;
	l->goal_anim_state = 4;
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

	if (l->hit_points <= 0 && l->current_anim_state != 5)
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 5;
		l->frame_number = anims[l->anim_number].frame_base;
		l->current_anim_state = 5;
		l->goal_anim_state = 5;
	}

	frame = l->frame_number - anims[l->anim_number].frame_base;

	switch (l->current_anim_state)
	{
	case 0:

		if (!(input & IN_BACK))
			l->goal_anim_state = 1;

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

	case 1:

		if (input & IN_ROLL && !lara.current_active && !lara.current_xvel && !lara.current_zvel)
		{
			if (input & IN_LEFT && CanGetOut(item, -1))
			{
				l->goal_anim_state = 9;
				l->required_anim_state = 13;
			}
			else if (input & IN_RIGHT && CanGetOut(item, 1))
			{
				l->goal_anim_state = 9;
				l->required_anim_state = 14;
			}
		}
		else if (input & IN_FORWARD)
		{
			l->goal_anim_state = 3;
			kayak->Turn = 0;
			kayak->Forward = 1;
		}
		else if (input & IN_BACK)
			l->goal_anim_state = 0;
		else if (input & IN_LEFT)
		{
			l->goal_anim_state = 2;

			if (kayak->Vel)
				kayak->Turn = 0;
			else
				kayak->Turn = 1;

			kayak->Forward = 0;
		}
		else if (input & IN_RIGHT)
		{
			l->goal_anim_state = 3;

			if (kayak->Vel)
				kayak->Turn = 0;
			else
				kayak->Turn = 1;

			kayak->Forward = 0;
		}
		else if (input & IN_LSTEP && (kayak->Vel || lara.current_xvel || lara.current_zvel))
			l->goal_anim_state = 10;
		else if (input & IN_RSTEP && (kayak->Vel || lara.current_xvel || lara.current_zvel))
			l->goal_anim_state = 11;

		break;

	case 2:

		if (!kayak->Forward)
		{
			if (!(input & IN_LEFT))
				l->goal_anim_state = 1;
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
						l->goal_anim_state = 3;
				}
				else
					l->goal_anim_state = 3;
			}
			else
				l->goal_anim_state = 1;
		}

		if (frame == 7)
		{
			if (kayak->Forward)
			{
				kayak->Rot -= 0x800000;

				if (kayak->Rot < -0x1000000)
					kayak->Rot = -0x1000000;
				
				kayak->Vel =+ 0x180000;
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

	case 3:

		if (!kayak->Forward)
		{
			if (!(input & IN_RIGHT))
				l->goal_anim_state = 1;
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
						l->goal_anim_state = 2;
				}
				else
					l->goal_anim_state = 2;
			}
			else
				l->goal_anim_state = 1;
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

	case 4:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 4 && frame == 24 && !(kayak->Flags & 0x80))
		{
			tmpMesh = lara.mesh_ptrs[HAND_R];
			lara.mesh_ptrs[HAND_R] = meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R];
			meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = tmpMesh;
			l->mesh_bits &= ~0x7F;
			kayak->Flags |= 0x80;
		}

		break;

	case 9:

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

	case 10:

		if (!(input & IN_LSTEP) || (!kayak->Vel && !lara.current_xvel && !lara.current_zvel))
			l->goal_anim_state = 1;
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

	case 11:

		if (!(input & IN_RSTEP) || (!kayak->Vel && !lara.current_xvel && !lara.current_zvel))
			l->goal_anim_state = 1;
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

	case 13:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 24 && frame == 83)
		{
			pos.x = 0;
			pos.y = 350;
			pos.z = 500;
			GetLaraHandAbsPosition(&pos, LARA_HIPS);
			l->pos.x_pos = pos.x;
			l->pos.y_pos = pos.y;
			l->pos.z_pos = pos.z;
			l->pos.x_rot = 0;
			l->pos.y_rot = item->pos.y_rot - 0x4000;
			l->pos.z_rot = 0;
			l->anim_number = ANIM_FASTFALL;
			l->frame_number = anims[ANIM_FASTFALL].frame_base;
			l->current_anim_state = 9;
			l->goal_anim_state = 9;
			l->gravity_status = 1;
			l->fallspeed = 0;
			lara.gun_status = 0;
			lara.skidoo = NO_ITEM;
		}

		break;

	case 14:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 32 && frame == 83)
		{
			pos.x = 0;
			pos.y = 350;
			pos.z = 500;
			GetLaraHandAbsPosition(&pos, LARA_HIPS);
			l->pos.x_pos = pos.x;
			l->pos.y_pos = pos.y;
			l->pos.z_pos = pos.z;
			l->pos.x_rot = 0;
			l->pos.y_rot = item->pos.y_rot + 0x4000;
			l->pos.z_rot = 0;
			l->anim_number = ANIM_FASTFALL;
			l->frame_number = anims[ANIM_FASTFALL].frame_base;
			l->current_anim_state = 9;
			l->goal_anim_state = 9;
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
		absvel = ABS(lara.current_xvel);

		if (absvel > 16)
			shifter = 4;
		else if (absvel > 8)
			shifter = 3;
		else
			shifter = 2;

		lara.current_xvel -= lara.current_xvel >> shifter;

		if (ABS(lara.current_xvel) < 4)
			lara.current_xvel = 0;

		absvel = ABS(lara.current_zvel);

		if (absvel > 16)
			shifter = 4;
		else if (absvel > 8)
			shifter = 3;
		else
			shifter = 2;

		lara.current_zvel -= lara.current_zvel >> shifter;

		if (ABS(lara.current_zvel) < 4)
			lara.current_zvel = 0;

		if (!lara.current_xvel && !lara.current_zvel)
			return;
	}

	item->pos.x_pos += lara.current_xvel >> 8;
	item->pos.z_pos += lara.current_zvel >> 8;
	lara.current_active = 0;
}

void inject_kayak(bool replace)
{
	INJECT(0x0043B390, LaraRapidsDrown, replace);
	INJECT(0x0043B410, KayakInitialise, replace);
	INJECT(0x0043B620, GetInKayak, replace);
	INJECT(0x0043B4C0, KayakCollision, replace);
	INJECT(0x0043D4A0, DoRipple, replace);
	INJECT(0x0043C840, TestHeight, replace);
	INJECT(0x0043D550, CanGetOut, replace);
	INJECT(0x0043CC00, KayakUserInput, replace);
	INJECT(0x0043C6B0, DoCurrent, replace);
}
