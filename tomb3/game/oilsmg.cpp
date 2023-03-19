#include "../tomb3/pch.h"
#include "oilsmg.h"
#include "box.h"
#include "objects.h"

void InitialiseOilSMG(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];
	InitialiseCreature(item_number);
	item->anim_number = objects[WHITE_SOLDIER].anim_index + 12;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = OILSMG_STOP;
	item->goal_anim_state = OILSMG_STOP;
	
}

void inject_oilsmg(bool replace)
{
	INJECT(0x0045AA00, InitialiseOilSMG, replace);
}
