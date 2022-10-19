#include "../tomb3/pch.h"
#include "cinema.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "../specific/picture.h"
#include "../specific/output.h"
#include "items.h"

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

void inject_cinema(bool replace)
{
	INJECT(0x0041A890, DrawPhaseCinematic, replace);
	INJECT(0x0041A8F0, InitialiseGenPlayer, replace);
	INJECT(0x0041AA40, InitCinematicRooms, replace);
}
