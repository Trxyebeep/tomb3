#include "../tomb3/pch.h"
#include "game.h"
#include "../game/text.h"
#include "input.h"
#include "output.h"
#include "picture.h"
#include "../game/inventry.h"
#include "../game/invfunc.h"

static long rand_1 = 0xD371F947;
static long rand_2 = 0xD371F947;

long GetRandomControl()
{
	rand_1 = 0x41C64E6D * rand_1 + 0x3039;
	return (rand_1 >> 10) & 0x7FFF;
}

void SeedRandomControl(long seed)
{
	rand_1 = seed;
}

long GetRandomDraw()
{
	rand_2 = 0x41C64E6D * rand_2 + 0x3039;
	return (rand_2 >> 10) & 0x7FFF;
}

void SeedRandomDraw(long seed)
{
	rand_2 = seed;
}

long GameStats(long level_num, long type)
{
	uchar* pS;
	long totallevels, numsecrets, num;

	savegame.start[CurrentLevel].timer = savegame.timer;
	savegame.start[CurrentLevel].ammo_used = savegame.ammo_used;
	savegame.start[CurrentLevel].ammo_hit = savegame.ammo_hit;
	savegame.start[CurrentLevel].distance_travelled = savegame.distance_travelled;
	savegame.start[CurrentLevel].kills = savegame.kills;
	savegame.start[CurrentLevel].secrets_found = savegame.secrets;
	savegame.start[CurrentLevel].health_used = savegame.health_used;
	T_InitPrint();

	while (input & IN_SELECT)
		S_UpdateInput();

	do
	{
		S_InitialisePolyList(0);
		S_UpdateInput();

		if (reset_flag)
			input = IN_SELECT;

		inputDB = GetDebouncedInput(input);
		ShowEndStatsText();
		T_DrawText();
		S_OutputPolyList();
		S_DumpScreen();
	}
	while (!(input & IN_SELECT));

	if (!reset_flag && type != 5)
	{
		totallevels = gameflow.num_levels - gameflow.num_demos - 1;
		numsecrets = 0;
		num = 0;
		pS = &savegame.start[0].secrets_found;

		for (int i = 0; i < totallevels; i++, pS += sizeof(START_INFO))
		{
			numsecrets += (*pS & 1) + ((*pS >> 1) & 1) + ((*pS >> 2) & 1) + ((*pS >> 3) & 1) +
				((*pS >> 4) & 1) + ((*pS >> 5) & 1) + ((*pS >> 6) & 1) + ((*pS >> 7) & 1);
			num += LevelSecrets[i];
		}

		if (numsecrets >= num - 1)
		{
			GF_BonusLevelEnabled = 1;
			FadePictureDown(32);
			FreePictureTextures(CurPicTexIndices);
			FreePictureTextures(OldPicTexIndices);
			LoadPicture("pix\\theend2.bmp", App.lpPictureBuffer, 1);
			nLoadedPictures = 1;
		}
	}

	return 0;
}

void SortOutAdventureSave(long world)
{
	if (savegame.WorldRequired == 1)
		savegame.AfterIndia = world;

	if (savegame.WorldRequired == 2)
		savegame.AfterSPacific = world;

	if (savegame.WorldRequired == 3)
		savegame.AfterLondon = world;

	if (savegame.WorldRequired == 4)
		savegame.AfterNevada = world;

	savegame.AfterAdventureSave = 0;
	savegame.WorldRequired = 0;
}

void inject_sgame(bool replace)
{
	INJECT(0x004841F0, GetRandomControl, replace);
	INJECT(0x00484210, SeedRandomControl, replace);
	INJECT(0x00484220, GetRandomDraw, replace);
	INJECT(0x00484240, SeedRandomDraw, replace);
	INJECT(0x00484010, GameStats, replace);
	INJECT(0x004838E0, SortOutAdventureSave, replace);
}
