#include "../tomb3/pch.h"
#include "demo.h"
#include "gameflow.h"
#include "items.h"
#include "control.h"

void GetDemoInput()
{
	if (democount < 3608)
		input = demoptr[democount];
	else
		input = -1;

	if (input != -1)
		democount++;
}

long DoDemoSequence(long level)
{
	static long l;

	if (level < 0 && !gameflow.num_demos)
		return EXIT_TO_TITLE;

	if (level >= 0)
		l = level;
	else
	{
		if (l >= gameflow.num_demos)
			l = 0;

		level = GF_valid_demos[l];
		l++;
	}

	return GF_DoLevelSequence(level, 3);
}

void LoadLaraDemoPos()
{
	FLOOR_INFO* floor;
	short room_number;

	lara_item->pos.x_pos = demoptr[0];
	lara_item->pos.y_pos = demoptr[1];
	lara_item->pos.z_pos = demoptr[2];
	lara_item->pos.x_rot = *((short*)demoptr + 6);
	lara_item->pos.y_rot = *((short*)demoptr + 8);
	lara_item->pos.z_rot = *((short*)demoptr + 10);
	room_number = *((short*)demoptr + 12);

	if (lara_item->room_number != room_number)
		ItemNewRoom(lara.item_number, room_number);

	floor = GetFloor(lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos, &room_number);
	lara_item->floor = GetHeight(floor, lara_item->pos.x_pos, lara_item->pos.y_pos, lara_item->pos.z_pos);
	lara.last_gun_type = *((short*)demoptr + 14);
	democount += 8;
}

void inject_demo(bool replace)
{
	INJECT(0x00423970, GetDemoInput, replace);
	INJECT(0x004236B0, DoDemoSequence, replace);
	INJECT(0x004238A0, LoadLaraDemoPos, replace);
}
