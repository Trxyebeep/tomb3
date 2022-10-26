#include "../tomb3/pch.h"
#include "setup.h"
#include "objects.h"
#include "gameflow.h"
#include "items.h"
#include "../specific/smain.h"

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
					objects[ai_item->object_number].collision == dummyColFunc && ai_item->object_number < AI_PATROL2)
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

#ifdef TROYESTUFF	//clear footprints
	for (int i = 0; i < 32; i++)
		FootPrint[i].Active = 0;

	FootPrintNum = 0;
#endif
}

void inject_setup(bool replace)
{
	INJECT(0x00466590, GetAIPickups, inject_rando ? 1 : replace);
	INJECT(0x00463B70, InitialiseLevelFlags, replace);
}
