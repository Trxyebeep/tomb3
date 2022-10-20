#include "../tomb3/pch.h"
#include "gameflow.h"
#include "../specific/file.h"
#include "invfunc.h"

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

void inject_gameflow(bool replace)
{
    INJECT(0x00432030, GF_LoadScriptFile, replace);
}
