#include "../tomb3/pch.h"
#include "gameflow.h"
#include "../specific/file.h"
#include "invfunc.h"
#include "objects.h"
#include "health.h"
#include "../specific/frontend.h"
#include "../specific/game.h"
#include "setup.h"
#include "cinema.h"
#include "savegame.h"
#include "inventry.h"
#include "../specific/picture.h"
#include "demo.h"
#include "../specific/smain.h"
#include "control.h"
#include "lara.h"
#if (DIRECT3D_VERSION >= 0x900)
#include "../newstuff/Picture2.h"
#endif

GAMEFLOW_INFO gameflow;

short* GF_level_sequence_list[24];
short GF_Offsets[200];
short GF_valid_demos[24];
short GF_CDtracks[16];
char GF_Description[256];
char GF_Add2InvItems[30];
char GF_SecretInvItems[30];

char** GF_picfilenames;
char** GF_fmvfilenames;
char** GF_titlefilenames;
char** GF_levelfilenames;
char** GF_cutscenefilenames;
char** GF_Level_Names;
char** GF_GameStrings;
char** GF_PCStrings;
char** GF_Pickup1Strings;
char** GF_Pickup2Strings;
char** GF_Puzzle1Strings;
char** GF_Puzzle2Strings;
char** GF_Puzzle3Strings;
char** GF_Puzzle4Strings;
char** GF_Key1Strings;
char** GF_Key2Strings;
char** GF_Key3Strings;
char** GF_Key4Strings;

char* GF_picfilenames_buffer;
char* GF_fmvfilenames_buffer;
char* GF_titlefilenames_buffer;
char* GF_levelfilenames_buffer;
char* GF_cutscenefilenames_buffer;
char* GF_levelnames_buffer;
char* GF_GameStrings_buffer;
char* GF_PCStrings_buffer;
char* GF_Pickup1Strings_buffer;
char* GF_Pickup2Strings_buffer;
char* GF_Puzzle1Strings_buffer;
char* GF_Puzzle2Strings_buffer;
char* GF_Puzzle3Strings_buffer;
char* GF_Puzzle4Strings_buffer;
char* GF_Key1Strings_buffer;
char* GF_Key2Strings_buffer;
char* GF_Key3Strings_buffer;
char* GF_Key4Strings_buffer;

short* GF_sequence_buffer;
short* GF_frontendSequence;
long GF_ScriptVersion;
long GF_BonusLevelEnabled;
long GF_PlayingFMV;
long GF_LaraStartAnim;
ushort GF_Cutscene_Orientation = 0x4000;
short GF_LoadingPic = -1;
short GF_NoFloor;
short GF_DeadlyWater;
short GF_SunsetEnabled;
short GF_RemoveWeapons;
short GF_NumSecrets;
short GF_RemoveAmmo;
char GF_StartGame;
char GF_Kill2Complete;
char GF_Playing_Story;

/*New events*/
char GF_Rain;
char GF_Snow;
char GF_WaterParts;
char GF_Cold;
short GF_DeathTile;
long GF_WaterColor;

short NextAdventure;

long GF_LoadScriptFile(const char* name)
{
	GF_SunsetEnabled = 0;

	if (!S_LoadGameFlow(name))
		return 0;

#ifdef _DEBUG
	gameflow.play_any_level = 1;
#endif
	gameflow.dozy_cheat_enabled = 1;	//huh?
	gameflow.stats_track = 14;

	icompass_option.itemText = GF_GameStrings[GT_STOPWATCH];

	igun_option.itemText = GF_GameStrings[GT_PISTOLS];
	iflare_option.itemText = GF_GameStrings[GT_FLARE];
	ishotgun_option.itemText = GF_GameStrings[GT_SHOTGUN];
	imagnum_option.itemText = GF_GameStrings[GT_AUTOPISTOLS];
	iuzi_option.itemText = GF_GameStrings[GT_UZIS];
	iharpoon_option.itemText = GF_GameStrings[GT_HARPOON];
	im16_option.itemText = GF_GameStrings[GT_M16];
	irocket_option.itemText = GF_GameStrings[GT_ROCKETLAUNCHER];
	igrenade_option.itemText = GF_GameStrings[GT_GRENADELAUNCHER];
	igunammo_option.itemText = GF_GameStrings[GT_PISTOLCLIPS];
	isgunammo_option.itemText = GF_GameStrings[GT_SHOTGUNSHELLS];
	imagammo_option.itemText = GF_GameStrings[GT_AUTOPISTOLCLIPS];
	iuziammo_option.itemText = GF_GameStrings[GT_UZICLIPS];
	iharpoonammo_option.itemText = GF_GameStrings[GT_HARPOONBOLTS];
	im16ammo_option.itemText = GF_GameStrings[GT_M16CLIPS];
	irocketammo_option.itemText = GF_GameStrings[GT_ROCKETS];
	igrenadeammo_option.itemText = GF_GameStrings[GT_GRENADES];

	imedi_option.itemText = GF_GameStrings[GT_SMALLMEDI];
	ibigmedi_option.itemText = GF_GameStrings[GT_LARGEMEDI];

	ipickup1_option.itemText = GF_GameStrings[GT_PICKUP];
	ipickup2_option.itemText = GF_GameStrings[GT_PICKUP];

	ipuzzle1_option.itemText = GF_GameStrings[GT_PUZZLE];
	ipuzzle2_option.itemText = GF_GameStrings[GT_PUZZLE];
	ipuzzle3_option.itemText = GF_GameStrings[GT_PUZZLE];
	ipuzzle4_option.itemText = GF_GameStrings[GT_PUZZLE];

	ikey1_option.itemText = GF_GameStrings[GT_KEY];
	ikey2_option.itemText = GF_GameStrings[GT_KEY];
	ikey3_option.itemText = GF_GameStrings[GT_KEY];
	ikey4_option.itemText = GF_GameStrings[GT_KEY];

	ipassport_option.itemText = GF_GameStrings[GT_GAME];
	igamma_option.itemText = GF_GameStrings[GT_LEVELSELECT];

	icon1_option.itemText = GF_GameStrings[GT_ICON1];
	icon2_option.itemText = GF_GameStrings[GT_ICON2];
	icon3_option.itemText = GF_GameStrings[GT_ICON3];
	icon4_option.itemText = GF_GameStrings[GT_ICON4];

	sgcrystal_option.itemText = GF_GameStrings[GT_CRYSTAL];
	idetail_option.itemText = GF_PCStrings[PCSTR_DETAILLEVEL];
	isound_option.itemText = GF_PCStrings[PCSTR_SOUND];
	icontrol_option.itemText = GF_PCStrings[PCSTR_CONTROLS];
	iphoto_option.itemText = GF_GameStrings[GT_GYM];

	SetRequesterHeading(&Load_Game_Requester, GF_GameStrings[GT_SELECTLEVEL], 0, 0, 0);
	SetRequesterHeading(&Level_Select_Requester, GF_GameStrings[GT_SELECTLEVEL], 0, 0, 0);
	return 1;
}

long GF_DoFrontEndSequence()
{
	return GF_InterpretSequence(GF_frontendSequence, 1, 1) == EXITGAME;
}

long GF_DoLevelSequence(long level, long type)
{
	long option;

	do
	{
		if (level > gameflow.num_levels - 1)
		{
			title_loaded = 0;
			return EXIT_TO_TITLE;
		}

		option = GF_InterpretSequence(GF_level_sequence_list[level], type, 0);
		level++;

		if (gameflow.singlelevel >= 0)
			break;

	} while ((option & ~0xFF) == LEVELCOMPLETE);

	return option;
}

void GF_ModifyInventory(long level, long type)
{
	START_INFO* start;

	start = &savegame.start[level];

	if (!start->got_pistols && GF_Add2InvItems[ADDINV_PISTOLS])
	{
		start->got_pistols = 1;
		Inv_AddItem(GUN_ITEM);
	}

	if (Inv_RequestItem(SHOTGUN_ITEM))
	{
		if (type)
		{
			lara.shotgun.ammo += 12 * GF_SecretInvItems[ADDINV_SHOTGUN_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_SHOTGUN_AMMO]; i++)
				AddDisplayPickup(SG_AMMO_ITEM);
		}
		else
			lara.shotgun.ammo += 12 * GF_Add2InvItems[ADDINV_SHOTGUN_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_SHOTGUN] || type == 1 && GF_SecretInvItems[ADDINV_SHOTGUN])
	{
		start->got_shotgun = 1;
		Inv_AddItem(SHOTGUN_ITEM);

		if (type)
		{
			lara.shotgun.ammo += 12 * GF_SecretInvItems[ADDINV_SHOTGUN_AMMO];
			AddDisplayPickup(SHOTGUN_ITEM);
		}
		else
			lara.shotgun.ammo += 12 * GF_Add2InvItems[ADDINV_SHOTGUN_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_SHOTGUN_AMMO]; i++)
		{
			AddDisplayPickup(SG_AMMO_ITEM);
			Inv_AddItem(SG_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_SHOTGUN_AMMO]; i++)
			Inv_AddItem(SG_AMMO_ITEM);
	}

	if (Inv_RequestItem(MAGNUM_ITEM))
	{
		if (type)
		{
			lara.magnums.ammo += 10 * GF_SecretInvItems[ADDINV_AUTOPISTOLS_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_AUTOPISTOLS_AMMO]; i++)
				AddDisplayPickup(MAG_AMMO_ITEM);
		}
		else
			lara.magnums.ammo += 10 * GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_AUTOPISTOLS] || type == 1 && GF_SecretInvItems[ADDINV_AUTOPISTOLS])
	{
		start->got_magnums = 1;
		Inv_AddItem(MAGNUM_ITEM);

		if (type)
		{
			AddDisplayPickup(MAGNUM_ITEM);
			lara.magnums.ammo += 10 * GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_AUTOPISTOLS]; i++)
				AddDisplayPickup(MAG_AMMO_ITEM);
		}
		else
			lara.magnums.ammo += 10 * GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_AUTOPISTOLS_AMMO]; i++)
		{
			Inv_AddItem(MAG_AMMO_ITEM);
			AddDisplayPickup(MAG_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_AUTOPISTOLS_AMMO]; i++)
			Inv_AddItem(MAG_AMMO_ITEM);
	}

	if (Inv_RequestItem(UZI_ITEM))
	{
		if (type)
		{
			lara.uzis.ammo += 40 * GF_SecretInvItems[ADDINV_UZI_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_UZI_AMMO]; i++)
				AddDisplayPickup(UZI_AMMO_ITEM);
		}
		else
			lara.uzis.ammo += 40 * GF_Add2InvItems[ADDINV_UZI_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_UZIS] || type == 1 && GF_SecretInvItems[ADDINV_UZIS])
	{
		start->got_uzis = 1;
		Inv_AddItem(UZI_ITEM);

		if (type)
		{
			AddDisplayPickup(UZI_ITEM);
			lara.uzis.ammo += 40 * GF_SecretInvItems[ADDINV_UZI_AMMO];

			for (int i = 0; i < ADDINV_UZI_AMMO; i++)
				AddDisplayPickup(UZI_AMMO_ITEM);
		}
		else
			lara.uzis.ammo += 40 * GF_Add2InvItems[ADDINV_UZI_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_UZI_AMMO]; i++)
		{
			Inv_AddItem(UZI_AMMO_ITEM);
			AddDisplayPickup(UZI_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_UZI_AMMO]; i++)
			Inv_AddItem(UZI_AMMO_ITEM);
	}

	if (Inv_RequestItem(HARPOON_ITEM))
	{
		if (type)
		{
			lara.harpoon.ammo += GF_SecretInvItems[ADDINV_HARPOON_AMMO] + 2 * GF_SecretInvItems[ADDINV_HARPOON_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_HARPOON_AMMO]; i++)
				AddDisplayPickup(HARPOON_AMMO_ITEM);
		}
		else
			lara.harpoon.ammo += GF_Add2InvItems[ADDINV_HARPOON_AMMO] + 2 * GF_Add2InvItems[ADDINV_HARPOON_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_HARPOON] || type == 1 && GF_SecretInvItems[ADDINV_HARPOON])
	{
		start->got_harpoon = 1;
		Inv_AddItem(HARPOON_ITEM);

		if (type)
		{
			AddDisplayPickup(HARPOON_ITEM);
			lara.harpoon.ammo += GF_SecretInvItems[ADDINV_HARPOON_AMMO] + 2 * GF_SecretInvItems[ADDINV_HARPOON_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_HARPOON_AMMO]; i++)
				AddDisplayPickup(HARPOON_AMMO_ITEM);
		}
		else
			lara.harpoon.ammo += GF_Add2InvItems[ADDINV_HARPOON_AMMO] + 2 * GF_Add2InvItems[ADDINV_HARPOON_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_HARPOON_AMMO]; i++)
		{
			if (!i)
			{
				Inv_AddItem(HARPOON_AMMO_ITEM);
				lara.harpoon.ammo -= 3;
			}

			lara.harpoon.ammo += 3;
			AddDisplayPickup(HARPOON_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_HARPOON_AMMO]; i++)
		{
			if (!i)
			{
				Inv_AddItem(HARPOON_AMMO_ITEM);
				lara.harpoon.ammo -= 3;
			}
			
			lara.harpoon.ammo += 3;
		}
	}

	if (Inv_RequestItem(M16_ITEM))
	{
		if (type)
		{
			lara.m16.ammo += 60 * GF_SecretInvItems[ADDINV_M16_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_M16_AMMO]; i++)
				AddDisplayPickup(M16_AMMO_ITEM);
		}
		else
			lara.m16.ammo += 60 * GF_Add2InvItems[ADDINV_M16_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_M16] || type == 1 && GF_SecretInvItems[ADDINV_M16])
	{
		start->got_m16 = 1;
		Inv_AddItem(M16_ITEM);

		if (type)
		{
			AddDisplayPickup(M16_ITEM);
			lara.m16.ammo += 60 * GF_SecretInvItems[ADDINV_M16_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_M16_AMMO]; i++)
				AddDisplayPickup(M16_AMMO_ITEM);
		}
		else
			lara.m16.ammo += 60 * GF_Add2InvItems[ADDINV_M16_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_M16_AMMO]; i++)
		{
			Inv_AddItem(M16_AMMO_ITEM);
			AddDisplayPickup(M16_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_M16_AMMO]; i++)
			Inv_AddItem(M16_AMMO_ITEM);
	}

	if (Inv_RequestItem(ROCKET_GUN_ITEM))
	{
		if (type)
		{
			lara.rocket.ammo += GF_SecretInvItems[ADDINV_ROCKET_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_ROCKET_AMMO]; i++)
				AddDisplayPickup(ROCKET_AMMO_ITEM);
		}
		else
			lara.rocket.ammo += GF_Add2InvItems[ADDINV_ROCKET_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_ROCKET] || type == 1 && GF_SecretInvItems[ADDINV_ROCKET])
	{
		start->got_rocket = 1;
		Inv_AddItem(ROCKET_GUN_ITEM);

		if (type)
		{
			lara.rocket.ammo += GF_SecretInvItems[ADDINV_ROCKET_AMMO];
			AddDisplayPickup(ROCKET_GUN_ITEM);

			for (int i = 0; i < GF_SecretInvItems[ADDINV_ROCKET_AMMO]; i++)
				AddDisplayPickup(ROCKET_AMMO_ITEM);
		}
		else
			lara.rocket.ammo += GF_Add2InvItems[ADDINV_ROCKET_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_ROCKET_AMMO]; i++)
		{
			Inv_AddItem(ROCKET_AMMO_ITEM);
			AddDisplayPickup(ROCKET_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_ROCKET_AMMO]; i++)
			Inv_AddItem(ROCKET_AMMO_ITEM);
	}

	if (Inv_RequestItem(GRENADE_GUN_ITEM))
	{
		if (type)
		{
			lara.grenade.ammo += 2 * GF_SecretInvItems[ADDINV_GRENADE_AMMO];

			for (int i = 0; i < GF_SecretInvItems[ADDINV_GRENADE_AMMO]; i++)
				AddDisplayPickup(GRENADE_AMMO_ITEM);
		}
		else
			lara.grenade.ammo += 2 * GF_Add2InvItems[ADDINV_GRENADE_AMMO];
	}
	else if (!type && GF_Add2InvItems[ADDINV_GRENADE] || type == 1 && GF_SecretInvItems[ADDINV_GRENADE])
	{
		start->got_grenade = 1;
		Inv_AddItem(GRENADE_GUN_ITEM);

		if (type)
		{
			lara.grenade.ammo += 2 * GF_SecretInvItems[ADDINV_GRENADE_AMMO];
			AddDisplayPickup(GRENADE_GUN_ITEM);

			for (int i = 0; i < GF_SecretInvItems[ADDINV_GRENADE_AMMO]; i++)
				AddDisplayPickup(GRENADE_AMMO_ITEM);
		}
		else
			lara.grenade.ammo += 2 * GF_Add2InvItems[ADDINV_GRENADE_AMMO];
	}
	else if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_GRENADE_AMMO]; i++)
		{
			Inv_AddItem(GRENADE_AMMO_ITEM);
			AddDisplayPickup(GRENADE_AMMO_ITEM);
		}
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_GRENADE_AMMO]; i++)
			Inv_AddItem(GRENADE_AMMO_ITEM);
	}

	if (type)
	{
		for (int i = 0; i < GF_SecretInvItems[ADDINV_FLARES]; i++)
		{
			Inv_AddItem(FLARE_ITEM);
			AddDisplayPickup(FLARE_ITEM);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_MEDI]; i++)
		{
			Inv_AddItem(MEDI_ITEM);
			AddDisplayPickup(MEDI_ITEM);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_BIGMEDI]; i++)
		{
			Inv_AddItem(BIGMEDI_ITEM);
			AddDisplayPickup(BIGMEDI_ITEM);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PICKUP1]; i++)
		{
			Inv_AddItem(PICKUP_ITEM1);
			AddDisplayPickup(PICKUP_ITEM1);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PICKUP2]; i++)
		{
			Inv_AddItem(PICKUP_ITEM2);
			AddDisplayPickup(PICKUP_ITEM2);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE1]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM1);
			AddDisplayPickup(PUZZLE_ITEM1);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE2]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM2);
			AddDisplayPickup(PUZZLE_ITEM2);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE3]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM3);
			AddDisplayPickup(PUZZLE_ITEM3);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_PUZZLE4]; i++)
		{
			Inv_AddItem(PUZZLE_ITEM4);
			AddDisplayPickup(PUZZLE_ITEM4);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY1]; i++)
		{
			Inv_AddItem(KEY_ITEM1);
			AddDisplayPickup(KEY_ITEM1);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY2]; i++)
		{
			Inv_AddItem(KEY_ITEM2);
			AddDisplayPickup(KEY_ITEM2);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY3]; i++)
		{
			Inv_AddItem(KEY_ITEM3);
			AddDisplayPickup(KEY_ITEM3);
		}

		for (int i = 0; i < GF_SecretInvItems[ADDINV_KEY4]; i++)
		{
			Inv_AddItem(KEY_ITEM4);
			AddDisplayPickup(KEY_ITEM4);
		}

		memset(GF_SecretInvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
	}
	else
	{
		for (int i = 0; i < GF_Add2InvItems[ADDINV_FLARES]; i++)
			Inv_AddItem(FLARE_ITEM);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_MEDI]; i++)
			Inv_AddItem(MEDI_ITEM);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_BIGMEDI]; i++)
			Inv_AddItem(BIGMEDI_ITEM);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PICKUP1]; i++)
			Inv_AddItem(PICKUP_ITEM1);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PICKUP2]; i++)
			Inv_AddItem(PICKUP_ITEM2);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE1]; i++)
			Inv_AddItem(PUZZLE_ITEM1);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE2]; i++)
			Inv_AddItem(PUZZLE_ITEM2);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE3]; i++)
			Inv_AddItem(PUZZLE_ITEM3);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_PUZZLE4]; i++)
			Inv_AddItem(PUZZLE_ITEM4);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY1]; i++)
			Inv_AddItem(KEY_ITEM1);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY2]; i++)
			Inv_AddItem(KEY_ITEM2);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY3]; i++)
			Inv_AddItem(KEY_ITEM3);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_KEY4]; i++)
			Inv_AddItem(KEY_ITEM4);

		for (int i = 0; i < GF_Add2InvItems[ADDINV_SAVEGAME_CRYSTAL]; i++)
			Inv_AddItem(SAVEGAME_CRYSTAL_ITEM);

		memset(GF_Add2InvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
	}
}

static void SetCutsceneTrack(long track)
{
	cutscene_track = track;
}

long GF_InterpretSequence(short* ptr, long type, long seq_type)
{
	long option, level, val;
	short ntracks;
	char string[80];
	char Adventure[6] = { LV_JUNGLE, LV_SHORE, LV_DESERT, LV_JUNGLE, LV_ROOFTOPS, LV_ANTARC };	//starting levels for: india, pacific, nevada, peru, london, antarc.

	GF_NoFloor = 0;
	GF_DeadlyWater = 0;
	GF_SunsetEnabled = 0;
	GF_LaraStartAnim = 0;
	GF_Kill2Complete = 0;
	GF_RemoveAmmo = 0;
	GF_RemoveWeapons = 0;
	GF_Rain = 0;
	GF_Snow = 0;
	GF_WaterParts = 0;
	GF_Cold = 0;
	GF_DeathTile = DEATH_LAVA;
	GF_WaterColor = -1;
	GF_NumSecrets = -1;
	memset(GF_Add2InvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
	memset(GF_SecretInvItems, 0, sizeof(char) * ADDINV_NUMBEROF);
	GF_CDtracks[0] = 2;
	ntracks = 0;
	option = EXIT_TO_TITLE;

	while (*ptr != GFE_END_SEQ)
	{
		switch (*ptr)
		{
		case GFE_PICTURE:

			if (type != 2)
				sprintf(string, "PICTURE %s", GF_picfilenames[ptr[1]]);

			ptr += 2;
			break;

		case GFE_LIST_START:
		case GFE_LIST_END:
			ptr++;
			break;

		case GFE_PLAYFMV:

			if (type != 2)
			{
				if (ptr[2] == GFE_PLAYFMV)
				{
					if (S_IntroFMV(GF_fmvfilenames[ptr[1]], GF_fmvfilenames[ptr[3]]))
						return EXITGAME;

					ptr += 2;
				}
				else
				{
					GF_PlayingFMV = 1;

					if (S_PlayFMV(GF_fmvfilenames[ptr[1]]))
						return EXITGAME;
				}
			}

			ptr += 2;
			break;

		case GFE_STARTLEVEL:
			level = ptr[1];

			if (level > gameflow.num_levels)
			{
				sprintf(string, "INVALID LEVEL %d", level);
				option = EXIT_TO_TITLE;
			}
			else if (type != 5)
			{
				if (type == 7)
					return EXIT_TO_TITLE;

				option = StartGame(level, type);
				GF_StartGame = 0;

				if (type == 2)
					type = 1;

				if ((option & ~0xFF) != LEVELCOMPLETE)
					return option;
			}

			ptr += 2;
			break;

		case GFE_CUTSCENE:

			if (type != 2)
			{
				sprintf(string, "CUTSCENE %d %s", ptr[1], GF_cutscenefilenames[ptr[1]]);
				level = CurrentLevel;
				S_FadeToBlack();
				SetPictureToFade(1);

				if (InitialiseLevel(ptr[1], 4))
					val = StartCinematic(ptr[1]);
				else
					val = 2;

				CurrentLevel = level;

				if (val == 2 && (type == 5 || type == 7))
					return EXIT_TO_TITLE;

				if (val == 3)
					return EXITGAME;
			}

			ptr += 2;
			break;

		case GFE_LEVCOMPLETE:

			if (type != 5 && type != 7)
			{
				level = LevelStats(CurrentLevel);

				if (level == -1)
					return EXIT_TO_TITLE;

				if (level)
				{
					Display_Inventory(INV_LEVELSELECT_MODE);
					option = Adventure[NextAdventure];
					CreateStartInfo(option);
					FadePictureDown(32);
				}
				else
				{
					CreateStartInfo(CurrentLevel + 1);
					option = CurrentLevel + 1;
					savegame.current_level = short(CurrentLevel + 1);
				}
			}

			ptr++;
			break;

		case GFE_DEMOPLAY:

			if (type != 2 && type != 5 && type != 7)
				return StartDemo(ptr[1]);

			ptr += 2;
			break;

		case GFE_JUMPTO_SEQ:
			sprintf(string, "JUMPSEQ %d", ptr[1]);
			ptr += 2;
			break;

		case GFE_SETTRACK:
			GF_CDtracks[ntracks] = ptr[1];
			SetCutsceneTrack(ptr[1]);
			ntracks++;
			ptr += 2;
			break;

		case GFE_SUNSET:

			if (type != 5 && type != 7)
				GF_SunsetEnabled = 1;

			ptr++;
			break;

		case GFE_LOADINGPIC:
			GF_LoadingPic = ptr[1];
			ptr += 2;
			break;

		case GFE_DEADLY_WATER:

			if (type != 5 && type != 7)
				GF_DeadlyWater = 1;

			ptr++;
			break;

		case GFE_REMOVE_WEAPONS:

			if (type != 5 && type != 7 && type != 2)
				GF_RemoveWeapons = 1;

			ptr++;
			break;

		case GFE_REMOVE_AMMO:

			if (type != 5 && type != 7 && type != 2)
				GF_RemoveAmmo = 1;

			ptr++;
			break;

		case GFE_GAMECOMPLETE:
			DisplayCredits();
			GameStats(CurrentLevel, type);

			if (!GF_BonusLevelEnabled)
				return EXIT_TO_TITLE;

			CreateStartInfo(CurrentLevel + 1);
			savegame.current_level = short(CurrentLevel + 1);
			FadePictureDown(32);
			return CurrentLevel + 1;

		case GFE_CUTANGLE:

			if (type != 2)
				GF_Cutscene_Orientation = ptr[1];

			ptr += 2;
			break;

		case GFE_NOFLOOR:

			if (type != 5 && type != 7)
				GF_NoFloor = ptr[1];

			ptr += 2;
			break;

		case GFE_ADD2INV:

			if (type != 5 && type != 7)
			{
				if (ptr[1] < 1000)
					GF_SecretInvItems[ptr[1]]++;
				else if (type != 2)
					GF_Add2InvItems[ptr[1] - 1000]++;
			}

			ptr += 2;
			break;

		case GFE_STARTANIM:

			if (type != 5 && type != 7)
				GF_LaraStartAnim = ptr[1];

			ptr += 2;
			break;

		case GFE_NUMSECRETS:

			if (type != 5 && type != 7)
				GF_NumSecrets = ptr[1];

			ptr += 2;
			break;

		case GFE_KILL2COMPLETE:

			if (type != 5 && type != 7)
				GF_Kill2Complete = 1;

			ptr++;
			break;

		case GFE_RAIN:
			GF_Rain = 1;
			ptr++;
			break;

		case GFE_SNOW:
			GF_Snow = 1;
			ptr++;
			break;

		case GFE_WATER_PARTS:
			GF_WaterParts = 1;
			ptr++;
			break;

		case GFE_COLD:
			GF_Cold = 1;
			ptr++;
			break;

		case GFE_DEATHTILE:
			GF_DeathTile = ptr[1];
			ptr += 2;
			break;

		case GFE_WATERCLR:
			GF_WaterColor = (ushort)ptr[1] | (ushort)ptr[2] << 16;
			ptr += 3;
			break;
		}
	}

	if (type == 5 || type == 7)
		return 0;

	return option;
}
