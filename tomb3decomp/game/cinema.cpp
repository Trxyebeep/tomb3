#include "../tomb3/pch.h"
#include "cinema.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "../specific/picture.h"
#include "../specific/output.h"
#include "items.h"
#include "sphere.h"
#include "laramisc.h"
#include "objects.h"
#include "../specific/game.h"
#include "effect2.h"

long DrawPhaseCinematic()
{
	camera_underwater = 0;
	DrawRooms(camera.pos.room_number);
	S_DrawSparks();
	S_FadePicture();
	S_OutputPolyList();
	camera.number_frames = S_DumpScreen();

	if (S_DumpCine() == 1)
		camera.number_frames = 2;

	S_AnimateTextures(camera.number_frames);
	return camera.number_frames;
}

void InitialiseGenPlayer(short item_number)
{
	ITEM_INFO* item;

	AddActiveItem(item_number);
	item = &items[item_number];
	item->pos.y_rot = 0;
	item->dynamic_light = 0;
}

void InitCinematicRooms()
{
	ROOM_INFO* r;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (r->flipped_room >= 0)
			room[r->flipped_room].bound_active = 1;

		r->flags |= ROOM_OUTSIDE;
	}

	number_draw_rooms = 0;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (!r->bound_active)
		{
			draw_rooms[number_draw_rooms] = i;
			number_draw_rooms++;
		}
	}
}

long GetCinematicRoom(long x, long y, long z)
{
	ROOM_INFO* r;

	for (int i = 0; i < number_rooms; i++)
	{
		r = &room[i];

		if (x >= r->x + WALL_SIZE && x < (r->y_size << WALL_SHIFT) + r->x - WALL_SIZE &&
			y >= r->maxceiling && y <= r->minfloor &&
			z >= r->z + WALL_SIZE && z < (r->x_size << WALL_SHIFT) + r->z - WALL_SIZE)
			return i;
	}

	return -1;
}

void LaraControlCinematic(short item_number)
{
	ITEM_INFO* item;
	PHD_VECTOR pos;
	short room_number;

	item = &items[item_number];
	item->pos.y_rot = camera.target_angle;
	item->pos.x_pos = camera.pos.x;
	item->pos.y_pos = camera.pos.y;
	item->pos.z_pos = camera.pos.z;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	GetJointAbsPosition(item, &pos, 0);
	room_number = (short)GetCinematicRoom(pos.x, pos.y, pos.z);

	if (room_number != -1 && item->room_number != room_number)
		ItemNewRoom(item_number, room_number);

	AnimateLara(item);
}

void InitialisePlayer1(short item_number)
{
	objects[LARA].draw_routine = dummyDrawFunc;
	objects[LARA].control = LaraControlCinematic;
	objects[LARA].shadow_size = 0;
	AddActiveItem(item_number);
	lara_item = &items[item_number];

	camera.pos.x = lara_item->pos.x_pos;
	camera.pos.y = lara_item->pos.y_pos;
	camera.pos.z = lara_item->pos.z_pos;
	lara_item->pos.y_rot = 0;
	camera.target_angle = 0;
	camera.pos.room_number = lara_item->room_number;
	lara_item->dynamic_light = 0;
	lara_item->anim_number = 0;
	lara_item->frame_number = 0;
	lara_item->current_anim_state = 0;
	lara_item->goal_anim_state = 0;
	lara.hit_direction = -1;
}

void UpdateLaraGuns()
{
	FX_INFO* fx;
	PHD_VECTOR* pos;
	PHD_VECTOR lpos;
	PHD_VECTOR rpos;
	long flash, r;
	short room_number, fx_num;

	if (!lara.left_arm.flash_gun && !lara.right_arm.flash_gun && !SmokeCountL && !SmokeCountR)
		return;

	flash = 0;
	r = 0;

	lpos.x = -12;
	lpos.y = 48;
	lpos.z = 40;
	GetJointAbsPosition(lara_item, &lpos, HAND_L);

	rpos.x = 8;
	rpos.y = 48;
	rpos.z = 40;
	GetJointAbsPosition(lara_item, &rpos, HAND_R);

	pos = &lpos;

	if (lara.left_arm.flash_gun)
	{
		flash = lara.left_arm.flash_gun;
		lara.left_arm.flash_gun--;
	}
	else if (lara.right_arm.flash_gun)
	{
		flash = lara.right_arm.flash_gun;
		pos = &rpos;
		r = 1;
		lara.right_arm.flash_gun--;
	}

	if (flash)
	{
		if (flash == 3)
		{
			room_number = (short)GetCinematicRoom(pos->x, pos->y, pos->z);
			fx_num = CreateEffect(room_number);

			if (fx_num != NO_ITEM)
			{
				fx = &effects[fx_num];
				fx->pos.x_pos = pos->x;
				fx->pos.y_pos = pos->y;
				fx->pos.z_pos = pos->z;
				fx->room_number = room_number;
				fx->pos.x_rot = 0;
				fx->pos.y_rot = 0;
				fx->pos.z_rot = GetRandomControl();
				fx->speed = (GetRandomControl() & 0x1F) + 16;
				fx->object_number = GUNSHELL;
				fx->fallspeed = -48 - (GetRandomControl() & 7);
				fx->frame_number = objects[GUNSHELL].mesh_index;
				fx->shade = 0x4210;
				fx->counter = (GetRandomControl() & 1) + 1;
			}

			if (r)
				SmokeCountR = 28;
			else
				SmokeCountL = 28;
		}

		TriggerDynamic(pos->x + (GetRandomControl() & 0xFF) - 128,
			pos->y - (GetRandomControl() & 0x7F) + 63,
			pos->z + (GetRandomControl() & 0xFF) - 128,
			10, (GetRandomControl() & 7) + 24, (GetRandomControl() & 3) + 16, GetRandomControl() & 7);
	}

	if (SmokeCountL)
	{
		TriggerGunSmoke(lpos.x, lpos.y, lpos.z, 0, 0, 0, 0, LG_PISTOLS, SmokeCountL);
		SmokeCountL--;
	}

	if (SmokeCountR)
	{
		TriggerGunSmoke(rpos.x, rpos.y, rpos.z, 0, 0, 0, 0, LG_PISTOLS, SmokeCountR);
		SmokeCountR--;
	}
}

void inject_cinema(bool replace)
{
	INJECT(0x0041A890, DrawPhaseCinematic, replace);
	INJECT(0x0041A8F0, InitialiseGenPlayer, replace);
	INJECT(0x0041AA40, InitCinematicRooms, replace);
	INJECT(0x0041AC20, GetCinematicRoom, replace);
	INJECT(0x0041AB80, LaraControlCinematic, replace);
	INJECT(0x0041AAD0, InitialisePlayer1, replace);
	INJECT(0x0041AE10, UpdateLaraGuns, replace);
}
