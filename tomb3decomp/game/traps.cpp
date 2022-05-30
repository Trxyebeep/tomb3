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
#include "effects.h"
#include "../specific/game.h"

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
				SoundEffect(SFX_SHIVA_SWORD_2, &item->pos, SFX_ALWAYS);
			else
				SoundEffect(SFX_LARA_GETOUT, &item->pos, SFX_ALWAYS);

		}

		AnimateItem(item);
	}
}

void PropellerControl(short item_number)
{
	ITEM_INFO * item;

	item = &items[item_number];

	if (!TriggerActive(item) || item->flags & IFL_INVISIBLE)
	{
		if (item->goal_anim_state != 1)
		{
			if (item->object_number == FAN)
				SoundEffect(SFX_UNDERWATER_FAN_STOP, &item->pos, SFX_WATER);

			item->goal_anim_state = 1;
		}
	}
	else
	{
		item->goal_anim_state = 0;

		if (item->touch_bits & 6)
		{
#ifdef RANDO_STUFF
			if (rando.levels[RANDOLEVEL].original_id == LV_ROOFTOPS)
#else
			if (CurrentLevel == LV_ROOFTOPS)
#endif
			{
				lara_item->hit_points = -1;
				DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos,
					GetRandomControl() >> 10, item->pos.y_rot + 0x4000, lara_item->room_number, 5);
			}
			else
				lara_item->hit_points -= 200;

			lara_item->hit_status = 1;
			DoLotsOfBlood(lara_item->pos.x_pos, lara_item->pos.y_pos - 512, lara_item->pos.z_pos,
				GetRandomControl() >> 10, item->pos.y_rot + 0x4000, lara_item->room_number, 3);

			if (item->object_number == SAW)
				SoundEffect(SFX_VERY_SMALL_WINCH, &item->pos, 0);
		}
		else if (item->object_number == SAW)
			SoundEffect(SFX_DRILL_BIT_1, &item->pos, SFX_DEFAULT);
		else if (item->object_number == FAN)
			SoundEffect(SFX_UNDERWATER_FAN_ON, &item->pos, SFX_WATER);
		else
			SoundEffect(SFX_SMALL_FAN_ON, &item->pos, SFX_DEFAULT);
	}

	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
	{
		RemoveActiveItem(item_number);

		if (item->object_number != SAW)
			item->collidable = 0;
	}
}

void inject_traps(bool replace)
{
	INJECT(0x0046FAE0, LaraBurn, replace);
	INJECT(0x0046FB30, LavaBurn, replace);
	INJECT(0x0046E340, SpikeControl, replace);
	INJECT(0x0046D340, PropellerControl, replace);
}
