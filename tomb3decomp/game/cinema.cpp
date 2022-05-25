#include "../tomb3/pch.h"
#include "cinema.h"
#include "draw.h"
#include "../specific/draweffects.h"
#include "../specific/picture.h"
#include "../specific/output.h"

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

void inject_cinema(bool replace)
{
	INJECT(0x0041A890, DrawPhaseCinematic, replace);
}
