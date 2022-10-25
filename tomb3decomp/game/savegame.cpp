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

void InitialiseStartInfo()
{
	START_INFO* pInfo;
	FILE* file;

	if (savegame.bonus_flag)
		return;

	for (int i = 0; i < 24; i++)
	{
		pInfo = &savegame.start[i];

		ModifyStartInfo(i);
		pInfo->available = 0;
		pInfo->timer = 0;
		pInfo->ammo_used = 0;
		pInfo->ammo_hit = 0;
		pInfo->distance_travelled = 0;
		pInfo->kills = 0;
		pInfo->secrets_found = 0;
		pInfo->health_used = 0;
	}

	for (int i = 0; i < 10; i++)
	{
		savegame.best_assault_times[i] = 0;
		savegame.best_quadbike_times[i] = 0;
	}

	savegame.start[LV_GYM].available = 1;
	savegame.start[LV_JUNGLE].available = 1;
	savegame.AfterAdventureSave = 0;
	savegame.WorldRequired = 0;
	savegame.IndiaComplete = 0;
	savegame.SPacificComplete = 0;
	savegame.LondonComplete = 0;
	savegame.NevadaComplete = 0;
	savegame.AntarcticaComplete = 0;
	savegame.PeruComplete = 0;
	savegame.AfterIndia = 0;
	savegame.AfterSPacific = 0;
	savegame.AfterLondon = 0;
	savegame.AfterNevada = 0;
	savegame.QuadbikeKeyFlag = 0;
	savegame.bonus_flag = 0;
	savegame.GameComplete = 0;

	file = fopen("data.bin", "rb");

	if (file)
	{
		fread(&savegame.best_assault_times, sizeof(long), sizeof(savegame.best_assault_times) / sizeof(long), file);
		fread(&savegame.best_quadbike_times, sizeof(long), sizeof(savegame.best_quadbike_times) / sizeof(long), file);
		fread(&savegame.QuadbikeKeyFlag, sizeof(long), 1, file);
#ifdef TROYESTUFF
		fclose(file);
#endif
	}
}

void inject_savegame(bool replace)
{
	INJECT(0x00461A60, ModifyStartInfo, replace);
	INJECT(0x00461950, InitialiseStartInfo, replace);
}
