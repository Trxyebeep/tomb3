#include "../tomb3/pch.h"
#include "setup.h"
#include "objects.h"
#include "gameflow.h"
#include "items.h"
#include "../specific/smain.h"
#include "../specific/file.h"
#include "laramisc.h"
#include "../specific/init.h"
#include "invfunc.h"
#include "text.h"
#include "health.h"
#include "../specific/output.h"
#include "sound.h"
#include "../specific/specific.h"
#include "lot.h"
#include "savegame.h"
#include "pickup.h"
#include "collide.h"
#ifdef TROYESTUFF
#include "../newstuff/map.h"
#include "../tomb3/tomb3.h"
#endif

void GetAIPickups()
{
	ITEM_INFO* item;
	ITEM_INFO* ai_item;
	short ai_item_number;

	for (int i = 0; i < level_items; i++)
	{
		item = &items[i];

		if (objects[item->object_number].intelligent)
		{
			for (ai_item_number = room[item->room_number].item_number; ai_item_number != NO_ITEM; ai_item_number = ai_item->next_item)
			{
				ai_item = &items[ai_item_number];

				if (item->pos.x_pos == ai_item->pos.x_pos && item->pos.z_pos == ai_item->pos.z_pos &&
					objects[ai_item->object_number].collision == AIPickupCollision && ai_item->object_number < AI_PATROL2)
				{
					item->ai_bits |= 1 << (ai_item->object_number - AI_GUARD);
					item->item_flags[3] = ai_item->pos.y_rot;

					if (!(ai_item->object_number == AI_PATROL1 &&
#ifdef RANDO_STUFF
						(rando.levels[RANDOLEVEL].original_id == LV_AREA51 || rando.levels[RANDOLEVEL].original_id == LV_COMPOUND)))
#else
						(CurrentLevel == LV_AREA51 || CurrentLevel == LV_COMPOUND)))
#endif
					{
						KillItem(ai_item_number);
						ai_item->room_number = NO_ROOM;
					}
				}
				else if (ai_item->object_number == FLAME_EMITTER && item->object_number == PUNK1)
				{
					item->item_flags[2] = 3;
					KillItem(ai_item_number);
					ai_item->room_number = NO_ROOM;
				}
			}
		}
	}
}

void GetCarriedItems()
{
	ITEM_INFO* item;
	ITEM_INFO* pickup;
	long lp;
	short pickup_number;

	for (lp = 0; lp < level_items; lp++)
	{
		item = &items[lp];

		if (!objects[item->object_number].intelligent)
			continue;

		item->carried_item = NO_ITEM;
		pickup_number = room[item->room_number].item_number;

		do
		{
			pickup = &items[pickup_number];

			if (pickup->pos.x_pos == item->pos.x_pos && pickup->pos.y_pos == item->pos.y_pos && pickup->pos.z_pos == item->pos.z_pos &&
				objects[pickup->object_number].collision == orig_PickUpCollision)
			{
				pickup->carried_item = item->carried_item;
				item->carried_item = pickup_number;
				RemoveDrawnItem(pickup_number);
				pickup->room_number = NO_ROOM;
			}

			pickup_number = pickup->next_item;

		} while (pickup_number != NO_ITEM);
	}
}

void InitialiseLevelFlags()
{
	CarcassItem = NO_ITEM;
	savegame.secrets = 0;
	savegame.timer = 0;
	savegame.kills = 0;
	savegame.distance_travelled = 0;
	savegame.ammo_used = 0;
	savegame.ammo_hit = 0;
	savegame.health_used = 0;
	assault_timer_active = 0;
	QuadbikeLapTimeDisplayTimer = 0;
	QuadbikeLapTime = 0;
	compy_scared_timer = 0;
	compys_attack_lara = 0;
}

void InitialiseGameFlags()
{
	flip_status = 0;
	memset(flipmap, 0, sizeof(flipmap));
	memset(cd_flags, 0, sizeof(cd_flags));

	for (int i = 0; i < NUMBER_OBJECTS; i++)
		objects[i].loaded = 0;

	SunsetTimer = 0;
	ammotext = 0;
	level_complete = 0;
	flipeffect = -1;
	CarcassItem = NO_ITEM;
	compys_attack_lara = 0;
}

#ifdef TROYESTUFF
static long DoLevelLoad(long level, long type)
{
	char name[128];

	if (!type)
		strcpy(name, GF_titlefilenames[0]);
	else if (type == 2 || type != 4)
		strcpy(name, GF_levelfilenames[level]);
	else
		strcpy(name, GF_cutscenefilenames[level]);

	if (tomb3.gold && type != 4)
		T3_GoldifyString(name);

	if (!S_LoadLevelFile(name, level, type))
		return 0;

	return 1;
}
#endif

long InitialiseLevel(long level, long type)
{
	ITEM_INFO* item;

	IsAtmospherePlaying = 0;

	if (type && type != 4)
		CurrentLevel = level;

	if (type == 3)
		DemoPlay = 1;
	else
		DemoPlay = 0;

	InitialiseGameFlags();
	lara.item_number = NO_ITEM;
	title_loaded = 0;

#ifdef TROYESTUFF
	if (!DoLevelLoad(level, type))
		return 0;
#else
	if (!type)
	{
		if (!S_LoadLevelFile(GF_titlefilenames[0], level, 0))
			return 0;
	}
	else if (type == 2 || type != 4)
	{
		if (!S_LoadLevelFile(GF_levelfilenames[level], level, type))
			return 0;
	}
	else
	{
		if (!S_LoadLevelFile(GF_cutscenefilenames[level], level, 4))
			return 0;
	}
#endif

	if (lara.item_number != NO_ITEM)
		InitialiseLara(type);

	if (type == 1 || type == 2 || type == 3)
	{
		GetCarriedItems();
		GetAIPickups();
	}

	effects = (FX_INFO*)game_malloc(sizeof(FX_INFO) * 50, 32);
	InitialiseFXArray();
	InitialiseLOTarray();
	InitColours();
	T_InitPrint();
	InitialisePickUpDisplay();
	S_InitialiseScreen(type);
	health_bar_timer = 100;
	SOUND_Stop();

#ifdef TROYESTUFF
	for (int i = 0; i < 255; i++)
		RoomVisited[i] = 0;
#endif

	if (type == 2)
		ExtractSaveGameInfo();
	else
		GF_ModifyInventory(CurrentLevel, 0);

	if (objects[FINAL_LEVEL].loaded)
		InitialiseFinalLevel();

#ifdef TROYESTUFF
	if (type == 2)	//mid_level_save is not used on PC (right now), on PSX it means any save that's NOT from the level stats prompt
					//so on PC this check will never pass, although it should since PC cannot save other than "mid level".
#else
	if (type == 2 && savegame.mid_level_save)
#endif
	{
		S_CDPlay(CurrentAtmosphere, 1);
		IsAtmospherePlaying = 1;
	}
	else if ((type == 1 || type == 2 || type == 3) && CurrentLevel != LV_GYM)
	{
		if (GF_CDtracks[0])
		{
			S_CDPlay(GF_CDtracks[0], 1);
			CurrentAtmosphere = (char)GF_CDtracks[0];
			IsAtmospherePlaying = 1;
		}
	}
	
	if (CurrentLevel == LV_GYM && savegame.QuadbikeKeyFlag)
	{
		Inv_AddItem(KEY_ITEM1);

		for (int i = 0; i < level_items; i++)
		{
			item = &items[i];

			if (item->object_number == KEY_ITEM1)
			{
				item->status = ITEM_INVISIBLE;
				RemoveDrawnItem(i);
				break;
			}
		}
	}

	assault_timer_active = 0;
	assault_timer_display = 0;
	camera.underwater = 0;

#ifdef TROYESTUFF
	for (int i = 0; i < 32; i++)
		FootPrint[i].Active = 0;

	FootPrintNum = 0;
#endif

	return 1;
}

void inject_setup(bool replace)
{
	INJECT(0x00466590, GetAIPickups, inject_rando ? 1 : replace);
	INJECT(0x004664B0, GetCarriedItems, replace);
	INJECT(0x00463B70, InitialiseLevelFlags, replace);
	INJECT(0x00463B00, InitialiseGameFlags, replace);
	INJECT(0x004638F0, InitialiseLevel, replace);
}
