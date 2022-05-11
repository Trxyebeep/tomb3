#include "../tomb3/pch.h"
#include "traps.h"
#include "control.h"
#include "gameflow.h"
#include "objects.h"
#include "items.h"
#include "kayak.h"
#include "sound.h"

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

		if (CurrentLevel == LV_RAPIDS)
			LaraRapidsDrown();
		else
		{
			item->hit_status = 1;
			item->hit_points = -1;

			if (CurrentLevel == LV_AREA51 || CurrentLevel == LV_OFFICE)
				lara.electric = 1;
			else
				LaraBurn();
		}
	}
}

void SpikeControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (TriggerActive(item) && (CurrentLevel == LV_SHORE || CurrentLevel == LV_RAPIDS))
	{
		if (item->frame_number == anims[item->anim_number].frame_base)
		{
			if (CurrentLevel == LV_SHORE)
				SoundEffect(259, &item->pos, 2);
			else
				SoundEffect(34, &item->pos, 2);

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
