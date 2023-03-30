#include "../tomb3/pch.h"
#include "target.h"
#include "items.h"
#include "sound.h"
#include "control.h"
#include "objects.h"
#include "lara.h"
#include "effects.h"

static long reset;

void InitialiseTarget(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->active)
		RemoveActiveItem(item_number);

	item->anim_number = objects[item->object_number].anim_index;
	item->frame_number = anims[item->anim_number].frame_base;
	item->current_anim_state = anims[item->anim_number].current_anim_state;
	item->goal_anim_state = item->current_anim_state;
	item->required_anim_state = TARGET_RISE;
	item->active = 0;
	item->status = ITEM_INACTIVE;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;
	item->item_flags[2] = 0;
	item->timer = 0;
	item->flags = 0;
	item->hit_points = objects[item->object_number].hit_points;
	item->data = 0;
}

void TargetControl(short item_number)
{
	ITEM_INFO* item;

	item = &items[item_number];

	if (item->status != ITEM_ACTIVE)
		return;

	if (item->hit_points != 8)
		reset = 0;

	if (item->hit_points != DONT_TARGET)
	{
		if (item->hit_status)
			SoundEffect(SFX_TARGET_HITS, &item->pos, SFX_DEFAULT);

		switch (item->current_anim_state)
		{
		case TARGET_RISE:

			if (item->hit_points < 6)
			{
				item->hit_points = 6;
				item->goal_anim_state = TARGET_HIT1;
			}

			break;

		case TARGET_HIT1:

			if (item->hit_points < 4)
			{
				item->anim_number = objects[item->object_number].anim_index + 2;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = TARGET_HIT2;
				item->hit_points = 4;
			}

			break;

		case TARGET_HIT2:

			if (item->hit_points < 2)
			{
				item->anim_number = objects[item->object_number].anim_index + 3;
				item->frame_number = anims[item->anim_number].frame_base;
				item->current_anim_state = TARGET_HIT3;
				item->hit_points = 2;
			}

			break;

		case TARGET_HIT3:

			if (item->hit_points <= 0 && item->hit_points != DONT_TARGET)
			{
				lara.target = 0;
				item->hit_points = DONT_TARGET;
				item->item_flags[0] = 1820;
				item->item_flags[1] = 0;
				item->item_flags[2] = 1;
			}

			break;
		}

		item->timer++;

		if (item->timer > 300)
		{
			lara.target = 0;
			item->hit_points = DONT_TARGET;
			item->item_flags[0] = 182;
			item->item_flags[1] = 0;
			item->item_flags[2] = 0;
		}
	}

	if (item->hit_points == DONT_TARGET)
	{
		if (item->item_flags[2] == 1)
		{
			item->pos.x_rot += item->item_flags[0];
			item->item_flags[0] += 728 >> item->item_flags[1];

			if (item->pos.x_rot > 0x3800)
			{
				if (item->item_flags[1] == 2)
				{
					item->pos.x_rot = 0x3800;
					RemoveActiveItem(item_number);
					assault_targets--;
					return;
				}

				if (item->item_flags[1] == 1)
					SoundEffect(SFX_TARGET_SMASH, &item->pos, SFX_DEFAULT);

				item->item_flags[0] = -item->item_flags[0] >> 2;
				item->item_flags[1]++;
				item->pos.x_rot = 0x3800;
			}
		}
		else if (!item->item_flags[2])
		{
			item->pos.x_rot -= item->item_flags[0];
			item->item_flags[0] += (91 >> item->item_flags[1]);

			if (item->pos.x_rot < -0x2A00)
			{
				if (item->item_flags[1] == 2)
				{
					item->pos.x_rot = -0x2A00;
					RemoveActiveItem(item_number);
					return;
				}

				SoundEffect(SFX_TARGET_HITS, &item->pos, 0x2000000 | SFX_SETPITCH);
				item->item_flags[0] = -item->item_flags[0] >> 2;
				item->item_flags[1]++;
				item->pos.x_rot = -0x2A00;
			}
		}
	}

	AnimateItem(item);
}

void ResetTargets()
{
	long lp;

	if (!reset)
	{
		assault_targets = 0;

		for (lp = 0; lp < level_items; lp++)
		{
			if (items[lp].object_number == TARGETS)
			{
				InitialiseTarget((short)lp);
				assault_targets++;
			}
		}

		reset = 1;
	}
}
