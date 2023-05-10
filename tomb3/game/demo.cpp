#include "../tomb3/pch.h"
#include "demo.h"
#include "gameflow.h"
#include "items.h"
#include "control.h"
#include "setup.h"
#include "../specific/winmain.h"
#include "text.h"
#include "../specific/game.h"
#include "../specific/smain.h"
#include "../specific/input.h"
#include "savegame.h"
#include "lara.h"
#include "inventry.h"

ulong* demoptr;
long democount;
long demo_loaded;
long DemoPlay;

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

long StartDemo(long level)
{
	TEXTSTRING* demoText;
	START_INFO* s;
	START_INFO start;
	long lp;
	static long demo_level;
	char buf[64];

	if (level < 0 && !gameflow.num_demos)
		return EXIT_TO_TITLE;

	if (level >= 0)
		demo_level = level;
	else
	{
		if (demo_level >= gameflow.num_demos)
			demo_level = 0;

		level = GF_valid_demos[demo_level];
		demo_level++;
	}

	s = &savegame.start[level];
	memcpy(&start, s, sizeof(START_INFO));
	s->available = 1;
	s->pistol_ammo = 1000;
	s->gun_status = LG_ARMLESS;
	s->gun_type = LG_PISTOLS;
	title_loaded = 0;

	if (!InitialiseLevel(level, 3))
		return EXITGAME;

	level_complete = 0;

	if (!demo_loaded)
	{
		sprintf(buf, "Level '%s' has no demo data!", GF_levelfilenames[level]);
		S_ExitSystem(buf);
	}

	LoadLaraDemoPos();
	demoText = T_Print(0, -16, 0, GF_PCStrings[PCSTR_DEMOMODE]);
	T_BottomAlign(demoText, 1);
	T_FlashText(demoText, 1, 20);
	T_CentreH(demoText, 1);
	Inventory_DemoMode = 1;
	lp = GameLoop(1);
	Inventory_DemoMode = 0;
	T_RemovePrint(demoText);
	memcpy(s, &start, sizeof(START_INFO));
	return lp;
}
