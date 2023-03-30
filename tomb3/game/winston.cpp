#include "../tomb3/pch.h"
#include "winston.h"
#include "box.h"
#include "sound.h"
#include "../specific/game.h"
#include "objects.h"
#include "lot.h"
#include "control.h"
#include "lara.h"

void OldWinstonControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* winston;
	AI_INFO info;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	winston = (CREATURE_INFO*)item->data;
	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, winston->maximum_turn);

	if (item->current_anim_state == WINSTON_STOP)
	{
		if ((info.distance > 0x240000 || !info.ahead) && item->goal_anim_state != 2)
		{
			item->goal_anim_state = WINSTON_WALK;
			SoundEffect(SFX_WILARD_STAB, &item->pos, SFX_DEFAULT);
		}
	}
	else if (info.distance < 0x240000)
	{
		if (info.ahead)
		{
			item->goal_anim_state = WINSTON_STOP;

			if (winston->flags & 1)
				winston->flags--;
		}
		else if (!(winston->flags & 1))
		{
			SoundEffect(SFX_WILARD_ODD_NOISE, &item->pos, SFX_DEFAULT);
			SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);
			winston->flags |= 1;
		}
	}

	if (item->touch_bits)
	{
		if (!(winston->flags & 2))
		{
			SoundEffect(SFX_LITTLE_SUB_LOOP, &item->pos, SFX_DEFAULT);
			SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);
			winston->flags |= 2;
		}
	}
	else if (winston->flags & 2)
		winston->flags -= 2;

	if (GetRandomDraw() < 256)
		SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);

	CreatureAnimation(item_number, angle, 0);
}

void WinstonControl(short item_number)
{
	ITEM_INFO* item;
	CREATURE_INFO* winston;
	CREATURE_INFO* old;
	AI_INFO info;
	short angle;

	if (!CreatureActive(item_number))
		return;

	item = &items[item_number];
	winston = (CREATURE_INFO*)item->data;
	CreatureAIInfo(item, &info);
	GetCreatureMood(item, &info, 1);
	CreatureMood(item, &info, 1);
	angle = CreatureTurn(item, winston->maximum_turn);

	if (!item->item_flags[1])
	{
		for (int i = 0; i < MAX_LOT; i++)
		{
			old = &baddie_slots[i];

			if (items[old->item_num].object_number == WINSTON)
			{
				items[old->item_num].status = ITEM_INVISIBLE;
				CreatureDie(old->item_num, 0);
				break;
			}
		}
	}

	if (item->hit_points <= 0)
	{
		winston->maximum_turn = 0;

		switch (item->current_anim_state)
		{
		case WINSTON_HITDOWN:
		case WINSTON_FALLDOWN:

			if (item->hit_status)
				item->goal_anim_state = WINSTON_HITDOWN;
			else
			{
				item->item_flags[0]--;

				if (item->item_flags[0] < 0)
					item->goal_anim_state = WINSTON_ONFLOOR;
			}

			break;

		case WINSTON_GETUP:
			item->hit_points = 16;

			if (GetRandomControl() & 1)
				winston->flags = 999;

			break;

		case WINSTON_ONFLOOR:

			if (item->hit_status)
				item->goal_anim_state = WINSTON_HITDOWN;
			else
			{
				item->item_flags[0]--;

				if (item->item_flags[0] < 0)
					item->goal_anim_state = WINSTON_GETUP;
			}

			break;

		default:
			item->anim_number = objects[ARMY_WINSTON].anim_index + 16;
			item->frame_number = anims[item->anim_number].frame_base;
			item->current_anim_state = WINSTON_FALLDOWN;
			item->goal_anim_state = WINSTON_FALLDOWN;
			item->item_flags[0] = 150;
			break;
		}
	}
	else
	{
		switch (item->current_anim_state)
		{
		case WINSTON_STOP:
			winston->maximum_turn = 364;

			if (winston->flags == 999)
				item->goal_anim_state = WINSTON_BRUSHOFF;
			else if (lara.target == item)
				item->goal_anim_state = WINSTON_DEF1;
			else if ((info.distance > 0x240000 || !info.ahead) && item->goal_anim_state != WINSTON_WALK)
			{
				item->goal_anim_state = WINSTON_WALK;
				SoundEffect(SFX_WILARD_STAB, &item->pos, SFX_DEFAULT);
			}

			break;

		case WINSTON_WALK:
			winston->maximum_turn = 364;

			if (lara.target == item)
				item->goal_anim_state = WINSTON_STOP;
			else if (info.distance < 0x240000)
			{
				if (info.ahead)
				{
					item->goal_anim_state = WINSTON_STOP;

					if (winston->flags & 1)
						winston->flags--;
				}
				else if (!(winston->flags & 1))
				{
					SoundEffect(SFX_WILARD_ODD_NOISE, &item->pos, SFX_DEFAULT);
					SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);
					winston->flags |= 1;
				}
			}

			break;

		case WINSTON_DEF1:
			winston->maximum_turn = 364;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;

			if (item->hit_status)
				item->goal_anim_state = WINSTON_HIT1;
			else if (lara.target != item)
				item->goal_anim_state = WINSTON_STOP;

			break;

		case WINSTON_DEF2:
			winston->maximum_turn = 364;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;

			if (item->hit_status)
				item->goal_anim_state = WINSTON_HIT2;

			break;

		case WINSTON_DEF3:
			winston->maximum_turn = 364;

			if (item->required_anim_state)
				item->goal_anim_state = item->required_anim_state;

			if (item->hit_status)
				item->goal_anim_state = WINSTON_HIT3;
			else if (lara.target != item)
				item->goal_anim_state = WINSTON_DEF1;

			break;

		case WINSTON_HIT1:

			if (GetRandomControl() & 1)
				item->required_anim_state = WINSTON_DEF3;
			else
				item->required_anim_state = WINSTON_DEF2;

			break;

		case WINSTON_HIT2:
		case WINSTON_HIT3:
			item->required_anim_state = WINSTON_DEF1;
			break;

		case WINSTON_BRUSHOFF:
			winston->maximum_turn = 0;
			winston->flags = 0;
			break;
		}
	}

	if (GetRandomControl() < 256)
		SoundEffect(SFX_LITTLE_SUB_START, &item->pos, SFX_DEFAULT);

	CreatureAnimation(item_number, angle, 0);
}
