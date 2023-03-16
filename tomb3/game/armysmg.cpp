#include "../tomb3/pch.h"
#include "armysmg.h"
#include "box.h"
#include "objects.h"

void InitialiseArmySMG(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[STHPAC_MERCENARY].anim_index + 12;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = ARMY_STOP;
	item->goal_anim_state = ARMY_STOP;
}

void inject_armysmg(bool replace)
{
	INJECT(0x0040FEF0, InitialiseArmySMG, replace);
}
