#include "../tomb3/pch.h"
#include "savegame.h"
#include "gameflow.h"

void ModifyStartInfo(long level)
{
	START_INFO* pInfo;

	pInfo = &savegame.start[level];
	pInfo->got_pistols = 1;
	pInfo->gun_type = LG_PISTOLS;
	pInfo->pistol_ammo = 1000;

	if (level == LV_GYM)
	{
		pInfo->available = 1;

		pInfo->got_pistols = 0;
		pInfo->got_magnums = 0;
		pInfo->got_uzis = 0;
		pInfo->got_shotgun = 0;
		pInfo->got_m16 = 0;
		pInfo->got_rocket = 0;
		pInfo->got_grenade = 0;
		pInfo->got_harpoon = 0;

		pInfo->pistol_ammo = 0;
		pInfo->magnum_ammo = 0;
		pInfo->uzi_ammo = 0;
		pInfo->shotgun_ammo = 0;
		pInfo->m16_ammo = 0;
		pInfo->rocket_ammo = 0;
		pInfo->grenade_ammo = 0;
		pInfo->harpoon_ammo = 0;

		pInfo->num_flares = 0;
		pInfo->num_big_medis = 0;
		pInfo->num_medis = 0;
		pInfo->num_sgcrystals = 1;
		pInfo->gun_type = LG_UNARMED;
	}
	else if (level == LV_JUNGLE)	//forgets to reset shotgun!
	{
		pInfo->available = 1;

		pInfo->got_magnums = 0;
		pInfo->got_uzis = 0;
		pInfo->got_m16 = 0;
		pInfo->got_rocket = 0;
		pInfo->got_grenade = 0;
		pInfo->got_harpoon = 0;

		pInfo->magnum_ammo = 0;
		pInfo->uzi_ammo = 0;
		pInfo->m16_ammo = 0;
		pInfo->rocket_ammo = 0;
		pInfo->grenade_ammo = 0;
		pInfo->harpoon_ammo = 0;

		pInfo->num_flares = 2;
		pInfo->num_medis = 1;
		pInfo->num_big_medis = 1;
		pInfo->num_sgcrystals = 1;

#ifdef TROYESTUFF
		pInfo->got_shotgun = 0;
		pInfo->shotgun_ammo = 0;
#endif
	}

	pInfo->gun_status = LG_ARMLESS;

	if (savegame.bonus_flag && level != LV_GYM)
	{
		pInfo->available = 1;

		pInfo->got_pistols = 1;
		pInfo->got_magnums = 1;
		pInfo->got_uzis = 1;
		pInfo->got_shotgun = 1;
		pInfo->got_m16 = 1;
		pInfo->got_rocket = 1;
		pInfo->got_grenade = 1;
		pInfo->got_harpoon = 1;

		pInfo->magnum_ammo = 10000;
		pInfo->uzi_ammo = 10000;
		pInfo->shotgun_ammo = 10000;
		pInfo->m16_ammo = 10000;
		pInfo->rocket_ammo = 10000;
		pInfo->grenade_ammo = 10000;
		pInfo->harpoon_ammo = 10000;
		pInfo->gun_type = LG_ROCKET;
		pInfo->num_flares = 255;
		pInfo->num_sgcrystals = 50;
	}
}

void inject_savegame(bool replace)
{
	INJECT(0x00461A60, ModifyStartInfo, replace);
}
