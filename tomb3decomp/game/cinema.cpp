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

void inject_cinema(bool replace)
{
	INJECT(0x0041A890, DrawPhaseCinematic, replace);
	INJECT(0x0041A8F0, InitialiseGenPlayer, replace);
	INJECT(0x0041AA40, InitCinematicRooms, replace);
	INJECT(0x0041AC20, GetCinematicRoom, replace);
	INJECT(0x0041AB80, LaraControlCinematic, replace);
	INJECT(0x0041AAD0, InitialisePlayer1, replace);
}
