#include "../tomb3/pch.h"
#include "savegame.h"
#include "gameflow.h"
#include "invfunc.h"
#include "objects.h"
#include "../specific/winmain.h"
#include "laramisc.h"
#include "control.h"
#include "moveblok.h"
#include "items.h"
#include "fish.h"
#include "lot.h"
#include "traps.h"
#include "pickup.h"
#include "triboss.h"
#include "compy.h"
#include "setup.h"
#include "../specific/file.h"
#include "camera.h"
#include "lara.h"
#include "londboss.h"
#include "../specific/draweffects.h"
#include "../newstuff/map.h"
#include "../tomb3/tomb3.h"

SAVEGAME_INFO savegame;
static char* SGpoint;
static long SGcount;

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
	else if (level == LV_FIRSTLEVEL)
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

		pInfo->got_shotgun = 0;
		pInfo->shotgun_ammo = 0;
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
		pInfo->harpoon_ammo = 10001;
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
	savegame.start[LV_FIRSTLEVEL].available = 1;
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
		fclose(file);
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
		pInfo->harpoon_ammo = (ushort)lara.harpoon.ammo;
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

	save_tomb3_data();
}

void ExtractSaveGameInfo()
{
	ITEM_INFO* item;
	OBJECT_INFO* obj;
	FLOOR_INFO* floor;
	CARTINFO* cart;
	KAYAKINFO* kayak;
	ulong flags;
	long flip, nFlares, age;
	short objnum, room_number;
	char flag;

	InitialiseLaraInventory(CurrentLevel);

	for (int i = savegame.num_pickup1; i > 0; i--)
		Inv_AddItem(PICKUP_ITEM1);

	for (int i = savegame.num_pickup2; i > 0; i--)
		Inv_AddItem(PICKUP_ITEM2);

	for (int i = savegame.num_puzzle1; i > 0; i--)
		Inv_AddItem(PUZZLE_ITEM1);

	for (int i = savegame.num_puzzle2; i > 0; i--)
		Inv_AddItem(PUZZLE_ITEM2);

	for (int i = savegame.num_puzzle3; i > 0; i--)
		Inv_AddItem(PUZZLE_ITEM3);

	for (int i = savegame.num_puzzle4; i > 0; i--)
		Inv_AddItem(PUZZLE_ITEM4);

	for (int i = savegame.num_key1; i > 0; i--)
		Inv_AddItem(KEY_ITEM1);

	for (int i = savegame.num_key2; i > 0; i--)
		Inv_AddItem(KEY_ITEM2);

	for (int i = savegame.num_key3; i > 0; i--)
		Inv_AddItem(KEY_ITEM3);

	for (int i = savegame.num_key4; i > 0; i--)
		Inv_AddItem(KEY_ITEM4);

	ResetSG();

	ReadSG(&flip, sizeof(long));

	if (flip)
		FlipMap();

	for (int i = 0; i < 10; i++)
	{
		ReadSG(&flag, sizeof(char));
		flipmap[i] = flag << 8;
	}

	ReadSG(cd_flags, 128);

	for (int i = 0; i < number_cameras; i++)
		ReadSG(&camera.fixed[i].flags, sizeof(short));

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];
		objnum = item->object_number;
		obj = &objects[objnum];

		if (obj->control == MovableBlock)
			AlterFloorHeight(item, 1024);

		if (obj->save_position)
		{
			ReadSG(&item->pos, sizeof(PHD_3DPOS));
			ReadSG(&room_number, sizeof(short));
			ReadSG(&item->speed, sizeof(short));
			ReadSG(&item->fallspeed, sizeof(short));

			if (item->room_number != room_number)
				ItemNewRoom(i, room_number);

			if (obj->shadow_size)
			{
				floor = GetFloor(item->pos.x_pos, item->pos.y_pos, item->pos.z_pos, &room_number);
				item->floor = GetHeight(floor, item->pos.x_pos, item->pos.y_pos, item->pos.z_pos);
			}
		}

		if (obj->save_anim)
		{
			ReadSG(&item->current_anim_state, sizeof(short));
			ReadSG(&item->goal_anim_state, sizeof(short));
			ReadSG(&item->required_anim_state, sizeof(short));
			ReadSG(&item->anim_number, sizeof(short));
			ReadSG(&item->frame_number, sizeof(short));
		}

		if (obj->save_hitpoints)
			ReadSG(&item->hit_points, sizeof(short));

		if (obj->save_flags)
		{
			ReadSG(&flags, sizeof(ulong));
			item->flags = (short)flags;

			if (obj->intelligent)
				ReadSG(&item->carried_item, sizeof(short));

			ReadSG(&item->timer, sizeof(short));

			if (item->flags & IFL_CLEARBODY)
			{
				KillItem(i);
				item->status = ITEM_DEACTIVATED;
			}
			else
			{
				if (flags & 0x10000 && !item->active)
					AddActiveItem(i);

				item->active = (flags >> 16) & 1;
				item->status = (flags >> 17) & 3;
				item->gravity_status = (flags >> 19) & 1;
				item->hit_status = (flags >> 20) & 1;
				item->collidable = (flags >> 21) & 1;
				item->looked_at = (flags >> 22) & 1;
				item->dynamic_light = (flags >> 23) & 1;
				item->clear_body = (flags >> 24) & 1;
				item->ai_bits = (flags >> 25) & 31;
				item->really_active = (flags >> 30) & 1;
			}

			if (flags & (1 << 31))
			{
				EnableBaddieAI(i, 0);

				if (item->data)
					ReadSG(item->data, 18);
				else
					SGpoint += 18;
			}
			else if (obj->intelligent)
			{
				item->data = 0;

				if (item->clear_body && item->hit_points <= 0 && !(item->flags & IFL_CLEARBODY))
				{
					item->next_active = body_bag;
					body_bag = i;
				}
			}

			if (obj->collision == PuzzleHoleCollision)
			{
				if (item->status == ITEM_ACTIVE || item->status == ITEM_DEACTIVATED)
					item->object_number += 4;
			}

			if (obj->collision == PickUpCollision)
			{
				if (item->status == ITEM_DEACTIVATED)
					RemoveDrawnItem(i);
			}

			if (objnum == SMASH_WINDOW || objnum == SMASH_OBJECT1 || objnum == SMASH_OBJECT2 || objnum == SMASH_OBJECT3)
			{
				if (item->flags & IFL_INVISIBLE)
					item->mesh_bits = 0x100;
			}

			ReadSG(item->item_flags, sizeof(short) * 4);
		}

		if (obj->control == MovableBlock)
		{
			if (item->status == ITEM_INACTIVE)
				AlterFloorHeight(item, -1024);
			else if (obj->control == MovableBlock && item->status != ITEM_INACTIVE)
				SetupCleanerFromSavegame(item, 1);
		}

		if (objnum == ELECTRIC_CLEANER)
			SetupCleanerFromSavegame(item, 0);

		if (objnum == TROPICAL_FISH || objnum == PIRAHNAS)
		{
			if (item->hit_points != NO_ITEM)
				SetupShoal(item->hit_points);
		}

		if (item->object_number == BIGGUN)
			ReadSG(item->data, sizeof(BIGGUNINFO));

		if (item->object_number == BOAT)
			ReadSG(item->data, sizeof(BOAT_INFO));

		if (item->object_number == KAYAK)
			ReadSG(item->data, sizeof(KAYAKINFO));

		if (item->object_number == MINECART)
			ReadSG(item->data, sizeof(CARTINFO));

		if (item->object_number == QUADBIKE)
			ReadSG(item->data, sizeof(QUADINFO));

		if (item->object_number == UPV)
			ReadSG(item->data, sizeof(SUBINFO));

		if (item->object_number == AREA51_LASER)
			ReadSG(&item->shadeB, sizeof(short));

		if (item->object_number == MONKEY)
		{
			if (item->carried_item != NO_ITEM)
			{
				RemoveDrawnItem(item->carried_item);
				items[item->carried_item].room_number = NO_ROOM;
				items[item->carried_item].carried_item = NO_ITEM;
			}
		}
	}

	ReadSG(&bossdata, sizeof(BOSS_STRUCT));
	ReadSG(&lara, sizeof(LARA_INFO));
	lara.creature = 0;
	lara.spaz_effect = 0;

	for (int i = 0; i < 15; i++)
		lara.mesh_ptrs[i] = (short*)((long)lara.mesh_ptrs[i] + (long)mesh_base);

	lara.target = 0;
	lara.left_arm.frame_base = (short*)((long)lara.left_arm.frame_base + (long)frames);
	lara.right_arm.frame_base = (short*)((long)lara.right_arm.frame_base + (long)frames);

	if (lara.skidoo != NO_ITEM)
	{
		if (items[lara.skidoo].object_number == MINECART)
		{
			cart = (CARTINFO*)items[lara.skidoo].data;

			if (cart->Flags & 1)
				meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = meshes[objects[LARA_SKIN].mesh_index + HAND_R];
		}
		else if (items[lara.skidoo].object_number == KAYAK)
		{
			kayak = (KAYAKINFO*)items[lara.skidoo].data;

			if (kayak->Flags & 0x80)
				meshes[objects[VEHICLE_ANIM].mesh_index + HAND_R] = meshes[objects[LARA_SKIN].mesh_index + HAND_R];
		}
	}

	if (lara.weapon_item != NO_ITEM)
	{
		lara.weapon_item = CreateItem();
		item = &items[lara.weapon_item];
		ReadSG(&item->object_number, sizeof(short));
		ReadSG(&item->anim_number, sizeof(short));
		ReadSG(&item->frame_number, sizeof(short));
		ReadSG(&item->current_anim_state, sizeof(short));
		ReadSG(&item->goal_anim_state, sizeof(short));
		item->status = ITEM_ACTIVE;
		item->room_number = NO_ROOM;
	}

	if (lara.burn)
	{
		lara.burn = 0;
		LaraBurn();
	}

	ReadSG(&flipeffect, sizeof(long));
	ReadSG(&fliptimer, sizeof(long));
	ReadSG(&CurrentAtmosphere, sizeof(char));
	ReadSG(&compy_scared_timer, sizeof(long));
	ReadSG(&compys_attack_lara, sizeof(long));
	ReadSG(&CarcassItem, sizeof(short));
	ReadSG(&TribeBossShieldOn, sizeof(char));
	ReadSG(&TribeBossItem, sizeof(long));
	ReadSG(&lizard_man_active, sizeof(char));

	ReadSG(&nFlares, sizeof(long));

	for (int i = 0; i < nFlares; i++)
	{
		objnum = CreateItem();
		item = &items[objnum];
		item->object_number = FLARE_ITEM;
		ReadSG(&item->pos, sizeof(PHD_3DPOS));
		ReadSG(&item->room_number, sizeof(short));
		ReadSG(&item->speed, sizeof(short));
		ReadSG(&item->fallspeed, sizeof(short));
		InitialiseItem(objnum);
		AddActiveItem(objnum);
		ReadSG(&age, sizeof(long));
		item->data = (void*)age;
	}

	load_tomb3_data();
}

void save_tomb3_data()
{
	memcpy(tomb3_save.RoomsVisited, RoomVisited, 255);
	tomb3_save.dash_timer = DashTimer;
	tomb3_save.exposure_meter = ExposureMeter;
	memcpy(tomb3_save.fish_leaders, lead_info, sizeof(lead_info));
	memcpy(tomb3_save.fishies, fish, sizeof(fish));
	memcpy(tomb3_save.exp_rings, ExpRings, sizeof(ExpRings));
	memcpy(tomb3_save.kb_rings, KBRings, sizeof(KBRings));
	tomb3_save.lara_meshbits = lara_item->mesh_bits;
}

void load_tomb3_data()
{
	if (tomb3_save_size > offsetof(TOMB3_SAVE, RoomsVisited))
		memcpy(RoomVisited, tomb3_save.RoomsVisited, 255);

	if (tomb3_save_size > offsetof(TOMB3_SAVE, dash_timer))
		DashTimer = tomb3_save.dash_timer;

	if (tomb3_save_size > offsetof(TOMB3_SAVE, exposure_meter))
		ExposureMeter = tomb3_save.exposure_meter;

	if (tomb3_save_size > offsetof(TOMB3_SAVE, fish_leaders))
		memcpy(lead_info, tomb3_save.fish_leaders, sizeof(lead_info));

	if (tomb3_save_size > offsetof(TOMB3_SAVE, fishies))
		memcpy(fish, tomb3_save.fishies, sizeof(fish));

	if (tomb3_save_size > offsetof(TOMB3_SAVE, exp_rings))
		memcpy(ExpRings, tomb3_save.exp_rings, sizeof(ExpRings));

	if (tomb3_save_size > offsetof(TOMB3_SAVE, kb_rings))
		memcpy(KBRings, tomb3_save.kb_rings, sizeof(KBRings));

	if (tomb3_save_size > offsetof(TOMB3_SAVE, lara_meshbits))
		lara_item->mesh_bits = tomb3_save.lara_meshbits;
}
