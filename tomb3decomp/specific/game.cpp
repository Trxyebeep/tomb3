#include "../tomb3/pch.h"
#include "game.h"
#include "../game/text.h"
#include "input.h"
#include "output.h"
#include "picture.h"
#include "../game/inventry.h"
#include "../game/invfunc.h"
#include "../game/gameflow.h"
#include "specific.h"
#include "display.h"
#include "texture.h"
#include "file.h"
#include "../game/objects.h"
#include "../game/savegame.h"
#include "../game/setup.h"
#include "frontend.h"
#include "../game/camera.h"
#include "../game/control.h"
#include "../game/draw.h"

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

long Level2World(long level)
{
	if (level <= LV_INDIABOSS)
		return 1;

	if (level <= LV_PACBOSS)
		return 2;

	if (level <= LV_OFFICE)
		return 3;

	if (level <= LV_AREA51)
		return 4;

	return 5;
}

long World2Level(long world)
{
	if (world == 1)
		return LV_JUNGLE - 1;

	if (world == 2)
		return LV_SHORE - 1;

	if (world == 3)
		return LV_ROOFTOPS - 1;

	if (world == 4)
		return LV_DESERT - 1;

	return LV_ANTARC - 1;
}

long LevelStats(long level)
{
	long ret, s, world;
	char buf[32];

	ret = 0;
	savegame.start[level].timer = savegame.timer;
	savegame.start[level].ammo_used = savegame.ammo_used;
	savegame.start[level].ammo_hit = savegame.ammo_hit;
	savegame.start[level].distance_travelled = savegame.distance_travelled;
	savegame.start[level].kills = savegame.kills;
	savegame.start[level].secrets_found = savegame.secrets;
	savegame.start[level].health_used = savegame.health_used;
	s = savegame.timer / 30;
	sprintf(buf, "%02d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);
	S_CDPlay(gameflow.stats_track, 1);
	TempVideoAdjust(HiResFlag, 1.0);
	T_InitPrint();

	if (!GF_PlayingFMV || CurrentLevel == LV_ANTARC || CurrentLevel == LV_STPAULS)
		CreateMonoScreen();
	else
	{
		DXTextureSetGreyScale(1);
		LoadPicture(GF_picfilenames[GF_LoadingPic], App.lpPictureBuffer, 1);
		FadePictureUp(32);
		DXTextureSetGreyScale(0);
	}

	GF_PlayingFMV = 0;

	while (input & IN_SELECT)
		S_UpdateInput();

	do
	{
		S_InitialisePolyList(0);
		S_UpdateInput();
		DrawMonoScreen(0, 0, 0);

		if (reset_flag)
			input = IN_SELECT;

		inputDB = GetDebouncedInput(input);
		ShowStatsText(buf, 0);
		T_DrawText();
		S_OutputPolyList();
		S_DumpScreen();
	} while (!(input & IN_SELECT));

	if (level != gameflow.num_levels - gameflow.num_demos - 1)
		S_LoadLevelFile(GF_titlefilenames[0], 0, 6);

	world = Level2World(level);

	if (level == LV_INDIABOSS)
	{
		if (savegame.IndiaComplete)
		{
			if (savegame.AfterIndia)
				CurrentLevel = World2Level(savegame.AfterIndia);
			else
				ret = world;
		}
		else
		{
			savegame.IndiaComplete = 1;
			savegame.AfterAdventureSave = 1;
			savegame.WorldRequired = world;
			ret = world;
			CreateStartInfo(24);
		}
	}
	else if (level == LV_PACBOSS)
	{
		if (savegame.SPacificComplete)
		{
			if (savegame.AfterSPacific)
				CurrentLevel = World2Level(savegame.AfterSPacific);
			else
				ret = world;
		}
		else
		{
			savegame.SPacificComplete = 1;
			savegame.AfterAdventureSave = 1;
			savegame.WorldRequired = world;
			ret = world;
			CreateStartInfo(24);
		}
	}
	else if (level == LV_OFFICE)
	{
		if (savegame.LondonComplete)
		{
			if (savegame.AfterLondon)
				CurrentLevel = World2Level(savegame.AfterLondon);
			else
				ret = world;
		}
		else
		{
			savegame.LondonComplete = 1;
			savegame.AfterAdventureSave = 1;
			savegame.WorldRequired = world;
			ret = world;
			CreateStartInfo(24);
		}
	}
	else if (level == LV_AREA51)
	{
		if (savegame.NevadaComplete)
		{
			if (savegame.AfterNevada)
				CurrentLevel = World2Level(savegame.AfterNevada);
			else
				ret = world;
		}
		else
		{
			savegame.NevadaComplete = 1;
			savegame.AfterAdventureSave = 1;
			savegame.WorldRequired = world;
			ret = world;
			CreateStartInfo(24);
		}
	}
	else if (level == LV_CHAMBER && !savegame.AntarcticaComplete)
	{
		savegame.bonus_flag = 1;

		for (int i = 1; i < gameflow.num_levels; i++)
			ModifyStartInfo(i);

		savegame.AfterAdventureSave = 0;
		savegame.AntarcticaComplete = 1;
		savegame.current_level = LV_JUNGLE;
	}
	else
	{
		if (savegame.WorldRequired)
		{
			if (world == 1 && savegame.IndiaComplete ||
				world == 2 && savegame.SPacificComplete ||
				world == 3 && savegame.LondonComplete ||
				world == 4 && savegame.NevadaComplete)
				savegame.AfterAdventureSave = 1;
			else
				SortOutAdventureSave(world);
		}

		if (level == LV_ANTARC)
		{
			Inv_RemoveItem(ICON_PICKUP1_ITEM);
			Inv_RemoveItem(ICON_PICKUP2_ITEM);
			Inv_RemoveItem(ICON_PICKUP3_ITEM);
			Inv_RemoveItem(ICON_PICKUP4_ITEM);
		}

		savegame.current_level = short(level + 1);
		CreateStartInfo(level + 1);
	}

	if (level == LV_STPAULS)
	{
		savegame.bonus_flag = 1;

		for (int i = 1; i < gameflow.num_levels; i++)
			ModifyStartInfo(i);
	}

	if (!ret)
	{
		FadePictureDown(32);
		TempVideoRemove();
	}

	return ret;
}

void GetValidLevelsList(REQUEST_INFO* req)
{
	RemoveAllReqItems(req);

	for (int i = 1; i < gameflow.num_levels; i++)
		AddRequesterItem(req, GF_Level_Names[i], R_CENTRE, 0, 0);
}

void GetSavedGamesList(REQUEST_INFO* req)
{
	SetPCRequesterSize(req, 10, -32);

	if (req->selected >= req->vis_lines)
		req->line_offset = req->selected - req->vis_lines + 1;

	memcpy(RequesterFlags1, SaveGameReqFlags1, sizeof(RequesterFlags1));
	memcpy(RequesterFlags2, SaveGameReqFlags2, sizeof(RequesterFlags2));
}

void DisplayCredits()
{
	char buf[64];

	strcpy(buf, "pix\\credit0?.bmp");
	memset(&buf[17], 0, sizeof(buf) - 17);
	S_UnloadLevelFile();

	if (!InitialiseLevel(0, 0))
		return;

	S_StartSyncedAudio(121);
	LoadPicture("pix\\theend.bmp", App.lpPictureBuffer, 1);
	FadePictureUp(32);
	S_Wait(300, 0);
	FadePictureDown(32);

	for (int i = 1; i < 10; i++)
	{
		buf[11] = i + '0';
		LoadPicture(buf, App.lpPictureBuffer, 1);
		FadePictureUp(32);
		S_Wait(300, 0);
		FadePictureDown(32);
	}

	LoadPicture("pix\\theend2.bmp", App.lpPictureBuffer, 1);
	FadePictureUp(32);
}

long LevelCompleteSequence()
{
	return EXIT_TO_TITLE;
}

long S_FrontEndCheck(SAVEGAME_INFO* pData, long nBytes)
{
	HANDLE handle;
	ulong read;
	long num;
	char name[80];
	char ntxt[16];

	Init_Requester(&Load_Game_Requester);
	SavedGames = 0;

	for (int i = 0; i < 16; i++)
	{
		wsprintf(name, "savegame.%d", i);
		handle = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (handle == INVALID_HANDLE_VALUE)
		{
			AddRequesterItem(&Load_Game_Requester, GF_PCStrings[PCSTR_SAVESLOT], 0, 0, 0);
			saved_levels[i] = 0;
		}
		else
		{
			ReadFile(handle, name, 75, &read, 0);
			ReadFile(handle, &num, sizeof(long), &read, 0);
			CloseHandle(handle);
			wsprintf(ntxt, "%d", num);
			AddRequesterItem(&Load_Game_Requester, name, R_LEFTALIGN, ntxt, R_RIGHTALIGN);

			if (num > save_counter)
			{
				save_counter = num;
				Load_Game_Requester.selected = i;
			}

			saved_levels[i] = 0;
			SavedGames++;
		}
	}

	memcpy(SaveGameReqFlags1, RequesterFlags1, sizeof(SaveGameReqFlags1));
	memcpy(SaveGameReqFlags2, RequesterFlags2, sizeof(SaveGameReqFlags2));
	save_counter++;
	return 1;
}

long S_LoadGame(LPVOID data, long size, long slot)
{
	HANDLE file;
	ulong bytes;
	long value;
	char buffer[80];

	wsprintf(buffer, "savegame.%d", slot);
	file = CreateFile(buffer, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (file != INVALID_HANDLE_VALUE)
	{
		ReadFile(file, buffer, 75, &bytes, 0);
		ReadFile(file, &value, sizeof(long), &bytes, 0);
		ReadFile(file, data, size, &bytes, 0);
		CloseHandle(file);
		return 1;
	}

	return 0;
}

long S_SaveGame(LPVOID data, long size, long slot)
{
	HANDLE file;
	ulong bytes;
	char buffer[80], counter[16];

	wsprintf(buffer, "savegame.%d", slot);
	file = CreateFile(buffer, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (file != INVALID_HANDLE_VALUE)
	{
		wsprintf(buffer, "%s", GF_Level_Names[savegame.current_level]);
		WriteFile(file, buffer, 75, &bytes, 0);
		WriteFile(file, &save_counter, 4, &bytes, 0);
		WriteFile(file, data, size, &bytes, 0);
		CloseHandle(file);

		wsprintf(counter, "%d", save_counter);
		ChangeRequesterItem(&Load_Game_Requester, slot, buffer, R_LEFTALIGN, counter, R_RIGHTALIGN);
		save_counter++;
		SavedGames++;
		saved_levels[slot] = 1;
		return 1;
	}

	return 0;
}

ulong mGetAngle(long x, long z, long x1, long z1)
{
	long dx, dz, octant, swap, angle;

	dx = x1 - x;
	dz = z1 - z;

	if (!dx && !dz)
		return 0;

	octant = 0;

	if (dx < 0)
	{
		octant = 4;
		dx = -dx;
	}

	if (dz < 0)
	{
		octant += 2;
		dz = -dz;
	}

	if (dz > dx)
	{
		octant++;
		swap = dx;
		dx = dz;
		dz = swap;
	}

	while (short(dz) != dz)
	{
		dx >>= 1;
		dz >>= 1;
	}

	angle = phdtan2[octant] + phdtantab[(dz << 11) / dx];

	if (angle < 0)
		angle = -angle;

	return -angle & 0xFFFF;
}

long GameLoop(long demo_mode)
{
	long lp;

	overlay_flag = 1;
	InitialiseCamera();
	noinput_count = 0;

	if (demo_mode)
		GnGameMode = GAMEMODE_IN_DEMO;
	else
		GnGameMode = GAMEMODE_IN_GAME;

	lp = ControlPhase(1, demo_mode);

	while (lp == STARTGAME)
	{
		if (GtWindowClosed)
			lp = EXITGAME;
		else
			lp = ControlPhase(DrawPhaseGame(), demo_mode);
	}

	if (lp != 1)		//1 means level complete
		S_FadeToBlack();

	GnGameMode = GAMEMODE_NOT_IN_GAME;
	S_SoundStopAllSamples();
	S_CDStop();

	if (Option_Music_Volume)
		S_CDVolume(25 * Option_Music_Volume + 5);

	return lp;
}

long StartGame(long level, long type)
{
	long result;

	if (type == 1 || type == 2 || type == 3)
		CurrentLevel = level;

	if (type != 2)
		ModifyStartInfo(level);

	title_loaded = 0;

	if (type != 2)
		InitialiseLevelFlags();

	if (!InitialiseLevel(level, type))
	{
		CurrentLevel = 0;
		return EXITGAME;
	}

	result = GameLoop(0);

	if (result == EXIT_TO_TITLE || result == STARTDEMO)
		return result;

	if (result == EXITGAME)
	{
		CurrentLevel = 0;
		return result;
	}

	if (level_complete)
	{
		if (!gameflow.demoversion || !gameflow.singlelevel)
		{
			if (CurrentLevel != LV_GYM)
			{
				S_FadeInInventory(1);
				result = CurrentLevel | LEVELCOMPLETE;
			}
			else
			{
				//empty function call here
				result = EXIT_TO_TITLE;
			}
		}
		else
			result = EXIT_TO_TITLE;

		return result;
	}

	if (!Inventory_Chosen)
		return EXIT_TO_TITLE;

	if (!Inventory_ExtraData[0])
	{
		S_LoadGame(&savegame, sizeof(SAVEGAME_INFO), Inventory_ExtraData[1]);
		return Inventory_ExtraData[1] | STARTSAVEDGAME;
	}

	if (Inventory_ExtraData[0] != 1)
		return EXIT_TO_TITLE;

	if (gameflow.play_any_level)
		return Inventory_ExtraData[1] + 1;
	else
		return LV_JUNGLE;
}

void inject_sgame(bool replace)
{
	INJECT(0x004841F0, GetRandomControl, replace);
	INJECT(0x00484210, SeedRandomControl, replace);
	INJECT(0x00484220, GetRandomDraw, replace);
	INJECT(0x00484240, SeedRandomDraw, replace);
	INJECT(0x00484010, GameStats, replace);
	INJECT(0x004838E0, SortOutAdventureSave, replace);
	INJECT(0x00483FA0, Level2World, replace);
	INJECT(0x00483FE0, World2Level, replace);
	INJECT(0x00483B60, LevelStats, replace);
	INJECT(0x00484250, GetValidLevelsList, replace);
	INJECT(0x004842A0, GetSavedGamesList, replace);
	INJECT(0x004842F0, DisplayCredits, replace);
	INJECT(0x00483B50, LevelCompleteSequence, replace);
	INJECT(0x00484410, S_FrontEndCheck, replace);
	INJECT(0x004846A0, S_LoadGame, replace);
	INJECT(0x00484580, S_SaveGame, replace);
	INJECT(0x00483860, mGetAngle, replace);
	INJECT(0x00483AA0, GameLoop, replace);
	INJECT(0x00483960, StartGame, replace);
}
