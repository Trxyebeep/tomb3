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
#include "../specific/winmain.h"
#include "draw.h"
#include "cobra.h"
#include "monkey.h"
#include "autogun.h"
#include "diver.h"
#include "orca.h"
#include "trex.h"
#include "raptor.h"
#include "dog.h"
#include "rat.h"
#include "compy.h"
#include "triboss.h"
#include "tonyboss.h"
#include "londboss.h"
#include "willboss.h"
#include "tiger.h"
#include "shiva.h"
#include "oilsmg.h"
#include "clawmute.h"
#include "hybrid.h"
#include "sealmute.h"
#include "swat.h"
#include "armysmg.h"
#include "lizman.h"
#include "51baton.h"
#include "51civvy.h"
#include "prisoner.h"
#include "flamer.h"
#include "tribeaxe.h"
#include "blowpipe.h"
#include "wingmute.h"
#include "bird.h"
#include "oilred.h"
#include "londsec.h"
#include "cleaner.h"
#include "mpgun.h"
#include "punk.h"
#include "croc.h"
#include "winston.h"
#include "target.h"
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

void BuildOutsideTable()
{
	ROOM_INFO* r;
	uchar* pTable;
	uchar* oTable;
	uchar* cTable;
	long max_slots, roomx, roomy, cont, offset, z, z2;
	long x, y, i, rx, ry, lp;

	max_slots = 0;
	OutsideRoomTable = (char*)game_malloc(0xB640, 0);
	memset(OutsideRoomTable, 0xFF, 0xB640);
	printf("X %d, Y %d, Z %d, Xs %d, Ys %d\n", room->x, room->y, room->z, room->x_size, room->y_size);

	for (y = 0; y < 108; y += 4)
	{
		for (x = 0; x < 108; x += 4)
		{
			for (i = 0; i < number_rooms; i++)
			{
				r = &room[i];

				roomx = (r->z >> WALL_SHIFT) + 1;
				roomy = (r->x >> WALL_SHIFT) + 1;
				cont = 0;

				for (ry = 0; ry < 4; ry++)
				{
					for (rx = 0; rx < 4; rx++)
					{
						if (x + rx >= roomx && x + rx < roomx + r->x_size - 2 && y + ry >= roomy && y + ry < roomy + r->y_size - 2)
						{
							cont = 1;
							break;
						}
					}
				}

				if (!cont)
					continue;

				pTable = (uchar*)&OutsideRoomTable[64 * ((x >> 2) + 27 * (y >> 2))];

				for (lp = 0; lp < 64; lp++)
				{
					if (pTable[lp] == 255)
					{
						pTable[lp] = (uchar)i;

						if (lp > max_slots)
							max_slots = lp;

						break;
					}
				}
			}
		}
	}

	oTable = (uchar*)OutsideRoomTable;

	for (y = 0; y < 27; y++)
	{
		for (x = 0; x < 27; x++)
		{
			z = 0;
			offset = x + y * 27;
			pTable = (uchar*)&OutsideRoomTable[64 * (x + y * 27)];
			while (pTable[z] != 255) z++;

			if (!z)
				OutsideRoomOffsets[offset] = -1;
			else if (z == 1)
				OutsideRoomOffsets[offset] = *pTable | 0x8000;
			else
			{
				cTable = (uchar*)OutsideRoomTable;

				while (cTable < oTable)
				{
					if (!memcmp(cTable, pTable, z))
					{
						OutsideRoomOffsets[offset] = short((long)cTable - (long)OutsideRoomTable);
						break;
					}

					z2 = 0;
					while (cTable[z2] != 255) z2++;
					cTable += z2 + 1;
				}

				if (cTable >= oTable)
				{
					OutsideRoomOffsets[offset] = short((long)oTable - (long)OutsideRoomTable);

					do
					{
						*oTable++ = *pTable++;
						z--;

					} while (z);

					*oTable++ = 255;
				}
			}
		}
	}

	game_free(((long)OutsideRoomTable - (long)oTable + 0xB643) & ~3, 0);	//free unused space (get rid of this asap btw)
	printf("Ouside room table = %d bytes, max_slots = %d\n", (long)oTable - (long)OutsideRoomTable, max_slots);
}

static void BaddyObjects()
{
	OBJECT_INFO* obj;

	obj = &objects[LARA];
	obj->initialise = InitialiseLaraLoad;
	obj->draw_routine = DrawDummyItem;
	obj->shadow_size = 160;
	obj->hit_points = 1000;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[LARA_EXTRA];
	obj->control = ControlLaraExtra;

	obj = &objects[COBRA];

	if (obj->loaded)
	{
		obj->initialise = InitialiseCobra;
		obj->control = CobraControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 8;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->non_lot = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 24] |= 8;
	}

	obj = &objects[MONKEY];

	if (obj->loaded)
	{
		if (!objects[MESHSWAP2].loaded)
			S_ExitSystem("FATAL: Monkey requires MESHSWAP2 (Monkey + Pickups)");

		obj->initialise = InitialiseMonkey;
		obj->control = MonkeyControl;
		obj->collision = CreatureCollision;
		obj->draw_routine = DrawMonkey;
		obj->shadow_size = 128;
		obj->hit_points = 8;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 0x10;
		bones[obj->bone_index + 28] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[ROBOT_SENTRY_GUN];

	if (obj->loaded)
	{
		obj->initialise = InitialiseAutogun;
		obj->control = AutogunControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 0;
		obj->hit_points = 100;
		obj->radius = 102;
		obj->bite_offset = AUTOGUN_LEFT_BITE;
		obj->intelligent = 1;
		obj->non_lot = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index + 4] |= 4;
	}

	obj = &objects[DIVER];

	if (obj->loaded)
	{
		obj->control = DiverControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 20;
		obj->radius = 341;
		obj->pivot_length = 50;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
	}

	obj = &objects[WHALE];

	if (obj->loaded)
	{
		obj->control = OrcaControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = DONT_TARGET;
		obj->pivot_length = 200;
		obj->radius = 341;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
	}

	obj = &objects[TREX];

	if (obj->loaded)
	{
		obj->control = DinoControl;
		obj->collision = CreatureCollision;
		obj->hit_points = 800;
		obj->shadow_size = 128;
		obj->pivot_length = 1800;
		obj->radius = 512;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 36] |= 8;
		bones[obj->bone_index + 44] |= 8;
		bones[obj->bone_index + 80] |= 8;
		bones[obj->bone_index + 88] |= 8;
	}

	obj = &objects[RAPTOR];

	if (obj->loaded)
	{
		obj->control = RaptorControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 90;
		obj->pivot_length = 600;
		obj->radius = 341;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 80] |= 8;
		bones[obj->bone_index + 84] |= 8;
		bones[obj->bone_index + 92] |= 8;
		bones[obj->bone_index + 100] |= 8;
	}

	obj = &objects[HUSKIE];

	if (obj->loaded)
	{
		obj->initialise = InitialiseDog;
		obj->control = DogControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 16;
		obj->pivot_length = 300;
		obj->radius = 341;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 8] |= 8;
		bones[obj->bone_index + 8] |= 4;
	}

	obj = &objects[SMALL_RAT];

	if (obj->loaded)
	{
		obj->control = MouseControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 4;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 12] |= 8;
	}

	obj = &objects[COMPY];

	if (obj->loaded)
	{
		obj->initialise = InitialiseCompy;
		obj->control = CompyControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 85;
		obj->hit_points = 10;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->non_lot = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 4] |= 8;
		bones[obj->bone_index + 8] |= 8;
	}

	obj = &objects[TRIBEBOSS];

	if (obj->loaded)
	{
		obj->initialise = InitialiseTribeBoss;
		obj->control = TribeBossControl;
		obj->collision = CreatureCollision;
		obj->draw_routine = S_DrawTribeBoss;
		obj->shadow_size = 0;
		obj->hit_points = 200;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 16] |= 8;
		bones[obj->bone_index + 28] |= 0xC;
	}

	obj = &objects[TONY];

	if (obj->loaded)
	{
		obj->initialise = InitialiseTonyBoss;
		obj->control = TonyBossControl;
		obj->collision = CreatureCollision;
		obj->draw_routine = S_DrawTonyBoss;
		obj->shadow_size = 0;
		obj->hit_points = 200;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[LON_BOSS];

	if (obj->loaded)
	{
		obj->initialise = InitialiseLondonBoss;
		obj->control = LondonBossControl;
		obj->collision = CreatureCollision;
		obj->draw_routine = S_DrawLondonBoss;
		obj->shadow_size = 0;
		obj->hit_points = 300;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[WILLARD_BOSS];

	if (obj->loaded)
	{
		obj->initialise = InitialiseWillBoss;
		obj->control = WillBossControl;
		obj->collision = CreatureCollision;
		obj->draw_routine = S_DrawWillBoss;
		obj->shadow_size = 128;
		obj->hit_points = 200;
		obj->pivot_length = 50;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[TIGER];

	if (obj->loaded)
	{
		obj->control = TigerControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->pivot_length = 200;
		obj->radius = 341;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 84] |= 8;
	}

	obj = &objects[SHIVA];

	if (obj->loaded)
	{
		if (!objects[MESHSWAP1].loaded)
			S_ExitSystem("FATAL: Shiva requires MESHSWAP1 (Shiva statue)");

		obj->initialise = InitialiseShiva;
		obj->control = ShivaControl;
		obj->collision = CreatureCollision;
		obj->draw_routine = DrawShiva;
		obj->shadow_size = 128;
		obj->hit_points = 100;
		obj->radius = 341;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 100] |= 8;
		bones[obj->bone_index + 100] |= 4;
	}

	obj = &objects[WHITE_SOLDIER];

	if (obj->loaded)
	{
		obj->initialise = InitialiseOilSMG;
		obj->control = OilSMGControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 30;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = WHITE_SOLDIER_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[MUTANT2];

	if (obj->loaded)
	{
		obj->control = ClawmuteControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 130;
		obj->radius = 256;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 0x10;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[MUTANT3];

	if (obj->loaded)
	{
		obj->control = HybridControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 90;
		obj->radius = 128;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 0x10;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[BURNT_MUTANT];

	if (obj->loaded)
	{
		obj->control = SealmuteControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 50;
		obj->radius = 204;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 32] |= 0x10;
		bones[obj->bone_index + 32] |= 4;
		bones[obj->bone_index + 36] |= 8;
	}

	obj = &objects[SWAT_GUN];

	if (obj->loaded)
	{
		obj->initialise = InitialiseSwat;
		obj->control = SwatControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 45;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = SWAT_GUN_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[STHPAC_MERCENARY];

	if (obj->loaded)
	{
		obj->initialise = InitialiseArmySMG;
		obj->control = ArmySMGControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 30;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = ARMYSMG_GUN_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[LIZARD_MAN];

	if (obj->loaded)
	{
		obj->control = LizManControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 36;
		obj->radius = 204;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 4] |= 0x10;
		bones[obj->bone_index + 36] |= 0x10;
	}

	obj = &objects[MP1];

	if (obj->loaded)
	{
		obj->initialise = InitialiseBaton;
		obj->control = BatonControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 25;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[CIVVIE];

	if (obj->loaded)
	{
		obj->initialise = InitialiseCivvy;
		obj->control = CivvyControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 15;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[BOB];

	if (obj->loaded)
	{
		obj->initialise = InitialisePrisoner;
		obj->control = PrisonerControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 20;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[FLAMETHROWER_BLOKE];

	if (obj->loaded)
	{
		obj->control = FlamerControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 36;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[TRIBEAXE];

	if (obj->loaded)
	{
		obj->control = TribeAxeControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 28;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 24] |= 8;
	}

	obj = &objects[BLOWPIPE];

	if (obj->loaded)
	{
		obj->control = BlowpipeControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 28;
		obj->radius = 102;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
		bones[obj->bone_index + 52] |= 4;
	}

	obj = &objects[MUTANT1];

	if (obj->loaded)
	{
		obj->initialise = InitialiseWingmute;
		obj->control = WingmuteControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 24;
		obj->radius = 204;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[VULTURE];

	if (obj->loaded)
	{
		obj->initialise = InitialiseVulture;
		obj->control = VultureControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 18;
		obj->radius = 204;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[CROW];

	if (obj->loaded)
	{
		obj->initialise = InitialiseVulture;
		obj->control = VultureControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 8;
		obj->radius = 204;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[OILRED];

	if (obj->loaded)
	{
		obj->control = OilRedControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 34;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = OILRED_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[SECURITY_GUARD];

	if (obj->loaded)
	{
		obj->control = LondSecControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 28;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = OILRED_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[ELECTRIC_CLEANER];

	if (obj->loaded)
	{
		obj->initialise = InitialiseCleaner;
		obj->control = CleanerControl;
		obj->collision = ObjectCollision;
		obj->shadow_size = 128;
		obj->hit_points = DONT_TARGET;
		obj->radius = 512;
		obj->intelligent = 1;
		obj->non_lot = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[MP2];

	if (obj->loaded)
	{
		obj->control = MPGunControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 28;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = OILRED_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[LON_MERCENARY1];

	if (obj->loaded)
	{
		obj->initialise = InitialiseSwat;
		obj->control = SwatControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 45;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->bite_offset = SWAT_GUN_BITE;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index] |= 8;
		bones[obj->bone_index] |= 4;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[PUNK1];

	if (obj->loaded)
	{
		obj->initialise = InitialisePunk;
		obj->control = PunkControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 20;
		obj->radius = 102;
		obj->pivot_length = 0;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		bones[obj->bone_index + 24] |= 8;
		bones[obj->bone_index + 24] |= 4;
		bones[obj->bone_index + 52] |= 8;
	}

	obj = &objects[CROCODILE];

	if (obj->loaded)
	{
		obj->control = CrocControl;
		obj->collision = CreatureCollision;
		obj->shadow_size = 128;
		obj->hit_points = 42;
		obj->radius = 341;
		obj->pivot_length = 200;
		obj->intelligent = 1;
		obj->save_position = 1;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
		obj->water_creature = 1;
		bones[obj->bone_index + 28] |= 8;
	}

	obj = &objects[WINSTON];

	if (obj->loaded)
	{
		obj->control = OldWinstonControl;
		obj->collision = ObjectCollision;
		obj->hit_points = 20;
		obj->shadow_size = 64;
		obj->radius = 102;
		obj->intelligent = 1;
	}

	obj = &objects[ARMY_WINSTON];

	if (obj->loaded)
	{
		obj->control = WinstonControl;
		obj->collision = ObjectCollision;
		obj->hit_points = 20;
		obj->shadow_size = 64;
		obj->radius = 102;
		obj->intelligent = 1;
	}

	obj = &objects[TARGETS];

	if (obj->loaded)
	{
		obj->control = TargetControl;
		obj->collision = ObjectCollision;
		obj->hit_points = 8;
		obj->shadow_size = 128;
		obj->radius = 102;
		obj->intelligent = 1;
	}

	obj = &objects[AI_GUARD];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_AMBUSH];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_PATROL1];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_PATROL2];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_FOLLOW];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_X1];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_X2];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_X3];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}

	obj = &objects[AI_MODIFY];

	if (obj->loaded)
	{
		obj->draw_routine = DrawDummyItem;
		obj->collision = AIPickupCollision;
		obj->hit_points = 0;
	}
}

void inject_setup(bool replace)
{
	INJECT(0x00466590, GetAIPickups, inject_rando ? 1 : replace);
	INJECT(0x004664B0, GetCarriedItems, replace);
	INJECT(0x00463B70, InitialiseLevelFlags, replace);
	INJECT(0x00463B00, InitialiseGameFlags, replace);
	INJECT(0x004638F0, InitialiseLevel, replace);
	INJECT(0x004666C0, BuildOutsideTable, replace);
	INJECT(0x00463C30, BaddyObjects, replace);
}
