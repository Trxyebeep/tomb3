#include "../tomb3/pch.h"
#include "savegame.h"
#include "gameflow.h"
#include "invfunc.h"
#include "objects.h"
#include "../specific/winmain.h"

#define SGcount	VAR_(0x006D588C, long)
#define SGpoint	VAR_(0x006D2268, char*)

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

void CreateStartInfo(long level)
{
	START_INFO* pInfo;

	pInfo = &savegame.start[level];
	pInfo->available = 1;
	pInfo->pistol_ammo = 1000;

	if (Inv_RequestItem(GUN_ITEM))
		pInfo->got_pistols = 1;
	else
		pInfo->got_pistols = 0;

	if (Inv_RequestItem(MAGNUM_ITEM))
	{
		pInfo->got_magnums = 1;
		pInfo->magnum_ammo = (ushort)lara.magnums.ammo;
	}
	else
	{
		pInfo->got_magnums = 0;
		pInfo->magnum_ammo = ushort(Inv_RequestItem(MAG_AMMO_ITEM) * 10);
	}

	if (Inv_RequestItem(UZI_ITEM))
	{
		pInfo->got_uzis = 1;
		pInfo->uzi_ammo = (ushort)lara.uzis.ammo;
	}
	else
	{
		pInfo->got_uzis = 0;
		pInfo->uzi_ammo = ushort(Inv_RequestItem(UZI_AMMO_ITEM) * 40);
	}

	if (Inv_RequestItem(SHOTGUN_ITEM))
	{
		pInfo->got_shotgun = 1;
		pInfo->shotgun_ammo = (ushort)lara.shotgun.ammo;
	}
	else
	{
		pInfo->got_shotgun = 0;
		pInfo->shotgun_ammo = ushort(Inv_RequestItem(SG_AMMO_ITEM) * 12);
	}

	if (Inv_RequestItem(HARPOON_ITEM))
	{
		pInfo->got_harpoon = 1;
		pInfo->harpoon_ammo = (ushort)lara.harpoon.ammo;
	}
	else
	{
		pInfo->got_harpoon = 0;
		pInfo->harpoon_ammo = (ushort)lara.harpoon.ammo;	//...ok
	}

	if (Inv_RequestItem(M16_ITEM))
	{
		pInfo->got_m16 = 1;
		pInfo->m16_ammo = (ushort)lara.m16.ammo;
	}
	else
	{
		pInfo->got_m16 = 0;
		pInfo->m16_ammo = ushort(Inv_RequestItem(M16_AMMO_ITEM) * 60);
	}

	if (Inv_RequestItem(ROCKET_GUN_ITEM))
	{
		pInfo->got_rocket = 1;
		pInfo->rocket_ammo = (ushort)lara.rocket.ammo;
	}
	else
	{
		pInfo->got_rocket = 0;
		pInfo->rocket_ammo = (ushort)Inv_RequestItem(ROCKET_AMMO_ITEM);
	}

	if (Inv_RequestItem(GRENADE_GUN_ITEM))
	{
		pInfo->got_grenade = 1;
		pInfo->grenade_ammo = (ushort)lara.grenade.ammo;
	}
	else
	{
		pInfo->got_grenade = 0;
		pInfo->grenade_ammo = ushort(Inv_RequestItem(GRENADE_AMMO_ITEM) * 2);
	}

	pInfo->num_flares = (uchar)Inv_RequestItem(FLARE_ITEM);
	pInfo->num_medis = (uchar)Inv_RequestItem(MEDI_ITEM);
	pInfo->num_big_medis = (uchar)Inv_RequestItem(BIGMEDI_ITEM);
	pInfo->num_sgcrystals = (uchar)Inv_RequestItem(SAVEGAME_CRYSTAL_ITEM);
	pInfo->num_icon1 = (uchar)Inv_RequestItem(ICON_PICKUP1_ITEM);
	pInfo->num_icon2 = (uchar)Inv_RequestItem(ICON_PICKUP2_ITEM);
	pInfo->num_icon3 = (uchar)Inv_RequestItem(ICON_PICKUP3_ITEM);
	pInfo->num_icon4 = (uchar)Inv_RequestItem(ICON_PICKUP4_ITEM);

	if (lara.gun_type == LG_FLARE)
		pInfo->gun_type = (char)lara.last_gun_type;
	else
		pInfo->gun_type = (char)lara.gun_type;

	pInfo->gun_status = LG_ARMLESS;
}

void ResetSG()
{
	SGcount = 0;
	SGpoint = savegame.buffer;
}

void WriteSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	if (SGcount >= 0x3080)
		S_ExitSystem("FATAL: Savegame is too big to fit in buffer");

	for (data = (char*)pointer; size > 0; size--)
		*SGpoint++ = *data++;
}

void ReadSG(void* pointer, long size)
{
	char* data;

	SGcount += size;

	for (data = (char*)pointer; size > 0; size--)
		*data++ = *SGpoint++;
}

void CreateSaveGameInfo()
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	ulong flags;
	long nFlares, age;
	short objnum, explode_count;
	char TonyExploding, flip;

	TonyExploding = 0;
	savegame.current_level = (short)CurrentLevel;
	CreateStartInfo(CurrentLevel);
	savegame.num_pickup1 = (uchar)Inv_RequestItem(PICKUP_ITEM1);
	savegame.num_pickup2 = (uchar)Inv_RequestItem(PICKUP_ITEM2);
	savegame.num_puzzle1 = (uchar)Inv_RequestItem(PUZZLE_ITEM1);
	savegame.num_puzzle2 = (uchar)Inv_RequestItem(PUZZLE_ITEM2);
	savegame.num_puzzle3 = (uchar)Inv_RequestItem(PUZZLE_ITEM3);
	savegame.num_puzzle4 = (uchar)Inv_RequestItem(PUZZLE_ITEM4);
	savegame.num_key1 = (uchar)Inv_RequestItem(KEY_ITEM1);
	savegame.num_key2 = (uchar)Inv_RequestItem(KEY_ITEM2);
	savegame.num_key3 = (uchar)Inv_RequestItem(KEY_ITEM3);
	savegame.num_key4 = (uchar)Inv_RequestItem(KEY_ITEM4);

	ResetSG();
	memset(savegame.buffer, 0, sizeof(savegame.buffer));

	WriteSG(&flip_status, sizeof(long));

	for (int i = 0; i < 10; i++)
	{
		flip = char(flipmap[i] >> 8);
		WriteSG(&flip, sizeof(char));
	}

	WriteSG(cd_flags, 128);

	for (int i = 0; i < number_cameras; i++)
		WriteSG(&camera.fixed[i].flags, sizeof(short));

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];
		objnum = item->object_number;
		obj = &objects[objnum];

		if (objnum == TONY && item->hit_points > 0 && bossdata.explode_count || bossdata.dead)
			TonyExploding = 1;

		if (obj->save_position)
		{
			WriteSG(&item->pos, sizeof(PHD_3DPOS));
			WriteSG(&item->room_number, sizeof(short));
			WriteSG(&item->speed, sizeof(short));
			WriteSG(&item->fallspeed, sizeof(short));
		}

		if (obj->save_anim)
		{
			WriteSG(&item->current_anim_state, sizeof(short));
			WriteSG(&item->goal_anim_state, sizeof(short));
			WriteSG(&item->required_anim_state, sizeof(short));
			WriteSG(&item->anim_number, sizeof(short));
			WriteSG(&item->frame_number, sizeof(short));
		}

		if (obj->save_hitpoints)
			WriteSG(&item->hit_points, sizeof(short));

		if (obj->save_flags)
		{
			flags = (ushort)item->flags;
			flags |= item->active << 16;
			flags |= item->status << 17;
			flags |= item->gravity_status << 19;
			flags |= item->hit_status << 20;
			flags |= item->collidable << 21;
			flags |= item->looked_at << 22;
			flags |= item->dynamic_light << 23;
			flags |= item->clear_body << 24;
			flags |= item->ai_bits << 25;
			flags |= item->really_active << 30;

			if (obj->intelligent && item->data)
				flags |= 1 << 31;	//flag we will save creature data

			WriteSG(&flags, sizeof(ulong));

			if (obj->intelligent)
				WriteSG(&item->carried_item, sizeof(short));

			WriteSG(&item->timer, sizeof(short));

			if (flags & (1 << 31))
				WriteSG(item->data, 18);

			WriteSG(item->item_flags, sizeof(short) * 4);
		}

		if (objnum == BIGGUN)
			WriteSG(item->data, sizeof(BIGGUNINFO));

		if (objnum == BOAT)
			WriteSG(item->data, sizeof(BOAT_INFO));

		if (objnum == KAYAK)
			WriteSG(item->data, sizeof(KAYAKINFO));

		if (objnum == MINECART)
			WriteSG(item->data, sizeof(CARTINFO));

		if (objnum == QUADBIKE)
			WriteSG(item->data, sizeof(QUADINFO));

		if (objnum == UPV)
			WriteSG(item->data, sizeof(SUBINFO));

		if (objnum == AREA51_LASER)
			WriteSG(&item->shadeB, sizeof(short));
	}

	explode_count = bossdata.explode_count;

	if (!TonyExploding)
		bossdata.explode_count = 0;

	WriteSG(&bossdata, sizeof(BOSS_STRUCT));
	bossdata.explode_count = explode_count;

	for (int i = 0; i < NUM_LARA_MESHES; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] - (long)mesh_base);

	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base - (long)frames);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base - (long)frames);

	WriteSG(&lara, sizeof(LARA_INFO));

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] + (long)mesh_base);

	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base + (long)frames);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base + (long)frames);

	if (lara.weapon_item != NO_ITEM)
	{
		item = &items[lara.weapon_item];
		WriteSG(&item->object_number, sizeof(short));
		WriteSG(&item->anim_number, sizeof(short));
		WriteSG(&item->frame_number, sizeof(short));
		WriteSG(&item->current_anim_state, sizeof(short));
		WriteSG(&item->goal_anim_state, sizeof(short));
	}

	WriteSG(&flipeffect, sizeof(long));
	WriteSG(&fliptimer, sizeof(long));
	WriteSG(&CurrentAtmosphere, sizeof(char));
	WriteSG(&compy_scared_timer, sizeof(long));
	WriteSG(&compys_attack_lara, sizeof(long));
	WriteSG(&CarcassItem, sizeof(short));
	WriteSG(&TribeBossShieldOn, sizeof(char));
	WriteSG(&TribeBossItem, sizeof(long));
	WriteSG(&lizard_man_active, sizeof(char));

	nFlares = 0;

	for (int i = level_items; i < MAX_ITEMS; i++)
	{
		item = &items[i];

		if (item->active && item->object_number == FLARE_ITEM)
			nFlares++;
	}

	WriteSG(&nFlares, sizeof(long));

	for (int i = level_items; i < MAX_ITEMS; i++)
	{
		item = &items[i];

		if (item->active && item->object_number == FLARE_ITEM)
		{
			WriteSG(&item->pos, sizeof(PHD_3DPOS));
			WriteSG(&item->room_number, sizeof(short));
			WriteSG(&item->speed, sizeof(short));
			WriteSG(&item->fallspeed, sizeof(short));
			age = (long)item->data;
			WriteSG(&age, sizeof(long));
		}
	}
}

void inject_savegame(bool replace)
{
	INJECT(0x00461A60, ModifyStartInfo, replace);
	INJECT(0x00461950, InitialiseStartInfo, replace);
	INJECT(0x00461B50, CreateStartInfo, replace);
	INJECT(0x00462DF0, ResetSG, replace);
	INJECT(0x00462E10, WriteSG, replace);
	INJECT(0x00462E60, ReadSG, replace);
	INJECT(0x00461DD0, CreateSaveGameInfo, replace);
}
