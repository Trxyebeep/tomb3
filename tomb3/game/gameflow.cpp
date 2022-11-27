#include "../tomb3/pch.h"
#include "gameflow.h"
#include "../specific/file.h"
#include "invfunc.h"
#include "objects.h"
#include "health.h"

long GF_LoadScriptFile(const char* name)
{
	GF_SunsetEnabled = 0;

	if (!S_LoadGameFlow(name))
		return 0;

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

void inject_gameflow(bool replace)
{
	INJECT(0x00432030, GF_LoadScriptFile, replace);
	INJECT(0x00432280, GF_DoFrontEndSequence, replace);
	INJECT(0x004322A0, GF_DoLevelSequence, replace);
	INJECT(0x004328D0, GF_ModifyInventory, replace);
	INJECT(0x00432020, SetCutsceneTrack, replace);
}
