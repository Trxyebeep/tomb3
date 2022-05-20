#include "../tomb3/pch.h"
#include "traps.h"
#include "control.h"
#include "gameflow.h"
#include "objects.h"
#include "items.h"
#include "kayak.h"
#include "sound.h"
#ifdef RANDO_STUFF
#include "../specific/smain.h"
#endif

void LaraBurn()
{
	FX_INFO* fx;
	short fxNum;

	if (!lara.burn)
	{
		fxNum = CreateEffect(lara_item->room_number);

		if (fxNum != NO_ITEM)
		{
			fx = &effects[fxNum];
			fx->frame_number = 1;
			fx->object_number = FLAME;
			fx->counter = -1;
			lara.burn = 1;
		}
	}
}

void LavaBurn(ITEM_INFO* item)
{
	FLOOR_INFO* floor;
	long h;
	short room_num;

	if (item->hit_points >= 0 && lara.water_status != LARA_CHEAT)
	{
		room_num = item->room_number;
		floor = GetFloor(item->pos.x_pos, 32000, item->pos.z_pos, &room_num);
		h = GetHeight(floor, item->pos.x_pos, 32000, item->pos.z_pos);

		if (item->floor == h)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_RAPIDS)
#else
			if (CurrentLevel == LV_RAPIDS)
#endif
				LaraRapidsDrown();
			else
			{
				item->hit_status = 1;
				item->hit_points = -1;

#ifdef RANDO_STUFF
				if (rando.levels[RANDOLEVEL].original_id == LV_AREA51 || rando.levels[RANDOLEVEL].original_id == LV_OFFICE)
#else
				if (CurrentLevel == LV_AREA51 || CurrentLevel == LV_OFFICE)
#endif
					lara.electric = 1;
				else
					LaraBurn();
			}
		}
	}
}

void SpikeControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

#ifdef RANDO_STUFF
	if (TriggerActive(item) && (rando.levels[RANDOLEVEL].original_id == LV_SHORE || rando.levels[RANDOLEVEL].original_id == LV_RAPIDS))
#else
	if (TriggerActive(item) && (CurrentLevel == LV_SHORE || CurrentLevel == LV_RAPIDS))
#endif
	{
		if (item->frame_number == anims[item->anim_number].frame_base)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_SHORE)
#else
			if (CurrentLevel == LV_SHORE)
#endif
				SoundEffect(259, &item->pos, SFX_ALWAYS);
			else
				SoundEffect(34, &item->pos, SFX_ALWAYS);

		}

		AnimateItem(item);
	}
}

void inject_traps(bool replace)
{
	INJECT(0x0046FAE0, LaraBurn, replace);
	INJECT(0x0046FB30, LavaBurn, replace);
	INJECT(0x0046E340, SpikeControl, replace);
}
