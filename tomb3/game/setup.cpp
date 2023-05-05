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
#include "effect2.h"
#include "traps.h"
#include "lara.h"
#include "moveblok.h"
#include "rapmaker.h"
#include "flymaker.h"
#include "boomute.h"
#include "51rocket.h"
#include "51laser.h"
#include "../specific/draweffects.h"
#include "firehead.h"
#include "fusebox.h"
#include "lara1gun.h"
#include "missile.h"
#include "kayak.h"
#include "boat.h"
#include "quadbike.h"
#include "minecart.h"
#include "biggun.h"
#include "sub.h"
#include "train.h"
#include "laraflar.h"
#include "fish.h"
#include "objlight.h"
#include "cinema.h"
#include "effects.h"
#include "dragfire.h"
#include "lasers.h"
#include "hair.h"
#include "control.h"
#include "demo.h"
#include "camera.h"
#include "footprnt.h"
#include "../newstuff/map.h"
#include "../tomb3/tomb3.h"

short IsRoomOutsideNo;
char* OutsideRoomTable;
short OutsideRoomOffsets[729];
BOSS_STRUCT bossdata;

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

					if (!(ai_item->object_number == AI_PATROL1 && (CurrentLevel == LV_AREA51 || CurrentLevel == LV_COMPOUND)))
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
				objects[pickup->object_number].collision == PickUpCollision)
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

	ammotext = 0;
	level_complete = 0;
	flipeffect = -1;
	CarcassItem = NO_ITEM;
	compys_attack_lara = 0;
}

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

	if (!DoLevelLoad(level, type))
		return 0;

	if (lara.item_number != NO_ITEM)
		InitialiseLara(type);

	if (type == 1 || type == 2 || type == 3)
	{
		GetCarriedItems();
		GetAIPickups();
	}

	effects = (FX_INFO*)game_malloc(sizeof(FX_INFO) * 50);
	InitialiseFXArray();
	InitialiseLOTarray();
	T_InitPrint();
	InitialisePickUpDisplay();
	S_InitialiseScreen(type);
	health_bar_timer = 100;
	SOUND_Stop();

	for (int i = 0; i < 255; i++)
		RoomVisited[i] = 0;

	if (type == 2)
		ExtractSaveGameInfo();
	else
		GF_ModifyInventory(CurrentLevel, 0);

	if (objects[FINAL_LEVEL].loaded)
		InitialiseFinalLevel();

	if (type == 2)
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

	for (int i = 0; i < 32; i++)
		FootPrint[i].Active = 0;

	FootPrintNum = 0;
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
	OutsideRoomTable = (char*)game_malloc(0xB640);
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

	game_free(((long)OutsideRoomTable - (long)oTable + 0xB643) & ~3);	//free unused space (get rid of this asap btw)
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

	obj = &objects[DOG];

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

static void TrapObjects()
{
	OBJECT_INFO* obj;

	obj = &objects[KILL_ALL_TRIGGERS];
	obj->control = KillAllCurrentItems;
	obj->draw_routine = DrawDummyItem;
	obj->hit_points = 0;
	obj->save_flags = 1;

	obj = &objects[MINI_COPTER];
	obj->control = MiniCopterControl;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[MOVING_BAR];
	obj->control = GeneralControl;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->water_creature = 1;

	obj = &objects[DEATH_SLIDE];
	obj->initialise = InitialiseRollingBall;
	obj->control = ControlDeathSlide;
	obj->collision = DeathSlideCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPIKE_WALL];
	obj->control = ControlSpikeWall;
	obj->collision = ObjectCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[CEILING_SPIKES];
	obj->control = ControlCeilingSpikes;
	obj->collision = TrapCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[HOOK];
	obj->control = HookControl;
	obj->collision = CreatureCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
	
	obj = &objects[SAW];
	obj->control = PropellerControl;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[FAN];
	obj->control = PropellerControl;
	obj->collision = TrapCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->water_creature = 1;

	obj = &objects[SMALL_FAN];
	obj->control = PropellerControl;
	obj->collision = TrapCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPINNING_BLADE];
	obj->initialise = InitialiseKillerStatue;
	obj->control = SpinningBlade;
	obj->collision = ObjectCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[ICICLES];
	obj->control = IcicleControl;
	obj->collision = TrapCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[BLADE];
	obj->initialise = InitialiseBlade;
	obj->control = BladeControl;
	obj->collision = TrapCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPRING_BOARD];
	obj->control = SpringBoardControl;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[FALLING_BLOCK];
	obj->control = FallingBlock;
	obj->floor = FallingBlockFloor;
	obj->ceiling = FallingBlockCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[FALLING_BLOCK2];
	obj->control = FallingBlock;
	obj->floor = FallingBlockFloor;
	obj->ceiling = FallingBlockCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[FALLING_PLANK];
	obj->control = FallingBlock;
	obj->floor = FallingBlockFloor;
	obj->ceiling = FallingBlockCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PENDULUM];
	obj->control = Pendulum;
	obj->collision = ObjectCollision;
	obj->shadow_size = 128;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SWING_BOX];
	obj->control = Pendulum;
	obj->collision = ObjectCollision;
	obj->shadow_size = 128;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TEETH_TRAP];
	obj->control = TeethTrap;
	obj->collision = TrapCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[AVALANCHE];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[OILDRUMS];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[ROLLING_BALL];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[BIG_ROLLING_BALL];
	obj->initialise = InitialiseRollingBall;
	obj->control = RollingBallControl;
	obj->collision = RollingBallCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPIKES];
	obj->control = SpikeControl;
	obj->collision = SpikeCollision;

	obj = &objects[FALLING_CEILING1];
	obj->control = FallingCeiling;
	obj->collision = TrapCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (int i = MOVABLE_BLOCK; i <= MOVABLE_BLOCK4; i++)
	{
		obj = &objects[i];
		obj->initialise = InitialiseMovingBlock;
		obj->control = MovableBlock;
		obj->collision = MovableBlockCollision;
		obj->draw_routine = DrawMovableBlock;
		obj->save_position = 1;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[DART_EMITTER];
	obj->control = DartEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[HOMING_DART_EMITTER];
	obj->control = DartEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[RAPTOR_EMITTER];
	obj->initialise = InitialiseRaptorEmitter;
	obj->control = RaptorEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[FLYING_MUTANT_EMITTER];
	obj->control = FlyEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->hit_points = 150;
	obj->save_flags = 1;

	obj = &objects[BOO_MUTANT];
	obj->control = BoomuteControl;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPECIAL_FX1];
	obj->control = ControlArea51Rocket;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPECIAL_FX2];
	obj->control = ControlArea51Rocket;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SPECIAL_FX3];
	obj->initialise = InitialiseArea51Struts;
	obj->control = ControlArea51Struts;
	obj->save_anim = 1;

	obj = &objects[AREA51_LASER];
	obj->initialise = InitialiseArea51Laser;
	obj->control = ControlArea51Laser;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[DARTS];
	obj->control = DartsControl;
	obj->collision = ObjectCollision;
	obj->draw_routine = S_DrawDarts;
	obj->shadow_size = 128;

	obj = &objects[FLAME_EMITTER];
	obj->control = FlameEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[FLAME_EMITTER2];
	obj->control = FlameEmitter2Control;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[FLAME_EMITTER3];
	obj->control = FlameEmitter3Control;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[SIDE_FLAME_EMITTER];
	obj->control = SideFlameEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[FLAME];
	obj->control = FlameControl;
	obj->draw_routine = DrawDummyItem;
}

static void ObjectObjects()
{
	OBJECT_INFO* obj;
	long lp;

	obj = &objects[CAMERA_TARGET];
	obj->draw_routine = DrawDummyItem;

	obj = &objects[FIREHEAD];
	obj->initialise = InitialiseFireHead;
	obj->control = ControlFireHead;
	obj->collision = ObjectCollision;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TONYFIREBALL];
	obj->control = ControlTonyFireBall;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[EXTRAFX1];
	obj->control = ControlClawmutePlasmaBall;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[EXTRAFX2];
	obj->control = ControlWillbossPlasmaBall;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[EXTRAFX3];
	obj->control = ControlRotateyThing;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[EXTRAFX4];
	obj->control = ControlLaserBolts;
	obj->draw_routine = S_DrawLaserBolts;

	obj = &objects[EXTRAFX5];
	obj->control = ControlLondBossPlasmaBall;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[EXTRAFX6];
	obj->control = ControlFusebox;
	obj->collision = ObjectCollision;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[ROCKET];
	obj->control = ControlRocket;

	obj = &objects[GRENADE];
	obj->control = ControlGrenade;

	obj = &objects[HARPOON_BOLT];
	obj->control = ControlHarpoonBolt;

	obj = &objects[KNIFE];
	obj->control = ControlMissile;

	obj = &objects[DIVER_HARPOON];
	obj->control = ControlMissile;

	obj = &objects[KAYAK];
	obj->initialise = KayakInitialise;
	obj->collision = KayakCollision;
	obj->draw_routine = KayakDraw;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[BOAT];
	obj->initialise = InitialiseBoat;
	obj->control = BoatControl;
	obj->collision = BoatCollision;
	obj->draw_routine = DrawBoat;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;
	bones[obj->bone_index + 4] |= 0x10;

	obj = &objects[QUADBIKE];
	obj->initialise = InitialiseQuadBike;
	obj->collision = QuadBikeCollision;
	obj->draw_routine = QuadBikeDraw;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[MINECART];
	obj->initialise = MineCartInitialise;
	obj->collision = MineCartCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[BIGGUN];
	obj->initialise = BigGunInitialise;
	obj->collision = BigGunCollision;
	obj->draw_routine = BigGunDraw;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[UPV];
	obj->initialise = SubInitialise;
	obj->control = SubEffects;
	obj->collision = SubCollision;
	obj->draw_routine = SubDraw;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TRAIN];
	obj->control = TrainControl;
	obj->collision = TrainCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[FLARE_ITEM];
	obj->control = FlareControl;
	obj->collision = PickUpCollision;
	obj->draw_routine = DrawFlareInAir;
	obj->pivot_length = 256;
	obj->hit_points = 256;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[SMASH_WINDOW];
	obj->initialise = InitialiseWindow;
	obj->control = WindowControl;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SMASH_OBJECT1];
	obj->initialise = InitialiseWindow;
	obj->control = WindowControl;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SMASH_OBJECT2];
	obj->initialise = InitialiseWindow;
	obj->control = WindowControl;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SMASH_OBJECT3];
	obj->initialise = InitialiseWindow;
	obj->control = WindowControl;
	obj->collision = ObjectCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[CARCASS];
	obj->control = CarcassControl;
	obj->collision = ObjectCollision;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[LIFT];
	obj->initialise = InitialiseLift;
	obj->control = LiftControl;
	obj->floor = LiftFloor;
	obj->ceiling = LiftCeiling;
	obj->save_position = 1;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TROPICAL_FISH];
	obj->control = ControlFish;
	obj->draw_routine = S_DrawFish;
	obj->hit_points = -1;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;

	obj = &objects[PIRAHNAS];
	obj->control = ControlFish;
	obj->draw_routine = S_DrawFish;
	obj->hit_points = -1;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_flags = 1;

	obj = &objects[BAT_EMITTER];
	obj->control = BatEmitterControl;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[LIGHTNING_EMITTER2];
	obj->control = ControlElectricFence;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[STROBE_LIGHT];
	obj->control = ControlStrobeLight;
	obj->save_flags = 1;

	obj = &objects[PULSE_LIGHT];
	obj->control = ControlPulseLight;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[ON_OFF_LIGHT];
	obj->control = ControlOnOffLight;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[ELECTRICAL_LIGHT];
	obj->control = ControlElectricalLight;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[EXTRA_LIGHT1];
	obj->control = ControlBeaconLight;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[BRIDGE_FLAT];
	obj->floor = BridgeFlatFloor;
	obj->ceiling = BridgeFlatCeiling;

	obj = &objects[BRIDGE_TILT1];
	obj->floor = BridgeTilt1Floor;
	obj->ceiling = BridgeTilt1Ceiling;

	obj = &objects[BRIDGE_TILT2];
	obj->floor = BridgeTilt2Floor;
	obj->ceiling = BridgeTilt2Ceiling;

	obj = &objects[DRAW_BRIDGE];

	if (obj->loaded)
	{
		obj->control = GeneralControl;
		obj->collision = DrawBridgeCollision;
		obj->floor = DrawBridgeFloor;
		obj->ceiling = DrawBridgeCeiling;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[SMALL_SWITCH];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PUSH_SWITCH];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[AIRLOCK_SWITCH];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SWITCH_TYPE1];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[SWITCH_TYPE2];
	obj->control = SwitchControl;
	obj->collision = SwitchCollision2;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (lp = DOOR_TYPE1; lp <= DOOR_TYPE8; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseDoor;
		obj->control = DoorControl;
		obj->collision = DoorCollision;
		obj->draw_routine = DrawUnclippedItem;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	obj = &objects[TRAPDOOR];
	obj->control = TrapDoorControl;
	obj->floor = TrapDoorFloor;
	obj->ceiling = TrapDoorCeiling;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[TRAPDOOR2];
	obj->control = TrapDoorControl;
	obj->floor = TrapDoorFloor;
	obj->ceiling = TrapDoorCeiling;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PICKUP_ITEM1];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[PICKUP_ITEM2];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KEY_ITEM1];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KEY_ITEM2];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KEY_ITEM3];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KEY_ITEM4];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[PUZZLE_ITEM1];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[PUZZLE_ITEM2];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[PUZZLE_ITEM3];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[PUZZLE_ITEM4];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[GUN_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[SHOTGUN_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[HARPOON_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ROCKET_GUN_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[GRENADE_GUN_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[M16_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[MAGNUM_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[UZI_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[FLAREBOX_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[GUN_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[SG_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[MAG_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[UZI_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[HARPOON_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[M16_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ROCKET_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[GRENADE_AMMO_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[MEDI_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[BIGMEDI_ITEM];
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ICON_PICKUP1_ITEM];
	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ICON_PICKUP2_ITEM];
	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ICON_PICKUP3_ITEM];
	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[ICON_PICKUP4_ITEM];
	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[SAVEGAME_CRYSTAL_ITEM];
	obj->control = AnimatingPickUp;
	obj->collision = PickUpCollision;
	obj->save_position = 1;
	obj->save_flags = 1;

	obj = &objects[KEY_HOLE1];
	obj->collision = KeyHoleCollision;
	obj->save_flags = 1;

	obj = &objects[KEY_HOLE2];
	obj->collision = KeyHoleCollision;
	obj->save_flags = 1;

	obj = &objects[KEY_HOLE3];
	obj->collision = KeyHoleCollision;
	obj->save_flags = 1;

	obj = &objects[KEY_HOLE4];
	obj->collision = KeyHoleCollision;
	obj->save_flags = 1;

	obj = &objects[DETONATOR];
	obj->control = DetonatorControl;
	obj->collision = DetonatorCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PUZZLE_HOLE1];
	obj->control = ControlAnimating_1_4;
	obj->collision = PuzzleHoleCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PUZZLE_HOLE2];
	obj->control = ControlAnimating_1_4;
	obj->collision = PuzzleHoleCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PUZZLE_HOLE3];
	obj->control = ControlAnimating_1_4;
	obj->collision = PuzzleHoleCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[PUZZLE_HOLE4];
	obj->control = ControlAnimating_1_4;
	obj->collision = PuzzleHoleCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;

	for (lp = PUZZLE_DONE1; lp <= PUZZLE_DONE4; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlAnimating_1_4;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = PLAYER_1; lp <= PLAYER_10; lp++)
	{
		obj = &objects[lp];
		obj->initialise = InitialiseGenPlayer;
		obj->control = ControlCinematicPlayer;
		obj->hit_points = 1;
	}

	for (lp = ANIMATING1; lp <= ANIMATING6; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlAnimating_1_4;
		obj->collision = ObjectCollision;
		obj->save_flags = 1;
		obj->save_anim = 1;
	}

	for (lp = SMOKE_EMITTER_WHITE; lp <= STEAM_EMITTER; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlSmokeEmitter;
		obj->draw_routine = DrawDummyItem;
		obj->save_flags = 1;
	}

	obj = &objects[GHOST_GAS_EMITTER];
	obj->control = ControlGhostGasEmitter;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	for (lp = RED_LIGHT; lp <= WHITE_LIGHT; lp++)
	{
		obj = &objects[lp];
		obj->control = ControlColouredLights;
		obj->draw_routine = DrawDummyItem;
		obj->save_flags = 1;
	}

	obj = &objects[BUBBLES1];
	obj->control = ControlBubble1;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[DRAGON_FIRE];
	obj->control = ControlFlameThrower;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[WATERFALL];
	obj->control = WaterFall;
	obj->draw_routine = DrawDummyItem;

	for (lp = SECURITY_LASER_ALARM; lp <= SECURITY_LASER_KILLER; lp++)
	{
		obj = &objects[lp];
		obj->initialise = LaserControl;
		obj->control = LaserControl;
		obj->draw_routine = S_DrawLaser;
		obj->save_hitpoints = 1;
		obj->save_flags = 1;
	}

	obj = &objects[BODY_PART];
	obj->control = ControlBodyPart;
	obj->nmeshes = 0;
	obj->loaded = 1;

	obj = &objects[BIRD_TWEETER];
	obj->control = ControlBirdTweeter;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[WATER_DRIP];
	obj->control = ControlBirdTweeter;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[DING_DONG];
	obj->control = ControlDingDong;
	obj->draw_routine = DrawDummyItem;

	obj = &objects[LARA_ALARM];
	obj->control = ControlLaraAlarm;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[CLOCK_CHIMES];
	obj->control = ControlClockChimes;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[FINAL_LEVEL];
	obj->control = FinalLevelCounter;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[CUT_SHOTGUN];
	obj->control = ControlCutShotgun;
	obj->save_flags = 1;
	obj->save_anim = 1;

	obj = &objects[EARTHQUAKE];
	obj->control = EarthQuake;
	obj->draw_routine = DrawDummyItem;
	obj->save_flags = 1;

	obj = &objects[GUNSHELL];
	obj->control = ControlGunShell;

	obj = &objects[SHOTGUNSHELL];
	obj->control = ControlGunShell;
}

void InitialiseObjects()
{
	OBJECT_INFO* obj;
	long lp;

	for (lp = 0; lp < NUMBER_OBJECTS; lp++)
	{
		obj = &objects[lp];
		obj->initialise = 0;
		obj->control = 0;
		obj->collision = 0;
		obj->draw_routine = DrawAnimatingItem;
		obj->floor = 0;
		obj->ceiling = 0;
		obj->pivot_length = 0;
		obj->radius = 10;
		obj->shadow_size = 0;
		obj->hit_points = DONT_TARGET;
		obj->intelligent = 0;
		obj->save_position = 0;
		obj->save_hitpoints = 0;
		obj->save_flags = 0;
		obj->save_anim = 0;
		obj->water_creature = 0;
	}

	BaddyObjects();
	TrapObjects();
	ObjectObjects();
	InitialiseHair();
	InitialiseSparks();
}
