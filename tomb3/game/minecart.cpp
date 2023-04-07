#include "../tomb3/pch.h"
#include "minecart.h"
#include "../specific/init.h"
#include "collide.h"
#include "sphere.h"
#include "control.h"
#include "laraflar.h"
#include "../specific/game.h"
#include "objects.h"
#include "../specific/specific.h"
#include "../3dsystem/phd_math.h"
#include "effects.h"
#include "sound.h"
#include "lara.h"
#include "items.h"
#include "../specific/input.h"
#include "camera.h"
#include "effect2.h"
#include "../newstuff/LaraDraw.h"

void MineCartInitialise(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	item->data = game_malloc(sizeof(CARTINFO));
}

static long GetInMineCart(ITEM_INFO* item, ITEM_INFO* l, COLL_INFO* coll)
{
	FLOOR_INFO* floor;
	long dx, dz, dist, h;
	short room_number;

	if (!(input & IN_ACTION) || lara.gun_status != LG_ARMLESS || l->gravity_status ||
		!TestBoundsCollide(item, l, coll->radius) || !TestCollision(item, l))
		return 0;

	dx = l->pos.x_pos - item->pos.x_pos;
	dz = l->pos.z_pos - item->pos.z_pos;
	dist = SQUARE(dx) + SQUARE(dz);

	if (dist > 200000)
		return 0;

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	h = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (h < -32000)
		return 0;

	return 1;
}

void MineCartCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item;
	CARTINFO* cart;
	short ang;

	if (l->hit_points < 0 || lara.skidoo != NO_ITEM)
		return;

	item = &items[item_number];

	if (!GetInMineCart(item, l, coll))
		return ObjectCollision(item_number, l, coll);

	lara.skidoo = item_number;

	if (lara.gun_type == LG_FLARE)
	{
		CreateFlare(0);
		undraw_flare_meshes();
		lara.flare_control_left = 0;
		lara.gun_type = LG_UNARMED;
		lara.request_gun_type = LG_UNARMED;
	}

	lara.gun_status = LG_HANDSBUSY;
	ang = short(mGetAngle(item->pos.x_pos, item->pos.z_pos, l->pos.x_pos, l->pos.z_pos) - item->pos.y_rot);

	if (ang > -0x1FFE && ang < 0x5FFA)
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 46;
	else
		l->anim_number = objects[VEHICLE_ANIM].anim_index;

	l->frame_number = anims[l->anim_number].frame_base;
	l->current_anim_state = 0;
	l->goal_anim_state = 0;
	l->pos = item->pos;

	cart = (CARTINFO*)item->data;
	cart->Flags = 0;
	cart->Speed = 0;
	cart->YVel = 0;
	cart->Gradient = 0;
	S_CDPlay(12, 0);
}

static long CanGetOut(long lr)
{
	ITEM_INFO* item;
	FLOOR_INFO* floor;
	long x, y, z, h, c;
	short ang, room_number;

	item = &items[lara.skidoo];

	if (lr < 0)
		ang = item->pos.y_rot + 0x4000;
	else
		ang = item->pos.y_rot - 0x4000;

	x = item->pos.x_pos - ((330 * phd_sin(ang)) >> W2V_SHIFT);
	y = item->pos.y_pos;
	z = item->pos.z_pos - ((330 * phd_cos(ang)) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);

	if (height_type != BIG_SLOPE && height_type != DIAGONAL && h != NO_HEIGHT && abs(h) > 512)
	{
		c = GetCeiling(floor, x, y, z);

		if (c - item->pos.y_pos <= -762 && h - c >= 762)
			return 1;
	}

	return 0;
}

static void CartToBaddieCollision(ITEM_INFO* cart)
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FLOOR_INFO* floor;
	short* doors;
	long dx, dy, dz;
	short roomies[20];
	short room_count, item_number, frame, room_number;

	room_count = 1;
	roomies[0] = cart->room_number;
	doors = room[cart->room_number].door;

	if (doors)
	{
		for (int i = *doors++; i > 0; i--, doors += 16)
		{
			roomies[room_count] = *doors;
			room_count++;
		}
	}

	for (int i = 0; i < room_count; i++)
	{
		for (item_number = room[roomies[i]].item_number; item_number != NO_ITEM; item_number = item->next_item)
		{
			item = &items[item_number];

			if (!item->collidable || item->status == ITEM_INVISIBLE)
				continue;
			
			obj = &objects[item->object_number];

			if (obj->collision && (obj->intelligent || item->object_number == ANIMATING2))
			{
				dx = cart->pos.x_pos - item->pos.x_pos;
				dy = cart->pos.y_pos - item->pos.y_pos;
				dz = cart->pos.z_pos - item->pos.z_pos;

				if (dx > -2048 && dx < 2048 && dz > -2048 && dz < 2048 && dy > -2048 && dy < 2048 && TestBoundsCollide(item, cart, 256))
				{
					if (item->object_number == ANIMATING2)
					{
						if (item->frame_number == anims[item->anim_number].frame_base && lara_item->current_anim_state == 18 &&
							lara_item->anim_number == objects[VEHICLE_ANIM].anim_index + 6)
						{
							frame = lara_item->frame_number - anims[lara_item->anim_number].frame_base;

							if (frame >= 12 && frame <= 22)
							{
								SoundEffect(SFX_SPANNER, &item->pos, SFX_ALWAYS);
								room_number = item->room_number;
								floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
								GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
								TestTriggers(trigger_index, 1);
								item->frame_number++;
							}
						}

						continue;
					}

					DoLotsOfBlood(item->pos.x_pos, cart->pos.y_pos - 256, item->pos.z_pos, cart->speed, cart->pos.y_rot, item->room_number, 3);
					item->hit_points = 0;
				}
			}
		}
	}
}

static short GetCollision(ITEM_INFO* item, short ang, long dist, short* c)
{
	FLOOR_INFO* floor;
	long x, y, z, h;
	short room_number;

	x = item->pos.x_pos + ((dist * phd_sin(ang)) >> W2V_SHIFT);
	y = item->pos.y_pos - 762;
	z = item->pos.z_pos + ((dist * phd_cos(ang)) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(x, y, z, &room_number);
	h = GetHeight(floor, x, y, z);
	*c = (short)GetCeiling(floor, x, y, z);

	if (h != NO_HEIGHT)
		h -= item->pos.y_pos;

	return (short)h;
}

static long TestHeight(ITEM_INFO* item, long x, long z)
{
	PHD_VECTOR pos;
	FLOOR_INFO* floor;
	long s, c, h;
	short room_number;

	s = phd_sin(item->pos.y_rot);
	c = phd_cos(item->pos.y_rot);
	pos.x = item->pos.x_pos + ((x * c + z * s) >> W2V_SHIFT);
	pos.y = (item->pos.y_pos + ((x * phd_sin(item->pos.z_rot)) >> W2V_SHIFT)) - ((z * phd_sin(item->pos.x_rot)) >> W2V_SHIFT);
	pos.z = item->pos.z_pos + ((z * c - x * s) >> W2V_SHIFT);
	room_number = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, &room_number);
	h = GetHeight(floor, pos.x, pos.y, pos.z);
	return h;
}

static void DoUserInput(ITEM_INFO* item, ITEM_INFO* l, CARTINFO* cart)
{
	PHD_VECTOR pos;
	COLL_INFO coll;
	short* tmp;
	long damage;
	short h, c;

	switch (l->current_anim_state)
	{
	case 0:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 5 && l->frame_number == anims[l->anim_number].frame_base + 20 &&
			!cart->Flags)
		{
			tmp = lara.mesh_ptrs[HAND_R];
			lara.mesh_ptrs[HAND_R] = meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R];
			meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = tmp;
			cart->Flags |= 1;
		}

		break;

	case 1:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 7)
		{
			if (l->frame_number == anims[objects[VEHICLE_ANIM].anim_index + 7].frame_base + 20 && cart->Flags & 1)
			{
				tmp = lara.mesh_ptrs[HAND_R];
				lara.mesh_ptrs[HAND_R] = meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R];
				meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = tmp;
				cart->Flags &= ~1;
			}

			if (cart->Flags & 8)
				l->goal_anim_state = 3;
			else
				l->goal_anim_state = 2;
		}

		break;

	case 2:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 1 && l->frame_number == anims[l->anim_number].frame_end)
		{
			pos.x = 0;
			pos.y = 640;
			pos.z = 0;
			GetLaraMeshPos(&pos, LMX_HIPS);

			l->pos.x_pos = pos.x;
			l->pos.y_pos = pos.y;
			l->pos.z_pos = pos.z;
			l->pos.x_rot = 0;
			l->pos.y_rot = item->pos.y_rot + 0x4000;
			l->pos.z_rot = 0;
			l->anim_number = 11;
			l->frame_number = anims[VEHICLE_ANIM].frame_base;
			l->current_anim_state = 2;
			l->goal_anim_state = 2;
			lara.skidoo = NO_ITEM;
			lara.gun_status = LG_ARMLESS;
		}

		break;

	case 3:

		if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 47 && l->frame_number == anims[l->anim_number].frame_end)
		{
			pos.x = 0;
			pos.y = 640;
			pos.z = 0;
			GetLaraMeshPos(&pos, LMX_HIPS);

			l->pos.x_pos = pos.x;
			l->pos.y_pos = pos.y;
			l->pos.z_pos = pos.z;
			l->pos.x_rot = 0;
			l->pos.y_rot = item->pos.y_rot - 0x4000;
			l->pos.z_rot = 0;
			l->anim_number = 11;
			l->frame_number = anims[VEHICLE_ANIM].frame_base;
			l->current_anim_state = 2;
			l->goal_anim_state = 2;
			lara.skidoo = NO_ITEM;
			lara.gun_status = LG_ARMLESS;
		}

		break;

	case 4:

		if (!(cart->Flags & 0x10))
		{
			SoundEffect(SFX_MINE_CART_CLUNK_START, &item->pos, SFX_ALWAYS);
			cart->StopDelay = 64;
			cart->Flags |= 0x10;
		}

		if (input & IN_ROLL && cart->Flags & 0x20)
		{
			if (input & IN_LEFT && CanGetOut(-1))
			{
				l->goal_anim_state = 1;
				cart->Flags &= ~8;
			}
			else if (input & IN_RIGHT && CanGetOut(1))
			{
				l->goal_anim_state = 1;
				cart->Flags |= 8;
			}
		}

		if (input & IN_DUCK)
			l->goal_anim_state = 5;
		else if (cart->Speed > 32)
			l->goal_anim_state = 6;

		break;

	case 5:

		if (input & IN_ACTION)
			l->goal_anim_state = 18;
		else if (input & IN_JUMP)
			l->goal_anim_state = 11;
		else if (!(input & IN_DUCK))
			l->goal_anim_state = 4;

		break;

	case 6:

		if (input & IN_ACTION)
			l->goal_anim_state = 18;
		else if (input & IN_DUCK)
			l->goal_anim_state = 5;
		else if (input & IN_JUMP)
			l->goal_anim_state = 11;
		else if (cart->Speed == 32 || cart->Flags & 0x20)
			l->goal_anim_state = 4;
		else if (cart->Gradient < -128)
			l->goal_anim_state = 12;
		else if (cart->Gradient > 128)
			l->goal_anim_state = 13;
		else if (input & IN_LEFT)
			l->goal_anim_state = 9;
		else if (input & IN_RIGHT)
			l->goal_anim_state = 7;

		break;

	case 7:

		if (input & IN_ACTION)
			l->goal_anim_state = 18;
		else if (input & IN_DUCK)
			l->goal_anim_state = 5;
		else if (input & IN_JUMP)
			l->goal_anim_state = 11;

		if (!(input & IN_RIGHT))
			l->goal_anim_state = 6;

		break;

	case 9:

		if (input & IN_ACTION)
			l->goal_anim_state = 18;
		else if (input & IN_DUCK)
			l->goal_anim_state = 5;
		else if (input & IN_JUMP)
			l->goal_anim_state = 11;

		if (!(input & IN_LEFT))
			l->goal_anim_state = 6;

		break;

	case 11:
		l->goal_anim_state = 19;
		break;

	case 12:

		if (input & IN_ACTION)
			l->goal_anim_state = 18;
		else if (input & IN_DUCK)
			l->goal_anim_state = 5;
		else if (input & IN_JUMP)
			l->goal_anim_state = 11;
		else if (cart->Gradient > -128)
			l->goal_anim_state = 6;

		break;

	case 13:

		if (input & IN_ACTION)
			l->goal_anim_state = 18;
		else if (input & IN_DUCK)
			l->goal_anim_state = 5;
		else if (input & IN_JUMP)
			l->goal_anim_state = 11;
		else if (cart->Gradient < 128)
			l->goal_anim_state = 6;

		break;

	case 14:
		camera.target_elevation = -8190;
		camera.target_distance = 2048;
		h = GetCollision(item, item->pos.y_rot, 512, &c);

		if (h > -256 && h < 256)
		{
			if (!(wibble & 7))
				SoundEffect(SFX_QUAD_FRONT_IMPACT, &item->pos, SFX_ALWAYS);

			item->pos.x_pos += 128 * phd_sin(item->pos.y_rot) >> W2V_SHIFT;
			item->pos.z_pos += 128 * phd_cos(item->pos.y_rot) >> W2V_SHIFT;
		}
		else if (l->anim_number == objects[VEHICLE_ANIM].anim_index + 30)
		{
			cart->Flags |= 0x40;
			l->hit_points = -1;
		}

		break;

	case 16:
		camera.target_elevation = -4550;
		camera.target_distance = 4096;
		break;

	case 17:

		if (l->hit_points <= 0 && l->frame_number == anims[objects[VEHICLE_ANIM].anim_index + 34].frame_base + 28)
		{
			l->frame_number = anims[objects[VEHICLE_ANIM].anim_index + 34].frame_base + 28;
			cart->Flags = cart->Flags & ~0x50 | 0x40;
			item->speed = 0;
			cart->Speed = 0;
		}

		break;

	case 18:
		l->goal_anim_state = 6;
		break;

	case 19:

		if (input & IN_DUCK)
		{
			l->goal_anim_state = 5;
			StopSoundEffect(SFX_MINE_CART_SREECH_BRAKE);
		}
		else if (!(input & IN_JUMP) || cart->Flags & 0x20)
		{
			l->goal_anim_state = 6;
			StopSoundEffect(SFX_MINE_CART_SREECH_BRAKE);
		}
		else
		{
			cart->Speed -= 1536;
			SoundEffect(SFX_MINE_CART_SREECH_BRAKE, &l->pos, SFX_ALWAYS);
		}

		break;
	}

	if (lara.skidoo != NO_ITEM && !(cart->Flags & 0x40))
	{
		AnimateItem(l);
		item->anim_number = l->anim_number + objects[MINECART].anim_index - objects[VEHICLE_ANIM].anim_index;
		item->frame_number = l->frame_number + anims[item->anim_number].frame_base - anims[l->anim_number].frame_base;
	}

	if (l->current_anim_state == 14 || l->current_anim_state == 16 || l->hit_points <= 0)
		return;

	if (item->pos.z_rot > 4096 || item->pos.z_rot < -4096)
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 31;
		l->frame_number = anims[l->anim_number].frame_base;
		l->current_anim_state = 14;
		l->goal_anim_state = 14;
		cart->Flags = cart->Flags & 0x4F | 0xA0;
		item->speed = 0;
		cart->Speed = 0;
		return;
	}

	h = GetCollision(item, item->pos.y_rot, 512, &c);

	if (h < -512)
	{
		l->anim_number = objects[VEHICLE_ANIM].anim_index + 23;
		l->frame_number = anims[l->anim_number].frame_base;
		l->current_anim_state = 16;
		l->goal_anim_state = 16;
		cart->Flags = cart->Flags & ~0xB0 | 0xA0;
		item->speed = 0;
		cart->Speed = 0;
		l->hit_points = -1;
		return;
	}

	if (l->current_anim_state != 5 && l->current_anim_state != 17)
	{
		coll.radius = 100;
		coll.quadrant = ushort(item->pos.y_rot + 0x2000) / 0x4000;

		if (CollideStaticObjects(&coll, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, item->room_number, 768))
		{
			l->anim_number = objects[VEHICLE_ANIM].anim_index + 34;
			l->frame_number = anims[l->anim_number].frame_base;
			l->current_anim_state = 17;
			l->goal_anim_state = 17;
			DoLotsOfBlood(l->pos.x_pos, l->pos.y_pos - 768, l->pos.z_pos, item->speed, item->pos.y_rot, l->room_number, 3);

			damage = 25 * ((ushort)cart->Speed >> 11);

			if (damage < 20)
				damage = 20;

			l->hit_points -= (short)damage;
			return;
		}
	}

	if (h > 576 && !cart->YVel)
		cart->YVel = -1024;

	CartToBaddieCollision(item);
}

static void MoveCart(ITEM_INFO* item, ITEM_INFO* l, CARTINFO* cart)
{
	long x, z;
	ushort rot, quad, deg;
	short ang;

	if (cart->StopDelay)
		cart->StopDelay--;

	if (lara.MineL && lara.MineR && !cart->StopDelay && ((item->pos.x_pos & 0x380) == 512 || (item->pos.z_pos & 0x380) == 512))
	{
		if (cart->Speed < 0xF000)
		{
			cart->Flags |= 0x30;
			item->speed = 0;
			cart->Speed = 0;
			return;
		}

		cart->StopDelay = 16;
	}

	if ((lara.MineL || lara.MineR) && !(lara.MineL && lara.MineR) && !cart->StopDelay && !(cart->Flags & 6))
	{
		rot = ushort((lara.MineL << 2) | ((ushort)item->pos.y_rot >> 14));

		switch (rot)
		{
		case 0:
			cart->TurnX = (item->pos.x_pos + 4096) & ~WALL_MASK;
			cart->TurnZ = item->pos.z_pos & ~WALL_MASK;
			break;

		case 1:
			cart->TurnX = item->pos.x_pos & ~WALL_MASK;
			cart->TurnZ = (item->pos.z_pos - 4096) | WALL_MASK;
			break;

		case 2:
			cart->TurnX = (item->pos.x_pos - 4096) | WALL_MASK;
			cart->TurnZ = item->pos.z_pos | WALL_MASK;
			break;

		case 3:
			cart->TurnX = item->pos.x_pos | WALL_MASK;
			cart->TurnZ = (item->pos.z_pos + 4096) & ~WALL_MASK;
			break;

		case 4:
			cart->TurnX = (item->pos.x_pos - 4096) | WALL_MASK;
			cart->TurnZ = item->pos.z_pos & ~WALL_MASK;
			break;

		case 5:
			cart->TurnX = item->pos.x_pos & ~WALL_MASK;
			cart->TurnZ = (item->pos.z_pos + 4096) & ~WALL_MASK;
			break;

		case 6:
			cart->TurnX = (item->pos.x_pos + 4096) & ~WALL_MASK;
			cart->TurnZ = item->pos.z_pos | WALL_MASK;
			break;

		case 7:
			cart->TurnX = item->pos.x_pos | WALL_MASK;
			cart->TurnZ = (item->pos.z_pos - 4096) | WALL_MASK;
			break;
		}

		ang = mGetAngle(item->pos.x_pos, item->pos.z_pos, cart->TurnX, cart->TurnZ) & 0x3FFF;

		if (rot >= 4 && ang)
			ang = 0x4000 - ang;

		cart->TurnRot = item->pos.y_rot;
		cart->TurnLen = ang;
		cart->Flags |= lara.MineL ? 2 : 4;
	}

	if (cart->Speed < 2560)
		cart->Speed = 2560;

	cart->Speed += -4 * cart->Gradient;
	item->speed = short(cart->Speed >> 8);

	if (item->speed < 32)
	{
		item->speed = 32;
		StopSoundEffect(SFX_MINE_CART_TRACK_LOOP);

		if (cart->YVel)
			StopSoundEffect(SFX_MINE_CART_PULLY_LOOP);
		else
			SoundEffect(SFX_MINE_CART_PULLY_LOOP, &item->pos, SFX_ALWAYS);
	}
	else
	{
		StopSoundEffect(SFX_MINE_CART_PULLY_LOOP);

		if (cart->YVel)
			StopSoundEffect(SFX_MINE_CART_TRACK_LOOP);
		else
			SoundEffect(SFX_MINE_CART_TRACK_LOOP, &item->pos, (item->speed << 15) + 0x1000000 | SFX_SETPITCH | SFX_ALWAYS);
	}

	if (cart->Flags & 6)
	{
		cart->TurnLen += 3 * item->speed;

		if (cart->TurnLen > 0x3FFC)
		{
			if (cart->Flags & 2)
				item->pos.y_rot = cart->TurnRot - 0x4000;
			else
				item->pos.y_rot = cart->TurnRot + 0x4000;

			cart->Flags &= ~6;
		}
		else
		{
			if (cart->Flags & 2)
				item->pos.y_rot = cart->TurnRot - cart->TurnLen;
			else
				item->pos.y_rot = cart->TurnRot + cart->TurnLen;
		}

		if (cart->Flags & 6)
		{
			quad = (ushort)item->pos.y_rot >> W2V_SHIFT;
			deg = item->pos.y_rot & 0x3FFF;

			switch (quad)
			{
			case 0:
				x = -phd_cos(deg);
				z = phd_sin(deg);
				break;

			case 1:
				x = phd_sin(deg);
				z = phd_cos(deg);
				break;

			case 2:
				x = phd_cos(deg);
				z = -phd_sin(deg);
				break;

			default:
				x = -phd_sin(deg);
				z = -phd_cos(deg);
				break;
			}

			if (cart->Flags & 2)
			{
				x = -x;
				z = -z;
			}

			item->pos.x_pos = cart->TurnX + ((3584 * x) >> W2V_SHIFT);
			item->pos.z_pos = cart->TurnZ + ((3584 * z) >> W2V_SHIFT);
		}
	}
	else
	{
		item->pos.x_pos += (item->speed * phd_sin(item->pos.y_rot)) >> W2V_SHIFT;
		item->pos.z_pos += (item->speed * phd_cos(item->pos.y_rot)) >> W2V_SHIFT;
	}

	cart->MidPos = TestHeight(item, 0, 0);

	if (!cart->YVel)
	{
		cart->FrontPos = TestHeight(item, 0, 256);
		cart->Gradient = short(cart->MidPos - cart->FrontPos);
		item->pos.y_pos = cart->MidPos;
	}
	else if (item->pos.y_pos > cart->MidPos)
	{
		if (cart->YVel > 0)
			SoundEffect(SFX_QUAD_FRONT_IMPACT, &item->pos, SFX_ALWAYS);

		item->pos.y_pos = cart->MidPos;
		cart->YVel = 0;
	}
	else
	{
		cart->YVel += 1025;

		if (cart->YVel > 0x3F00)
			cart->YVel = 0x3F00;

		item->pos.y_pos += cart->YVel >> 8;
	}

	item->pos.x_rot = cart->Gradient << 5;
	ang = item->pos.y_rot & 0x3FFF;

	if (cart->Flags & 6)
	{
		if (cart->Flags & 4)
			item->pos.z_rot = -(item->speed * ang) >> 9;
		else
			item->pos.z_rot = (item->speed * (0x4000 - ang)) >> 9;
	}
	else
		item->pos.z_rot -= item->pos.z_rot >> 3;
}

long MineCartControl()
{
	ITEM_INFO* item;
	ITEM_INFO* l;
	CARTINFO* cart;
	FLOOR_INFO* floor;
	short room_number;

	l = lara_item;
	item = &items[lara.skidoo];
	cart = (CARTINFO*)item->data;
	DoUserInput(item, l, cart);

	if (cart->Flags & 0x10)
		MoveCart(item, l, cart);

	if (lara.skidoo != -1)
	{
		l->pos.x_pos = item->pos.x_pos;
		l->pos.y_pos = item->pos.y_pos;
		l->pos.z_pos = item->pos.z_pos;
		l->pos.x_rot = item->pos.x_rot;
		l->pos.y_rot = item->pos.y_rot;
		l->pos.z_rot = item->pos.z_rot;
	}

	room_number = item->room_number;
	floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
	GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);

	if (room_number != item->room_number)
	{
		ItemNewRoom(lara.skidoo, room_number);
		ItemNewRoom(lara.item_number, room_number);
	}

	TestTriggers(trigger_index, 0);

	if (!(cart->Flags & 0x80))
	{
		camera.target_elevation = -8190;
		camera.target_distance = 2048;
	}

	return lara.skidoo != NO_ITEM;
}
