#include "../tomb3/pch.h"
#include "footprnt.h"
#include "lara.h"
#include "control.h"
#include "sound.h"
#include "../newstuff/LaraDraw.h"

static char footsounds[14] = { 0, 5, 3, 2, 1, 9, 9, 4, 6, 9, 3, 9, 4, 6 };

FOOTPRINT FootPrint[32];
long FootPrintNum;

void AddFootprint(ITEM_INFO* item)
{
	FOOTPRINT* print;
	FLOOR_INFO* floor;
	PHD_VECTOR pos;
	short room_num;

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	if (FXType == 0x4000)
		GetLaraMeshPos(&pos, LMX_FOOT_L);
	else
		GetLaraMeshPos(&pos, LMX_FOOT_R);

	room_num = item->room_number;
	floor = GetFloor(pos.x, pos.y, pos.z, &room_num);

	if (floor->fx != 6 && floor->fx != 5 && floor->fx != 9 && floor->fx != 11)
		SoundEffect(footsounds[floor->fx] + SFX_FOOTSTEPS_MUD, &lara_item->pos, SFX_DEFAULT);

	if (floor->fx <= 4 && !OnObject)
	{
		print = &FootPrint[FootPrintNum];
		print->x = pos.x;
		print->y = GetHeight(floor, pos.x, pos.y, pos.z);
		print->z = pos.z;
		print->YRot = item->pos.y_rot;
		print->Active = 512;
		FootPrintNum = (FootPrintNum + 1) & 0x1F;
	}
}
